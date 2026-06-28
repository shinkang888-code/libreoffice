/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * lofice — multi-turn AI conversation history
 *
 * Copyright (c) Lonex. Inc. All Rights Reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_LOFICE_AI_AICONVERSATIONHISTORY_HXX
#define INCLUDED_LOFICE_AI_AICONVERSATIONHISTORY_HXX

#include <cstddef>
#include <string>
#include <vector>

namespace lofice::ai
{

struct AiChatMessage
{
    std::string role;
    std::string content;
};

/** In-memory chat turns for OpenAI-compatible messages[] context. */
class AiConversationHistory
{
public:
    static constexpr std::size_t kDefaultMaxMessages = 20;

    void clear();
    void appendUser(std::string rContent);
    void appendAssistant(std::string rContent);
    void populatePriorMessages(std::vector<AiChatMessage>& rOut) const;
    bool lastAssistantMessage(std::string& rOut) const;
    std::size_t size() const;

private:
    void trimToMax();

    std::vector<AiChatMessage> m_aMessages;
};

} // namespace lofice::ai

#endif // INCLUDED_LOFICE_AI_AICONVERSATIONHISTORY_HXX
