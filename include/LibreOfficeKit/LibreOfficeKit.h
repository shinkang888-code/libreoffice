/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the lofice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_loficeKIT_loficeKIT_H
#define INCLUDED_loficeKIT_loficeKIT_H

#include <stddef.h>

// the unstable API needs C99's bool
// TODO remove the C99 types from the API before making stable
#if defined LOK_USE_UNSTABLE_API || defined LIBO_INTERNAL_ONLY
# ifndef _WIN32
#  include <stdbool.h>
# endif
# include <stdint.h>
#endif

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

#include "loficeKitTypes.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct loficeKitStruct loficeKit;
typedef struct loficeKitClassStruct loficeKitClass;

typedef struct loficeKitDocumentStruct loficeKitDocument;
typedef struct loficeKitDocumentClassStruct loficeKitDocumentClass;

// Do we have an extended member in this struct ?
#define loficeKIT_HAS_MEMBER(strct,member,nSize) \
    (offsetof(strct, member) < (nSize))

#define loficeKIT_HAS(pKit,member) loficeKIT_HAS_MEMBER(loficeKitClass,member,(pKit)->pClass->nSize)

struct loficeKitStruct
{
    loficeKitClass* pClass;
};

struct loficeKitClassStruct
{
    size_t  nSize;

    void (*destroy) (loficeKit* pThis);

    loficeKitDocument* (*documentLoad) (loficeKit* pThis,
                                             const char* pURL);

    char* (*getError) (loficeKit* pThis);

    /// @since lofice 5.0
    loficeKitDocument* (*documentLoadWithOptions) (loficeKit* pThis,
                                                        const char* pURL,
                                                        const char* pOptions);
    /// @since lofice 5.2

    /// The name "freeError" is a historical accident, actually this
    /// is a generic deallocation function for dynamically allocated
    /// memory returned by other loficeKit functions.

    /// Especially on Windows it is important to not call free() in
    /// your own code on a pointer returned from some random other
    /// dynamic library (like the one this code goes into) where it
    /// might have been allocated by calling malloc() (etc) in a C
    /// runtime library that is different from the one used by your
    /// code. That will lead to a crash. Always call the free() in the
    /// same C runtime where the malloc() that allocated the pointer
    /// is.

    void (*freeError) (char* pFree);

    /// @since lofice 6.0
    void (*registerCallback) (loficeKit* pThis,
                              loficeKitCallback pCallback,
                              void* pData);

    /** @see lok::Office::getFilterTypes().
        @since lofice 6.0
     */
    char* (*getFilterTypes) (loficeKit* pThis);

    /** @see lok::Office::setOptionalFeatures().
        @since lofice 6.0
     */
    void (*setOptionalFeatures)(loficeKit* pThis, unsigned long long features);

    /** @see lok::Office::setDocumentPassword().
        @since lofice 6.0
     */
    void (*setDocumentPassword) (loficeKit* pThis,
            char const* pURL,
            char const* pPassword);

    /** @see lok::Office::getVersionInfo().
        @since lofice 6.0
     */
    char* (*getVersionInfo) (loficeKit* pThis);

    /** @see lok::Office::runMacro().
        @since lofice 6.0
     */
    int (*runMacro) (loficeKit *pThis, const char* pURL);

    /** @see lok::Office::signDocument().
        @since lofice 6.2
     */
     bool (*signDocument) (loficeKit* pThis,
                           const char* pUrl,
                           const unsigned char* pCertificateBinary,
                           const int nCertificateBinarySize,
                           const unsigned char* pPrivateKeyBinary,
                           const int nPrivateKeyBinarySize);

    /// @see lok::Office::runLoop()
    void (*runLoop) (loficeKit* pThis,
                     loficeKitPollCallback pPollCallback,
                     loficeKitWakeCallback pWakeCallback,
                     void* pData);

    /// @see lok::Office::setOption
    void (*setOption) (loficeKit* pThis, const char* pOption, const char* pValue);

    /// @see lok::Office::dumpState
    /// @since lofice 7.5
    void (*dumpState) (loficeKit* pThis, const char* pOptions, char** pState);

