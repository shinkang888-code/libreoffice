/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * lofice — background AI HTTP request job (implementation)
 *
 * Copyright (c) Lonex. Inc. All Rights Reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <lofice/ai/AiAsyncHttpJob.hxx>

#include <lofice/ai/AiHttpTransport.hxx>

#include <vcl/svapp.hxx>

namespace lofice::ai
{

AiAsyncHttpJob::AiAsyncHttpJob() = default;

AiAsyncHttpJob::~AiAsyncHttpJob()
{
    cancel();
}

void AiAsyncHttpJob::start(
    const AiHttpConfig& rConfig,
    const AiPromptRequest& rRequest,
    const Link<void*, void>& rUiLink)
{
    cancel();

    {
        std::lock_guard aGuard(m_aMutex);
        m_aUiLink = rUiLink;
        m_bCancelled.store(false);
        m_bRunning.store(true);
    }

    m_aThread = std::thread([this, rConfig, rRequest]() {
        const AiHttpResponse aResponse = postChatCompletion(rConfig, rRequest, &m_bCancelled);

        auto pPayload = std::make_unique<AiHttpUiPayload>();
        pPayload->success = aResponse.success;
        pPayload->content = aResponse.content;
        pPayload->errorDetail = aResponse.errorDetail;
        postUiEvent(std::move(pPayload));

        std::lock_guard aGuard(m_aMutex);
        m_bRunning.store(false);
    });
}

void AiAsyncHttpJob::cancel()
{
    m_bCancelled.store(true);

    std::lock_guard aGuard(m_aMutex);
    joinThreadLocked();
    m_aUiLink = Link<void*, void>();
}

bool AiAsyncHttpJob::isRunning() const
{
    return m_bRunning.load();
}

void AiAsyncHttpJob::joinThreadLocked()
{
    if (m_aThread.joinable())
        m_aThread.join();
}

void AiAsyncHttpJob::postUiEvent(std::unique_ptr<AiHttpUiPayload> pPayload)
{
    Link<void*, void> aUiLink;
    {
        std::lock_guard aGuard(m_aMutex);
        aUiLink = m_aUiLink;
    }

    if (!aUiLink.IsSet())
        return;

    Application::PostUserEvent(aUiLink, pPayload.release());
}

} // namespace lofice::ai
