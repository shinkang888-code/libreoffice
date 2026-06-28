/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * lofice — background AI HTTP request job (non-streaming)
 *
 * Copyright (c) Lonex. Inc. All Rights Reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_LOFICE_AI_AIASYNCHTTPJOB_HXX
#define INCLUDED_LOFICE_AI_AIASYNCHTTPJOB_HXX

#include <lofice/ai/AiHttpConfig.hxx>
#include <lofice/ai/AiPromptService.hxx>

#include <atomic>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

#include <tools/link.hxx>

namespace lofice::ai
{

/** Heap payload for connection test / non-streaming HTTP UI updates. */
struct AiHttpUiPayload
{
    bool success = false;
    std::string content;
    std::string errorDetail;
};

/** Runs postChatCompletion on a worker thread; delivers result on the main loop. */
class AiAsyncHttpJob final
{
public:
    AiAsyncHttpJob();
    ~AiAsyncHttpJob();

    AiAsyncHttpJob(const AiAsyncHttpJob&) = delete;
    AiAsyncHttpJob& operator=(const AiAsyncHttpJob&) = delete;

    void start(
        const AiHttpConfig& rConfig,
        const AiPromptRequest& rRequest,
        const Link<void*, void>& rUiLink);

    void cancel();
    bool isRunning() const;

private:
    void joinThreadLocked();
    void postUiEvent(std::unique_ptr<AiHttpUiPayload> pPayload);

    mutable std::mutex m_aMutex;
    std::thread m_aThread;
    std::atomic<bool> m_bCancelled{ false };
    std::atomic<bool> m_bRunning{ false };
    Link<void*, void> m_aUiLink;
};

} // namespace lofice::ai

#endif // INCLUDED_LOFICE_AI_AIASYNCHTTPJOB_HXX
