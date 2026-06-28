/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * lofice — multi-turn AI conversation history (implementation)
 *
 * Copyright (c) Lonex. Inc. All Rights Reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <lofice/ai/AiConversationHistory.hxx>

namespace lofice::ai
{

void AiConversationHistory::clear()
{
    m_aMessages.clear();
}

void AiConversationHistory::appendUser(std::string rContent)
{
    if (rContent.empty())
        return;
    m_aMessages.push_back(AiChatMessage{ "user", std::move(rContent) });
    trimToMax();
}

void AiConversationHistory::appendAssistant(std::string rContent)
{
    if (rContent.empty())
        return;
    m_aMessages.push_back(AiChatMessage{ "assistant", std::move(rContent) });
    trimToMax();
}

void AiConversationHistory::populatePriorMessages(std::vector<AiChatMessage>& rOut) const
{
    rOut = m_aMessages;
}

bool AiConversationHistory::lastAssistantMessage(std::string& rOut) const
{
    for (auto aIt = m_aMessages.rbegin(); aIt != m_aMessages.rend(); ++aIt)
    {
        if (aIt->role == "assistant" && !aIt->content.empty())
        {
            rOut = aIt->content;
            return true;
        }
    }
    return false;
}

std::size_t AiConversationHistory::size() const
{
    return m_aMessages.size();
}

void AiConversationHistory::trimToMax()
{
    if (m_aMessages.size() <= kDefaultMaxMessages)
        return;

    const std::size_t nDrop = m_aMessages.size() - kDefaultMaxMessages;
    m_aMessages.erase(m_aMessages.begin(), m_aMessages.begin() + static_cast<std::ptrdiff_t>(nDrop));
}

} // namespace lofice::ai
