/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * lofice — AI conversation persistence (implementation)
 *
 * Copyright (c) Lonex. Inc. All Rights Reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <lofice/ai/AiConversationStore.hxx>

#include <lofice/ai/AiConversationHistory.hxx>
#include <lofice/ai/AiConversationJson.hxx>

#include <comphelper/configurationhelper.hxx>
#include <officecfg/Office/Lofice.hxx>

#include <rtl/textenc.h>
#include <sal/log.hxx>

namespace lofice::ai
{

namespace
{

constexpr std::size_t kMaxPersistBytes = 65536;

} // namespace

void loadConversationHistory(AiConversationHistory& rHistory)
{
    rHistory.clear();

    const OUString aJsonOu = officecfg::Office::Lofice::AiSettings::ChatHistoryJson::get();
    if (aJsonOu.isEmpty())
        return;

    const OString aJson = aJsonOu.toUtf8();
    std::string aError;
    if (!parseConversationJson(
            std::string_view(aJson.getStr(), aJson.getLength()), rHistory, &aError))
    {
        SAL_WARN("lofice.ai", "ChatHistoryJson parse failed: " << aError.c_str());
    }
}

void saveConversationHistory(const AiConversationHistory& rHistory)
{
    std::string aJson = serializeConversationToJson(rHistory);
    if (aJson.size() > kMaxPersistBytes)
    {
        SAL_WARN("lofice.ai", "Chat history exceeds persist limit — truncating save");
        aJson.resize(kMaxPersistBytes);
    }

    const OUString aJsonOu = OStringToOUString(
        OString(aJson.c_str(), static_cast<sal_Int32>(aJson.size()), RTL_TEXTENCODING_UTF8),
        RTL_TEXTENCODING_UTF8);

    auto xBatch = comphelper::ConfigurationChanges::create();
    officecfg::Office::Lofice::AiSettings::ChatHistoryJson::set(aJsonOu, xBatch);
    xBatch->commit();
}

} // namespace lofice::ai
