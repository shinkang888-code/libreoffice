/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * lofice — AI Assistant sidebar panel (implementation)
 *
 * Copyright (c) Lonex. Inc. All Rights Reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "AiAssistantPanel.hxx"

#include <lofice/IEventListener.hxx>
#include <lofice/ai/AiAsyncHttpJob.hxx>
#include <lofice/ai/AiAsyncPromptJob.hxx>
#include <lofice/ai/AiAsyncRagJob.hxx>
#include <lofice/ai/AiAsyncStreamJob.hxx>
#include <lofice/ai/AiConversationStore.hxx>
#include <lofice/ai/AiConversationFileIo.hxx>
#include <lofice/ai/AiHttpTransport.hxx>
#include <lofice/ai/AiPromptService.hxx>
#include <lofice/ai/AiRagContext.hxx>
#include <lofice/ai/AiServiceBootstrap.hxx>
#include <lofice/ai/AiSettingsStore.hxx>

#include <comphelper/processfactory.hxx>
#include <comphelper/solarmutex.hxx>
#include <com/sun/star/lang/IllegalArgumentException.hpp>

#include <sfx2/bindings.hxx>
#include <vcl/svapp.hxx>
#include <vcl/weld/Builder.hxx>

#include <algorithm>
#include <chrono>

using namespace css;

