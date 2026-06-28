/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * lofice — Toolbar dispatch bridge (preserves existing .uno: slot connections)
 *
 * Copyright (c) Lonex. Inc. All Rights Reserved.
 * Rights Holder: Lonex. Inc — shinkang888@gmail.com
 * Inventor: kangjunchul8@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Mirrors framework::ButtonToolbarController::execute() — routes UI clicks
 * through XFrame → XDispatchProvider → XDispatch without replacing SfxBindings.
 */

#ifndef INCLUDED_LOFICE_UI_TOOLBARDISPATCHBRIDGE_HXX
#define INCLUDED_LOFICE_UI_TOOLBARDISPATCHBRIDGE_HXX

#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/util/XURLTransformer.hpp>

#include <sfx2/bindings.hxx>

#include <memory>
#include <string_view>

namespace lofice::ui
{

/**
 * Non-owning bridge to the legacy toolbar dispatch pipeline.
 * SfxBindings* is borrowed — same lifetime as sidebar panel parent frame.
 */
class ToolbarDispatchBridge
{
public:
    ToolbarDispatchBridge(
        css::uno::Reference<css::uno::XComponentContext> xContext,
        css::uno::Reference<css::frame::XFrame> xFrame,
        SfxBindings* pBindings);

    ToolbarDispatchBridge(const ToolbarDispatchBridge&) = delete;
    ToolbarDispatchBridge& operator=(const ToolbarDispatchBridge&) = delete;

    /** Dispatch .uno: command — identical path to ButtonToolbarController::execute(). */
    void dispatchCommand(std::u16string_view rUnoCommand, sal_Int16 nKeyModifier = 0);

    /** Invalidate toolbar state after dispatch (Undo/Bold enabled etc.). */
    void invalidateBindings();

    SfxBindings* getBindings() const { return m_pBindings; }
    css::uno::Reference<css::frame::XFrame> const& getFrame() const { return m_xFrame; }

private:
    css::uno::Reference<css::uno::XComponentContext> m_xContext;
    css::uno::Reference<css::frame::XFrame> m_xFrame;
    css::uno::Reference<css::util::XURLTransformer> m_xURLTransformer;
    SfxBindings* m_pBindings;
};

} // namespace lofice::ui

#endif // INCLUDED_LOFICE_UI_TOOLBARDISPATCHBRIDGE_HXX
