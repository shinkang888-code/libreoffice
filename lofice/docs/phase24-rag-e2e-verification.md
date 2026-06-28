# Phase 24 — RAG E2E 검증 + 기본 RAG ON

Vercel Production RAG API가 `AiRagContext`와 동일한 URL 형식으로 semantic 컨텍스트를 반환하는지 자동 검증하고, 신규 설치 기본값으로 RAG를 켰습니다.

## 1. E2E 검증 스크립트

`AiRagContext.cxx`와 동일:

```
GET {RagEndpoint}?q=...&mode=semantic&limit=6
→ JSON.context (lofice/ 경로 포함)
```

| 명령 | 설명 |
|------|------|
| `npm run verify-rag-e2e` | Production semantic + context 검증 |
| `npm run verify-ci` | mk/officecfg/ui + rag health + **ragE2e** |

## 2. officecfg 기본값 변경

`UseRagContext` 기본값: **false → true**

- `RagEndpoint`: `https://lofice-rag-api.vercel.app/search` (유지)
- 빌드된 lofice 실행 시 Send to AI마다 Vercel RAG 컨텍스트 자동 주입 (curl 필요)

사이드바/옵션에서 **RAG 사용** 체크 해제로 끌 수 있습니다.

## 3. 동작 흐름 (앱 실행 시)

```
Send to AI
  → enrichPromptWithRag()
  → GET Vercel /search?mode=semantic
  → system prompt에 [코드베이스 RAG 컨텍스트] 삽입
  → OpenAI / 내장 AI 응답
```

로컬 `make` 없이도 **스크립트로 E2E는 검증 완료** 상태입니다.

## 검증 결과 (예시)

```json
{
  "ok": true,
  "mode": "semantic",
  "hits": 6,
  "contextChars": 6000
}
```

## 다음 단계

- lofice 바이너리 빌드 환경(클라oud VM 등)에서 실제 UI Send 테스트
- RAG fetch 진행 UI 메시지 개선 (선택)
