/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * lofice — Quick action catalog (implementation)
 *
 * Copyright (c) Lonex. Inc. All Rights Reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <lofice/ui/QuickActionCatalog.hxx>

#include <comphelper/processfactory.hxx>

#include <com/sun/star/frame/ModuleManager.hpp>
#include <com/sun/star/frame/XModuleManager2.hpp>

using namespace css;

namespace lofice::ui
{

namespace
{

constexpr QuickActionDef kCatalog[] = {
    { "Bold", ".uno:Bold",
      u8"현재 문서에서 선택한 텍스트에 굵게(Bold) 서식을 적용하는 방법을 단계별로 설명해 주세요.",
      QuickActionScope::Universal },
    { "Italic", ".uno:Italic",
      u8"선택 영역에 기울임(Italic) 서식을 적용하는 방법과 단축키를 알려 주세요.",
      QuickActionScope::Universal },
    { "Under", ".uno:Underline",
      u8"텍스트에 밑줄(Underline)을 넣는 방법과 스타일 옵션을 설명해 주세요.",
      QuickActionScope::Universal },
    { "Strike", ".uno:Strikeout",
      u8"취소선(Strikethrough) 서식을 적용·해제하는 방법을 알려 주세요.",
      QuickActionScope::Universal },
    { "Undo", ".uno:Undo",
      u8"방금 실행한 작업을 실행 취소(Undo)하는 방법과 되돌릴 수 있는 범위를 설명해 주세요.",
      QuickActionScope::Universal },
    { "Redo", ".uno:Redo",
      u8"실행 취소한 작업을 다시 실행(Redo)하는 방법을 알려 주세요.",
      QuickActionScope::Universal },
    { "Cut", ".uno:Cut",
      u8"선택한 내용을 잘라내기(Cut)하고 다른 위치에 붙여넣는 방법을 설명해 주세요.",
      QuickActionScope::Universal },
    { "Copy", ".uno:Copy",
      u8"선택 영역을 복사(Copy)해 다른 문서나 앱에 붙여넣는 방법을 알려 주세요.",
      QuickActionScope::Universal },
    { "Paste", ".uno:Paste",
      u8"클립보드 내용을 현재 커서 위치에 붙여넣기(Paste)하는 방법과 서식 유지 옵션을 설명해 주세요.",
      QuickActionScope::Universal },
    { "Save", ".uno:Save",
      u8"현재 문서를 저장(Save)하는 방법과 다른 이름으로 저장·자동 저장 설정을 알려 주세요.",
      QuickActionScope::Universal },
    { "Find", ".uno:SearchDialog",
      u8"문서에서 텍스트를 찾기(Find)하고 바꾸기(Replace)하는 방법을 단계별로 설명해 주세요.",
      QuickActionScope::Universal },
    { "Print", ".uno:Print",
      u8"현재 문서를 인쇄(Print)하는 방법과 페이지 범위·프린터 설정을 알려 주세요.",
      QuickActionScope::Universal },
    { "PDF", ".uno:ExportToPDF",
      u8"문서를 PDF로 내보내는(Export to PDF) 방법과 품질·보안 옵션을 설명해 주세요.",
      QuickActionScope::Universal },
    { "Spell", ".uno:SpellingDialog",
      u8"Writer에서 맞춤법 검사(Spelling)를 실행하고 제안을 적용하는 방법을 알려 주세요.",
      QuickActionScope::Writer },
    { "Table", ".uno:InsertTable",
      u8"Writer에 표(Table)를 삽입하고 행·열을 조정하는 방법을 설명해 주세요.",
      QuickActionScope::Writer },
    { "Recalc", ".uno:Calculate",
      u8"Calc에서 전체 시트를 다시 계산(Recalculate)하는 방법과 자동/수동 계산 모드를 알려 주세요.",
      QuickActionScope::Calc },
    { "AutoSum", ".uno:AutoSum",
      u8"Calc에서 선택한 셀 범위에 자동 합계(AutoSum)를 넣는 방법을 설명해 주세요.",
      QuickActionScope::Calc },
    { "NewSlide", ".uno:InsertPageQuick",
      u8"Impress/Draw에서 새 슬라이드 또는 페이지를 빠르게 추가하는 방법을 알려 주세요.",
      QuickActionScope::Impress | QuickActionScope::Draw },
    { "Slideshow", ".uno:Presentation",
      u8"프레젠테이션 슬라이드쇼(Slideshow)를 시작하고 발표자 보기를 사용하는 방법을 설명해 주세요.",
      QuickActionScope::Impress | QuickActionScope::Draw },
    { "Sidebar", ".uno:Sidebar",
      u8"LibreOffice 사이드바를 열고 AI 어시스턴트 패널을 사용하는 방법을 알려 주세요.",
      QuickActionScope::Universal },
};

LoficeAppKind moduleIdToAppKind(std::u16string_view rModuleId)
{
    if (rModuleId.starts_with(u"com.sun.star.text."))
        return LoficeAppKind::Writer;
    if (rModuleId.starts_with(u"com.sun.star.sheet."))
        return LoficeAppKind::Calc;
    if (rModuleId.starts_with(u"com.sun.star.presentation."))
        return LoficeAppKind::Impress;
    if (rModuleId.starts_with(u"com.sun.star.drawing."))
        return LoficeAppKind::Draw;
    return LoficeAppKind::Generic;
}

} // namespace

LoficeAppKind detectAppKind(const uno::Reference<frame::XFrame>& xFrame)
{
    if (!xFrame.is())
        return LoficeAppKind::Generic;

    try
    {
        uno::Reference<frame::XModuleManager2> xModuleManager = frame::ModuleManager::create(
            comphelper::getProcessComponentContext());
        if (!xModuleManager.is())
            return LoficeAppKind::Generic;
        return moduleIdToAppKind(xModuleManager->identify(xFrame));
    }
    catch (...)
    {
        return LoficeAppKind::Generic;
    }
}

std::u16string_view appKindDisplayName(LoficeAppKind eKind)
{
    switch (eKind)
    {
        case LoficeAppKind::Writer:   return u"Writer";
        case LoficeAppKind::Calc:     return u"Calc";
        case LoficeAppKind::Impress:  return u"Impress";
        case LoficeAppKind::Draw:     return u"Draw";
        default:                      return u"Universal";
    }
}

QuickActionScope scopeForApp(LoficeAppKind eKind)
{
    switch (eKind)
    {
        case LoficeAppKind::Writer:   return QuickActionScope::Writer;
        case LoficeAppKind::Calc:     return QuickActionScope::Calc;
        case LoficeAppKind::Impress:  return QuickActionScope::Impress;
        case LoficeAppKind::Draw:     return QuickActionScope::Draw;
        default:                      return QuickActionScope::Universal;
    }
}

std::vector<QuickActionDef> getQuickActionsForApp(LoficeAppKind eKind)
{
    const QuickActionScope eAppScope = scopeForApp(eKind);
    std::vector<QuickActionDef> aResult;
    aResult.reserve(kMaxQuickActionSlots);

    for (const QuickActionDef& rDef : kCatalog)
    {
        if (aResult.size() >= kMaxQuickActionSlots)
            break;

        const bool bUniversal = (rDef.scope & QuickActionScope::Universal) != QuickActionScope{};
        const bool bAppMatch = (rDef.scope & eAppScope) != QuickActionScope{};
        if (bUniversal || bAppMatch || eKind == LoficeAppKind::Generic)
            aResult.push_back(rDef);
    }
    return aResult;
}

} // namespace lofice::ui
