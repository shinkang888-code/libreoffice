# Self-hosted runner — lofice Linux build

## Runner labels

GitHub self-hosted runner 등록 시 labels:

```
self-hosted, linux, lofice
```

## Runner 준비 (1회)

```bash
bash lofice/scripts/setup-selfhosted-runner.sh
```

또는 수동:

```bash
git clone <repo> ~/lofice-build && cd ~/lofice-build
./autogen.sh
./configure --enable-curl=YES
# deps: see LibreOffice build docs
```

## 워크플로

`.github/workflows/lofice-build-selfhosted.yml`

1. **verify-fast** (ubuntu-latest) — `npm run verify-ci`
2. **build** (self-hosted) — `verify-lofice-build.sh` → `make officecfg lofice`

수동 실행: GitHub Actions → **lofice Linux Build** → Run workflow

## 로컬 (WSL / Linux)

```bash
bash lofice/scripts/verify-wsl-build-ready.sh
LOFICE_WSL_INSTALL_DEPS=1 bash lofice/scripts/bootstrap-wsl-build.sh
ENABLE_CURL=YES bash lofice/scripts/verify-lofice-build.sh
```

Windows:

```powershell
.\lofice\scripts\setup-wsl-build.ps1 -CheckOnly
.\lofice\scripts\setup-wsl-build.ps1 -InstallDeps
```

상세: [phase23-wsl-build-bootstrap.md](./phase23-wsl-build-bootstrap.md)

환경 변수:

| 변수 | 설명 |
|------|------|
| `ENABLE_CURL=YES` | HTTP/RAG curl transport |
| `LOFICE_SKIP_OPTIONAL=1` | cui/sw/sc/sd 스킵 |
