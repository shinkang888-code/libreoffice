/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */

/*

 * lofice — background RAG context fetch job

 *

 * Copyright (c) Lonex. Inc. All Rights Reserved.

 *

 * This Source Code Form is subject to the terms of the Mozilla Public

 * License, v. 2.0. If a copy of the MPL was not distributed with this

 * file, You can obtain one at http://mozilla.org/MPL/2.0/.

 */



#ifndef INCLUDED_LOFICE_AI_AIASYNCRAGJOB_HXX

#define INCLUDED_LOFICE_AI_AIASYNCRAGJOB_HXX



#include <lofice/ai/AiRagContext.hxx>



#include <atomic>

#include <memory>

#include <mutex>

#include <string>

#include <thread>



#include <vcl/vcllink.hxx>



namespace lofice::ai

{



/** Heap payload for async RAG test UI updates. */

struct AiRagUiPayload

{

    bool success = false;

    std::string context;

    std::string errorDetail;

};



/** Runs fetchRagContextForSettings on a worker thread. */

class AiAsyncRagJob final

{

public:

    AiAsyncRagJob();

    ~AiAsyncRagJob();



    AiAsyncRagJob(const AiAsyncRagJob&) = delete;

    AiAsyncRagJob& operator=(const AiAsyncRagJob&) = delete;



    void start(

        const AiRagSettings& rSettings,

        const std::string& rQueryUtf8,

        const Link<void*, void>& rUiLink);



    void cancel();

    bool isRunning() const;



private:

    void joinThreadLocked();

    void postUiEvent(std::unique_ptr<AiRagUiPayload> pPayload);



    mutable std::mutex m_aMutex;

    std::thread m_aThread;

    std::atomic<bool> m_bCancelled{ false };

    std::atomic<bool> m_bRunning{ false };

    Link<void*, void> m_aUiLink;

};



} // namespace lofice::ai



#endif // INCLUDED_LOFICE_AI_AIASYNCRAGJOB_HXX

