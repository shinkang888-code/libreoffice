/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */

/*

 * lofice — background AI SSE streaming job

 *

 * Copyright (c) Lonex. Inc. All Rights Reserved.

 *

 * This Source Code Form is subject to the terms of the Mozilla Public

 * License, v. 2.0. If a copy of the MPL was not distributed with this

 * file, You can obtain one at http://mozilla.org/MPL/2.0/.

 */



#ifndef INCLUDED_LOFICE_AI_AIASYNCSTREAMJOB_HXX

#define INCLUDED_LOFICE_AI_AIASYNCSTREAMJOB_HXX



#include <lofice/ai/AiHttpConfig.hxx>

#include <lofice/ai/AiPromptService.hxx>



#include <atomic>

#include <memory>

#include <mutex>

#include <string>

#include <string_view>

#include <thread>



#include <tools/link.hxx>



namespace lofice::ai

{



enum class AiStreamEventKind

{

    Status,

    Token,

    Finished,

};



/** Heap payload for Application::PostUserEvent — ownership transferred to UI handler. */

struct AiStreamEventPayload

{

    AiStreamEventKind kind = AiStreamEventKind::Token;

    bool success = false;

    std::string text;

    /** AiPromptResult::provider when kind == Finished (async prompt job). */

    std::string provider;

};



/** Runs postChatCompletionStreaming on a worker thread; delivers UI events on the main loop. */

class AiAsyncStreamJob final

{

public:

    AiAsyncStreamJob();

    ~AiAsyncStreamJob();



    AiAsyncStreamJob(const AiAsyncStreamJob&) = delete;

    AiAsyncStreamJob& operator=(const AiAsyncStreamJob&) = delete;



    void start(

        const AiHttpConfig& rConfig,

        const AiPromptRequest& rRequest,

        const Link<void*, void>& rUiLink);



    void cancel();

    bool isRunning() const;



private:

    class WorkerSink final;



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



#endif // INCLUDED_LOFICE_AI_AIASYNCSTREAMJOB_HXX

