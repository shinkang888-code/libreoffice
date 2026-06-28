# Neon + Vercel RAG API 연동

lofice 코드베이스 RAG 검색 API를 **Neon Postgres** + **Vercel Serverless**로 호스팅합니다.

## 아키텍처

```
lofice C++ (AiRagContext)
  → GET https://<vercel>/search?q=...
       → Vercel api/search.js
            → Neon lofice_code_index (FTS / pgvector)
```

로컬 개발: `npm run rag-server` (`127.0.0.1:8787`)

## 1. Neon 연결 (CLI)

```powershell
cd c:\Users\FORYOUCOM\Documents\cursor\loffice_li\lofice
neonctl link --org-id org-wispy-brook-68991240 --project-id gentle-base-41825553
neonctl checkout main
# → .env.local DATABASE_URL 생성
```

## 2. 코드 인덱싱

```powershell
cd lofice\scripts
npm run index
# semantic: OPENAI_API_KEY 설정 후 npm run embed
```

## 3. Vercel 배포 (Neon env 동기화)

```powershell
cd lofice\scripts
npm run sync-vercel      # Neon DATABASE_URL → Vercel production env
npm run deploy-rag-api   # sync + vercel deploy --prod
```

프로젝트: `shinkang888-codes-projects/lofice-rag-api`  
디렉터리: `lofice/rag-api/`

## 4. lofice AI 설정

사이드바 / 도구→옵션:

- **코드베이스 RAG 사용** ✓
- **RAG 서버 URL**: `https://lofice-rag-api.vercel.app/search`

또는 env:

```powershell
$env:LOFICE_RAG_ENABLED = "1"
$env:LOFICE_RAG_ENDPOINT = "https://lofice-rag-api.vercel.app/search"
```

## API

| Endpoint | 설명 |
|----------|------|
| `GET /health` | 상태 확인 |
| `GET /search?q=...&mode=semantic\|fulltext&limit=6` | RAG 검색 + context |

## 환경 변수 (Vercel)

| 변수 | 출처 |
|------|------|
| `DATABASE_URL` | `neonctl checkout` / sync-vercel |
| `OPENAI_API_KEY` | (선택) semantic embedding |

## 주의

- `.env.local`, `.neon`, `lofice/rag-api/.vercel` — gitignore
- Preview env는 `sync-vercel`에서 production만 설정 (Preview branch 별도 설정 가능)
