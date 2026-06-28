# Phase 16 — AI 사이드바 RAG (Neon → 프롬프트 컨텍스트)

로컬 RAG HTTP 서버가 Neon 코드베이스 검색 결과를 LLM system 메시지에 주입합니다.

## 아키텍처

```
Send to AI
  → AiAsyncStreamJob worker (또는 processPrompt)
       → enrichPromptWithRag()
            → GET http://127.0.0.1:8787/search?q=...&mode=semantic
                 → rag-search-server.mjs → Neon (FTS / pgvector)
       → buildChatRequestJson() — system 메시지에 ragContext 포함
       → OpenAI-compatible API
```

## 실행 (개발)

```powershell
cd lofice\scripts
npm run index
npm run embed          # OPENAI_API_KEY 필요 (semantic)
npm run rag-server     # 터미널 1

$env:LOFICE_RAG_ENABLED = "1"
# lofice 실행 후 AI 사이드바에서 Send
```

## 설정

| 항목 | officecfg | env |
|------|-----------|-----|
| RAG 사용 | `UseRagContext` | `LOFICE_RAG_ENABLED=1` |
| 서버 URL | `RagEndpoint` | `LOFICE_RAG_ENDPOINT` |
| 타임아웃 | `RagTimeoutSeconds` (2–30) | `LOFICE_RAG_TIMEOUT` |

기본 URL: `http://127.0.0.1:8787/search`

## 주요 파일

- `lofice/scripts/rag-search-server.mjs` — 로컬 HTTP 서버
- `lofice/include/lofice/ai/AiRagContext.hxx` — curl GET 클라이언트
- `lofice/source/ai/AiHttpTransport.cxx` — system prompt에 RAG 삽입
- `officecfg/.../Lofice.xcs` — UseRagContext, RagEndpoint, RagTimeoutSeconds

## 다음 단계 (Phase 17)

- 사이드바/도구→옵션 RAG 설정 UI
- RAG fetch 비동기 UI 상태 ("코드베이스 검색 중...")
- Linux 빌드 검증
