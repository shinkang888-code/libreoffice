# Phase 14 — 마지막 assistant 응답 UI 복원

Phase 13에서 officecfg에 저장된 대화 기록을 불러올 때, **응답 영역**에 마지막 assistant 메시지를 표시합니다.

## 동작

```
패널 생성
  → loadConversationHistory()
  → restoreLastResponseFromHistory()
       → AiConversationHistory::lastAssistantMessage()
       → text_response에 표시
```

- API 컨텍스트(`priorMessages`)와 화면 응답이 일치
- **응답 지우기** 시 UI + officecfg 모두 초기화 (기존 Phase 13)

## 주요 파일

- `lofice/include/lofice/ai/AiConversationHistory.hxx` — `lastAssistantMessage()`
- `lofice/source/ui/AiAssistantPanel.*` — `restoreLastResponseFromHistory()`

## 다음 단계 후보 (Phase 15)

- pgvector OpenAI embedding 시맨틱 코드 검색 (`lofice/scripts`)
- 마지막 user 프롬프트 입력란 복원 (선택)
