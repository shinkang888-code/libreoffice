# Phase 23 — WSL/Linux 빌드 부트스트랩

embed(`OPENAI_API_KEY`)와 Linux `make lofice`가 막혀 있을 때, **WSL에서 빌드 트리를 준비**하는 스크립트를 추가했습니다.

## 1. `.env.local.example`

리포 루트에 템플릿 추가 (복사 후 키 입력):

```powershell
Copy-Item .env.local.example .env.local
# OPENAI_API_KEY, DATABASE_URL 편집
cd lofice\scripts
npm run embed
```

## 2. WSL 빌드 준비

| 스크립트 | 역할 |
|----------|------|
| `verify-wsl-build-ready.sh` | gcc/make/autogen/Makefile 상태 JSON 출력 |
| `bootstrap-wsl-build.sh` | `install_deps.sh` + `autogen.sh` + `configure --enable-curl=YES` |
| `setup-wsl-build.ps1` | Windows → WSL 래퍼 |

### Windows (PowerShell)

```powershell
# 상태만 확인
.\lofice\scripts\setup-wsl-build.ps1 -CheckOnly

# 의존성 설치 + autogen/configure (sudo, 수십 분~수 시간)
.\lofice\scripts\setup-wsl-build.ps1 -InstallDeps
```

### WSL 직접

```bash
cd /mnt/c/Users/.../lofice
LOFICE_WSL_INSTALL_DEPS=1 bash lofice/scripts/bootstrap-wsl-build.sh
ENABLE_CURL=YES LOFICE_SKIP_OPTIONAL=1 bash lofice/scripts/verify-lofice-build.sh
```

## 3. Self-hosted runner (대안)

전용 Linux 머신이 있으면:

```bash
bash lofice/scripts/setup-selfhosted-runner.sh
```

GitHub Actions → **lofice Linux Build**

## 검증

```powershell
cd lofice\scripts
npm run verify-wsl
npm run verify-ci
```

## 다음 단계

- WSL `-InstallDeps` 완료 후 `make lofice` 로컬 검증
- `.env.local`에 `OPENAI_API_KEY` → `npm run embed`
- CI workflow를 remote에 push 후 self-hosted runner 등록
