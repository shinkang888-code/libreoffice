# Phase 15 — pgvector OpenAI embedding 시맨틱 검색

Neon `lofice_code_index.embedding` 컬럼을 OpenAI embedding으로 채우고, CLI에서 의미 기반 검색을 지원합니다.

## 워크플로

```
npm run index     → FTS 색인 (DATABASE_URL)
npm run embed     → embedding 백필 (OPENAI_API_KEY)
npm run search -- --semantic "<natural language query>"
```

## 추가 파일

| 파일 | 역할 |
|------|------|
| `lofice/scripts/lib/openai-embeddings.mjs` | OpenAI Embeddings API |
| `lofice/scripts/embed-codebase.mjs` | NULL embedding 백필 (배치 16) |
| `lofice/scripts/search-codebase.mjs` | `--semantic` 모드 |

## 스키마

- `vector(1536)` — `text-embedding-3-small`
- HNSW cosine index: `lofice_code_index_embedding_idx`

## 환경 변수

| 변수 | 용도 |
|------|------|
| `DATABASE_URL` | Neon (index/search/embed) |
| `OPENAI_API_KEY` | embed + semantic search |

## 다음 단계 후보 (Phase 16)

- AI 사이드바에서 Neon 시맨틱 검색 호출 (RAG 컨텍스트)
- Linux 빌드 검증
