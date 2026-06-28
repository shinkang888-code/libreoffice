/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * lofice — AI HTTP transport (implementation)
 *
 * Copyright (c) Lonex. Inc. All Rights Reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <lofice/ai/AiConversationHistory.hxx>
#include <lofice/ai/AiHttpTransport.hxx>

#include <sal/config.h>

#if ENABLE_CURL

#include <curl/curl.h>
#include <systools/curlinit.hxx>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <rtl/string.hxx>
#include <sal/log.hxx>
#include <tools/long.hxx>

#include <atomic>
#include <functional>
#include <sstream>
#include <string_view>

namespace lofice::ai
{

namespace
{

std::string escapeJsonString(std::string_view rText)
{
    std::string aOut;
    aOut.reserve(rText.size() + 16);
    for (char c : rText)
    {
        switch (c)
        {
            case '\\': aOut += "\\\\"; break;
            case '"':  aOut += "\\\""; break;
            case '\n': aOut += "\\n"; break;
            case '\r': aOut += "\\r"; break;
            case '\t': aOut += "\\t"; break;
            default:   aOut += c; break;
        }
    }
    return aOut;
}

std::string buildChatRequestJson(const AiHttpConfig& rConfig, const AiPromptRequest& rRequest, bool bStream)
{
    std::ostringstream aOut;
    aOut << "{\"model\":\"" << escapeJsonString(rConfig.model) << "\",";
    aOut << "\"stream\":" << (bStream ? "true" : "false") << ",";
    aOut << "\"messages\":[";
    aOut << "{\"role\":\"system\",\"content\":\"You are lofice AI assistant for "
         << escapeJsonString(rRequest.appDisplayName)
         << ". Answer concisely in the user's language.";

    if (!rRequest.ragContext.empty())
    {
        aOut << "\\n\\nRelevant lofice codebase context:\\n---\\n"
             << escapeJsonString(rRequest.ragContext)
             << "\\n---\\nUse this context when it helps answer the user.";
    }

    aOut << "\"}";

    for (const AiChatMessage& rMessage : rRequest.priorMessages)
    {
        if (rMessage.content.empty())
            continue;
        if (rMessage.role != "user" && rMessage.role != "assistant" && rMessage.role != "system")
            continue;
        aOut << ",{\"role\":\"" << escapeJsonString(rMessage.role)
             << "\",\"content\":\"" << escapeJsonString(rMessage.content) << "\"}";
    }

    aOut << ",{\"role\":\"user\",\"content\":\"" << escapeJsonString(rRequest.prompt) << "\"}";
    aOut << "]}";
    return aOut.str();
}

std::string extractStringField(const boost::property_tree::ptree& rNode, const char* pKey)
{
    const auto aIt = rNode.find(pKey);
    if (aIt == rNode.not_found())
        return {};
    try
    {
        return aIt->second.get_value<std::string>();
    }
    catch (...)
    {
        return {};
    }
}

std::string extractContentFromJson(const std::string& rBody)
{
    if (rBody.empty())
        return {};

    try
    {
        boost::property_tree::ptree aRoot;
        std::istringstream aStream{rBody};
        boost::property_tree::read_json(aStream, aRoot);

        if (const auto aError = aRoot.get_child_optional("error"))
        {
            const std::string aMessage = extractStringField(*aError, "message");
            if (!aMessage.empty())
                return "[API error] " + aMessage;
        }

        if (const auto aChoices = aRoot.get_child_optional("choices"))
        {
            for (const auto& rChoice : *aChoices)
            {
                if (const auto aMessage = rChoice.second.get_child_optional("message"))
                {
                    const std::string aContent = extractStringField(*aMessage, "content");
                    if (!aContent.empty())
                        return aContent;
                }
                const std::string aText = extractStringField(rChoice.second, "text");
                if (!aText.empty())
                    return aText;
            }
        }

        for (const char* pKey : { "response", "text", "content", "message", "output" })
        {
            const std::string aValue = extractStringField(aRoot, pKey);
            if (!aValue.empty())
                return aValue;
        }
    }
    catch (const boost::property_tree::json_parser::json_parser_error& rError)
    {
        SAL_WARN("lofice.ai", "JSON parse failed: " << rError.what());
    }
    catch (...)
    {
        SAL_WARN("lofice.ai", "JSON parse failed with unknown error");
    }

    if (rBody.size() <= 4096)
        return rBody;
    return rBody.substr(0, 4096) + "\n...(truncated)";
}

std::string extractDeltaContentFromSsePayload(std::string_view rPayload)
{
    if (rPayload.empty() || rPayload == "[DONE]")
        return {};

    try
    {
        boost::property_tree::ptree aRoot;
        std::istringstream aStream{std::string(rPayload)};
        boost::property_tree::read_json(aStream, aRoot);

        if (const auto aChoices = aRoot.get_child_optional("choices"))
        {
            for (const auto& rChoice : *aChoices)
            {
                if (const auto aDelta = rChoice.second.get_child_optional("delta"))
                {
                    const std::string aContent = extractStringField(*aDelta, "content");
                    if (!aContent.empty())
                        return aContent;
                }
                const std::string aText = extractStringField(rChoice.second, "text");
                if (!aText.empty())
                    return aText;
            }
        }
    }
    catch (...)
    {
    }
    return {};
}

class SseStreamParser
{
public:
    void feed(std::string_view rChunk, AiStreamSink* pSink, std::string& rAssembled)
    {
        m_aLineBuffer.append(rChunk);
        std::size_t nPos = 0;
        while (true)
        {
            const std::size_t nNewline = m_aLineBuffer.find('\n', nPos);
            if (nNewline == std::string::npos)
            {
                m_aLineBuffer.erase(0, nPos);
                return;
            }

            std::string aLine = m_aLineBuffer.substr(nPos, nNewline - nPos);
            if (!aLine.empty() && aLine.back() == '\r')
                aLine.pop_back();
            nPos = nNewline + 1;

            if (!aLine.starts_with("data:"))
                continue;

            std::string_view aPayload(aLine);
            aPayload.remove_prefix(5);
            while (!aPayload.empty() && aPayload.front() == ' ')
                aPayload.remove_prefix(1);

            if (aPayload.empty() || aPayload == "[DONE]")
                continue;

            const std::string aToken = extractDeltaContentFromSsePayload(aPayload);
            if (aToken.empty())
                continue;

            rAssembled += aToken;
            if (pSink != nullptr)
                pSink->onStreamToken(aToken);
        }
    }

private:
    std::string m_aLineBuffer;
};

struct StreamWriteContext
{
    std::string* pBody = nullptr;
    SseStreamParser* pParser = nullptr;
    AiStreamSink* pSink = nullptr;
    std::string* pAssembled = nullptr;
};

size_t writeCallback(void* pBuffer, size_t nSize, size_t nCount, void* pUserData)
{
    if (pUserData == nullptr)
        return 0;

    auto* pCtx = static_cast<StreamWriteContext*>(pUserData);
    const size_t nRealSize = nSize * nCount;
    const char* pData = static_cast<char*>(pBuffer);

    if (pCtx->pBody != nullptr)
        pCtx->pBody->append(pData, nRealSize);
    if (pCtx->pParser != nullptr && pCtx->pAssembled != nullptr)
        pCtx->pParser->feed(std::string_view(pData, nRealSize), pCtx->pSink, *pCtx->pAssembled);

    return nRealSize;
}

int cancelXferCallback(
    void* pClientp, curl_off_t /*nDltotal*/, curl_off_t /*nDlnow*/,
    curl_off_t /*nUltotal*/, curl_off_t /*nUlnow*/)
{
    const auto* pCancel = static_cast<const std::atomic<bool>*>(pClientp);
    if (pCancel != nullptr && pCancel->load())
        return 1;
    return 0;
}

AiHttpResponse performCurlRequest(
    const AiHttpConfig& rConfig,
    const std::string& rRequestBody,
    StreamWriteContext& rWriteCtx,
    const std::atomic<bool>* pCancelFlag)
{
    AiHttpResponse aResponse;

    std::unique_ptr<CURL, std::function<void(CURL*)>> pCurl(
        curl_easy_init(), [](CURL* pHandle) { curl_easy_cleanup(pHandle); });
    if (!pCurl)
    {
        aResponse.errorDetail = "curl_easy_init failed.";
        return aResponse;
    }

    InitCurl_easy(pCurl.get());

    struct curl_slist* pHeaders = nullptr;
    pHeaders = curl_slist_append(pHeaders, "Content-Type: application/json");
    pHeaders = curl_slist_append(pHeaders, "Accept: text/event-stream");
    if (!rConfig.apiKey.empty())
    {
        const OString aAuthHeader = "Authorization: Bearer " + OString(rConfig.apiKey);
        pHeaders = curl_slist_append(pHeaders, aAuthHeader.getStr());
    }

    (void)curl_easy_setopt(pCurl.get(), CURLOPT_URL, rConfig.endpoint.c_str());
    (void)curl_easy_setopt(pCurl.get(), CURLOPT_HTTPHEADER, pHeaders);
    (void)curl_easy_setopt(pCurl.get(), CURLOPT_POST, 1L);
    (void)curl_easy_setopt(pCurl.get(), CURLOPT_POSTFIELDS, rRequestBody.c_str());
    (void)curl_easy_setopt(pCurl.get(), CURLOPT_FAILONERROR, 0L);
    (void)curl_easy_setopt(pCurl.get(), CURLOPT_TIMEOUT, rConfig.timeoutSeconds);
    (void)curl_easy_setopt(pCurl.get(), CURLOPT_WRITEFUNCTION, &writeCallback);
    (void)curl_easy_setopt(pCurl.get(), CURLOPT_WRITEDATA, static_cast<void*>(&rWriteCtx));

    if (pCancelFlag != nullptr)
    {
        (void)curl_easy_setopt(pCurl.get(), CURLOPT_NOPROGRESS, 0L);
        (void)curl_easy_setopt(pCurl.get(), CURLOPT_XFERINFOFUNCTION, &cancelXferCallback);
        (void)curl_easy_setopt(
            pCurl.get(), CURLOPT_XFERINFODATA,
            const_cast<std::atomic<bool>*>(pCancelFlag));
    }

    const CURLcode eCode = curl_easy_perform(pCurl.get());
    curl_slist_free_all(pHeaders);

    tools::Long nStatusCode = 0;
    curl_easy_getinfo(pCurl.get(), CURLINFO_RESPONSE_CODE, &nStatusCode);
    aResponse.httpStatus = nStatusCode;

    if (eCode == CURLE_ABORTED)
    {
        aResponse.errorDetail = "Request cancelled.";
        return aResponse;
    }

    if (eCode != CURLE_OK)
    {
        aResponse.errorDetail = std::string("HTTP request failed: ") + curl_easy_strerror(eCode);
        return aResponse;
    }

    if (nStatusCode < 200 || nStatusCode >= 300)
    {
        aResponse.errorDetail = "HTTP status " + std::to_string(nStatusCode);
        if (rWriteCtx.pBody != nullptr && !rWriteCtx.pBody->empty())
            aResponse.errorDetail += " — " + rWriteCtx.pBody->substr(0, 512);
        return aResponse;
    }

    aResponse.success = true;
    return aResponse;
}

} // namespace

bool isHttpTransportAvailable()
{
    return true;
}

AiHttpResponse postChatCompletion(
    const AiHttpConfig& rConfig,
    const AiPromptRequest& rRequest,
    const std::atomic<bool>* pCancelFlag)
{
    AiHttpResponse aResponse;
    if (!rConfig.isValid())
    {
        aResponse.errorDetail = "LOFICE_AI_ENDPOINT is not set.";
        return aResponse;
    }

    if (pCancelFlag != nullptr && pCancelFlag->load())
    {
        aResponse.errorDetail = "Request cancelled.";
        return aResponse;
    }

    std::string aResponseBody;
    StreamWriteContext aWriteCtx;
    aWriteCtx.pBody = &aResponseBody;

    aResponse = performCurlRequest(
        rConfig, buildChatRequestJson(rConfig, rRequest, false), aWriteCtx, pCancelFlag);
    if (!aResponse.success)
        return aResponse;

    aResponse.content = extractContentFromJson(aResponseBody);
    if (aResponse.content.empty())
    {
        aResponse.success = false;
        aResponse.errorDetail = "Empty or unparseable API response.";
    }
    return aResponse;
}

AiHttpResponse postChatCompletionStreaming(
    const AiHttpConfig& rConfig,
    const AiPromptRequest& rRequest,
    AiStreamSink& rSink,
    const std::atomic<bool>* pCancelFlag)
{
    AiHttpResponse aResponse;
    if (!rConfig.isValid())
    {
        aResponse.errorDetail = "LOFICE_AI_ENDPOINT is not set.";
        rSink.onStreamFinished(false, aResponse.errorDetail);
        return aResponse;
    }

    if (pCancelFlag != nullptr && pCancelFlag->load())
    {
        aResponse.errorDetail = "Request cancelled.";
        rSink.onStreamFinished(false, aResponse.errorDetail);
        return aResponse;
    }

    std::string aResponseBody;
    std::string aAssembled;
    SseStreamParser aParser;
    StreamWriteContext aWriteCtx;
    aWriteCtx.pBody = &aResponseBody;
    aWriteCtx.pParser = &aParser;
    aWriteCtx.pSink = &rSink;
    aWriteCtx.pAssembled = &aAssembled;

    aResponse = performCurlRequest(
        rConfig, buildChatRequestJson(rConfig, rRequest, true), aWriteCtx, pCancelFlag);

    if (!aResponse.success)
    {
        rSink.onStreamFinished(false, aResponse.errorDetail);
        return aResponse;
    }

    if (aAssembled.empty())
    {
        aAssembled = extractContentFromJson(aResponseBody);
        if (!aAssembled.empty())
            rSink.onStreamToken(aAssembled);
    }

    if (aAssembled.empty())
    {
        aResponse.success = false;
        aResponse.errorDetail = "Empty or unparseable streaming response.";
        rSink.onStreamFinished(false, aResponse.errorDetail);
        return aResponse;
    }

    aResponse.content = aAssembled;
    rSink.onStreamFinished(true, {});
    return aResponse;
}

} // namespace lofice::ai

#else // !ENABLE_CURL

namespace lofice::ai
{

bool isHttpTransportAvailable()
{
    return false;
}

AiHttpResponse postChatCompletion(
    const AiHttpConfig& /*rConfig*/,
    const AiPromptRequest& /*rRequest*/,
    const std::atomic<bool>* /*pCancelFlag*/)
{
    AiHttpResponse aResponse;
    aResponse.errorDetail = "lofice was built without curl (ENABLE_CURL=NO).";
    return aResponse;
}

AiHttpResponse postChatCompletionStreaming(
    const AiHttpConfig& /*rConfig*/,
    const AiPromptRequest& /*rRequest*/,
    AiStreamSink& rSink,
    const std::atomic<bool>* /*pCancelFlag*/)
{
    AiHttpResponse aResponse;
    aResponse.errorDetail = "lofice was built without curl (ENABLE_CURL=NO).";
    rSink.onStreamFinished(false, aResponse.errorDetail);
    return aResponse;
}

} // namespace lofice::ai

#endif // ENABLE_CURL
