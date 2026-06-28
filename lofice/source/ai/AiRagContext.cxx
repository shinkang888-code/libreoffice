/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * lofice — Neon RAG context fetch (implementation)
 *
 * Copyright (c) Lonex. Inc. All Rights Reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <lofice/ai/AiRagContext.hxx>

#include <lofice/ai/AiHttpTransport.hxx>
#include <lofice/ai/AiSettingsStore.hxx>

#include <sal/config.h>

#if ENABLE_CURL

#include <curl/curl.h>
#include <systools/curlinit.hxx>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <tools/long.hxx>

#include <cctype>
#include <sstream>
#include <string_view>

namespace lofice::ai
{

namespace
{

std::string urlEncodeQuery(std::string_view rText)
{
    static const char* pHex = "0123456789ABCDEF";
    std::string aOut;
    aOut.reserve(rText.size() + 16);

    for (unsigned char c : rText)
    {
        if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
            aOut.push_back(static_cast<char>(c));
        else if (c == ' ')
            aOut.push_back('+');
        else
        {
            aOut.push_back('%');
            aOut.push_back(pHex[c >> 4]);
            aOut.push_back(pHex[c & 0x0F]);
        }
    }
    return aOut;
}

size_t writeToString(void* pData, size_t nSize, size_t nMemb, void* pUser)
{
    const size_t nTotal = nSize * nMemb;
    static_cast<std::string*>(pUser)->append(static_cast<char*>(pData), nTotal);
    return nTotal;
}

std::string extractContextFromJson(const std::string& rBody)
{
    if (rBody.empty())
        return {};

    try
    {
        boost::property_tree::ptree aRoot;
        std::istringstream aStream(rBody);
        boost::property_tree::read_json(aStream, aRoot);

        if (aRoot.get_optional<std::string>("error"))
            return {};

        return aRoot.get<std::string>("context", {});
    }
    catch (...)
    {
        return {};
    }
}

std::string performRagGet(const AiRagSettings& rSettings, const std::string& rUrl)
{
    std::unique_ptr<CURL, void (*)(CURL*)> pCurl(curl_easy_init(), curl_easy_cleanup);
    if (!pCurl)
        return {};

    InitCurl_easy(pCurl.get());

    struct curl_slist* pHeaders = nullptr;
    pHeaders = curl_slist_append(pHeaders, "Accept: application/json");

    std::string aBody;
    (void)curl_easy_setopt(pCurl.get(), CURLOPT_URL, rUrl.c_str());
    (void)curl_easy_setopt(pCurl.get(), CURLOPT_HTTPHEADER, pHeaders);
    (void)curl_easy_setopt(pCurl.get(), CURLOPT_HTTPGET, 1L);
    (void)curl_easy_setopt(pCurl.get(), CURLOPT_FAILONERROR, 0L);
    (void)curl_easy_setopt(pCurl.get(), CURLOPT_TIMEOUT, rSettings.timeoutSeconds);
    (void)curl_easy_setopt(pCurl.get(), CURLOPT_WRITEFUNCTION, &writeToString);
    (void)curl_easy_setopt(pCurl.get(), CURLOPT_WRITEDATA, static_cast<void*>(&aBody));

    const CURLcode eCode = curl_easy_perform(pCurl.get());
    curl_slist_free_all(pHeaders);

    if (eCode != CURLE_OK)
        return {};

    tools::Long nStatusCode = 0;
    curl_easy_getinfo(pCurl.get(), CURLINFO_RESPONSE_CODE, &nStatusCode);
    if (nStatusCode < 200 || nStatusCode >= 300)
        return {};

    return extractContextFromJson(aBody);
}

std::string buildSearchUrl(const AiRagSettings& rSettings, const std::string& rQueryUtf8)
{
    return rSettings.endpoint
        + (rSettings.endpoint.find('?') != std::string::npos ? "&" : "?")
        + "q=" + urlEncodeQuery(rQueryUtf8)
        + "&mode=semantic&limit=6";
}

} // namespace

AiRagSettings loadRagSettings()
{
    return toRagSettings(loadSettings());
}

bool isRagConfigured()
{
    if (!isHttpTransportAvailable())
        return false;
    const AiRagSettings aSettings = loadRagSettings();
    return aSettings.useRagContext && !aSettings.endpoint.empty();
}

std::string fetchRagContextForSettings(
    const AiRagSettings& rSettings, const std::string& rQueryUtf8)
{
    if (!rSettings.useRagContext || rQueryUtf8.empty() || rSettings.endpoint.empty())
        return {};

    return performRagGet(rSettings, buildSearchUrl(rSettings, rQueryUtf8));
}

std::string fetchRagContext(const std::string& rQueryUtf8)
{
    return fetchRagContextForSettings(loadRagSettings(), rQueryUtf8);
}

void enrichPromptWithRag(AiPromptRequest& rRequest)
{
    if (!isRagConfigured() || rRequest.prompt.empty())
        return;

    rRequest.ragContext = fetchRagContext(rRequest.prompt);
}

std::string formatRagReadyStatus(const std::string& rContext, bool bStreaming)
{
    const char* pSuffix = bStreaming ? "AI 스트리밍 수신 중..." : "AI 처리 중...";
    if (rContext.empty())
        return std::string("RAG 결과 없음 — ") + pSuffix;
    return "RAG " + std::to_string(rContext.size()) + "자 — " + pSuffix;
}

} // namespace lofice::ai

#else // !ENABLE_CURL

namespace lofice::ai
{

AiRagSettings loadRagSettings()
{
    return {};
}

bool isRagConfigured()
{
    return false;
}

std::string fetchRagContextForSettings(
    const AiRagSettings& /*rSettings*/, const std::string& /*rQueryUtf8*/)
{
    return {};
}

std::string fetchRagContext(const std::string& /*rQueryUtf8*/)
{
    return {};
}

void enrichPromptWithRag(AiPromptRequest& /*rRequest*/)
{
}

std::string formatRagReadyStatus(const std::string& /*rContext*/, bool bStreaming)
{
    const char* pSuffix = bStreaming ? "AI 스트리밍 수신 중..." : "AI 처리 중...";
    return std::string("RAG 사용 불가 — ") + pSuffix;
}

} // namespace lofice::ai

#endif // ENABLE_CURL
