#!/usr/bin/env bash
# lofice — Linux build verification helper
# Run from lofice repo root after ./autogen.sh && make

set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
cd "$ROOT"

if [[ ! -f Makefile ]]; then
  echo "ERROR: Makefile not found. Run ./autogen.sh first." >&2
  exit 1
fi

if [[ "${ENABLE_CURL:-}" != "YES" ]]; then
  echo "WARN: ENABLE_CURL is not YES — lofice HTTP/RAG transport may be stubbed." >&2
fi

# WSL/CI often runs as root; LibreOffice Makefile requires container=1.
if [[ "$(id -u)" -eq 0 ]] && [[ -z "${container:-}" ]]; then
  export container=1
  echo "WARN: building as root — container=1 (LibreOffice CI/WSL)"
fi

JOBS="${LOFICE_BUILD_JOBS:-$(nproc)}"

# Cold trees: build solenv tools (concat-deps, etc.) before parallel module builds.
echo "==> bootstrap: solenv fetch (serial warm-up)"
make -j1 solenv fetch

echo "==> make officecfg"
make -j"${JOBS}" officecfg

echo "==> make lofice"
make -j"${JOBS}" lofice

if [[ "${LOFICE_SKIP_OPTIONAL:-}" == "1" ]]; then
  echo "SKIP: optional modules (LOFICE_SKIP_OPTIONAL=1)"
else
  echo "==> optional UI modules (cui sw sc sd)"
  make -j"${JOBS}" cui sw sc sd || {
    echo "WARN: optional module build failed — lofice core may still be OK." >&2
  }
fi

echo "OK: lofice build verification complete."
