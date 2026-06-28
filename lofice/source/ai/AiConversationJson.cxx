/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * lofice — AI conversation JSON serialization (implementation)
 *
 * Copyright (c) Lonex. Inc. All Rights Reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <lofice/ai/AiConversationJson.hxx>

#include <lofice/ai/AiConversationHistory.hxx>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <chrono>
#include <iomanip>
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

std::string iso8601UtcNow()
{
    const auto aNow = std::chrono::system_clock::now();
    const std::time_t aTime = std::chrono::system_clock::to_time_t(aNow);
    std::tm aUtc{};
#ifdef _WIN32
    gmtime_s(&aUtc, &aTime);
#else
    gmtime_r(&aTime, &aUtc);
#endif
    std::ostringstream aOut;
    aOut << std::put_time(&aUtc, "%Y-%m-%dT%H:%M:%SZ");
    return aOut.str();
}

void appendMessagesFromArray(
    const boost::property_tree::ptree& rMessages,
    AiConversationHistory& rHistory)
{
    for (const auto& rEntry : rMessages)
    {
        const std::string aRole = rEntry.second.get<std::string>("role", "");
        const std::string aContent = rEntry.second.get<std::string>("content", "");
        if (aContent.empty())
            continue;

        if (aRole == "user")
            rHistory.appendUser(aContent);
        else if (aRole == "assistant")
            rHistory.appendAssistant(aContent);
    }
}

} // namespace

std::string serializeConversationToJson(const AiConversationHistory& rHistory)
{
    std::vector<AiChatMessage> aMessages;
    rHistory.populatePriorMessages(aMessages);

    std::ostringstream aOut;
    aOut << "[";
    for (std::size_t i = 0; i < aMessages.size(); ++i)
    {
        if (i > 0)
            aOut << ",";
        const AiChatMessage& rMsg = aMessages[i];
        aOut << "{\"role\":\"" << escapeJsonString(rMsg.role)
             << "\",\"content\":\"" << escapeJsonString(rMsg.content) << "\"}";
    }
    aOut << "]";
    return aOut.str();
}

std::string serializeConversationFileEnvelope(const AiConversationHistory& rHistory)
{
    const std::string aMessagesJson = serializeConversationToJson(rHistory);
    std::ostringstream aOut;
    aOut << "{\"format\":\"lofice-chat-history\",\"version\":1,\"exportedAt\":\""
         << escapeJsonString(iso8601UtcNow()) << "\",\"messages\":" << aMessagesJson << "}";
    return aOut.str();
}

bool parseConversationJson(
    std::string_view rJson,
    AiConversationHistory& rHistory,
    std::string* pErrorDetail)
{
    if (rJson.empty())
    {
        if (pErrorDetail)
            *pErrorDetail = "Empty JSON input.";
        return false;
    }

    try
    {
        boost::property_tree::ptree aRoot;
        std::istringstream aStream{std::string(rJson)};
        boost::property_tree::read_json(aStream, aRoot);

        rHistory.clear();

        if (aRoot.get_child_optional("messages"))
        {
            appendMessagesFromArray(aRoot.get_child("messages"), rHistory);
            return true;
        }

        appendMessagesFromArray(aRoot, rHistory);
        return true;
    }
    catch (const boost::property_tree::json_parser::json_parser_error& rError)
    {
        if (pErrorDetail)
            *pErrorDetail = rError.what();
        return false;
    }
    catch (...)
    {
        if (pErrorDetail)
            *pErrorDetail = "Unknown JSON parse error.";
        return false;
    }
}

} // namespace lofice::ai
