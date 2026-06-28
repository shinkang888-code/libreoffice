/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */

/*

 * lofice — Options dialog AI tab page (implementation)

 *

 * Copyright (c) Lonex. Inc. All Rights Reserved.

 *

 * This Source Code Form is subject to the terms of the Mozilla Public

 * License, v. 2.0. If a copy of the MPL was not distributed with this

 * file, You can obtain one at http://mozilla.org/MPL/2.0/.

 */



#include <lofice/ui/OptLoficeAiTabPage.hxx>



#include <lofice/ai/AiHttpTransport.hxx>

#include <lofice/ai/AiPromptService.hxx>

#include <lofice/ai/AiRagContext.hxx>

#include <lofice/ai/AiSettingsStore.hxx>



#include <algorithm>

#include <comphelper/solarmutex.hxx>

#include <rtl/ustrbuf.hxx>

#include <vcl/svapp.hxx>
#include <vcl/vclenum.hxx>
#include <vcl/weld/Builder.hxx>

#include <vcl/weld/MessageDialog.hxx>



OptLoficeAiTabPage::OptLoficeAiTabPage(weld::Container* pPage, weld::DialogController* pController,

    const SfxItemSet& rSet)

    : SfxTabPage(pPage, pController, u"modules/lofice/ui/opt_lofice_ai_page.ui"_ustr,

        u"OptLoficeAiPage"_ustr, &rSet)

    , m_xUseBuiltinCheck(m_xBuilder->weld_check_button(u"check_use_builtin"_ustr))

    , m_xEndpointEntry(m_xBuilder->weld_entry(u"entry_endpoint"_ustr))

    , m_xApiKeyEntry(m_xBuilder->weld_entry(u"entry_api_key"_ustr))

    , m_xModelEntry(m_xBuilder->weld_entry(u"entry_model"_ustr))

    , m_xTimeoutSpin(m_xBuilder->weld_spin_button(u"spin_timeout"_ustr))

    , m_xTestConnectionButton(m_xBuilder->weld_button(u"btn_test_connection"_ustr))

    , m_xUseRagCheck(m_xBuilder->weld_check_button(u"check_use_rag"_ustr))

    , m_xRagEndpointEntry(m_xBuilder->weld_entry(u"entry_rag_endpoint"_ustr))

    , m_xRagTimeoutSpin(m_xBuilder->weld_spin_button(u"spin_rag_timeout"_ustr))

    , m_xTestRagButton(m_xBuilder->weld_button(u"btn_test_rag"_ustr))

{

    m_xTimeoutSpin->set_range(5, 120);

    m_xTimeoutSpin->set_increments(1, 5);

    m_xRagTimeoutSpin->set_range(2, 30);

    m_xRagTimeoutSpin->set_increments(1, 2);

    m_xUseBuiltinCheck->connect_toggled(LINK(this, OptLoficeAiTabPage, OnUseBuiltinToggled));

    m_xUseRagCheck->connect_toggled(LINK(this, OptLoficeAiTabPage, OnUseRagToggled));

    m_xTestConnectionButton->connect_clicked(

        LINK(this, OptLoficeAiTabPage, OnTestConnectionClicked));

    m_xTestRagButton->connect_clicked(LINK(this, OptLoficeAiTabPage, OnTestRagClicked));

}



OptLoficeAiTabPage::~OptLoficeAiTabPage()

{

    if (m_xHttpJob)

        m_xHttpJob->cancel();

    if (m_xRagJob)

        m_xRagJob->cancel();

}



bool OptLoficeAiTabPage::isAsyncTestRunning() const

{

    return (m_xHttpJob && m_xHttpJob->isRunning())

        || (m_xRagJob && m_xRagJob->isRunning());

}



void OptLoficeAiTabPage::Reset(const SfxItemSet*)