    /** @see lok::Office::extractRequest.
     */
    char* (*extractRequest) (loficeKit* pThis,
                           const char* pFilePath);

    /// @see lok::Office::trimMemory
    /// @since lofice 7.6
    void (*trimMemory) (loficeKit* pThis, int nTarget);

    /// @see lok::Office::startURP
    void* (*startURP)(loficeKit* pThis,
                    void* pReceiveURPFromLOContext, void* pSendURPToLOContext,
                    int (*fnReceiveURPFromLO)(void* pContext, const signed char* pBuffer, int nLen),
                    int (*fnSendURPToLO)(void* pContext, signed char* pBuffer, int nLen));

    /// @see lok::Office::stopURP
    void (*stopURP)(loficeKit* pThis, void* pSendURPToLOContext);

    /// @see lok::Office::joinThreads
    int (*joinThreads)(loficeKit* pThis);

    /// @see lok::Office::startThreads
    void (*startThreads)(loficeKit* pThis);

    /// @see lok::Office::setForkedChild
    void (*setForkedChild)(loficeKit* pThis, bool bIsChild);

    /** @see lok::Office::extractDocumentStructureRequest.
     */
    char* (*extractDocumentStructureRequest)(loficeKit* pThis, const char* pFilePath,
                                             const char* pFilter);

    /// @see lok::Office::registerAnyInputCallback()
    void (*registerAnyInputCallback)(loficeKit* pThis,
                                     loficeKitAnyInputCallback pCallback, void* pData);

    /// @see lok::Office::getDocsCount().
    int (*getDocsCount) (loficeKit* pThis);

    /// @see lok::Office::registerFileSaveDialogCallback()
    void (*registerFileSaveDialogCallback)(loficeKit* pThis,
            loficeKitFileSaveDialogCallback pCallback);
};

#define loficeKIT_DOCUMENT_HAS(pDoc,member) loficeKIT_HAS_MEMBER(loficeKitDocumentClass,member,(pDoc)->pClass->nSize)

struct loficeKitDocumentStruct
{
    loficeKitDocumentClass* pClass;
};

struct loficeKitDocumentClassStruct
{
    size_t  nSize;

    void (*destroy) (loficeKitDocument* pThis);

    int (*saveAs) (loficeKitDocument* pThis,
                   const char* pUrl,
                   const char* pFormat,
                   const char* pFilterOptions);

    /** @see lok::Document::getDocumentType().
        @since lofice 6.0
     */
    int (*getDocumentType) (loficeKitDocument* pThis);

#if defined LOK_USE_UNSTABLE_API || defined LIBO_INTERNAL_ONLY
    /// @see lok::Document::getParts().
    int (*getParts) (loficeKitDocument* pThis);

    /// @see lok::Document::getPartPageRectangles().
    char* (*getPartPageRectangles) (loficeKitDocument* pThis);

    /// @see lok::Document::getPart().
    int (*getPart) (loficeKitDocument* pThis);

    /// @see lok::Document::setPart().
    void (*setPart) (loficeKitDocument* pThis,
                     int nPart);

    /// @see lok::Document::getPartName().
    char* (*getPartName) (loficeKitDocument* pThis,
                          int nPart);

    /// @see lok::Document::setPartMode().
    void (*setPartMode) (loficeKitDocument* pThis,
                         int nMode);

    /// @see lok::Document::paintTile().
    void (*paintTile) (loficeKitDocument* pThis,
                       unsigned char* pBuffer,
                       const int nCanvasWidth,
                       const int nCanvasHeight,
                       const int nTilePosX,
                       const int nTilePosY,
                       const int nTileWidth,
                       const int nTileHeight);

    /// @see lok::Document::getTileMode().
    int (*getTileMode) (loficeKitDocument* pThis);

    /// @see lok::Document::getDocumentSize().
    void (*getDocumentSize) (loficeKitDocument* pThis,
                             long* pWidth,
                             long* pHeight);

    /// @see lok::Document::initializeForRendering().
    void (*initializeForRendering) (loficeKitDocument* pThis,
                                    const char* pArguments);

    /// @see lok::Document::registerCallback().
    void (*registerCallback) (loficeKitDocument* pThis,
                              loficeKitCallback pCallback,
                              void* pData);

