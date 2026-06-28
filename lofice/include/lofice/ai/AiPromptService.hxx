/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * lofice — AI prompt service (Phase 4 backend bridge)
 *
 * Copyright (c) Lonex. Inc. All Rights Reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_LOFICE_AI_AIPROMPTSERVICE_HXX
#define INCLUDED_LOFICE_AI_AIPROMPTSERVICE_HXX

#include <lofice/ai/AiConversationHistory.hxx>

#include <string>
#include <vector>

namespace lofice::ai
{

struct AiPromptRequest
{
    std::string prompt;
    std::string appModule;
    std::string appDisplayName;
    /** Prior user/assistant turns — current prompt is sent separately. */
    std::vector<AiChatMessage> priorMessages;
    /** Codebase RAG context from local Neon search server (optional). */
    std::string ragContext;
    /** When true, processPrompt skips enrichPromptWithRag (already applied on worker). */
    bool ragPrefetched = false;
};

struct AiPromptResult
{
    bool success = false;
    std::string response;
    std::string provider;
};

/** Process a user prompt — builtin stub or LOFICE_AI_ENDPOINT when configured. */
AiPromptResult processPrompt(const AiPromptRequest& rRequest);

/** Returns true when LOFICE_AI_ENDPOINT environment variable is set. */
bool isExternalEndpointConfigured();

} // namespace lofice::ai

#endif // INCLUDED_LOFICE_AI_AIPROMPTSERVICE_HXX
