# Phase 12 — 멀티턴 대화 컨텍스트

외부 API 호출 시 OpenAI `messages[]`에 **이전 user/assistant 턴**을 포함합니다.

## 데이터 흐름

```
Send to AI
  → AiConversationHistory::populatePriorMessages()
  → AiPromptRequest.priorMessages + prompt (현재 턴)
  → buildChatRequestJson()
       system + priorMessages[] + user(prompt)
  → 스트리밍 완료 / 내장 AI 응답
  → commitConversationTurn(user, assistant)
```

## 주요 파일

| 파일 | 역할 |
|------|------|
| `lofice/include/lofice/ai/AiConversationHistory.hxx` | 턴 저장 (최대 20 messages) |
| `lofice/source/ai/AiConversationHistory.cxx` | append / trim |
| `lofice/include/lofice/ai/AiPromptService.hxx` | `priorMessages` 필드 |
| `lofice/source/ai/AiHttpTransport.cxx` | JSON messages 배열 생성 |
| `AiAssistantPanel` | `m_aConversation`, `commitConversationTurn` |

## UI 동작

- **응답 지우기** → 응답 텍스트 + **대화 기록** 초기화
- **연결 테스트** (`ping`) → 히스토리 미포함
- **취소** → pending user prompt 폐기 (히스토리에 미반영)

## 제한

- 세션 메모리만 — 앱 재시작 시 초기화
- 내장 AI는 히스토리를 API에 보내지 않으나, 턴은 기록되어 이후 외부 API 전환 시 활용
- 최대 20 messages (약 10턴) — 토큰 폭주 방지

## 다음 단계 후보 (Phase 13)

- 대화 기록 officecfg 영속화
- pgvector embedding 시맨틱 코드 검색 연동
