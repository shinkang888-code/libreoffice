# Phase 11 — 스트리밍·연결 테스트 취소 버튼

AI 스트리밍 또는 연결 테스트 진행 중 **취소** 버튼을 표시하고, Phase 10 curl abort와 연동합니다.

## UI

- `sidebar_ai_assistant.ui` — `btn_cancel_async` (기본 숨김)
- 스트리밍/연결 테스트 시작 → **취소** 표시, Send·프롬프트 지우기 비활성
- 완료/취소/오류 → **취소** 숨김, 버튼 복구

## 동작

| 작업 | 취소 시 |
|------|---------|
| AI 스트리밍 | `AiAsyncStreamJob::cancel()` → curl abort → `Finished` UI 이벤트 |
| 연결 테스트 | `AiAsyncHttpJob::cancel()` → 즉시 UI 복구 |

## 관련 파일

- `lofice/uiconfig/modules/lofice/ui/sidebar_ai_assistant.ui`
- `lofice/include/lofice/ui/LoficeSidebarLayout.hxx`
- `lofice/source/ui/AiAssistantPanel.*`

## 다음 단계 후보 (Phase 12)

- 멀티턴 대화 컨텍스트 (messages[] 히스토리)
- OpenAI embedding + Neon pgvector 시맨틱 검색
