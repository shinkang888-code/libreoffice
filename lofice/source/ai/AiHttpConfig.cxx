/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * lofice — AI HTTP configuration (legacy env bridge)
 *
 * Copyright (c) Lonex. Inc. All Rights Reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <lofice/ai/AiHttpConfig.hxx>

#include <lofice/ai/AiSettingsStore.hxx>

namespace lofice::ai
{

AiHttpConfig AiHttpConfig::fromEnvironment()
{
    return loadEffectiveHttpConfig();
}

} // namespace lofice::ai
