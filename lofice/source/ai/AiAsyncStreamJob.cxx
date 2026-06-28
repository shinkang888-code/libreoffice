/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * lofice — background AI SSE streaming job (implementation)
 *
 * Copyright (c) Lonex. Inc. All Rights Reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <lofice/ai/AiAsyncStreamJob.hxx>

#include <lofice/ai/AiHttpTransport.hxx>
#include <lofice/ai/AiRagContext.hxx>
#include <lofice/ai/AiStreamSink.hxx>

#include <vcl/svapp.hxx>

namespace lofice::ai
{

namespace
{

class AiAsyncStreamJob::WorkerSink : public AiStreamSink
{
public:
    explicit WorkerSink(AiAsyncStreamJob& rJob)
        : m_rJob(rJob)
    {
    }

    void onStreamToken(std::string_view rToken) override
    {
        if (m_rJob.m_bCancelled.load())
            return;

        auto pPayload = std::make_unique<AiStreamEventPayload>();
        pPayload->kind = AiStreamEventKind::Token;
        pPayload->success = true;
        pPayload->text.assign(rToken);
        m_rJob.postUiEvent(std::move(pPayload));
    }

    void onStreamFinished(bool bSuccess, std::string_view rErrorDetail) override
    {
        auto pPayload = std::make_unique<AiStreamEventPayload>();
        pPayload->kind = AiStreamEventKind::Finished;
        pPayload->success = bSuccess && !m_rJob.m_bCancelled.load();
        if (m_rJob.m_bCancelled.load())
            pPayload->text = "Request cancelled.";
        else
            pPayload->text.assign(rErrorDetail);
        m_rJob.postUiEvent(std::move(pPayload));
    }

private:
    AiAsyncStreamJob& m_rJob;
};

} // namespace

AiAsyncStreamJob::AiAsyncStreamJob() = default;

AiAsyncStreamJob::~AiAsyncStreamJob()
{
    cancel();
}

void AiAsyncStreamJob::start(
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
        WorkerSink aSink(*this);
        AiPromptRequest aEnriched = rRequest;

        if (isRagConfigured())
        {
            postStatusEvent("코드베이스 검색 중...");
            enrichPromptWithRag(aEnriched);
            if (m_bCancelled.load())
                return;
            postStatusEvent(formatRagReadyStatus(aEnriched.ragContext, true));
        }

        (void)postChatCompletionStreaming(rConfig, aEnriched, aSink, &m_bCancelled);

        {
            std::lock_guard aGuard(m_aMutex);
            m_bRunning.store(false);
        }
    });
}

void AiAsyncStreamJob::cancel()
{
    m_bCancelled.store(true);

    std::lock_guard aGuard(m_aMutex);
    joinThreadLocked();
    m_aUiLink = Link<void*, void>();
}

bool AiAsyncStreamJob::isRunning() const
{
    return m_bRunning.load();
}

void AiAsyncStreamJob::joinThreadLocked()
{
    if (m_aThread.joinable())
        m_aThread.join();
}

void AiAsyncStreamJob::postUiEvent(std::unique_ptr<AiStreamEventPayload> pPayload)
{
    if (m_bCancelled.load() && pPayload->kind != AiStreamEventKind::Finished)
        return;

    Link<void*, void> aUiLink;
    {
        std::lock_guard aGuard(m_aMutex);
        aUiLink = m_aUiLink;
    }

    if (!aUiLink.IsSet())
        return;

    Application::PostUserEvent(aUiLink, pPayload.release());
}

void AiAsyncStreamJob::postStatusEvent(std::string_view rText)
{
    auto pPayload = std::make_unique<AiStreamEventPayload>();
    pPayload->kind = AiStreamEventKind::Status;
    pPayload->success = true;
    pPayload->text.assign(rText);
    postUiEvent(std::move(pPayload));
}

} // namespace lofice::ai
