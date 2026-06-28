#!/usr/bin/env bash
# lofice — Linux build verification helper
# Run from LibreOffice repo root after ./autogen.sh && make

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

echo "==> make officecfg"
make officecfg

echo "==> make lofice"
make lofice

if [[ "${LOFICE_SKIP_OPTIONAL:-}" == "1" ]]; then
  echo "SKIP: optional modules (LOFICE_SKIP_OPTIONAL=1)"
else
  echo "==> optional UI modules (cui sw sc sd)"
  make cui sw sc sd || {
    echo "WARN: optional module build failed — lofice core may still be OK." >&2
  }
fi

echo "OK: lofice build verification complete."
