/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * lofice — AI settings persistence (officecfg + env fallback)
 *
 * Copyright (c) Lonex. Inc. All Rights Reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_LOFICE_AI_AISETTINGSSTORE_HXX
#define INCLUDED_LOFICE_AI_AISETTINGSSTORE_HXX

#include <lofice/ai/AiHttpConfig.hxx>
#include <lofice/ai/AiRagContext.hxx>

#include <rtl/ustring.hxx>

namespace lofice::ai
{

struct AiSettings
{
    bool useBuiltinAi = true;
    OUString endpoint;
    OUString apiKey;
    OUString model = u"lofice-default"_ustr;
    sal_Int32 timeoutSeconds = 30;
    bool useRagContext = false;
    OUString ragEndpoint = u"https://lofice-rag-api.vercel.app/search"_ustr;
    sal_Int32 ragTimeoutSeconds = 8;
    bool autoSendQuickActionPrompt = true;
};

/** Load user settings — officecfg first, empty fields filled from env vars. */
AiSettings loadSettings();

/** Persist settings to the user configuration layer. */
void saveSettings(const AiSettings& rSettings);

/** Convert UI/registry settings to HTTP config (empty endpoint when builtin). */
AiHttpConfig toHttpConfig(const AiSettings& rSettings);

/** Effective HTTP config for AiPromptService. */
AiHttpConfig loadEffectiveHttpConfig();

/** True when external HTTP should be used. */
bool isExternalAiConfigured();

/** Convert UI/registry settings to RAG HTTP config. */
AiRagSettings toRagSettings(const AiSettings& rSettings);

} // namespace lofice::ai

#endif // INCLUDED_LOFICE_AI_AISETTINGSSTORE_HXX
