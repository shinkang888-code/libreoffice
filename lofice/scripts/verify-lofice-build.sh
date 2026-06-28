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

JOBS="${LOFICE_BUILD_JOBS:-$(nproc)}"

bootstrap_target() {
  local target="$1"
  if make -n "$target" >/dev/null 2>&1; then
    echo "==> bootstrap: $target"
    make -j"${JOBS}" "$target"
  fi
}

# Cold trees: parallel make officecfg can race before solenv tools / bundled libxml2 exist.
bootstrap_target concat-deps
bootstrap_target libxml2

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