    /// @see lok::Document::postKeyEvent
    void (*postKeyEvent) (loficeKitDocument* pThis,
                          int nType,
                          int nCharCode,
                          int nKeyCode);

    /// @see lok::Document::postMouseEvent
    void (*postMouseEvent) (loficeKitDocument* pThis,
                            int nType,
                            int nX,
                            int nY,
                            int nCount,
                            int nButtons,
                            int nModifier);

    /// @see lok::Document::postUnoCommand
    void (*postUnoCommand) (loficeKitDocument* pThis,
                            const char* pCommand,
                            const char* pArguments,
                            bool bNotifyWhenFinished);

    /// @see lok::Document::setTextSelection
    void (*setTextSelection) (loficeKitDocument* pThis,
                              int nType,
                              int nX,
                              int nY);

    /// @see lok::Document::getTextSelection
    char* (*getTextSelection) (loficeKitDocument* pThis,
                               const char* pMimeType,
                               char** pUsedMimeType);

    /// @see lok::Document::paste().
    bool (*paste) (loficeKitDocument* pThis,
                   const char* pMimeType,
                   const char* pData,
                   size_t nSize);

    /// @see lok::Document::setGraphicSelection
    void (*setGraphicSelection) (loficeKitDocument* pThis,
                                 int nType,
                                 int nX,
                                 int nY);

    /// @see lok::Document::resetSelection
    void (*resetSelection) (loficeKitDocument* pThis);

    /// @see lok::Document::getCommandValues().
    char* (*getCommandValues) (loficeKitDocument* pThis, const char* pCommand);

    /// @see lok::Document::setClientZoom().
    void (*setClientZoom) (loficeKitDocument* pThis,
            int nTilePixelWidth,
            int nTilePixelHeight,
            int nTileTwipWidth,
            int nTileTwipHeight);

    /// @see lok::Document::setVisibleArea).
    void (*setClientVisibleArea) (loficeKitDocument* pThis, int nX, int nY, int nWidth, int nHeight);

    /// @see lok::Document::createView().
    int (*createView) (loficeKitDocument* pThis);
    /// @see lok::Document::destroyView().
    void (*destroyView) (loficeKitDocument* pThis, int nId);
    /// @see lok::Document::setView().
    void (*setView) (loficeKitDocument* pThis, int nId);
    /// @see lok::Document::getView().
    int (*getView) (loficeKitDocument* pThis);
    /// @see lok::Document::getViewsCount().
    int (*getViewsCount) (loficeKitDocument* pThis);

    /// @see lok::Document::renderFont().
    unsigned char* (*renderFont) (loficeKitDocument* pThis,
                       const char* pFontName,
                       const char* pChar,
                       int* pFontWidth,
                       int* pFontHeight);

    /// @see lok::Document::getPartHash().
    char* (*getPartHash) (loficeKitDocument* pThis,
                          int nPart);

    /// Paints a tile from a specific part.
    /// @see lok::Document::paintTile().
    void (*paintPartTile) (loficeKitDocument* pThis,
                           unsigned char* pBuffer,
                           const int nPart,
                           const int nMode,
                           const int nCanvasWidth,
                           const int nCanvasHeight,
                           const int nTilePosX,
                           const int nTilePosY,
                           const int nTileWidth,
                           const int nTileHeight);

    /// @see lok::Document::getViewIds().
    bool (*getViewIds) (loficeKitDocument* pThis,
                       int* pArray,
                       size_t nSize);

    /// @see lok::Document::setOutlineState).
    void (*setOutlineState) (loficeKitDocument* pThis, bool bColumn, int nLevel, int nIndex, bool bHidden);

    /// Paints window with given id to the buffer
    /// @see lok::Document::paintWindow().
    void (*paintWindow) (loficeKitDocument* pThis, unsigned nWindowId,
                         unsigned char* pBuffer,
                         const int x, const int y,
                         const int width, const int height);

    /// @see lok::Document::postWindow().
    void (*postWindow) (loficeKitDocument* pThis, unsigned nWindowId, int nAction, const char* pData);

