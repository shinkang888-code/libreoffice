/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * lofice — SwPanelFactory integration (implementation)
 *
 * Copyright (c) Lonex. Inc. All Rights Reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <lofice/ui/LoficePanelIntegration.hxx>
#include <lofice/ui/LoficeSidebarLayout.hxx>

#include "AiAssistantPanel.hxx"

#include <comphelper/namedvaluecollection.hxx>
#include <sfx2/bindings.hxx>
#include <sfx2/sidebar/SidebarPanelBase.hxx>
#include <vcl/weld/TransportAsXWindow.hxx>

#include <com/sun/star/awt/XWindow.hpp>
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/lang/IllegalArgumentException.hpp>
#include <com/sun/star/ui/XUIElement.hpp>

using namespace css;

namespace lofice::ui
{

css::uno::Reference<css::ui::XUIElement> createAiAssistantSidebarPanel(
    std::u16string_view rsResourceURL,
    const uno::Sequence<beans::PropertyValue>& rArguments)
{
    const comphelper::NamedValueCollection aArguments(rArguments);

    uno::Reference<frame::XFrame> xFrame(
        aArguments.getOrDefault(u"Frame"_ustr, uno::Reference<frame::XFrame>()));
    uno::Reference<awt::XWindow> xParentWindow(
        aArguments.getOrDefault(u"ParentWindow"_ustr, uno::Reference<awt::XWindow>()));
    const sal_uInt64 nBindingsValue(aArguments.getOrDefault(u"SfxBindings"_ustr, sal_uInt64(0)));
    SfxBindings* pBindings = reinterpret_cast<SfxBindings*>(nBindingsValue);

    weld::Widget* pParent = nullptr;
    if (weld::TransportAsXWindow* pTunnel
        = dynamic_cast<weld::TransportAsXWindow*>(xParentWindow.get()))
    {
        pParent = pTunnel->getWidget();
    }

    if (pParent == nullptr)
        throw lang::IllegalArgumentException(
            u"lofice: AiAssistantPanel requires ParentWindow"_ustr, nullptr, 0);
    if (!xFrame.is())
        throw lang::IllegalArgumentException(
            u"lofice: AiAssistantPanel requires Frame"_ustr, nullptr, 1);
    if (pBindings == nullptr)
        throw lang::IllegalArgumentException(
            u"lofice: AiAssistantPanel requires SfxBindings"_ustr, nullptr, 2);

    std::unique_ptr<PanelLayout> xPanel = AiAssistantPanel::Create(pParent, xFrame, pBindings);

    return sfx2::sidebar::SidebarPanelBase::Create(
        OUString(rsResourceURL),
        xFrame,
        std::move(xPanel),
        css::ui::LayoutSize(-1, -1, -1));
}

} // namespace lofice::ui
