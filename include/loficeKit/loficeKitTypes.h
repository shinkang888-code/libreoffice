/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the lofice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_loficeKIT_loficeKIT_TYPES_H
#define INCLUDED_loficeKIT_loficeKIT_TYPES_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @see lok::Office::registerCallback().
    @since lofice 6.0
 */
typedef void (*loficeKitCallback)(int nType, const char* pPayload, void* pData);

/** @see lok::Office::runLoop().
    @since lofice 6.3
 */
typedef int (*loficeKitPollCallback)(void* pData, int timeoutUs);
typedef void (*loficeKitWakeCallback)(void* pData);

/// @see lok::Office::registerAnyInputCallback()
typedef bool (*loficeKitAnyInputCallback)(void* pData, int nMostUrgentPriority);

/// @see lok::Office::registerFileSaveDialogCallback()
typedef void (*loficeKitFileSaveDialogCallback)(const char* pSuggestedUri, char* pResultUri,
                                                     size_t nResultUri);

#ifdef __cplusplus
}
#endif

#endif // INCLUDED_loficeKIT_loficeKIT_TYPES_H

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
