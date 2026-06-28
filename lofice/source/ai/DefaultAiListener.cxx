/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * lofice — Default AI event listener (implementation)
 *
 * Copyright (c) Lonex. Inc. All Rights Reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <lofice/ai/DefaultAiListener.hxx>

#include <lofice/ai/AiPromptService.hxx>

namespace lofice::ai
{

std::string_view DefaultAiListener::getListenerId() const
{
    return "lofice.default-ai-listener";
}

bool DefaultAiListener::onBeforeEvent(const EventPayload& /*rEvent*/)
{
    return false;
}

void DefaultAiListener::onAfterEvent(const EventPayload& /*rEvent*/)
{
}

bool DefaultAiListener::onTextHook(std::string& rText, const EventPayload& rContext)
{
    if (rContext.action != "aiPromptSubmit")
        return false;

    AiPromptRequest aRequest;
    aRequest.prompt = rText;
    aRequest.appModule = rContext.sourceModule;

    if (aRequest.appModule == "sw")
        aRequest.appDisplayName = "Writer";
    else if (aRequest.appModule == "sc")
        aRequest.appDisplayName = "Calc";
    else if (aRequest.appModule == "sd")
        aRequest.appDisplayName = "Impress/Draw";
    else
        aRequest.appDisplayName = "Universal";

    const AiPromptResult aResult = processPrompt(aRequest);
    if (!aResult.success)
        return false;

    rText = aResult.response;
    return true;
}

} // namespace lofice::ai
