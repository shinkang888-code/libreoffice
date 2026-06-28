/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * lofice — AI conversation JSON serialization
 *
 * Copyright (c) Lonex. Inc. All Rights Reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_LOFICE_AI_AICONVERSATIONJSON_HXX
#define INCLUDED_LOFICE_AI_AICONVERSATIONJSON_HXX

#include <string>
#include <string_view>

namespace lofice::ai
{

class AiConversationHistory;

/** Compact JSON array for officecfg persistence. */
std::string serializeConversationToJson(const AiConversationHistory& rHistory);

/** File export envelope with format metadata. */
std::string serializeConversationFileEnvelope(const AiConversationHistory& rHistory);

/**
 * Parse officecfg array or file envelope into rHistory (replaces existing turns).
 * Returns false and optional error detail on failure.
 */
bool parseConversationJson(
    std::string_view rJson,
    AiConversationHistory& rHistory,
    std::string* pErrorDetail = nullptr);

} // namespace lofice::ai

#endif // INCLUDED_LOFICE_AI_AICONVERSATIONJSON_HXX
