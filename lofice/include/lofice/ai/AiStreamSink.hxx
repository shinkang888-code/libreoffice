/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * lofice — AI streaming token sink
 *
 * Copyright (c) Lonex. Inc. All Rights Reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_LOFICE_AI_AISTREAMSINK_HXX
#define INCLUDED_LOFICE_AI_AISTREAMSINK_HXX

#include <string>
#include <string_view>

namespace lofice::ai
{

/** Receives incremental tokens during SSE chat completion. */
class AiStreamSink
{
public:
    virtual ~AiStreamSink() = default;

    virtual void onStreamToken(std::string_view rToken) = 0;

    virtual void onStreamFinished(bool bSuccess, std::string_view rErrorDetail) = 0;
};

} // namespace lofice::ai

#endif // INCLUDED_LOFICE_AI_AISTREAMSINK_HXX
