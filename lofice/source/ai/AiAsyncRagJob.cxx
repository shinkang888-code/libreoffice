/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */

/*

 * lofice — background RAG context fetch job (implementation)

 *

 * Copyright (c) Lonex. Inc. All Rights Reserved.

 *

 * This Source Code Form is subject to the terms of the Mozilla Public

 * License, v. 2.0. If a copy of the MPL was not distributed with this

 * file, You can obtain one at http://mozilla.org/MPL/2.0/.

 */



#include <lofice/ai/AiAsyncRagJob.hxx>



#include <lofice/ai/AiRagContext.hxx>



#include <vcl/svapp.hxx>



namespace lofice::ai

{



AiAsyncRagJob::AiAsyncRagJob() = default;



AiAsyncRagJob::~AiAsyncRagJob()

{

    cancel();

}



void AiAsyncRagJob::start(

    const AiRagSettings& rSettings,

    const std::string& rQueryUtf8,

    const Link<void*, void>& rUiLink)

{

    cancel();



    {

        std::lock_guard aGuard(m_aMutex);

        m_aUiLink = rUiLink;

        m_bCancelled.store(false);

        m_bRunning.store(true);

    }



    m_aThread = std::thread([this, rSettings, rQueryUtf8]() {

        auto pPayload = std::make_unique<AiRagUiPayload>();



        if (m_bCancelled.load())

        {

            pPayload->success = false;

            pPayload->errorDetail = "Request cancelled.";

        }

        else

        {

            pPayload->context = fetchRagContextForSettings(rSettings, rQueryUtf8);

            pPayload->success = !pPayload->context.empty();

            if (!pPayload->success)

            {

                pPayload->errorDetail = m_bCancelled.load()

                    ? "Request cancelled."

                    : "RAG search returned no context.";

            }

        }



        postUiEvent(std::move(pPayload));



        std::lock_guard aGuard(m_aMutex);

        m_bRunning.store(false);

    });

}



void AiAsyncRagJob::cancel()

{

    m_bCancelled.store(true);



    std::lock_guard aGuard(m_aMutex);

    joinThreadLocked();

    m_aUiLink = Link<void*, void>();

}



bool AiAsyncRagJob::isRunning() const

{

    return m_bRunning.load();

}



void AiAsyncRagJob::joinThreadLocked()

{

    if (m_aThread.joinable())

        m_aThread.join();

}



void AiAsyncRagJob::postUiEvent(std::unique_ptr<AiRagUiPayload> pPayload)

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