namespace lofice::ui
{

std::unique_ptr<PanelLayout> AiAssistantPanel::Create(
    weld::Widget* pParent,
    const uno::Reference<frame::XFrame>& rxFrame,
    SfxBindings* pBindings)
{
    if (pParent == nullptr)
        throw lang::IllegalArgumentException(
            u"no parent Window given to AiAssistantPanel::Create"_ustr, nullptr, 0);
    if (!rxFrame.is())
        throw lang::IllegalArgumentException(
            u"no XFrame given to AiAssistantPanel::Create"_ustr, nullptr, 1);
    if (pBindings == nullptr)
        throw lang::IllegalArgumentException(
            u"no SfxBindings given to AiAssistantPanel::Create"_ustr, nullptr, 2);

    return std::make_unique<AiAssistantPanel>(pParent, rxFrame, pBindings);
}

AiAssistantPanel::AiAssistantPanel(
    weld::Widget* pParent,
    const uno::Reference<frame::XFrame>& rxFrame,
    SfxBindings* pBindings)
    : PanelLayout(
        pParent,
        loficeLayoutId(m_aLayout.kGridId),
        loficeLayoutId(m_aLayout.kUiResourcePath))
    , m_eAppKind(detectAppKind(rxFrame))
    , m_xDispatchBridge(std::make_unique<ToolbarDispatchBridge>(
          comphelper::getProcessComponentContext(), rxFrame, pBindings))
    , m_xTitleLabel(m_xBuilder->weld_label(loficeLayoutId(m_aLayout.kTitleLabelId)))
    , m_xAppContextLabel(m_xBuilder->weld_label(loficeLayoutId(m_aLayout.kAppContextLabelId)))
    , m_xToggleSettingsButton(m_xBuilder->weld_button(loficeLayoutId(m_aLayout.kToggleSettingsButtonId)))
    , m_xSettingsGrid(m_xBuilder->weld_widget(loficeLayoutId(m_aLayout.kSettingsGridId)))
    , m_xUseBuiltinCheck(m_xBuilder->weld_check_button(loficeLayoutId(m_aLayout.kUseBuiltinCheckId)))
    , m_xAutoSendQuickActionCheck(
          m_xBuilder->weld_check_button(loficeLayoutId(m_aLayout.kAutoSendQuickActionCheckId)))
    , m_xEndpointEntry(m_xBuilder->weld_entry(loficeLayoutId(m_aLayout.kEndpointEntryId)))
    , m_xApiKeyEntry(m_xBuilder->weld_entry(loficeLayoutId(m_aLayout.kApiKeyEntryId)))
    , m_xModelEntry(m_xBuilder->weld_entry(loficeLayoutId(m_aLayout.kModelEntryId)))
    , m_xTimeoutSpin(m_xBuilder->weld_spin_button(loficeLayoutId(m_aLayout.kTimeoutSpinId)))
    , m_xUseRagCheck(m_xBuilder->weld_check_button(loficeLayoutId(m_aLayout.kUseRagCheckId)))
    , m_xRagEndpointEntry(m_xBuilder->weld_entry(loficeLayoutId(m_aLayout.kRagEndpointEntryId)))
    , m_xRagTimeoutSpin(m_xBuilder->weld_spin_button(loficeLayoutId(m_aLayout.kRagTimeoutSpinId)))
    , m_xTestRagButton(m_xBuilder->weld_button(loficeLayoutId(m_aLayout.kTestRagButtonId)))
    , m_xSaveSettingsButton(m_xBuilder->weld_button(loficeLayoutId(m_aLayout.kSaveSettingsButtonId)))
    , m_xTestConnectionButton(m_xBuilder->weld_button(loficeLayoutId(m_aLayout.kTestConnectionButtonId)))
    , m_xPromptText(m_xBuilder->weld_text_view(loficeLayoutId(m_aLayout.kPromptTextId)))
    , m_xResponseText(m_xBuilder->weld_text_view(loficeLayoutId(m_aLayout.kResponseTextId)))
    , m_xStatusLabel(m_xBuilder->weld_label(loficeLayoutId(m_aLayout.kStatusLabelId)))
    , m_xSendButton(m_xBuilder->weld_button(loficeLayoutId(m_aLayout.kSendButtonId)))
    , m_xCancelAsyncButton(m_xBuilder->weld_button(loficeLayoutId(m_aLayout.kCancelAsyncButtonId)))
    , m_xClearPromptButton(m_xBuilder->weld_button(loficeLayoutId(m_aLayout.kClearPromptButtonId)))
    , m_xExportHistoryButton(m_xBuilder->weld_button(loficeLayoutId(m_aLayout.kExportHistoryButtonId)))
    , m_xImportHistoryButton(m_xBuilder->weld_button(loficeLayoutId(m_aLayout.kImportHistoryButtonId)))
    , m_xClearResponseButton(m_xBuilder->weld_button(loficeLayoutId(m_aLayout.kClearResponseButtonId)))
{
    lofice::ai::ensureDefaultAiListener();

    m_xTitleLabel->set_label(u"lofice AI 어시스턴트"_ustr);
    m_xAppContextLabel->set_label(u"컨텍스트: "_ustr + OUString(appKindDisplayName(m_eAppKind)));

    m_xPromptText->set_size_request(
        m_aLayout.width.nDefaultPx - 24,
        80);
    m_xResponseText->set_size_request(
        m_aLayout.width.nDefaultPx - 24,
        160);
    m_xResponseText->set_editable(false);

    m_xTimeoutSpin->set_range(5, 120);
    m_xTimeoutSpin->set_increments(1, 5);
    m_xRagTimeoutSpin->set_range(2, 30);
    m_xRagTimeoutSpin->set_increments(1, 2);

    for (std::size_t i = 0; i < kMaxQuickActionSlots; ++i)
    {
        m_aQuickActionButtons[i] = m_xBuilder->weld_button(loficeLayoutId(kQuickActionSlotWidgetIds[i]));
        m_aQuickActionButtons[i]->connect_clicked(LINK(this, AiAssistantPanel, OnQuickActionClicked));
    }

    m_xToggleSettingsButton->connect_clicked(LINK(this, AiAssistantPanel, OnToggleSettingsClicked));
    m_xUseBuiltinCheck->connect_toggled(LINK(this, AiAssistantPanel, OnUseBuiltinToggled));
    m_xUseRagCheck->connect_toggled(LINK(this, AiAssistantPanel, OnUseRagToggled));
    m_xSaveSettingsButton->connect_clicked(LINK(this, AiAssistantPanel, OnSaveSettingsClicked));
    m_xTestConnectionButton->connect_clicked(LINK(this, AiAssistantPanel, OnTestConnectionClicked));
    m_xTestRagButton->connect_clicked(LINK(this, AiAssistantPanel, OnTestRagClicked));
    m_xSendButton->connect_clicked(LINK(this, AiAssistantPanel, OnSendClicked));
    m_xCancelAsyncButton->connect_clicked(LINK(this, AiAssistantPanel, OnCancelAsyncClicked));
    m_xClearPromptButton->connect_clicked(LINK(this, AiAssistantPanel, OnClearPromptClicked));
    m_xExportHistoryButton->connect_clicked(LINK(this, AiAssistantPanel, OnExportHistoryClicked));
    m_xImportHistoryButton->connect_clicked(LINK(this, AiAssistantPanel, OnImportHistoryClicked));
    m_xClearResponseButton->connect_clicked(LINK(this, AiAssistantPanel, OnClearResponseClicked));
    m_xCancelAsyncButton->hide();

    loadSettingsIntoUi();
    updateSettingsFieldSensitivity();
    m_xSettingsGrid->hide();

    setupQuickActionGrid(m_eAppKind);

    lofice::ai::loadConversationHistory(m_aConversation);
    restoreLastResponseFromHistory();
    if (m_aConversation.size() > 0)
    {
        setStatus(u"준비됨 — 저장된 대화 "_ustr
            + OUString::number(static_cast<sal_Int32>(m_aConversation.size()))
            + u"메시지, 16개 퀵액션"_ustr);
    }
    else
    {
        const lofice::ai::AiSettings aSettings = lofice::ai::loadSettings();
        if (aSettings.useRagContext)
            setStatus(u"준비됨 — 16개 퀵액션, RAG 활성"_ustr);
        else
            setStatus(u"준비됨 — 16개 퀵액션, 툴바 브릿지 활성"_ustr);
    }
}

AiAssistantPanel::~AiAssistantPanel()
{
    if (m_xPanelAdapter)
    {
        m_xPanelAdapter->alive = false;
        m_xPanelAdapter->pPanel = nullptr;
    }
    if (m_xStreamJob)
        m_xStreamJob->cancel();
    if (m_xPromptJob)
        m_xPromptJob->cancel();
    if (m_xHttpJob)
        m_xHttpJob->cancel();
    if (m_xRagJob)
        m_xRagJob->cancel();

    for (std::size_t i = 0; i < kMaxQuickActionSlots; ++i)
        m_aQuickActionButtons[i].reset();
    m_xClearResponseButton.reset();
    m_xImportHistoryButton.reset();
    m_xExportHistoryButton.reset();
    m_xClearPromptButton.reset();
    m_xCancelAsyncButton.reset();
    m_xSendButton.reset();
    m_xStatusLabel.reset();
    m_xResponseText.reset();
    m_xPromptText.reset();
    m_xTestConnectionButton.reset();
    m_xTestRagButton.reset();
    m_xSaveSettingsButton.reset();
    m_xRagTimeoutSpin.reset();
    m_xRagEndpointEntry.reset();
    m_xUseRagCheck.reset();
    m_xTimeoutSpin.reset();
    m_xModelEntry.reset();
    m_xApiKeyEntry.reset();
    m_xEndpointEntry.reset();
    m_xAutoSendQuickActionCheck.reset();
    m_xUseBuiltinCheck.reset();
    m_xSettingsGrid.reset();
    m_xToggleSettingsButton.reset();
    m_xAppContextLabel.reset();
    m_xTitleLabel.reset();
    m_xDispatchBridge.reset();
}

void AiAssistantPanel::loadSettingsIntoUi()
{
    const lofice::ai::AiSettings aSettings = lofice::ai::loadSettings();
    m_xUseBuiltinCheck->set_active(aSettings.useBuiltinAi);
    m_xAutoSendQuickActionCheck->set_active(aSettings.autoSendQuickActionPrompt);
    m_xEndpointEntry->set_text(aSettings.endpoint);
    m_xApiKeyEntry->set_text(aSettings.apiKey);
    m_xModelEntry->set_text(aSettings.model);
    m_xTimeoutSpin->set_value(aSettings.timeoutSeconds);
    m_xUseRagCheck->set_active(aSettings.useRagContext);
    m_xRagEndpointEntry->set_text(aSettings.ragEndpoint);
    m_xRagTimeoutSpin->set_value(aSettings.ragTimeoutSeconds);
}

lofice::ai::AiSettings AiAssistantPanel::collectSettingsFromUi() const
{
    lofice::ai::AiSettings aSettings;
    aSettings.useBuiltinAi = m_xUseBuiltinCheck->get_active();
    aSettings.autoSendQuickActionPrompt = m_xAutoSendQuickActionCheck->get_active();
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

void AiAssistantPanel::updateSettingsFieldSensitivity()
{
    const bool bBuiltin = m_xUseBuiltinCheck->get_active();
    m_xEndpointEntry->set_sensitive(!bBuiltin);
    m_xApiKeyEntry->set_sensitive(!bBuiltin);
    m_xModelEntry->set_sensitive(!bBuiltin);
    m_xTimeoutSpin->set_sensitive(!bBuiltin);
    m_xTestConnectionButton->set_sensitive(!bBuiltin && !m_xEndpointEntry->get_text().trim().isEmpty());

    const bool bRag = m_xUseRagCheck->get_active();
    m_xRagEndpointEntry->set_sensitive(bRag);
    m_xRagTimeoutSpin->set_sensitive(bRag);
    m_xTestRagButton->set_sensitive(bRag && !m_xRagEndpointEntry->get_text().trim().isEmpty());
}

void AiAssistantPanel::toggleSettingsVisibility()
{
    m_bSettingsVisible = !m_bSettingsVisible;
    if (m_bSettingsVisible)
        m_xSettingsGrid->show();
    else
        m_xSettingsGrid->hide();
    m_xToggleSettingsButton->set_label(
        m_bSettingsVisible ? u"AI 설정 닫기"_ustr : u"AI 설정"_ustr);
}

void AiAssistantPanel::setupQuickActionGrid(LoficeAppKind eApp)
{
    const std::vector<QuickActionDef> aActions = getQuickActionsForApp(eApp);

    for (std::size_t i = 0; i < kMaxQuickActionSlots; ++i)
    {
        m_aQuickActionCommands[i].clear();
        m_aQuickActionPrompts[i].clear();
        if (i < aActions.size())
        {
            const QuickActionDef& rDef = aActions[i];
            m_aQuickActionCommands[i] = OUString(rDef.unoCommand);
            if (!rDef.aiPrompt.empty())
            {
                m_aQuickActionPrompts[i] = OStringToOUString(
                    OString(rDef.aiPrompt.data(), static_cast<sal_Int32>(rDef.aiPrompt.size()),
                        RTL_TEXTENCODING_UTF8),
                    RTL_TEXTENCODING_UTF8);
            }
            m_aQuickActionButtons[i]->set_label(OUString(rDef.label));
            m_aQuickActionButtons[i]->set_tooltip_text(m_aQuickActionPrompts[i].isEmpty()
                ? OUString(rDef.unoCommand)
                : m_aQuickActionPrompts[i]);
            m_aQuickActionButtons[i]->show();
        }
        else
        {
            m_aQuickActionButtons[i]->hide();
        }
    }
}

void AiAssistantPanel::setStatus(const OUString& rText)
{
    m_xStatusLabel->set_label(rText);
}

void AiAssistantPanel::dispatchQuickAction(
    std::u16string_view rUnoCommand, std::string_view rAiPromptUtf8)
{
    m_xDispatchBridge->dispatchCommand(rUnoCommand, 0);
    m_xDispatchBridge->invalidateBindings();
    fireAiHookEvent(u"dispatchQuickAction"_ustr, rUnoCommand);
    fillPromptFromQuickAction(rAiPromptUtf8);
    const lofice::ai::AiSettings aSettings = lofice::ai::loadSettings();
    if (!rAiPromptUtf8.empty() && aSettings.autoSendQuickActionPrompt && !isAsyncBusy())
        submitPromptFromUi();
}

void AiAssistantPanel::fillPromptFromQuickAction(std::string_view rAiPromptUtf8)
{
    if (rAiPromptUtf8.empty())
        return;

    m_xPromptText->set_text(OStringToOUString(
        OString(rAiPromptUtf8.data(), static_cast<sal_Int32>(rAiPromptUtf8.size()),
            RTL_TEXTENCODING_UTF8),
        RTL_TEXTENCODING_UTF8));
}

bool AiAssistantPanel::isAsyncBusy() const
{
    return (m_xStreamJob && m_xStreamJob->isRunning())
        || (m_xPromptJob && m_xPromptJob->isRunning())
        || (m_xHttpJob && m_xHttpJob->isRunning())
        || (m_xRagJob && m_xRagJob->isRunning());
}

void AiAssistantPanel::submitPromptFromUi()
{
    const OUString aPrompt = m_xPromptText->get_text();
    if (aPrompt.isEmpty())
    {
        setStatus(u"프롬프트를 입력하세요"_ustr);
        return;
    }

    setStatus(u"AI 요청 처리 중..."_ustr);

    const std::string aPromptUtf8 = OUStringToOString(aPrompt, RTL_TEXTENCODING_UTF8).getStr();
    const lofice::ai::AiPromptRequest aRequest = buildPromptRequest(aPromptUtf8, true);

    if (trySendExternalStreaming(aRequest))
        return;

    if (trySendAsyncPrompt(aRequest))
        return;

    setStatus(u"AI 요청을 시작할 수 없습니다"_ustr);
}

void AiAssistantPanel::beginNewResponse()
{
    m_xResponseText->set_text(OUString());
}

void AiAssistantPanel::appendResponseLine(const OUString& rLine)
{
    OUString aCurrent = m_xResponseText->get_text();
    if (!aCurrent.isEmpty())
        aCurrent += "\n";
    aCurrent += rLine;
    m_xResponseText->set_text(aCurrent);
}

void AiAssistantPanel::appendResponseToken(std::string_view rToken)
{
    if (rToken.empty())
        return;

    OUString aCurrent = m_xResponseText->get_text();
    aCurrent += OStringToOUString(
        OString(rToken.data(), static_cast<sal_Int32>(rToken.size()), RTL_TEXTENCODING_UTF8),
        RTL_TEXTENCODING_UTF8);
    m_xResponseText->set_text(aCurrent);
}

void AiAssistantPanel::setAsyncBusyUiActive(bool bActive)
{
    if (bActive)
        m_xCancelAsyncButton->show();
    else
        m_xCancelAsyncButton->hide();

    m_xSendButton->set_sensitive(!bActive);
    m_xClearPromptButton->set_sensitive(!bActive);
    m_xExportHistoryButton->set_sensitive(!bActive);
    m_xImportHistoryButton->set_sensitive(!bActive);

    if (bActive)
    {
        m_xTestConnectionButton->set_sensitive(false);
        m_xTestRagButton->set_sensitive(false);
    }
    else
        updateSettingsFieldSensitivity();
}

void AiAssistantPanel::setStreamingUiActive(bool bActive)
{
    setAsyncBusyUiActive(bActive);
}

void AiAssistantPanel::setConnectionTestUiActive(bool bActive)
{
    setAsyncBusyUiActive(bActive);
}

void AiAssistantPanel::handleStreamFinished(bool bSuccess, std::string_view rErrorDetail)
{
    if (bSuccess)
    {
        if (!m_aPendingUserPrompt.empty())
            commitConversationTurn(m_aPendingUserPrompt);
        m_aPendingUserPrompt.clear();
        setStatus(u"AI 응답 완료 (외부 API · 스트리밍)"_ustr);
        return;
    }

    m_aPendingUserPrompt.clear();

    if (rErrorDetail == "Request cancelled.")
    {
        setStatus(u"AI 요청을 취소했습니다"_ustr);
        return;
    }

    if (!rErrorDetail.empty())
    {
        appendResponseLine(u"[오류] "_ustr + OStringToOUString(
            OString(rErrorDetail.data(), static_cast<sal_Int32>(rErrorDetail.size()),
                RTL_TEXTENCODING_UTF8),
            RTL_TEXTENCODING_UTF8));
    }
    setStatus(u"AI 요청 실패 (외부 API)"_ustr);
}

void AiAssistantPanel::handlePromptFinished(
    bool bSuccess, std::string_view rResponseUtf8, std::string_view rProvider)
{
    if (bSuccess)
    {
        m_xResponseText->set_text(OStringToOUString(
            OString(rResponseUtf8.data(), static_cast<sal_Int32>(rResponseUtf8.size()),
                RTL_TEXTENCODING_UTF8),
            RTL_TEXTENCODING_UTF8));
        if (!m_aPendingUserPrompt.empty())
            commitConversationTurn(m_aPendingUserPrompt);
        m_aPendingUserPrompt.clear();

        if (rProvider == "http")
            setStatus(u"AI 응답 완료 (외부 API)"_ustr);
        else if (rProvider == "http-error")
            setStatus(u"AI 응답 완료 (외부 API 오류)"_ustr);
        else if (rProvider == "http-unavailable")
            setStatus(u"AI 응답 완료 (curl 비활성)"_ustr);
        else
            setStatus(u"AI 응답 완료 (내장 AI)"_ustr);
        return;
    }

    m_aPendingUserPrompt.clear();

    if (rResponseUtf8 == "Request cancelled.")
    {
        setStatus(u"AI 요청을 취소했습니다"_ustr);
        return;
    }

    if (!rResponseUtf8.empty())
    {
        m_xResponseText->set_text(OStringToOUString(
            OString(rResponseUtf8.data(), static_cast<sal_Int32>(rResponseUtf8.size()),
                RTL_TEXTENCODING_UTF8),
            RTL_TEXTENCODING_UTF8));
    }
    setStatus(u"AI 요청 실패"_ustr);
}

void AiAssistantPanel::commitConversationTurn(const std::string& rUserPromptUtf8)
{
    if (rUserPromptUtf8.empty())
        return;

    m_aConversation.appendUser(rUserPromptUtf8);

    const OUString aResponse = m_xResponseText->get_text();
    if (!aResponse.isEmpty())
    {
        m_aConversation.appendAssistant(
            OUStringToOString(aResponse, RTL_TEXTENCODING_UTF8).getStr());
    }

    lofice::ai::saveConversationHistory(m_aConversation);
}

void AiAssistantPanel::restoreLastResponseFromHistory()
{
    std::string aLastAssistant;
    if (!m_aConversation.lastAssistantMessage(aLastAssistant))
        return;

    m_xResponseText->set_text(OStringToOUString(
        OString(aLastAssistant.c_str(), static_cast<sal_Int32>(aLastAssistant.size()),
            RTL_TEXTENCODING_UTF8),
        RTL_TEXTENCODING_UTF8));
}

lofice::ai::AiPromptRequest AiAssistantPanel::buildPromptRequest(
    const std::string& rPromptUtf8, bool bIncludeHistory) const
{
    lofice::ai::AiPromptRequest aRequest;
    aRequest.prompt = rPromptUtf8;
    aRequest.appModule =
        m_eAppKind == LoficeAppKind::Writer ? "sw" :
        m_eAppKind == LoficeAppKind::Calc ? "sc" :
        m_eAppKind == LoficeAppKind::Impress || m_eAppKind == LoficeAppKind::Draw ? "sd" : "lofice";
    aRequest.appDisplayName = OUString(appKindDisplayName(m_eAppKind)).toUtf8();
    if (bIncludeHistory)
        m_aConversation.populatePriorMessages(aRequest.priorMessages);
    return aRequest;
}

bool AiAssistantPanel::trySendExternalStreaming(const lofice::ai::AiPromptRequest& rRequest)
{
    if (!lofice::ai::isExternalAiConfigured() || !lofice::ai::isHttpTransportAvailable())
        return false;

    const lofice::ai::AiHttpConfig aConfig = lofice::ai::loadEffectiveHttpConfig();
    if (!aConfig.isValid())
        return false;

    if (m_xStreamJob && m_xStreamJob->isRunning())
    {
        setStatus(u"이전 AI 요청이 진행 중입니다"_ustr);
        return true;
    }

    EventPayload aEvent;
    aEvent.category = EventCategory::Command;
    aEvent.sourceModule = rRequest.appModule;
    aEvent.sourceFile = "AiAssistantPanel.cxx";
    aEvent.sourceClass = "AiAssistantPanel";
    aEvent.action = "aiPromptSubmit";
    aEvent.textData = rRequest.prompt;
    aEvent.timestampMs = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();

    EventListenerRegistry::instance().dispatchBefore(aEvent);
    m_aActiveStreamEvent = aEvent;
    m_bStreamAfterPending = true;

    if (!m_xPanelAdapter)
        m_xPanelAdapter = std::make_shared<PanelUiAdapter>();
    m_xPanelAdapter->alive = true;
    m_xPanelAdapter->pPanel = this;

    beginNewResponse();
    setStreamingUiActive(true);
    m_aPendingUserPrompt = rRequest.prompt;
    if (lofice::ai::isRagConfigured())
        setStatus(u"코드베이스 검색 중..."_ustr);
    else
        setStatus(u"AI 스트리밍 수신 중..."_ustr);

    if (!m_xStreamJob)
        m_xStreamJob = std::make_shared<lofice::ai::AiAsyncStreamJob>();

    m_xStreamJob->start(aConfig, rRequest, LINK(this, AiAssistantPanel, OnAsyncStreamEvent));
    return true;
}

bool AiAssistantPanel::trySendAsyncPrompt(const lofice::ai::AiPromptRequest& rRequest)
{
    if (m_xPromptJob && m_xPromptJob->isRunning())
    {
        setStatus(u"이전 AI 요청이 진행 중입니다"_ustr);
        return true;
    }

    EventPayload aEvent;
    aEvent.category = EventCategory::Command;
    aEvent.sourceModule = rRequest.appModule;
    aEvent.sourceFile = "AiAssistantPanel.cxx";
    aEvent.sourceClass = "AiAssistantPanel";
    aEvent.action = "aiPromptSubmit";
    aEvent.textData = rRequest.prompt;
    aEvent.timestampMs = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();

    EventListenerRegistry::instance().dispatchBefore(aEvent);
    m_aActiveStreamEvent = aEvent;
    m_bStreamAfterPending = true;

    if (!m_xPanelAdapter)
        m_xPanelAdapter = std::make_shared<PanelUiAdapter>();
    m_xPanelAdapter->alive = true;
    m_xPanelAdapter->pPanel = this;

    beginNewResponse();
    setAsyncBusyUiActive(true);
    m_aPendingUserPrompt = rRequest.prompt;
    setStatus(u"AI 요청 처리 중..."_ustr);

    if (!m_xPromptJob)
        m_xPromptJob = std::make_shared<lofice::ai::AiAsyncPromptJob>();

    m_xPromptJob->start(rRequest, LINK(this, AiAssistantPanel, OnAsyncPromptEvent));
    return true;
}

void AiAssistantPanel::fireAiHookEvent(const OUString& rAction, std::u16string_view rTextData)
{
    EventPayload aEvent;
    aEvent.category = EventCategory::Command;
    aEvent.sourceModule =
        m_eAppKind == LoficeAppKind::Writer ? "sw" :
        m_eAppKind == LoficeAppKind::Calc ? "sc" :
        m_eAppKind == LoficeAppKind::Impress || m_eAppKind == LoficeAppKind::Draw ? "sd" : "lofice";
    aEvent.sourceFile = "AiAssistantPanel.cxx";
    aEvent.sourceClass = "AiAssistantPanel";
    aEvent.action = OUStringToOString(rAction, RTL_TEXTENCODING_UTF8).getStr();
    aEvent.unoCommand = OUStringToOString(OUString(rTextData), RTL_TEXTENCODING_UTF8).getStr();
    aEvent.textData = OUStringToOString(OUString(rTextData), RTL_TEXTENCODING_UTF8).getStr();
    aEvent.timestampMs = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();

    EventListenerRegistry::instance().dispatchBefore(aEvent);

    std::string aHookText = OUStringToOString(OUString(rTextData), RTL_TEXTENCODING_UTF8).getStr();
    const bool bModified = EventListenerRegistry::instance().dispatchTextHook(aHookText, aEvent);
    if (bModified)
        appendResponseLine(OStringToOUString(aHookText, RTL_TEXTENCODING_UTF8));

    EventListenerRegistry::instance().dispatchAfter(aEvent);
}

IMPL_LINK(AiAssistantPanel, OnQuickActionClicked, weld::Button&, rButton, void)
{
    for (std::size_t i = 0; i < kMaxQuickActionSlots; ++i)
    {
        if (m_aQuickActionButtons[i].get() == &rButton)
        {
            if (!m_aQuickActionCommands[i].isEmpty())
            {
                const std::u16string_view aCmd(
                    m_aQuickActionCommands[i].getStr(), m_aQuickActionCommands[i].getLength());
                const std::string aPromptUtf8 = m_aQuickActionPrompts[i].isEmpty()
                    ? std::string{}
                    : OUStringToOString(m_aQuickActionPrompts[i], RTL_TEXTENCODING_UTF8).getStr();
                const bool bHadPrompt = !aPromptUtf8.empty();
                const bool bWasBusy = isAsyncBusy();
                const bool bAutoSend = lofice::ai::loadSettings().autoSendQuickActionPrompt;
                dispatchQuickAction(aCmd, aPromptUtf8);
                if (!bHadPrompt)
                    setStatus(u"실행됨: "_ustr + m_aQuickActionCommands[i]);
                else if (bWasBusy)
                    setStatus(u"실행됨 + AI 프롬프트 입력: "_ustr + m_aQuickActionCommands[i]);
                else if (!bAutoSend)
                    setStatus(u"실행됨 + 프롬프트 입력 (자동 전송 OFF): "_ustr + m_aQuickActionCommands[i]);
            }
            return;
        }
    }
}

IMPL_LINK_NOARG(AiAssistantPanel, OnSendClicked, weld::Button&, void)
{
    submitPromptFromUi();
}

IMPL_LINK_NOARG(AiAssistantPanel, OnCancelAsyncClicked, weld::Button&, void)
{
    if (m_xStreamJob && m_xStreamJob->isRunning())
    {
        m_xStreamJob->cancel();
        m_aPendingUserPrompt.clear();
        setStatus(u"AI 요청 취소 중..."_ustr);
        return;
    }

    if (m_xPromptJob && m_xPromptJob->isRunning())
    {
        m_xPromptJob->cancel();
        m_aPendingUserPrompt.clear();
        setAsyncBusyUiActive(false);
        setStatus(u"AI 요청 취소 중..."_ustr);
        return;
    }

    if (m_xHttpJob && m_xHttpJob->isRunning())
    {
        m_xHttpJob->cancel();
        setAsyncBusyUiActive(false);
        setStatus(u"연결 테스트를 취소했습니다"_ustr);
        return;
    }

    if (m_xRagJob && m_xRagJob->isRunning())
    {
        m_xRagJob->cancel();
        setAsyncBusyUiActive(false);
        setStatus(u"RAG 테스트를 취소했습니다"_ustr);
    }
}

IMPL_LINK(AiAssistantPanel, OnAsyncStreamEvent, void*, pData, void)
{
    std::unique_ptr<lofice::ai::AiStreamEventPayload> pPayload(
        static_cast<lofice::ai::AiStreamEventPayload*>(pData));

    if (!m_xPanelAdapter || !m_xPanelAdapter->alive.load() || m_xPanelAdapter->pPanel != this)
        return;

    SolarMutexGuard aGuard;

    switch (pPayload->kind)
    {
        case lofice::ai::AiStreamEventKind::Status:
            setStatus(OStringToOUString(
                OString(pPayload->text.c_str(), static_cast<sal_Int32>(pPayload->text.size()),
                    RTL_TEXTENCODING_UTF8),
                RTL_TEXTENCODING_UTF8));
            break;
        case lofice::ai::AiStreamEventKind::Token:
            appendResponseToken(pPayload->text);
            break;
        case lofice::ai::AiStreamEventKind::Finished:
            handleStreamFinished(pPayload->success, pPayload->text);
            setAsyncBusyUiActive(false);
            if (m_bStreamAfterPending)
            {
                EventListenerRegistry::instance().dispatchAfter(m_aActiveStreamEvent);
                m_bStreamAfterPending = false;
            }
            break;
    }
}

IMPL_LINK(AiAssistantPanel, OnAsyncPromptEvent, void*, pData, void)
{
    std::unique_ptr<lofice::ai::AiStreamEventPayload> pPayload(
        static_cast<lofice::ai::AiStreamEventPayload*>(pData));

    if (!m_xPanelAdapter || !m_xPanelAdapter->alive.load() || m_xPanelAdapter->pPanel != this)
        return;

    SolarMutexGuard aGuard;

    switch (pPayload->kind)
    {
        case lofice::ai::AiStreamEventKind::Status:
            setStatus(OStringToOUString(
                OString(pPayload->text.c_str(), static_cast<sal_Int32>(pPayload->text.size()),
                    RTL_TEXTENCODING_UTF8),
                RTL_TEXTENCODING_UTF8));
            break;
        case lofice::ai::AiStreamEventKind::Token:
            appendResponseToken(pPayload->text);
            break;
        case lofice::ai::AiStreamEventKind::Finished:
            handlePromptFinished(pPayload->success, pPayload->text, pPayload->provider);
            setAsyncBusyUiActive(false);
            if (m_bStreamAfterPending)
            {
                EventListenerRegistry::instance().dispatchAfter(m_aActiveStreamEvent);
                m_bStreamAfterPending = false;
            }
            break;
    }
}

IMPL_LINK(AiAssistantPanel, OnAsyncRagEvent, void*, pData, void)
{
    std::unique_ptr<lofice::ai::AiRagUiPayload> pPayload(
        static_cast<lofice::ai::AiRagUiPayload*>(pData));

    if (!m_xPanelAdapter || !m_xPanelAdapter->alive.load() || m_xPanelAdapter->pPanel != this)
        return;

    SolarMutexGuard aGuard;

    setAsyncBusyUiActive(false);

    if (pPayload->errorDetail == "Request cancelled.")
    {
        setStatus(u"RAG 테스트를 취소했습니다"_ustr);
        return;
    }

    if (!pPayload->success || pPayload->context.empty())
    {
        setStatus(u"RAG 테스트 실패 — rag-server 실행 및 DATABASE_URL 확인"_ustr);
        return;
    }

    const std::string& rContext = pPayload->context;
    const OUString aPreview = OStringToOUString(
        OString(rContext.c_str(),
            static_cast<sal_Int32>(std::min(rContext.size(), std::size_t{1200})),
            RTL_TEXTENCODING_UTF8),
        RTL_TEXTENCODING_UTF8);
    m_xResponseText->set_text(u"[RAG 테스트]\n"_ustr + aPreview);
    setStatus(u"RAG 테스트 성공 — 컨텍스트 "_ustr
        + OUString::number(static_cast<sal_Int32>(rContext.size())) + u"자"_ustr);
}

IMPL_LINK(AiAssistantPanel, OnAsyncHttpEvent, void*, pData, void)
{
    std::unique_ptr<lofice::ai::AiHttpUiPayload> pPayload(
        static_cast<lofice::ai::AiHttpUiPayload*>(pData));

    if (!m_xPanelAdapter || !m_xPanelAdapter->alive.load() || m_xPanelAdapter->pPanel != this)
        return;

    SolarMutexGuard aGuard;

    setConnectionTestUiActive(false);

    if (pPayload->success)
    {
        m_xResponseText->set_text(OUString::createFromAscii(pPayload->content));
        setStatus(u"연결 테스트 성공"_ustr);
        return;
    }

    if (pPayload->errorDetail == "Request cancelled.")
    {
        setStatus(u"연결 테스트를 취소했습니다"_ustr);
        return;
    }

    appendResponseLine(u"[연결 테스트 실패] "_ustr
        + OUString::createFromAscii(pPayload->errorDetail));
    setStatus(u"연결 테스트 실패"_ustr);
}

IMPL_LINK_NOARG(AiAssistantPanel, OnClearPromptClicked, weld::Button&, void)
{
    m_xPromptText->set_text(OUString());
    setStatus(u"프롬프트를 지웠습니다"_ustr);
}

IMPL_LINK_NOARG(AiAssistantPanel, OnClearResponseClicked, weld::Button&, void)
{
    m_xResponseText->set_text(OUString());
    m_aConversation.clear();
    m_aPendingUserPrompt.clear();
    lofice::ai::saveConversationHistory(m_aConversation);
    setStatus(u"응답 및 대화 기록을 지웠습니다"_ustr);
}

IMPL_LINK_NOARG(AiAssistantPanel, OnExportHistoryClicked, weld::Button&, void)
{
    if (m_xStreamJob && m_xStreamJob->isRunning())
    {
        setStatus(u"AI 요청이 진행 중입니다 — 완료 후 내보내세요"_ustr);
        return;
    }

    const lofice::ai::ConversationFileOutcome aOutcome
        = lofice::ai::exportConversationToFile(GetFrameWeld(), m_aConversation);

    if (aOutcome.result == lofice::ai::ConversationFileResult::Cancelled)
        return;

    if (aOutcome.result == lofice::ai::ConversationFileResult::Error)
    {
        setStatus(u"대화 내보내기 실패"_ustr);
        return;
    }

    setStatus(u"대화 기록을 JSON 파일로 내보냈습니다"_ustr);
}

IMPL_LINK_NOARG(AiAssistantPanel, OnImportHistoryClicked, weld::Button&, void)
{
    if (m_xStreamJob && m_xStreamJob->isRunning())
    {
        setStatus(u"AI 요청이 진행 중입니다 — 완료 후 가져오세요"_ustr);
        return;
    }

    lofice::ai::ConversationFileOutcome aOutcome
        = lofice::ai::importConversationFromFile(GetFrameWeld(), m_aConversation);

    if (aOutcome.result == lofice::ai::ConversationFileResult::Cancelled)
        return;

    if (aOutcome.result == lofice::ai::ConversationFileResult::Error)
    {
        setStatus(u"대화 가져오기 실패 — JSON 형식을 확인하세요"_ustr);
        return;
    }

    m_aPendingUserPrompt.clear();
    restoreLastResponseFromHistory();
    lofice::ai::saveConversationHistory(m_aConversation);
    setStatus(u"대화 기록을 가져왔습니다 — "_ustr
        + OUString::number(static_cast<sal_Int32>(m_aConversation.size())) + u"메시지"_ustr);
}

IMPL_LINK_NOARG(AiAssistantPanel, OnToggleSettingsClicked, weld::Button&, void)
{
    toggleSettingsVisibility();
}

IMPL_LINK_NOARG(AiAssistantPanel, OnUseBuiltinToggled, weld::Toggleable&, void)
{
    updateSettingsFieldSensitivity();
}

IMPL_LINK_NOARG(AiAssistantPanel, OnUseRagToggled, weld::Toggleable&, void)
{
    updateSettingsFieldSensitivity();
}

IMPL_LINK_NOARG(AiAssistantPanel, OnSaveSettingsClicked, weld::Button&, void)
{
    const lofice::ai::AiSettings aSettings = collectSettingsFromUi();
    lofice::ai::saveSettings(aSettings);
    setStatus(u"AI 설정을 저장했습니다"_ustr);
}

IMPL_LINK_NOARG(AiAssistantPanel, OnTestConnectionClicked, weld::Button&, void)
{
    const lofice::ai::AiSettings aSettings = collectSettingsFromUi();
    if (aSettings.useBuiltinAi || aSettings.endpoint.isEmpty())
    {
        setStatus(u"외부 API 엔드포인트를 입력하세요"_ustr);
        return;
    }

    if (!lofice::ai::isHttpTransportAvailable())
    {
        setStatus(u"curl 빌드가 비활성화되어 연결 테스트를 할 수 없습니다"_ustr);
        return;
    }

    if (m_xHttpJob && m_xHttpJob->isRunning())
    {
        setStatus(u"연결 테스트가 진행 중입니다"_ustr);
        return;
    }

    if (m_xStreamJob && m_xStreamJob->isRunning())
    {
        setStatus(u"AI 요청이 진행 중입니다 — 완료 후 테스트하세요"_ustr);
        return;
    }

    if (m_xRagJob && m_xRagJob->isRunning())
    {
        setStatus(u"RAG 테스트가 진행 중입니다 — 완료 후 테스트하세요"_ustr);
        return;
    }

    if (!m_xPanelAdapter)
        m_xPanelAdapter = std::make_shared<PanelUiAdapter>();
    m_xPanelAdapter->alive = true;
    m_xPanelAdapter->pPanel = this;

    setConnectionTestUiActive(true);
    setStatus(u"API 연결 테스트 중..."_ustr);

    const lofice::ai::AiPromptRequest aRequest = buildPromptRequest("ping", false);
    const lofice::ai::AiHttpConfig aConfig = lofice::ai::toHttpConfig(aSettings);

    if (!m_xHttpJob)
        m_xHttpJob = std::make_shared<lofice::ai::AiAsyncHttpJob>();

    m_xHttpJob->start(aConfig, aRequest, LINK(this, AiAssistantPanel, OnAsyncHttpEvent));
}

IMPL_LINK_NOARG(AiAssistantPanel, OnTestRagClicked, weld::Button&, void)
{
    const lofice::ai::AiSettings aSettings = collectSettingsFromUi();
    if (!aSettings.useRagContext || aSettings.ragEndpoint.isEmpty())
    {
        setStatus(u"RAG를 사용하도록 설정하고 서버 URL을 입력하세요"_ustr);
        return;
    }

    if (!lofice::ai::isHttpTransportAvailable())
    {
        setStatus(u"curl 빌드가 비활성화되어 RAG 테스트를 할 수 없습니다"_ustr);
        return;
    }

    if (m_xStreamJob && m_xStreamJob->isRunning())
    {
        setStatus(u"AI 요청이 진행 중입니다 — 완료 후 테스트하세요"_ustr);
        return;
    }

    if (m_xRagJob && m_xRagJob->isRunning())
    {
        setStatus(u"RAG 테스트가 진행 중입니다"_ustr);
        return;
    }

    if (!m_xPanelAdapter)
        m_xPanelAdapter = std::make_shared<PanelUiAdapter>();
    m_xPanelAdapter->alive = true;
    m_xPanelAdapter->pPanel = this;

    setAsyncBusyUiActive(true);
    setStatus(u"코드베이스 검색 중..."_ustr);

    const lofice::ai::AiRagSettings aRag = lofice::ai::toRagSettings(aSettings);

    if (!m_xRagJob)
        m_xRagJob = std::make_shared<lofice::ai::AiAsyncRagJob>();

    m_xRagJob->start(aRag, "lofice AI RAG test", LINK(this, AiAssistantPanel, OnAsyncRagEvent));
}

void AiAssistantPanel::NotifyItemUpdate(
    sal_uInt16 /*nSId*/,
    SfxItemState /*eState*/,
    const SfxPoolItem* /*pState*/)
{
}

void AiAssistantPanel::GetControlState(
    sal_uInt16 /*nSId*/,
    boost::property_tree::ptree& /*rState*/)
{
}

} // namespace lofice::ui
