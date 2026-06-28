/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * lofice — AI conversation persistence (officecfg)
 *
 * Copyright (c) Lonex. Inc. All Rights Reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_LOFICE_AI_AICONVERSATIONSTORE_HXX
#define INCLUDED_LOFICE_AI_AICONVERSATIONSTORE_HXX

namespace lofice::ai
{

class AiConversationHistory;

/** Load persisted chat turns from user officecfg into rHistory. */
void loadConversationHistory(AiConversationHistory& rHistory);

/** Save chat turns to user officecfg (empty history clears storage). */
void saveConversationHistory(const AiConversationHistory& rHistory);

} // namespace lofice::ai

#endif // INCLUDED_LOFICE_AI_AICONVERSATIONSTORE_HXX
