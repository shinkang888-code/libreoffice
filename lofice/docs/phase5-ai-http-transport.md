# Phase 5: 외부 AI HTTP 연동

> **목표:** `LOFICE_AI_ENDPOINT`로 OpenAI 호환 Chat API 호출  
> **기술:** curl + JSON (boost property_tree), Phase 4 Hook 파이프라인 유지

---

## 1. 파이프LINE

```
Send to AI
  → DefaultAiListener::onTextHook
  → AiPromptService::processPrompt
  → AiHttpConfig::fromEnvironment()
  → AiHttpTransport::postChatCompletion (curl POST)
  → JSON 응답 파싱 → Response 영역
```

`LOFICE_AI_ENDPOINT` 미설정 시 Phase 4 **내장 AI**로 폴백.

---

## 2. 환경 변수

| 변수 | 필수 | 설명 |
|------|------|------|
| `LOFICE_AI_ENDPOINT` | 예 (외부 AI) | Chat Completions URL |
| `LOFICE_AI_API_KEY` | 아니오 | `Authorization: Bearer` 헤더 |
| `LOFICE_AI_MODEL` | 아니오 | JSON `model` (기본 `lofice-default`) |
| `LOFICE_AI_TIMEOUT` | 아니오 | 초 단위 타임아웃 (5–120, 기본 30) |

### PowerShell 예시 (OpenAI)

```powershell
$env:LOFICE_AI_ENDPOINT = "https://api.openai.com/v1/chat/completions"
$env:LOFICE_AI_API_KEY = "sk-..."
$env:LOFICE_AI_MODEL = "gpt-4o-mini"
```

### Ollama / 로컬 API 예시

```powershell
$env:LOFICE_AI_ENDPOINT = "http://127.0.0.1:11434/v1/chat/completions"
$env:LOFICE_AI_MODEL = "llama3"
```

---

## 3. 요청/응답 형식

**요청** — OpenAI Chat Completions 호환:

```json
{
  "model": "lofice-default",
  "messages": [
    {"role": "system", "content": "You are lofice AI assistant for Writer..."},
    {"role": "user", "content": "..."}
  ]
}
```

**응답 파싱 순서:**
1. `choices[0].message.content` (OpenAI chat)
2. `choices[0].text` (completions)
3. `response`, `text`, `content`, `message`, `output`
4. `error.message`

---

## 4. 신규 파일

| 파일 | 역할 |
|------|------|
| `lofice/include/lofice/ai/AiHttpConfig.hxx` | env → config struct |
| `lofice/source/ai/AiHttpConfig.cxx` | 환경 변수 읽기 |
| `lofice/include/lofice/ai/AiHttpTransport.hxx` | `postChatCompletion()` |
| `lofice/source/ai/AiHttpTransport.cxx` | curl POST (ENABLE_CURL) |

`Library_lofice.mk`: `boost_headers`, `curl`(조건부), `officecfg/registry`

---

## 5. UI 한국어

- `sidebar_ai_assistant.ui` — 프롬프트/응답/버튼 한국어 라벨
- `AiAssistantPanel` — 상태 메시지 한국어

---

## 6. 빌드

```bash
make lofice sw sc sd
```

`ENABLE_CURL=NO` 빌드 시 HTTP 비활성 — endpoint 설정해도 오류 메시지 표시.

---

*Copyright (c) Lonex. Inc. All Rights Reserved.*
