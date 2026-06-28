/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */

/*

 * lofice — Options dialog AI tab page

 *

 * Copyright (c) Lonex. Inc. All Rights Reserved.

 *

 * This Source Code Form is subject to the terms of the Mozilla Public

 * License, v. 2.0. If a copy of the MPL was not distributed with this

 * file, You can obtain one at http://mozilla.org/MPL/2.0/.

 */



#ifndef INCLUDED_LOFICE_UI_OPTLOFICEAITABPAGE_HXX

#define INCLUDED_LOFICE_UI_OPTLOFICEAITABPAGE_HXX



#include <lofice/ai/AiAsyncHttpJob.hxx>

#include <lofice/ai/AiAsyncRagJob.hxx>

#include <lofice/ai/AiSettingsStore.hxx>



#include <memory>



#include <sal/types.h>

#include <sfx2/tabdlg.hxx>

namespace weld
{
class Button;
class CheckButton;
class Container;
class DialogController;
class Entry;
class SpinButton;
class Toggleable;
}



class OptLoficeAiTabPage final : public SfxTabPage

{

public:

    OptLoficeAiTabPage(weld::Container* pPage, weld::DialogController* pController,

        const SfxItemSet& rSet);

    ~OptLoficeAiTabPage() override;



    static SAL_DLLPUBLIC_EXPORT std::unique_ptr<SfxTabPage> Create(weld::Container* pPage,

        weld::DialogController* pController, const SfxItemSet* rAttrSet);



    virtual void Reset(const SfxItemSet* rSet) override;

    virtual bool FillItemSet(SfxItemSet* rSet) override;

    virtual OUString GetAllStrings() override;



private:

    bool isAsyncTestRunning() const;

    void updateFieldSensitivity();

    lofice::ai::AiSettings collectSettingsFromUi() const;



    DECL_LINK(OnUseBuiltinToggled, weld::Toggleable&, void);

    DECL_LINK(OnUseRagToggled, weld::Toggleable&, void);

    DECL_LINK(OnTestConnectionClicked, weld::Button&, void);

    DECL_LINK(OnTestRagClicked, weld::Button&, void);

    DECL_LINK(OnAsyncHttpEvent, void*, void);

    DECL_LINK(OnAsyncRagEvent, void*, void);



    std::shared_ptr<lofice::ai::AiAsyncHttpJob> m_xHttpJob;

    std::shared_ptr<lofice::ai::AiAsyncRagJob> m_xRagJob;

    std::unique_ptr<weld::CheckButton> m_xUseBuiltinCheck;

    std::unique_ptr<weld::Entry> m_xEndpointEntry;

    std::unique_ptr<weld::Entry> m_xApiKeyEntry;

    std::unique_ptr<weld::Entry> m_xModelEntry;

    std::unique_ptr<weld::SpinButton> m_xTimeoutSpin;

    std::unique_ptr<weld::Button> m_xTestConnectionButton;

    std::unique_ptr<weld::CheckButton> m_xUseRagCheck;

    std::unique_ptr<weld::Entry> m_xRagEndpointEntry;

    std::unique_ptr<weld::SpinButton> m_xRagTimeoutSpin;

    std::unique_ptr<weld::Button> m_xTestRagButton;

};



#endif // INCLUDED_LOFICE_UI_OPTLOFICEAITABPAGE_HXX

