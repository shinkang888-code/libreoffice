#!/usr/bin/env bash
# lofice — one-time cold WSL/self-hosted build tree warm-up
#
# First run on a fresh configure tree (no instdir/sdk/bin/cppumaker).
# Builds build-non-l10n-only — may take several hours.
#
# Usage:
#   container=1 bash lofice/scripts/bootstrap-cold-build.sh

set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
cd "$ROOT"

if [[ ! -f Makefile ]]; then
  echo "ERROR: Makefile not found. Run bootstrap-wsl-build.sh first." >&2
  exit 1
fi

if [[ "$(id -u)" -eq 0 ]] && [[ -z "${container:-}" ]]; then
  export container=1
  echo "WARN: building as root — container=1 (LibreOffice CI/WSL)"
fi

if [[ -f autogen.lastrun ]] && grep -q '^--quick' autogen.lastrun 2>/dev/null; then
  echo "WARN: removing invalid autogen.lastrun (--quick)"
  rm -f autogen.lastrun
fi

JOBS="${LOFICE_BUILD_JOBS:-$(nproc)}"

if [[ -x "$ROOT/instdir/sdk/bin/cppumaker" ]]; then
  echo "Warm tree detected (cppumaker exists) — skip cold bootstrap"
  exit 0
fi

echo "==> cold bootstrap: solenv fetch"
make -j1 solenv fetch

for target in dragonbox fast_float cppunit boost; do
  echo "==> cold bootstrap: ${target}"
  make -j"${JOBS}" "${target}"
done

echo "==> cold bootstrap: build-non-l10n-only (hours on first run)"
make -j"${JOBS}" build-non-l10n-only

echo "OK: cold bootstrap complete — cppumaker and core libs should be ready."
