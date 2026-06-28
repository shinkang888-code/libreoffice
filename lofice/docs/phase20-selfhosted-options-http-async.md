# Phase 20 — 옵션 연결 테스트 비동기 + self-hosted Linux build

## 옵션 연결 테스트 비동기

`OptLoficeAiTabPage` — `AiAsyncHttpJob` + `OnAsyncHttpEvent`:

- RAG 테스트와 동일 패턴 (UI 비블로킹)
- `isAsyncTestRunning()` — HTTP/RAG job 상호 배타
- 소멸자에서 두 job 모두 cancel

## Self-hosted Linux build CI

`.github/workflows/lofice-build-selfhosted.yml`:

| Job | Runner | 내용 |
|-----|--------|------|
| verify-fast | ubuntu-latest | verify-ci |
| build | self-hosted, linux, lofice | make officecfg lofice |

Runner 사전 요구: `./autogen.sh && ./configure --enable-curl=YES`

문서: `lofice/docs/selfhosted-linux-build.md`

## verify-lofice-build.sh

- `LOFICE_SKIP_OPTIONAL=1` — cui/sw/sc/sd 스킵

## 다음 단계 (Phase 21)

- Quick Action → AI 프롬프트 자동 입력
- 대화 내보내기/가져오기 JSON