    /// @see lok::Document::postWindowKeyEvent().
    void (*postWindowKeyEvent) (loficeKitDocument* pThis,
                                unsigned nWindowId,
                                int nType,
                                int nCharCode,
                                int nKeyCode);

    /// @see lok::Document::postWindowMouseEvent().
    void (*postWindowMouseEvent) (loficeKitDocument* pThis,
                                  unsigned nWindowId,
                                  int nType,
                                  int nX,
                                  int nY,
                                  int nCount,
                                  int nButtons,
                                  int nModifier);

    /// @see lok::Document::setViewLanguage().
    void (*setViewLanguage) (loficeKitDocument* pThis, int nId, const char* language);

    /// @see lok::Document::postWindowExtTextInputEvent
    void (*postWindowExtTextInputEvent) (loficeKitDocument* pThis,
                                         unsigned nWindowId,
                                         int nType,
                                         const char* pText);

    /// @see lok::Document::getPartInfo().
    char* (*getPartInfo) (loficeKitDocument* pThis, int nPart);

    /// Paints window with given id to the buffer with the give DPI scale
    /// (every pixel is dpiscale-times larger).
    /// @see lok::Document::paintWindow().
    void (*paintWindowDPI) (loficeKitDocument* pThis, unsigned nWindowId,
                            unsigned char* pBuffer,
                            const int x, const int y,
                            const int width, const int height,
                            const double dpiscale);

// CERTIFICATE AND SIGNING

    /// @see lok::Document::insertCertificate().
    bool (*insertCertificate) (loficeKitDocument* pThis,
                                const unsigned char* pCertificateBinary,
                                const int nCertificateBinarySize,
                                const unsigned char* pPrivateKeyBinary,
                                const int nPrivateKeyBinarySize);

    /// @see lok::Document::addCertificate().
    bool (*addCertificate) (loficeKitDocument* pThis,
                                const unsigned char* pCertificateBinary,
                                const int nCertificateBinarySize);

    /// @see lok::Document::getSignatureState().
    int (*getSignatureState) (loficeKitDocument* pThis);
// END CERTIFICATE AND SIGNING

    /// @see lok::Document::renderShapeSelection
    size_t (*renderShapeSelection)(loficeKitDocument* pThis, char** pOutput);

    /// @see lok::Document::postWindowGestureEvent().
    void (*postWindowGestureEvent) (loficeKitDocument* pThis,
                                  unsigned nWindowId,
                                  const char* pType,
                                  int nX,
                                  int nY,
                                  int nOffset);

    /// @see lok::Document::createViewWithOptions().
    int (*createViewWithOptions) (loficeKitDocument* pThis, const char* pOptions);

    /// @see lok::Document::selectPart().
    void (*selectPart) (loficeKitDocument* pThis, int nPart, int nSelect);

    /// @see lok::Document::moveSelectedParts().
    void (*moveSelectedParts) (loficeKitDocument* pThis, int nPosition, bool bDuplicate);

    /// Resize window with given id.
    /// @see lok::Document::resizeWindow().
    void (*resizeWindow) (loficeKitDocument* pThis, unsigned nWindowId,
                          const int width, const int height);

    /// Pass a nullptr terminated array of mime-type strings
    /// @see lok::Document::getClipboard for more details
    int (*getClipboard) (loficeKitDocument* pThis,
                         const char **pMimeTypes,
                         size_t      *pOutCount,
                         char      ***pOutMimeTypes,
                         size_t     **pOutSizes,
                         char      ***pOutStreams);

    /// @see lok::Document::setClipboard
    int (*setClipboard) (loficeKitDocument* pThis,
                         const size_t   nInCount,
                         const char   **pInMimeTypes,
                         const size_t  *pInSizes,
                         const char   **pInStreams);

    /// @see lok::Document::getSelectionType
    int (*getSelectionType) (loficeKitDocument* pThis);

    /// @see lok::Document::removeTextContext
    void (*removeTextContext) (loficeKitDocument* pThis,
                               unsigned nWindowId,
                               int nBefore,
                               int nAfter);

    /// @see lok::Document::renderFontOrientation().
    unsigned char* (*renderFontOrientation) (loficeKitDocument* pThis,
                       const char* pFontName,
                       const char* pChar,
                       int* pFontWidth,
                       int* pFontHeight,
                       int pOrientation);

