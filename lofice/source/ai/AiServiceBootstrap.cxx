/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * lofice — AI service bootstrap (implementation)
 *
 * Copyright (c) Lonex. Inc. All Rights Reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <lofice/ai/AiServiceBootstrap.hxx>

#include <lofice/IEventListener.hxx>
#include <lofice/ai/DefaultAiListener.hxx>

#include <memory>
#include <mutex>

namespace lofice::ai
{

void ensureDefaultAiListener()
{
    static std::once_flag s_once;
    std::call_once(s_once, []() {
        EventListenerRegistry::instance().addListener(
            std::make_shared<DefaultAiListener>());
    });
}

} // namespace lofice::ai
