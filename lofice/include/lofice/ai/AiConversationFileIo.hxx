/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * lofice — AI conversation JSON file export/import
 *
 * Copyright (c) Lonex. Inc. All Rights Reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_LOFICE_AI_AICONVERSATIONFILEIO_HXX
#define INCLUDED_LOFICE_AI_AICONVERSATIONFILEIO_HXX

#include <string>

namespace weld { class Window; }

namespace lofice::ai
{

class AiConversationHistory;

enum class ConversationFileResult
{
    Ok,
    Cancelled,
    Error,
};

struct ConversationFileOutcome
{
    ConversationFileResult result = ConversationFileResult::Error;
    std::string errorDetail;
};

/** Show save dialog and write conversation envelope JSON. */
ConversationFileOutcome exportConversationToFile(
    weld::Window* pParent,
    const AiConversationHistory& rHistory);

/** Show open dialog and parse conversation JSON into rHistory. */
ConversationFileOutcome importConversationFromFile(
    weld::Window* pParent,
    AiConversationHistory& rHistory);

} // namespace lofice::ai

#endif // INCLUDED_LOFICE_AI_AICONVERSATIONFILEIO_HXX
