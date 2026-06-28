# Phase 4: AI Backend Hook

> **목표:** AI Assistant 패널 Prompt → 응답 파이프라인을 `IEventListener`와 연결  
> **원칙:** 코어 변경 없음, `lofice::ai` 네임스페이스로 백엔드 분리

---

## 1. 파이프라인

```
[AiAssistantPanel] Send to AI
      │  fireAiHookEvent("aiPromptSubmit", prompt)
      ▼
[EventListenerRegistry] dispatchTextHook()
      │  DefaultAiListener::onTextHook()
      ▼
[AiPromptService] processPrompt()
      │  builtin stub | LOFICE_AI_ENDPOINT (향후 HTTP)
      ▼
Response → 패널 text_response 영역
```

---

## 2. 신규 파일

| 파일 | 역할 |
|------|------|
| `lofice/include/lofice/ai/AiPromptService.hxx` | Prompt 요청/결과 struct, `processPrompt()` |
| `lofice/source/ai/AiPromptService.cxx` | Builtin 응답 + env 감지 |
| `lofice/include/lofice/ai/DefaultAiListener.hxx` | 기본 `IEventListener` |
| `lofice/source/ai/DefaultAiListener.cxx` | `aiPromptSubmit` → `processPrompt()` |
| `lofice/include/lofice/ai/AiServiceBootstrap.hxx` | `ensureDefaultAiListener()` |
| `lofice/source/ai/AiServiceBootstrap.cxx` | 1회 등록 |

패널 생성 시 `AiAssistantPanel` 생성자에서 `ensureDefaultAiListener()` 호출.

---

## 3. Builtin AI (기본)

등록된 리스너가 없어도 **DefaultAiListener**가 자동 등록되어 Prompt에 응답합니다.

- 앱 컨텍스트(Writer/Calc/Impress) 반영
- 키워드 기반 퀵액션 힌트 (bold, undo, sum, slide 등)
- 커스텀 AI는 `IEventListener` 추가 등록으로 덮어쓰기 가능

---

## 4. 외부 AI 연동

### 환경 변수

```powershell
$env:LOFICE_AI_ENDPOINT = "https://your-api.example/v1/chat"
```

설정 시 응답에 endpoint 정보가 표시됩니다. HTTP 클라이언트는 후속 Phase에서 `AiPromptService`에 연결합니다.

### 커스텀 리스너

```cpp
class MyAiListener : public lofice::IEventListener {
    bool onTextHook(std::string& rText, const EventPayload& ctx) override {
        if (ctx.action != "aiPromptSubmit") return false;
        rText = callYourApi(rText);
        return true;
    }
    // ...
};
lofice::EventListenerRegistry::instance().addListener(
    std::make_shared<MyAiListener>());
```

동일 ID로 등록하면 DefaultAiListener를 대체합니다.

---

## 5. 빌드

```bash
make lofice sw sc sd
```

---

*Copyright (c) Lonex. Inc. All Rights Reserved.*
