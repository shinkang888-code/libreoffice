/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * lofice — AI Assistant sidebar panel
 *
 * Copyright (c) Lonex. Inc. All Rights Reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_LOFICE_UI_AIASSISTANTPANEL_HXX
#define INCLUDED_LOFICE_UI_AIASSISTANTPANEL_HXX

#include <lofice/IEventListener.hxx>
#include <lofice/ai/AiAsyncHttpJob.hxx>
#include <lofice/ai/AiAsyncPromptJob.hxx>
#include <lofice/ai/AiAsyncRagJob.hxx>
#include <lofice/ai/AiAsyncStreamJob.hxx>
#include <lofice/ai/AiConversationHistory.hxx>
#include <lofice/ai/AiPromptService.hxx>
#include <lofice/ai/AiRagContext.hxx>
#include <lofice/ai/AiSettingsStore.hxx>
#include <lofice/ui/LoficeSidebarLayout.hxx>
#include <lofice/ui/QuickActionCatalog.hxx>
#include <lofice/ui/ToolbarDispatchBridge.hxx>

#include <sfx2/sidebar/PanelLayout.hxx>
#include <sfx2/sidebar/ControllerItem.hxx>

#include <boost/property_tree/ptree.hpp>

#include <com/sun/star/frame/XFrame.hpp>

#include <array>
#include <atomic>
#include <memory>
#include <string_view>

#include <vcl/weld/Button.hxx>
#include <vcl/weld/CheckButton.hxx>
#include <vcl/weld/Entry.hxx>
#include <vcl/weld/Label.hxx>
#include <vcl/weld/SpinButton.hxx>
#include <vcl/weld/TextView.hxx>
#include <vcl/weld/Widget.hxx>

class SfxBindings;

