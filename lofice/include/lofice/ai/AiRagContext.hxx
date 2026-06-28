/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * lofice — Neon RAG context fetch (HTTP client)
 *
 * Copyright (c) Lonex. Inc. All Rights Reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_LOFICE_AI_AIRAGCONTEXT_HXX
#define INCLUDED_LOFICE_AI_AIRAGCONTEXT_HXX

#include <lofice/ai/AiPromptService.hxx>

#include <sal/types.h>

#include <string>

namespace lofice::ai
{

struct AiRagSettings
{
    bool useRagContext = false;
    std::string endpoint = "https://lofice-rag-api.vercel.app/search";
    sal_Int32 timeoutSeconds = 8;
};

/** Load RAG settings from officecfg + env. */
AiRagSettings loadRagSettings();

/** True when RAG is enabled and curl transport is available. */
bool isRagConfigured();

/** GET /search?q=... — returns formatted context or empty on failure. */
std::string fetchRagContext(const std::string& rQueryUtf8);

/** Uses explicit settings (UI preview / test without save). */
std::string fetchRagContextForSettings(
    const AiRagSettings& rSettings, const std::string& rQueryUtf8);

/** Populate rRequest.ragContext when RAG is configured. */
void enrichPromptWithRag(AiPromptRequest& rRequest);

/** UI status line after RAG fetch — bStreaming selects 스트리밍 vs 처리 중 wording. */
std::string formatRagReadyStatus(const std::string& rContext, bool bStreaming);

} // namespace lofice::ai

#endif // INCLUDED_LOFICE_AI_AIRAGCONTEXT_HXX
