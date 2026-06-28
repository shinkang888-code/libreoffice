/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * lofice — Toolbar dispatch bridge (implementation)
 *
 * Copyright (c) Lonex. Inc. All Rights Reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <lofice/ui/ToolbarDispatchBridge.hxx>

#include <comphelper/propertyvalue.hxx>
#include <com/sun/star/frame/XDispatch.hpp>
#include <com/sun/star/frame/XDispatchProvider.hpp>
#include <com/sun/star/lang/DisposedException.hpp>
#include <com/sun/star/util/URLTransformer.hpp>

#include <vcl/svapp.hxx>

using namespace css;

namespace lofice::ui
{

ToolbarDispatchBridge::ToolbarDispatchBridge(
    uno::Reference<uno::XComponentContext> xContext,
    uno::Reference<frame::XFrame> xFrame,
    SfxBindings* pBindings)
    : m_xContext(std::move(xContext))
    , m_xFrame(std::move(xFrame))
    , m_pBindings(pBindings)
{
}

void ToolbarDispatchBridge::dispatchCommand(std::u16string_view rUnoCommand, sal_Int16 nKeyModifier)
{
    SolarMutexGuard aGuard;

    if (!m_xFrame.is() || !m_xContext.is() || rUnoCommand.empty())
        return;

    if (!m_xURLTransformer.is())
        m_xURLTransformer = util::URLTransformer::create(m_xContext);

    util::URL aTargetURL;
    aTargetURL.Complete = OUString(rUnoCommand);
    m_xURLTransformer->parseStrict(aTargetURL);

    uno::Reference<frame::XDispatchProvider> xProvider(m_xFrame, uno::UNO_QUERY);
    if (!xProvider.is())
        return;

    uno::Reference<frame::XDispatch> xDispatch = xProvider->queryDispatch(aTargetURL, OUString(), 0);
    if (!xDispatch.is())
        return;

    try
    {
        uno::Sequence<beans::PropertyValue> aArgs{
            comphelper::makePropertyValue(u"KeyModifier"_ustr, nKeyModifier) };
        xDispatch->dispatch(aTargetURL, aArgs);
    }
    catch (const lang::DisposedException&)
    {
    }
}

void ToolbarDispatchBridge::invalidateBindings()
{
    SolarMutexGuard aGuard;
    if (m_pBindings)
        m_pBindings->Invalidate();
}

} // namespace lofice::ui
