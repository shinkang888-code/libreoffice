/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * lofice — background AI prompt job (builtin / non-streaming HTTP)
 *
 * Copyright (c) Lonex. Inc. All Rights Reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_LOFICE_AI_AIASYNCPROMPTJOB_HXX
#define INCLUDED_LOFICE_AI_AIASYNCPROMPTJOB_HXX

#include <lofice/ai/AiAsyncStreamJob.hxx>
#include <lofice/ai/AiPromptService.hxx>

#include <atomic>
#include <memory>
#include <mutex>
#include <string_view>
#include <thread>

#include <tools/link.hxx>

namespace lofice::ai
{

/** Runs processPrompt on a worker thread; delivers status + finished events on the main loop. */
class AiAsyncPromptJob final
{
public:
    AiAsyncPromptJob();
    ~AiAsyncPromptJob();

    AiAsyncPromptJob(const AiAsyncPromptJob&) = delete;
    AiAsyncPromptJob& operator=(const AiAsyncPromptJob&) = delete;

    void start(const AiPromptRequest& rRequest, const Link<void*, void>& rUiLink);

    void cancel();
    bool isRunning() const;

private:
    void joinThreadLocked();
    void postUiEvent(std::unique_ptr<AiStreamEventPayload> pPayload);
    void postStatusEvent(std::string_view rText);

    mutable std::mutex m_aMutex;
    std::thread m_aThread;
    std::atomic<bool> m_bCancelled{ false };
    std::atomic<bool> m_bRunning{ false };
    Link<void*, void> m_aUiLink;
};

} // namespace lofice::ai

#endif // INCLUDED_LOFICE_AI_AIASYNCPROMPTJOB_HXX
