/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * lofice — AI HTTP transport (OpenAI-compatible chat API)
 *
 * Copyright (c) Lonex. Inc. All Rights Reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_LOFICE_AI_AIHTTPTRANSPORT_HXX
#define INCLUDED_LOFICE_AI_AIHTTPTRANSPORT_HXX

#include <lofice/ai/AiHttpConfig.hxx>
#include <lofice/ai/AiPromptService.hxx>
#include <lofice/ai/AiStreamSink.hxx>

#include <atomic>
#include <string>

namespace lofice::ai
{

struct AiHttpResponse
{
    bool success = false;
    std::string content;
    long httpStatus = 0;
    std::string errorDetail;
};

/** POST chat-completions JSON to LOFICE_AI_ENDPOINT. Requires ENABLE_CURL. */
AiHttpResponse postChatCompletion(
    const AiHttpConfig& rConfig,
    const AiPromptRequest& rRequest,
    const std::atomic<bool>* pCancelFlag = nullptr);

/** SSE streaming variant — tokens delivered via rSink during curl perform. */
AiHttpResponse postChatCompletionStreaming(
    const AiHttpConfig& rConfig,
    const AiPromptRequest& rRequest,
    AiStreamSink& rSink,
    const std::atomic<bool>* pCancelFlag = nullptr);

/** True when curl support was compiled in. */
bool isHttpTransportAvailable();

} // namespace lofice::ai

#endif // INCLUDED_LOFICE_AI_AIHTTPTRANSPORT_HXX
