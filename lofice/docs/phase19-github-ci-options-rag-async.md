# Phase 19 — GitHub Actions CI + 옵션 RAG 비동기

## GitHub Actions (`lofice-ci.yml`)

`lofice/` 또는 `Lofice.xcs/xcu` 변경 시:

```yaml
npm run verify-ci   # mk + 소스 파일 + officecfg props + ui
npm run verify-mk
```

전체 LibreOffice 빌드는 CI 시간상 제외 — Linux 전체 빌드는 `verify-lofice-build.sh` 로컬/전용 러너용.

## 옵션 대화상자 RAG 테스트 비동기

`OptLoficeAiTabPage` — `AiAsyncRagJob` + `OnAsyncRagEvent`:

- RAG 테스트 중 버튼 비활성화
- 완료 시 MessageDialog (메인 스레드)
- 소멸자에서 job cancel

## 로컬 CI

```powershell
cd lofice\scripts
npm run verify-ci
```

## 다음 단계 (Phase 20)

- self-hosted Linux runner에서 `verify-lofice-build.sh`
- 연결 테스트 옵션 비동기화