namespace lofice::ui
{

class AiAssistantPanel final
    : public PanelLayout
    , public sfx2::sidebar::ControllerItem::ItemUpdateReceiverInterface
{
public:
    static std::unique_ptr<PanelLayout> Create(
        weld::Widget* pParent,
        const css::uno::Reference<css::frame::XFrame>& rxFrame,
        SfxBindings* pBindings);

    AiAssistantPanel(
        weld::Widget* pParent,
        const css::uno::Reference<css::frame::XFrame>& rxFrame,
        SfxBindings* pBindings);

    ~AiAssistantPanel() override;

    void NotifyItemUpdate(
        sal_uInt16 nSId,
        SfxItemState eState,
        const SfxPoolItem* pState) override;

    void GetControlState(
        sal_uInt16 nSId,
        boost::property_tree::ptree& rState) override;

private:
    struct PanelUiAdapter
    {
        std::atomic<bool> alive{ true };
        AiAssistantPanel* pPanel = nullptr;
    };

    void setupQuickActionGrid(LoficeAppKind eApp);
    void loadSettingsIntoUi();
    lofice::ai::AiSettings collectSettingsFromUi() const;
    void updateSettingsFieldSensitivity();
    void toggleSettingsVisibility();
    void dispatchQuickAction(std::u16string_view rUnoCommand, std::string_view rAiPromptUtf8);
    void fillPromptFromQuickAction(std::string_view rAiPromptUtf8);
    void submitPromptFromUi();
    bool isAsyncBusy() const;
    void beginNewResponse();
    void appendResponseLine(const OUString& rLine);
    void appendResponseToken(std::string_view rToken);
    void handleStreamFinished(bool bSuccess, std::string_view rErrorDetail);
    void handlePromptFinished(bool bSuccess, std::string_view rResponseUtf8, std::string_view rProvider);
    void commitConversationTurn(const std::string& rUserPromptUtf8);
    void restoreLastResponseFromHistory();
    void fireAiHookEvent(const OUString& rAction, std::u16string_view rTextData);
    lofice::ai::AiPromptRequest buildPromptRequest(const std::string& rPromptUtf8, bool bIncludeHistory) const;
    bool trySendExternalStreaming(const lofice::ai::AiPromptRequest& rRequest);
    bool trySendAsyncPrompt(const lofice::ai::AiPromptRequest& rRequest);
    void setAsyncBusyUiActive(bool bActive);
    void setStreamingUiActive(bool bActive);
    void setConnectionTestUiActive(bool bActive);
    void setStatus(const OUString& rText);

    DECL_LINK(OnQuickActionClicked, weld::Button&, void);
    DECL_LINK(OnSendClicked, weld::Button&, void);
    DECL_LINK(OnCancelAsyncClicked, weld::Button&, void);
    DECL_LINK(OnAsyncStreamEvent, void*, void);
    DECL_LINK(OnAsyncPromptEvent, void*, void);
    DECL_LINK(OnAsyncHttpEvent, void*, void);
    DECL_LINK(OnAsyncRagEvent, void*, void);
    DECL_LINK(OnClearPromptClicked, weld::Button&, void);
    DECL_LINK(OnClearResponseClicked, weld::Button&, void);
    DECL_LINK(OnExportHistoryClicked, weld::Button&, void);
    DECL_LINK(OnImportHistoryClicked, weld::Button&, void);
    DECL_LINK(OnToggleSettingsClicked, weld::Button&, void);
    DECL_LINK(OnUseBuiltinToggled, weld::Toggleable&, void);
    DECL_LINK(OnUseRagToggled, weld::Toggleable&, void);
    DECL_LINK(OnSaveSettingsClicked, weld::Button&, void);
    DECL_LINK(OnTestConnectionClicked, weld::Button&, void);
    DECL_LINK(OnTestRagClicked, weld::Button&, void);

    LoficeSidebarLayout m_aLayout;
    LoficeAppKind m_eAppKind;
    bool m_bSettingsVisible = false;
    bool m_bStreamAfterPending = false;
    std::string m_aPendingUserPrompt;
    lofice::ai::AiConversationHistory m_aConversation;
    EventPayload m_aActiveStreamEvent;
    std::shared_ptr<PanelUiAdapter> m_xPanelAdapter;
    std::shared_ptr<lofice::ai::AiAsyncStreamJob> m_xStreamJob;
    std::shared_ptr<lofice::ai::AiAsyncPromptJob> m_xPromptJob;
    std::shared_ptr<lofice::ai::AiAsyncHttpJob> m_xHttpJob;
    std::shared_ptr<lofice::ai::AiAsyncRagJob> m_xRagJob;
    std::unique_ptr<ToolbarDispatchBridge> m_xDispatchBridge;

    std::unique_ptr<weld::Label> m_xTitleLabel;
    std::unique_ptr<weld::Label> m_xAppContextLabel;
    std::unique_ptr<weld::Button> m_xToggleSettingsButton;
    std::unique_ptr<weld::Widget> m_xSettingsGrid;
    std::unique_ptr<weld::CheckButton> m_xUseBuiltinCheck;
    std::unique_ptr<weld::CheckButton> m_xAutoSendQuickActionCheck;
    std::unique_ptr<weld::Entry> m_xEndpointEntry;
    std::unique_ptr<weld::Entry> m_xApiKeyEntry;
    std::unique_ptr<weld::Entry> m_xModelEntry;
    std::unique_ptr<weld::SpinButton> m_xTimeoutSpin;
    std::unique_ptr<weld::CheckButton> m_xUseRagCheck;
    std::unique_ptr<weld::Entry> m_xRagEndpointEntry;
    std::unique_ptr<weld::SpinButton> m_xRagTimeoutSpin;
    std::unique_ptr<weld::Button> m_xTestRagButton;
    std::unique_ptr<weld::Button> m_xSaveSettingsButton;
    std::unique_ptr<weld::Button> m_xTestConnectionButton;
    std::unique_ptr<weld::TextView> m_xPromptText;
    std::unique_ptr<weld::TextView> m_xResponseText;
    std::unique_ptr<weld::Label> m_xStatusLabel;
    std::unique_ptr<weld::Button> m_xSendButton;
    std::unique_ptr<weld::Button> m_xCancelAsyncButton;
    std::unique_ptr<weld::Button> m_xClearPromptButton;
    std::unique_ptr<weld::Button> m_xExportHistoryButton;
    std::unique_ptr<weld::Button> m_xImportHistoryButton;
    std::unique_ptr<weld::Button> m_xClearResponseButton;

    std::array<std::unique_ptr<weld::Button>, kMaxQuickActionSlots> m_aQuickActionButtons;
    std::array<OUString, kMaxQuickActionSlots> m_aQuickActionCommands;
    std::array<OUString, kMaxQuickActionSlots> m_aQuickActionPrompts;
};

} // namespace lofice::ui

#endif // INCLUDED_LOFICE_UI_AIASSISTANTPANEL_HXX
