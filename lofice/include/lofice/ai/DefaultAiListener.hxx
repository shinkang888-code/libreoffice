/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * lofice — Default AI event listener
 *
 * Copyright (c) Lonex. Inc. All Rights Reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_LOFICE_AI_DEFAULTAILISTENER_HXX
#define INCLUDED_LOFICE_AI_DEFAULTAILISTENER_HXX

#include <lofice/IEventListener.hxx>

namespace lofice::ai
{

/** Built-in IEventListener — handles aiPromptSubmit via onTextHook. */
class DefaultAiListener final : public IEventListener
{
public:
    std::string_view getListenerId() const override;

    bool onBeforeEvent(const EventPayload& rEvent) override;

    void onAfterEvent(const EventPayload& rEvent) override;

    bool onTextHook(std::string& rText, const EventPayload& rContext) override;
};

} // namespace lofice::ai

#endif // INCLUDED_LOFICE_AI_DEFAULTAILISTENER_HXX
