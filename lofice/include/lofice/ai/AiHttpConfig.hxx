/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * lofice — AI HTTP configuration (environment)
 *
 * Copyright (c) Lonex. Inc. All Rights Reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_LOFICE_AI_AIHTTPCONFIG_HXX
#define INCLUDED_LOFICE_AI_AIHTTPCONFIG_HXX

#include <string>

namespace lofice::ai
{

struct AiHttpConfig
{
    std::string endpoint;
    std::string apiKey;
    std::string model = "lofice-default";
    long timeoutSeconds = 30;

    static AiHttpConfig fromEnvironment();

    bool isValid() const { return !endpoint.empty(); }
};

} // namespace lofice::ai

#endif // INCLUDED_LOFICE_AI_AIHTTPCONFIG_HXX
