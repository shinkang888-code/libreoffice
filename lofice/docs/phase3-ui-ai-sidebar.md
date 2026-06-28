# Phase 3: UI 커스텀 — AI Assistant 사이드바

> **기준:** `framework/source/uielement/buttontoolbarcontroller.cxx` (사용자 `@toolbar.cpp` → 실제 경로)  
> **원칙:** 코어 엔진 유지, UI 껍데기만 lofice 교체, `.uno:` 디스패치 포인터 연결 보존

---

## 1. 아키텍처 요약

```
[기존 툴바] ButtonToolbarController::execute()
      │  XFrame → XDispatchProvider → XDispatch(.uno:Bold)
      ▼
[lofice AI 패널] ToolbarDispatchBridge::dispatchCommand()
      │  동일 경로 — SfxBindings* 유지
      ▼
SfxDispatcher → SwWrtShell / ScDocShell / SdViewShell  (코어 변경 없음)
```

| 계층 | lofice 파일 | 역할 |
|------|------------|------|
| 레이아웃 Struct | `lofice/include/lofice/ui/LoficeSidebarLayout.hxx` | 사이드바 치수·위젯 ID·Deck/Panel ID |
| 퀵액션 카탈로그 | `lofice/include/lofice/ui/QuickActionCatalog.hxx` | 앱별 16슬롯 UNO 명령 필터 |
| 디스패치 브릿지 | `lofice/include/lofice/ui/ToolbarDispatchBridge.hxx` | 기존 `.uno:` 슬롯 연결 보존 |
| AI 패널 | `lofice/source/ui/AiAssistantPanel.*` | 4×4 퀵액션 + Prompt/Response |
| 통합 | `lofice/source/ui/LoficePanelIntegration.cxx` | PanelFactory 공통 진입점 |
| UI XML | `lofice/uiconfig/modules/lofice/ui/sidebar_ai_assistant.ui` | GtkGrid 레이아웃 |
| AI Hook | `lofice/include/lofice/IEventListener.hxx` | Phase 2 이벤트 후킹 |

---

## 2. LoficeSidebarLayout Struct

```cpp
struct LoficeSidebarLayout {
    SidebarWidthConstraints width;      // min 260 / default 340 / max 520 px
    AiPanelRowWeights rowWeights;       // Prompt:1, Response:2
    // weld widget IDs + kUiResourcePath
};
constexpr std::string_view kAiAssistantDeckId = "LoficeAiDeck";
```

---

## 3. 등록 (officecfg + PanelFactory)

| 앱 | Panel ID | ImplementationURL | Factory |
|----|----------|-------------------|---------|
| Writer | `SwLoficeAiAssistantPanel` | `SwPanelFactory/AiAssistantPanel` | `SwPanelFactory.cxx` |
| Calc | `ScLoficeAiAssistantPanel` | `ScPanelFactory/AiAssistantPanel` | `ScPanelFactory.cxx` |
| Draw/Impress | `SdLoficeAiAssistantPanel` | `SdPanelFactory/AiAssistantPanel` | `PanelFactory.cxx` |

공통 Deck: `LoficeAiDeck` (Sidebar.xcu OrderIndex 50)

모든 Factory는 `rsResourceURL.endsWith("/AiAssistantPanel")` → `lofice::ui::createAiAssistantSidebarPanel()`

---

## 4. Quick Actions — 4×4 그리드 (앱 자동 감지)

`ModuleManager::identify(XFrame)`으로 Writer/Calc/Impress/Draw를 감지하고, `QuickActionCatalog`가 최대 16개 슬롯을 채웁니다.

| 범주 | 예시 UNO 명령 |
|------|--------------|
| Universal | Bold, Italic, Underline, Undo, Redo, Cut, Copy, Paste, Save, Find, Print, PDF, Sidebar |
| Writer | Spell, InsertTable |
| Calc | Calculate, AutoSum |
| Impress/Draw | InsertPageQuick, Presentation |

퀵액션 버튼은 **새 로직 없이** `ToolbarDispatchBridge::dispatchCommand()`로 기존 UNO 슬롯을 호출합니다.

추가 UI:
- **Clear Prompt** / **Clear Response**
- **Context:** 라벨 (Writer/Calc/Impress/Draw)
- **Send to AI** → `EventListenerRegistry` Hook (`aiPromptSubmit`)

---

## 5. 빌드

```bash
make lofice sw sc sd
```

- `lofice/Module_lofice.mk` — Library_lofice + UIConfig_lofice
- `sw/Library_sw.mk`, `sc/Library_sc.mk`, `sd/Library_sd.mk` — `lofice` 링크 + include path

---

## 6. 사용법

1. Writer / Calc / Impress / Draw 실행
2. 사이드바에서 **AI Assistant** 덱 선택
3. 4×4 퀵액션으로 Bold·Undo·AutoSum 등 즉시 실행
4. Prompt 입력 → **Send to AI** (EventListenerRegistry Hook)
5. 외부 AI는 `IEventListener::onTextHook` 등록으로 응답 주입

---

*Copyright (c) Lonex. Inc. All Rights Reserved.*
