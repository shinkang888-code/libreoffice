/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * lofice — Quick action catalog (app-aware UNO commands)
 *
 * Copyright (c) Lonex. Inc. All Rights Reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_LOFICE_UI_QUICKACTIONCATALOG_HXX
#define INCLUDED_LOFICE_UI_QUICKACTIONCATALOG_HXX

#include <cstdint>
#include <string_view>
#include <vector>

#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/uno/Reference.hxx>

namespace lofice::ui
{

enum class LoficeAppKind : std::uint8_t
{
    Generic = 0,
    Writer,
    Calc,
    Impress,
    Draw
};

enum class QuickActionScope : std::uint16_t
{
    Universal = 0x0001,
    Writer    = 0x0002,
    Calc      = 0x0004,
    Impress   = 0x0008,
    Draw      = 0x0010,
};

constexpr QuickActionScope operator|(QuickActionScope a, QuickActionScope b)
{
    return static_cast<QuickActionScope>(
        static_cast<std::uint16_t>(a) | static_cast<std::uint16_t>(b));
}

constexpr bool operator&(QuickActionScope a, QuickActionScope b)
{
    return (static_cast<std::uint16_t>(a) & static_cast<std::uint16_t>(b)) != 0;
}

struct QuickActionDef
{
    std::string_view label;
    std::string_view unoCommand;
    /** UTF-8 AI prompt inserted into the sidebar when the action is clicked. */
    std::string_view aiPrompt;
    QuickActionScope scope;
};

constexpr std::size_t kMaxQuickActionSlots = 16;

/** weld button IDs in sidebar_ai_assistant.ui (4×4 grid). */
constexpr std::string_view kQuickActionSlotWidgetIds[kMaxQuickActionSlots] = {
    "btn_qa_00", "btn_qa_01", "btn_qa_02", "btn_qa_03",
    "btn_qa_10", "btn_qa_11", "btn_qa_12", "btn_qa_13",
    "btn_qa_20", "btn_qa_21", "btn_qa_22", "btn_qa_23",
    "btn_qa_30", "btn_qa_31", "btn_qa_32", "btn_qa_33",
};

LoficeAppKind detectAppKind(
    const css::uno::Reference<css::frame::XFrame>& xFrame);

std::u16string_view appKindDisplayName(LoficeAppKind eKind);

std::vector<QuickActionDef> getQuickActionsForApp(LoficeAppKind eKind);

QuickActionScope scopeForApp(LoficeAppKind eKind);

} // namespace lofice::ui

#endif // INCLUDED_LOFICE_UI_QUICKACTIONCATALOG_HXX