    /// Switches view to viewId if viewId >= 0, and paints window
    /// @see lok::Document::paintWindowDPI().
    void (*paintWindowForView) (loficeKitDocument* pThis, unsigned nWindowId,
                                unsigned char* pBuffer,
                                const int x, const int y,
                                const int width, const int height,
                                const double dpiscale,
                                int viewId);

    /// @see lok::Document::completeFunction().
    void (*completeFunction) (loficeKitDocument* pThis, const char* pFunctionName);

    /// @see lok::Document::setWindowTextSelection
    void (*setWindowTextSelection) (loficeKitDocument* pThis,
                                    unsigned nWindowId,
                                    bool bSwap,
                                    int nX,
                                    int nY);

    /// @see lok::Document::sendFormFieldEvent
    void (*sendFormFieldEvent) (loficeKitDocument* pThis,
                                const char* pArguments);

    /// @see lok::Document::setBlockedCommandList
    void (*setBlockedCommandList) (loficeKitDocument* pThis,
                                int nViewId,
                                const char* blockedCommandList);

    /// @see lok::Document::renderSearchResult
    bool (*renderSearchResult) (loficeKitDocument* pThis,
                                const char* pSearchResult,
                                unsigned char** pBitmapBuffer,
                                int* pWidth, int* pHeight, size_t* pByteSize);

    /// @see lok::Document::sendContentControlEvent().
    void (*sendContentControlEvent)(loficeKitDocument* pThis, const char* pArguments);

    /// @see lok::Document::getSelectionTypeAndText
    /// @since lofice 7.4
    int (*getSelectionTypeAndText) (loficeKitDocument* pThis,
                                    const char* pMimeType,
                                    char** pText,
                                    char** pUsedMimeType);

    /// @see lok::Document::getDataArea().
    void (*getDataArea) (loficeKitDocument* pThis,
                         long nPart,
                         long* pCol,
                         long* pRow);

    /// @see lok::Document::getEditMode().
    int (*getEditMode) (loficeKitDocument* pThis);

    /// @see lok::Document::setViewTimezone().
    void (*setViewTimezone) (loficeKitDocument* pThis, int nId, const char* timezone);

    /// @see lok::Document::setAccessibilityState().
    void (*setAccessibilityState) (loficeKitDocument* pThis, int nId, bool nEnabled);

    /// @see lok::Document::getA11yFocusedParagraph.
    char* (*getA11yFocusedParagraph) (loficeKitDocument* pThis);

    /// @see lok::Document::getA11yCaretPosition.
    int (*getA11yCaretPosition) (loficeKitDocument* pThis);

    /// @see lok::Document::setViewReadOnly().
    void (*setViewReadOnly) (loficeKitDocument* pThis, int nId, const bool readOnly);

    /// @see lok::Document::getPresentationInfo
    char* (*getPresentationInfo) (loficeKitDocument* pThis);

    /// @see lok::Document::createSlideRenderer
    bool (*createSlideRenderer) (
        loficeKitDocument* pThis,
        const char* pSlideHash,
        int nSlideNumber, unsigned* nViewWidth, unsigned* nViewHeight,
        bool bRenderBackground, bool bRenderMasterPage);

    /// @see lok::Document::postSlideshowCleanup
    void (*postSlideshowCleanup)(loficeKitDocument* pThis);

    /// @see lok::Document::renderNextSlideLayer
    bool (*renderNextSlideLayer)(
        loficeKitDocument* pThis, unsigned char* pBuffer, bool* bIsBitmapLayer, double* pScale, char** pJsonMessage);

    /// @see lok::Document::setViewOption
    void (*setViewOption)(loficeKitDocument* pThis, const char* pOption, const char* pValue);

    /// @see lok::Document::setColorPreviewState().
    void (*setColorPreviewState) (loficeKitDocument* pThis, int nId, bool nEnabled);

#endif // defined LOK_USE_UNSTABLE_API || defined LIBO_INTERNAL_ONLY
};

#ifdef __cplusplus
}
#endif

#endif // INCLUDED_loficeKIT_loficeKIT_H

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
