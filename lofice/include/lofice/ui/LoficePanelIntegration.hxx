/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * lofice — SwPanelFactory integration entry point
 *
 * Copyright (c) Lonex. Inc. All Rights Reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_LOFICE_UI_LOFICEPANELINTEGRATION_HXX
#define INCLUDED_LOFICE_UI_LOFICEPANELINTEGRATION_HXX

#include <memory>

#include <com/sun/star/ui/XUIElement.hpp>
#include <com/sun/star/uno/Reference.hxx>
#include <com/sun/star/uno/Sequence.hxx>
#include <com/sun/star/beans/PropertyValue.hpp>

class SfxBindings;

namespace lofice::ui
{

/** Called from SwPanelFactory when ImplementationURL ends with /AiAssistantPanel. */
css::uno::Reference<css::ui::XUIElement> createAiAssistantSidebarPanel(
    std::u16string_view rsResourceURL,
    const css::uno::Sequence<css::beans::PropertyValue>& rArguments);

} // namespace lofice::ui

#endif // INCLUDED_LOFICE_UI_LOFICEPANELINTEGRATION_HXX
