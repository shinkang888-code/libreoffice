# Phase 18 — RAG 테스트 비동기화 + 빌드 검증

## RAG 테스트 비동기 (`AiAsyncRagJob`)

사이드바 **RAG 테스트** 버튼이 UI 스레드를 블로킹하지 않습니다.

```
OnTestRagClicked
  → AiAsyncRagJob (worker)
       → fetchRagContextForSettings()
  → PostUserEvent → OnAsyncRagEvent
       → 응답 영역 미리보기 + 상태
```

- **취소** 버튼으로 RAG 테스트 중단 가능
- Send / 연결 테스트 / RAG 테스트 상호 배타

## 빌드 검증 (Linux)

```bash
# repo root — autogen/configure 후
bash lofice/scripts/verify-lofice-build.sh
# → make officecfg lofice (+ cui sw sc sd optional)
```

Windows/WSL에서 Makefile 없으면 autogen 필요:

```bash
./autogen.sh
./configure --enable-curl=YES
bash lofice/scripts/verify-lofice-build.sh
```

## mk 무결성 (모든 OS)

```powershell
cd lofice\scripts
npm run verify-mk
```

`Library_lofice.mk`에 `source/**/*.cxx` 전부 등록됐는지 확인합니다.

## 다음 단계 (Phase 19)

- GitHub Actions lofice 빌드 워크플로
- 옵션 대화상자 RAG 테스트 비동기화
