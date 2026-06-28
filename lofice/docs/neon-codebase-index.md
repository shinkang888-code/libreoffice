# Neon 코드베이스 인덱스 (lofice)

`lofice/` 디렉터리를 Neon Postgres `lofice_code_index` 테이블에 색인합니다. AI 에이전트·검색용입니다.

## 전제

- Neon 프로젝트: **loice** (`gentle-base-41825553`)
- 리포 루트에 `.neon` 링크 (gitignore됨)
- `neonctl checkout main` → `.env.local`에 `DATABASE_URL` 자동 pull

## 1. Neon 연결 (최초 1회)

```powershell
cd c:\Users\FORYOUCOM\Documents\cursor\loffice_li\libreoffice
neonctl link --org-id org-wispy-brook-68991240 --project-id gentle-base-41825553
neonctl checkout main
```

## 2. 스키마

`lofice/sql/code_index_schema.sql` — pgvector + `lofice_code_index` (GIN full-text)

## 3. 인덱싱

```powershell
cd lofice\scripts
npm install
# repo 루트 .env.local 의 DATABASE_URL 자동 로드 (channel_binding 제거)
npm run index
```

**범위:** `lofice/`만 (전체 LibreOffice 코어 제외 — 용량·노이즈 방지)

**대상 확장자:** `.cxx`, `.hxx`, `.md`, `.ui`, `.mk`, `.json`, `.sql`, `.ps1`

## 4. 검색

```powershell
# 키워드 (FTS)
npm run search -- "AiAssistantPanel streaming"

# 의미 검색 (pgvector + OpenAI embedding)
$env:OPENAI_API_KEY = "sk-..."
npm run embed
npm run search -- --semantic "how does conversation history persist"
```

## 5. Embedding 백필

FTS 인덱싱 후 OpenAI embedding을 채웁니다 (`embedding IS NULL` 행만).

```powershell
$env:OPENAI_API_KEY = "sk-..."
npm run embed
```

- 모델: `text-embedding-3-small` (1536 dims, 스키마 `vector(1536)`과 일치)
- `.env.local`에 `OPENAI_API_KEY` 추가 가능 (gitignore)

## 6. 재색인

코드 변경 후 `npm run index` 재실행 — 파일별 DELETE 후 UPSERT.

## 7. RAG 서버 (AI 사이드바)

### 로컬

```powershell
npm run rag-server
$env:LOFICE_RAG_ENABLED = "1"
```

### Vercel (Neon 연동, 권장)

```powershell
npm run sync-vercel      # Neon DATABASE_URL → Vercel
npm run deploy-rag-api   # 배포
```

- Production: `https://lofice-rag-api.vercel.app/search`
- 상세: [`neon-vercel-rag-api.md`](neon-vercel-rag-api.md)

- `GET /search?q=...&mode=semantic|fulltext&limit=6`
- lofice C++ (`AiRagContext`)가 Send 전에 호출해 system prompt에 컨텍스트 주입

## MCP

Cursor Neon MCP(`run_sql`)로도 조회 가능:

```sql
SELECT file_path, chunk_no, left(content, 200)
FROM lofice_code_index
WHERE search_vector @@ websearch_to_tsquery('simple', 'DefaultAiListener')
ORDER BY ts_rank(search_vector, websearch_to_tsquery('simple', 'DefaultAiListener')) DESC
LIMIT 10;
```

## 주의

- `.env.local` / `.neon` — **커밋 금지** (비밀 포함)
- `embedding` 컬럼: `npm run embed`로 채운 뒤 `--semantic` 검색 사용
