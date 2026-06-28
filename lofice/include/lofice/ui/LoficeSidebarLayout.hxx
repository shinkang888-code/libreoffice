/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * lofice — Sidebar layout descriptors
 *
 * Copyright (c) Lonex. Inc. All Rights Reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_LOFICE_UI_LOFICESIDEBARLAYOUT_HXX
#define INCLUDED_LOFICE_UI_LOFICESIDEBARLAYOUT_HXX

#include <cstdint>
#include <string_view>

#include <rtl/string.hxx>
#include <rtl/ustring.hxx>
#include <sal/types.h>

namespace lofice::ui
{

inline OUString loficeLayoutId(std::string_view sv)
{
    return OStringToOUString(OString(sv.data(), sv.size()), RTL_TEXTENCODING_UTF8);
}

constexpr std::string_view kAiAssistantDeckId = "LoficeAiDeck";
constexpr std::string_view kAiAssistantPanelId = "LoficeAiAssistantPanel";
constexpr std::string_view kAiAssistantPanelResourceSuffix = "/AiAssistantPanel";

struct SidebarWidthConstraints
{
    sal_Int32 nMinPx = 260;
    sal_Int32 nDefaultPx = 340;
    sal_Int32 nMaxPx = 520;
};

enum class AiPanelRegion : std::uint8_t
{
    Header = 0,
    AppContext,
    QuickActionsGrid,
    PromptInput,
    PromptActions,
    ResponseOutput,
    StatusBar
};

struct AiPanelRowWeights
{
    int nHeaderWeight = 0;
    int nQuickActionsWeight = 0;
    int nPromptWeight = 1;
    int nResponseWeight = 2;
    int nStatusWeight = 0;
};

struct LoficeSidebarLayout
{
    SidebarWidthConstraints width;
    AiPanelRowWeights rowWeights;

    static constexpr std::string_view kGridId = "AiAssistantPanel";
    static constexpr std::string_view kTitleLabelId = "label_title";
    static constexpr std::string_view kAppContextLabelId = "label_app_context";
    static constexpr std::string_view kPromptTextId = "text_prompt";
    static constexpr std::string_view kResponseTextId = "text_response";
    static constexpr std::string_view kStatusLabelId = "label_status";
    static constexpr std::string_view kSendButtonId = "btn_send";
    static constexpr std::string_view kCancelAsyncButtonId = "btn_cancel_async";
    static constexpr std::string_view kClearPromptButtonId = "btn_clear_prompt";
    static constexpr std::string_view kClearResponseButtonId = "btn_clear_response";
    static constexpr std::string_view kExportHistoryButtonId = "btn_export_history";
    static constexpr std::string_view kImportHistoryButtonId = "btn_import_history";
    static constexpr std::string_view kToggleSettingsButtonId = "btn_toggle_settings";
    static constexpr std::string_view kSettingsGridId = "grid_settings";
    static constexpr std::string_view kUseBuiltinCheckId = "check_use_builtin";
    static constexpr std::string_view kAutoSendQuickActionCheckId = "check_auto_send_quickaction";
    static constexpr std::string_view kEndpointEntryId = "entry_endpoint";
    static constexpr std::string_view kApiKeyEntryId = "entry_api_key";
    static constexpr std::string_view kModelEntryId = "entry_model";
    static constexpr std::string_view kTimeoutSpinId = "spin_timeout";
    static constexpr std::string_view kUseRagCheckId = "check_use_rag";
    static constexpr std::string_view kRagEndpointEntryId = "entry_rag_endpoint";
    static constexpr std::string_view kRagTimeoutSpinId = "spin_rag_timeout";
    static constexpr std::string_view kTestRagButtonId = "btn_test_rag";
    static constexpr std::string_view kSaveSettingsButtonId = "btn_save_settings";
    static constexpr std::string_view kTestConnectionButtonId = "btn_test_connection";
    static constexpr std::string_view kUiResourcePath
        = "modules/lofice/ui/sidebar_ai_assistant.ui";
};

} // namespace lofice::ui

#endif // INCLUDED_LOFICE_UI_LOFICESIDEBARLAYOUT_HXX
