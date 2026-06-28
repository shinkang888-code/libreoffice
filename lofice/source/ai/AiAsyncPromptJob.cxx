/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * lofice — background AI prompt job (implementation)
 *
 * Copyright (c) Lonex. Inc. All Rights Reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <lofice/ai/AiAsyncPromptJob.hxx>

#include <lofice/ai/AiPromptService.hxx>
#include <lofice/ai/AiRagContext.hxx>

#include <vcl/svapp.hxx>

namespace lofice::ai
{

AiAsyncPromptJob::AiAsyncPromptJob() = default;

AiAsyncPromptJob::~AiAsyncPromptJob()
{
    cancel();
}

void AiAsyncPromptJob::start(const AiPromptRequest& rRequest, const Link<void*, void>& rUiLink)
{
    cancel();

    {
        std::lock_guard aGuard(m_aMutex);
        m_aUiLink = rUiLink;
        m_bCancelled.store(false);
        m_bRunning.store(true);
    }

    m_aThread = std::thread([this, rRequest]() {
        AiPromptRequest aEnriched = rRequest;

        if (isRagConfigured())
        {
            postStatusEvent("코드베이스 검색 중...");
            enrichPromptWithRag(aEnriched);
            if (m_bCancelled.load())
            {
                postStatusEvent("요청 취소됨");
                auto pPayload = std::make_unique<AiStreamEventPayload>();
                pPayload->kind = AiStreamEventKind::Finished;
                pPayload->success = false;
                pPayload->text = "Request cancelled.";
                postUiEvent(std::move(pPayload));
                std::lock_guard aGuard(m_aMutex);
                m_bRunning.store(false);
                return;
            }
            aEnriched.ragPrefetched = true;
            postStatusEvent(formatRagReadyStatus(aEnriched.ragContext, false));
        }
        else
        {
            postStatusEvent("AI 처리 중...");
        }

        const AiPromptResult aResult = processPrompt(aEnriched);

        if (m_bCancelled.load())
        {
            auto pPayload = std::make_unique<AiStreamEventPayload>();
            pPayload->kind = AiStreamEventKind::Finished;
            pPayload->success = false;
            pPayload->text = "Request cancelled.";
            postUiEvent(std::move(pPayload));
        }
        else
        {
            auto pPayload = std::make_unique<AiStreamEventPayload>();
            pPayload->kind = AiStreamEventKind::Finished;
            pPayload->success = aResult.success;
            if (aResult.success)
                pPayload->text = aResult.response;
            else
                pPayload->text = aResult.response.empty() ? "AI 요청 실패" : aResult.response;
            pPayload->provider = aResult.provider;
            postUiEvent(std::move(pPayload));
        }

        {
            std::lock_guard aGuard(m_aMutex);
            m_bRunning.store(false);
        }
    });
}

void AiAsyncPromptJob::cancel()
{
    m_bCancelled.store(true);

    std::lock_guard aGuard(m_aMutex);
    joinThreadLocked();
    m_aUiLink = Link<void*, void>();
}

bool AiAsyncPromptJob::isRunning() const
{
    return m_bRunning.load();
}

void AiAsyncPromptJob::joinThreadLocked()
{
    if (m_aThread.joinable())
        m_aThread.join();
}

void AiAsyncPromptJob::postUiEvent(std::unique_ptr<AiStreamEventPayload> pPayload)
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

void AiAsyncPromptJob::postStatusEvent(std::string_view rText)
{
    auto pPayload = std::make_unique<AiStreamEventPayload>();
    pPayload->kind = AiStreamEventKind::Status;
    pPayload->success = true;
    pPayload->text.assign(rText);
    postUiEvent(std::move(pPayload));
}

} // namespace lofice::ai