{

    const lofice::ai::AiSettings aSettings = lofice::ai::loadSettings();

    m_xUseBuiltinCheck->set_active(aSettings.useBuiltinAi);

    m_xEndpointEntry->set_text(aSettings.endpoint);

    m_xApiKeyEntry->set_text(aSettings.apiKey);

    m_xModelEntry->set_text(aSettings.model);

    m_xTimeoutSpin->set_value(aSettings.timeoutSeconds);

    m_xUseRagCheck->set_active(aSettings.useRagContext);

    m_xRagEndpointEntry->set_text(aSettings.ragEndpoint);

    m_xRagTimeoutSpin->set_value(aSettings.ragTimeoutSeconds);

    updateFieldSensitivity();

}



bool OptLoficeAiTabPage::FillItemSet(SfxItemSet*)

{

    lofice::ai::saveSettings(collectSettingsFromUi());

    return false;

}



OUString OptLoficeAiTabPage::GetAllStrings()

{

    OUStringBuffer aAllStrings;

    const OUString aLabels[] = {

        u"frame_title"_ustr, u"check_use_builtin"_ustr, u"label_endpoint"_ustr,

        u"label_api_key"_ustr, u"label_model"_ustr, u"label_timeout"_ustr,

        u"btn_test_connection"_ustr, u"check_use_rag"_ustr, u"label_rag_endpoint"_ustr,

        u"label_rag_timeout"_ustr, u"btn_test_rag"_ustr, u"label_hint"_ustr,

    };

    for (const OUString& rLabel : aLabels)

    {

        if (const auto pLabel = m_xBuilder->weld_label(rLabel))

            aAllStrings.append(pLabel->get_label() + " ");

    }

    return aAllStrings.toString().replaceAll("_", "");

}



lofice::ai::AiSettings OptLoficeAiTabPage::collectSettingsFromUi() const

{

    lofice::ai::AiSettings aSettings;

    aSettings.useBuiltinAi = m_xUseBuiltinCheck->get_active();

    aSettings.endpoint = m_xEndpointEntry->get_text().trim();

    aSettings.apiKey = m_xApiKeyEntry->get_text();

    aSettings.model = m_xModelEntry->get_text().trim();

    if (aSettings.model.isEmpty())

        aSettings.model = u"lofice-default"_ustr;

    aSettings.timeoutSeconds = static_cast<sal_Int32>(m_xTimeoutSpin->get_value());

    aSettings.useRagContext = m_xUseRagCheck->get_active();

    aSettings.ragEndpoint = m_xRagEndpointEntry->get_text().trim();

    if (aSettings.ragEndpoint.isEmpty())

        aSettings.ragEndpoint = u"https://lofice-rag-api.vercel.app/search"_ustr;

    aSettings.ragTimeoutSeconds = static_cast<sal_Int32>(m_xRagTimeoutSpin->get_value());

    return aSettings;

}



void OptLoficeAiTabPage::updateFieldSensitivity()

{

    const bool bBuiltin = m_xUseBuiltinCheck->get_active();

    m_xEndpointEntry->set_sensitive(!bBuiltin);

    m_xApiKeyEntry->set_sensitive(!bBuiltin);

    m_xModelEntry->set_sensitive(!bBuiltin);

    m_xTimeoutSpin->set_sensitive(!bBuiltin);



    const bool bRag = m_xUseRagCheck->get_active();

    m_xRagEndpointEntry->set_sensitive(bRag);

    m_xRagTimeoutSpin->set_sensitive(bRag);



    const bool bBusy = isAsyncTestRunning();

    m_xTestConnectionButton->set_sensitive(

        !bBusy && !bBuiltin && !m_xEndpointEntry->get_text().trim().isEmpty());

    m_xTestRagButton->set_sensitive(

        !bBusy && bRag && !m_xRagEndpointEntry->get_text().trim().isEmpty());

}



IMPL_LINK_NOARG(OptLoficeAiTabPage, OnUseBuiltinToggled, weld::Toggleable&, void)

{

    updateFieldSensitivity();

}



IMPL_LINK_NOARG(OptLoficeAiTabPage, OnUseRagToggled, weld::Toggleable&, void)

{

    updateFieldSensitivity();

}



IMPL_LINK_NOARG(OptLoficeAiTabPage, OnTestConnectionClicked, weld::Button&, void)

