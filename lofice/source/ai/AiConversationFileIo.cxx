/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * lofice — AI conversation JSON file export/import (implementation)
 *
 * Copyright (c) Lonex. Inc. All Rights Reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <lofice/ai/AiConversationFileIo.hxx>

#include <lofice/ai/AiConversationHistory.hxx>
#include <lofice/ai/AiConversationJson.hxx>

#include <com/sun/star/io/SequenceInputStream.hpp>
#include <com/sun/star/io/XInputStream.hpp>
#include <com/sun/star/ui/dialogs/TemplateDescription.hpp>

#include <comphelper/errcode.hxx>
#include <comphelper/processfactory.hxx>
#include <sfx2/filedlghelper.hxx>
#include <ucbhelper/content.hxx>

#include <sal/log.hxx>
#include <tools/urlobj.hxx>

using namespace css;

namespace lofice::ai
{

namespace
{

std::string readTextFromUrl(std::u16string_view rUrl)
{
    ucbhelper::Content aContent(
        OUString(rUrl),
        uno::Reference<ucb::XCommandEnvironment>(),
        comphelper::getProcessComponentContext());

    uno::Reference<io::XInputStream> xStream = aContent.openStream();
    if (!xStream.is())
        return {};

    std::string aOut;
    uno::Sequence<sal_Int8> aChunk;
    sal_Int32 nRead = xStream->readBytes(aChunk, 4096);
    while (nRead > 0)
    {
        aOut.append(reinterpret_cast<char const*>(aChunk.getConstArray()), nRead);
        nRead = xStream->readBytes(aChunk, 4096);
    }
    return aOut;
}

bool writeTextToUrl(std::u16string_view rUrl, std::string_view rUtf8)
{
    ucbhelper::Content aContent(
        OUString(rUrl),
        uno::Reference<ucb::XCommandEnvironment>(),
        comphelper::getProcessComponentContext());

    uno::Sequence<sal_Int8> aBytes(
        reinterpret_cast<sal_Int8 const*>(rUtf8.data()),
        static_cast<sal_Int32>(rUtf8.size()));
    uno::Reference<io::XInputStream> xInput
        = io::SequenceInputStream::createStreamFromSequence(
            comphelper::getProcessComponentContext(), aBytes);
    aContent.writeStream(xInput, true);
    return true;
}

ConversationFileOutcome pickFileUrl(
    weld::Window* pParent,
    sal_Int16 nDialogType,
    OUString& rOutUrl)
{
    ConversationFileOutcome aOutcome;
    sfx2::FileDialogHelper aDialog(nDialogType, FileDialogFlags::NONE, pParent);
    aDialog.SetTitle(nDialogType == ui::dialogs::TemplateDescription::FILESAVE_AUTOEXTENSION
        ? u"lofice AI 대화 내보내기"_ustr
        : u"lofice AI 대화 가져오기"_ustr);
    aDialog.AddFilter(u"JSON (*.json)"_ustr, u"*.json"_ustr);
    aDialog.SetCurrentFilter(u"JSON (*.json)"_ustr);
    if (nDialogType == ui::dialogs::TemplateDescription::FILESAVE_AUTOEXTENSION)
        aDialog.SetFileName(u"lofice-chat-history.json"_ustr);

    if (aDialog.Execute() != ERRCODE_NONE)
    {
        aOutcome.result = ConversationFileResult::Cancelled;
        return aOutcome;
    }

    const uno::Sequence<OUString> aFiles = aDialog.GetSelectedFiles();
    if (!aFiles.hasElements())
    {
        aOutcome.result = ConversationFileResult::Cancelled;
        return aOutcome;
    }

    rOutUrl = aFiles[0];
    aOutcome.result = ConversationFileResult::Ok;
    return aOutcome;
}

} // namespace

ConversationFileOutcome exportConversationToFile(
    weld::Window* pParent,
    const AiConversationHistory& rHistory)
{
    ConversationFileOutcome aOutcome;
    OUString aUrl;
    aOutcome = pickFileUrl(
        pParent, ui::dialogs::TemplateDescription::FILESAVE_AUTOEXTENSION, aUrl);
    if (aOutcome.result != ConversationFileResult::Ok)
        return aOutcome;

    const std::string aJson = serializeConversationFileEnvelope(rHistory);
    if (!writeTextToUrl(aUrl, aJson))
    {
        aOutcome.result = ConversationFileResult::Error;
        aOutcome.errorDetail = "Failed to write export file.";
        SAL_WARN("lofice.ai", "exportConversationToFile write failed: " << aUrl);
    }
    return aOutcome;
}

ConversationFileOutcome importConversationFromFile(
    weld::Window* pParent,
    AiConversationHistory& rHistory)
{
    ConversationFileOutcome aOutcome;
    OUString aUrl;
    aOutcome = pickFileUrl(
        pParent, ui::dialogs::TemplateDescription::FILEOPEN_SIMPLE, aUrl);
    if (aOutcome.result != ConversationFileResult::Ok)
        return aOutcome;

    const std::string aJson = readTextFromUrl(aUrl);
    if (aJson.empty())
    {
        aOutcome.result = ConversationFileResult::Error;
        aOutcome.errorDetail = "Failed to read import file.";
        return aOutcome;
    }

    if (!parseConversationJson(aJson, rHistory, &aOutcome.errorDetail))
    {
        aOutcome.result = ConversationFileResult::Error;
        return aOutcome;
    }

    aOutcome.result = ConversationFileResult::Ok;
    return aOutcome;
}

} // namespace lofice::ai
