# Phase 22 — 퀵액션 자동 AI 전송 + CI RAG/Embed 검증

## 1. 퀵액션 → 자동 AI 전송

Phase 21에서 프롬프트만 채우던 동작을 **자동 Send**까지 연결했습니다.

| 단계 | 동작 |
|------|------|
| 1 | `.uno:*` 명령 실행 (기존) |
| 2 | `aiPrompt` → 프롬프트 입력란 채움 |
| 3 | **비동기 작업 없으면** `submitPromptFromUi()` → AI 전송 |

- 다른 스트리밍/테스트 진행 중이면 **프롬프트만 입력** (Phase 21과 동일)
- `OnSendClicked`와 동일 경로 (`trySendExternalStreaming` → 내장 AI)

### 변경 파일

- `lofice/source/ui/AiAssistantPanel.hxx` — `submitPromptFromUi`, `isAsyncBusy`
- `lofice/source/ui/AiAssistantPanel.cxx`

## 2. CI 검증 확장

| 스크립트 | 설명 |
|----------|------|
| `npm run verify-rag` | Vercel RAG `/health` 프로브 |
| `npm run verify-embed` | `OPENAI_API_KEY` 있으면 embed, 없으면 skip |
| `npm run verify-ci` | mk + officecfg + ui + **rag** (rag 실패해도 ok 유지) |

엄격 RAG 검사:

```powershell
$env:LOFICE_CI_STRICT_RAG = "1"
npm run verify-rag
```

## 3. Self-hosted runner

```bash
bash lofice/scripts/setup-selfhosted-runner.sh
```

Labels: `self-hosted`, `linux`, `lofice`

## 검증

```powershell
cd lofice\scripts
npm run verify-ci
npm run verify-embed
```

## 다음 단계

- `.env.local`에 `OPENAI_API_KEY` → `npm run embed`
- self-hosted runner 등록 → `lofice Linux Build` workflow
- Linux: `make officecfg lofice cui sw sc sd`