{

    const lofice::ai::AiSettings aSettings = collectSettingsFromUi();

    if (aSettings.useBuiltinAi || aSettings.endpoint.isEmpty())

        return;



    if (!lofice::ai::isHttpTransportAvailable())

        return;



    if (isAsyncTestRunning())

        return;



    lofice::ai::AiPromptRequest aRequest;

    aRequest.prompt = "ping";

    aRequest.appModule = "lofice";

    aRequest.appDisplayName = "Options";



    const lofice::ai::AiHttpConfig aConfig = lofice::ai::toHttpConfig(aSettings);



    if (!m_xHttpJob)

        m_xHttpJob = std::make_shared<lofice::ai::AiAsyncHttpJob>();



    updateFieldSensitivity();

    m_xHttpJob->start(aConfig, aRequest, LINK(this, OptLoficeAiTabPage, OnAsyncHttpEvent));

}



IMPL_LINK(OptLoficeAiTabPage, OnAsyncHttpEvent, void*, pData, void)

{

    std::unique_ptr<lofice::ai::AiHttpUiPayload> pPayload(

        static_cast<lofice::ai::AiHttpUiPayload*>(pData));



    SolarMutexGuard aGuard;

    updateFieldSensitivity();



    if (pPayload->errorDetail == "Request cancelled.")

        return;



    std::unique_ptr<weld::MessageDialog> xBox(Application::CreateMessageDialog(

        m_xContainer.get(), VclMessageType::Info, VclButtonsType::Ok,

        pPayload->success ? u"연결 테스트 성공"_ustr

                          : u"연결 테스트 실패: "_ustr

                                + OUString::createFromAscii(pPayload->errorDetail)));

    xBox->run();

}



IMPL_LINK_NOARG(OptLoficeAiTabPage, OnTestRagClicked, weld::Button&, void)

{

    const lofice::ai::AiSettings aSettings = collectSettingsFromUi();

    if (!aSettings.useRagContext || aSettings.ragEndpoint.isEmpty())

        return;



    if (!lofice::ai::isHttpTransportAvailable())

        return;



    if (isAsyncTestRunning())

        return;



    const lofice::ai::AiRagSettings aRag = lofice::ai::toRagSettings(aSettings);



    if (!m_xRagJob)

        m_xRagJob = std::make_shared<lofice::ai::AiAsyncRagJob>();



    updateFieldSensitivity();

    m_xRagJob->start(aRag, "lofice AI RAG test", LINK(this, OptLoficeAiTabPage, OnAsyncRagEvent));

}



IMPL_LINK(OptLoficeAiTabPage, OnAsyncRagEvent, void*, pData, void)

{

    std::unique_ptr<lofice::ai::AiRagUiPayload> pPayload(

        static_cast<lofice::ai::AiRagUiPayload*>(pData));



    SolarMutexGuard aGuard;

    updateFieldSensitivity();



    if (pPayload->errorDetail == "Request cancelled.")

        return;



    OUString aMessage;

    if (!pPayload->success || pPayload->context.empty())

    {

        aMessage = u"RAG 테스트 실패 — rag-server 실행 및 DATABASE_URL 확인"_ustr;

    }

    else

    {

        const std::string& rContext = pPayload->context;

        const std::string aPreview = rContext.substr(

            0, std::min(rContext.size(), std::size_t{400}));

        aMessage = u"RAG 테스트 성공 ("_ustr

            + OUString::number(static_cast<sal_Int32>(rContext.size()))

            + u"자)\n\n"_ustr

            + OUString::createFromAscii(aPreview);

    }



    std::unique_ptr<weld::MessageDialog> xBox(Application::CreateMessageDialog(

        m_xContainer.get(),

        pPayload->success ? VclMessageType::Info : VclMessageType::Warning,

        VclButtonsType::Ok, aMessage));

    xBox->run();

}



std::unique_ptr<SfxTabPage> OptLoficeAiTabPage::Create(weld::Container* pPage,

    weld::DialogController* pController, const SfxItemSet* rAttrSet)

{

    return std::make_unique<OptLoficeAiTabPage>(pPage, pController, *rAttrSet);

}

