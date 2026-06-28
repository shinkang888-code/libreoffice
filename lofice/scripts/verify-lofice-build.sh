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

if [[ "$(id -u)" -eq 0 ]] && [[ -z "${container:-}" ]]; then
  export container=1
  echo "WARN: building as root — container=1 (LibreOffice CI/WSL)"
fi

if [[ -f autogen.lastrun ]] && grep -q '^--quick' autogen.lastrun 2>/dev/null; then
  echo "WARN: removing invalid autogen.lastrun (--quick)"
  rm -f autogen.lastrun
fi

# First run on persistent runner: full tree warm-up (no cppumaker yet).
if [[ ! -x "$ROOT/instdir/sdk/bin/cppumaker" ]]; then
  echo "Cold build tree — running bootstrap-cold-build.sh"
  bash "$(dirname "$0")/bootstrap-cold-build.sh"
fi

JOBS="${LOFICE_BUILD_JOBS:-$(nproc)}"

bootstrap_module() {
  local target="$1"
  echo "==> bootstrap: ${target}"
  make -j"${JOBS}" "${target}"
}

echo "==> bootstrap: solenv fetch (serial warm-up)"
make -j1 solenv fetch

# sal plugin libsal_textenclo.so (DLLPOSTFIX=lo) for lofice link/runtime deps
if [[ ! -f "$ROOT/instdir/program/libsal_textenclo.so" ]]; then
  for target in dragonbox fast_float cppunit boost sal; do
    bootstrap_module "${target}"
  done
fi

echo "==> make officecfg (Lofice headers)"
rm -f "$ROOT/workdir/CustomTarget/officecfg/registry/officecfg/Office/Lofice.hxx" 2>/dev/null || true
make -j"${JOBS}" CustomTarget_officecfg_registry
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
