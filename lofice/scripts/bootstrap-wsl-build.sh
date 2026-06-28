#!/usr/bin/env bash
# lofice — Bootstrap lofice build tree on WSL/Linux (autogen + configure)
#
# Usage (WSL, repo root):
#   LOFICE_WSL_INSTALL_DEPS=1 bash lofice/scripts/bootstrap-wsl-build.sh
#   bash lofice/scripts/bootstrap-wsl-build.sh   # skip apt if deps already installed
#
# After bootstrap:
#   ENABLE_CURL=YES bash lofice/scripts/verify-lofice-build.sh
#   LOFICE_SKIP_OPTIONAL=1 bash lofice/scripts/verify-lofice-build.sh

set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
cd "$ROOT"

log() { echo "==> $*"; }

if [[ ! -f ./autogen.sh ]]; then
  echo "ERROR: autogen.sh not found — run from lofice repo root" >&2
  exit 1
fi

if [[ "${LOFICE_WSL_INSTALL_DEPS:-}" == "1" ]]; then
  log "Installing build dependencies (sudo)"
  if [[ ! -f ./install_deps.sh ]]; then
    echo "ERROR: install_deps.sh missing" >&2
    exit 1
  fi
  sudo apt-get update -qq
  sudo bash ./install_deps.sh
  sudo apt-get install -y libcurl4-openssl-dev libssl-dev
fi

for cmd in gcc g++ make autoconf; do
  if ! command -v "$cmd" >/dev/null 2>&1; then
    echo "ERROR: $cmd not found. Run with LOFICE_WSL_INSTALL_DEPS=1" >&2
    exit 1
  fi
done

if [[ ! -f Makefile ]]; then
  log "./autogen.sh (may take several minutes)"
  ./autogen.sh

  log "./configure --enable-curl=YES"
  ./configure --enable-curl=YES
else
  log "Makefile exists — skip autogen/configure"
fi

log "Build tree ready at $ROOT"
log "Next: ENABLE_CURL=YES bash lofice/scripts/verify-lofice-build.sh"
