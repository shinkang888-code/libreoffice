# Phase 17 — RAG 설정 UI 및 검색 상태 표시

사이드바·도구→옵션에서 RAG를 설정하고, Send 시 worker 스레드에서 상태 메시지를 UI에 표시합니다.

## UI 추가

| 위젯 | 설명 |
|------|------|
| `check_use_rag` | 코드베이스 RAG 사용 |
| `entry_rag_endpoint` | RAG 서버 URL (기본 `:8787/search`) |
| `spin_rag_timeout` | RAG HTTP 타임아웃 (2–30초) |
| `btn_test_rag` | RAG 검색 테스트 |

**적용 위치:** `sidebar_ai_assistant.ui`, `opt_lofice_ai_page.ui`

## 설정 저장

`AiSettings`에 RAG 필드 통합 → `saveSettings()` / officecfg:

- `UseRagContext`, `RagEndpoint`, `RagTimeoutSeconds`
- env 폴백: `LOFICE_RAG_ENABLED`, `LOFICE_RAG_ENDPOINT`, `LOFICE_RAG_TIMEOUT`

## 스트리밍 상태

`AiStreamEventKind::Status` 추가:

```
worker → "코드베이스 검색 중..." → enrichPromptWithRag()
       → "AI 스트리밍 수신 중..." → postChatCompletionStreaming()
```

## RAG 테스트

현재 UI 값으로 `fetchRagContextForSettings()` 호출 (저장 전 미리보기 가능).

## 다음 단계 (Phase 18)

- Linux 빌드 검증 (`make officecfg lofice cui`)
- RAG 비동기 테스트 (UI 블로킹 제거)
