#!/usr/bin/env bash
# lofice — GitHub self-hosted runner (Linux/WSL) unattended install
#
# Usage:
#   bash lofice/scripts/install-github-runner.sh
#   LOFICE_BUILD_ROOT=~/lofice-build LOFICE_WSL_INSTALL_DEPS=1 bash lofice/scripts/install-github-runner.sh
#
set -euo pipefail

REPO="${LOFICE_GITHUB_REPO:-shinkang888-code/libreoffice}"
RUNNER_DIR="${LOFICE_RUNNER_DIR:-$HOME/actions-runner-lofice}"
BUILD_ROOT="${LOFICE_BUILD_ROOT:-$HOME/lofice-build}"
LABELS="${LOFICE_RUNNER_LABELS:-self-hosted,linux,lofice}"
RUNNER_VERSION="${LOFICE_RUNNER_VERSION:-2.323.0}"

log() { echo "==> $*"; }

if ! command -v gh >/dev/null 2>&1; then
  echo "ERROR: gh CLI required" >&2
  exit 1
fi

gh auth status >/dev/null 2>&1 || gh auth login

log "Install build deps + bootstrap at $BUILD_ROOT"
mkdir -p "$(dirname "$BUILD_ROOT")"
if [[ ! -d "$BUILD_ROOT/.git" ]]; then
  log "git clone → $BUILD_ROOT"
  gh repo clone "$REPO" "$BUILD_ROOT"
fi

cd "$BUILD_ROOT"
git fetch origin
git checkout master 2>/dev/null || git checkout main 2>/dev/null || true
git pull --ff-only || true

export LOFICE_WSL_INSTALL_DEPS="${LOFICE_WSL_INSTALL_DEPS:-1}"
bash lofice/scripts/bootstrap-wsl-build.sh

log "Register GitHub Actions runner at $RUNNER_DIR"
mkdir -p "$RUNNER_DIR"
cd "$RUNNER_DIR"

ARCH="x64"
case "$(uname -m)" in
  aarch64|arm64) ARCH="arm64" ;;
esac

TAR="actions-runner-linux-${ARCH}-${RUNNER_VERSION}.tar.gz"
URL="https://github.com/actions/runner/releases/download/v${RUNNER_VERSION}/${TAR}"

if [[ ! -f ./config.sh ]]; then
  log "Download runner $RUNNER_VERSION ($ARCH)"
  curl -fsSL -o "$TAR" "$URL"
  tar xzf "$TAR"
  rm -f "$TAR"
fi

TOKEN="$(gh api -X POST "repos/${REPO}/actions/runners/registration-token" --jq .token)"
RUNNER_NAME="${LOFICE_RUNNER_NAME:-lofice-wsl-$(hostname -s)}"

if [[ ! -f .runner ]]; then
  ./config.sh \
    --url "https://github.com/${REPO}" \
    --token "$TOKEN" \
    --name "$RUNNER_NAME" \
    --labels "$LABELS" \
    --unattended \
    --replace
else
  log "Runner already configured (.runner exists)"
fi

# Persistent build tree for CI (see lofice-build-selfhosted.yml)
grep -q '^LOFICE_BUILD_ROOT=' .env 2>/dev/null || echo "LOFICE_BUILD_ROOT=$BUILD_ROOT" >> .env

log "Install runner service (systemd user or svc.sh)"
if command -v systemctl >/dev/null 2>&1 && [[ -d /run/systemd/system ]]; then
  sudo ./svc.sh install || ./svc.sh install
  sudo ./svc.sh start || ./svc.sh start
else
  log "No systemd — start manually: cd $RUNNER_DIR && ./run.sh"
  nohup ./run.sh > runner.log 2>&1 &
  echo $! > runner.pid
fi

log "Done. Runner: $RUNNER_NAME labels=[$LABELS] build_root=$BUILD_ROOT"
gh api "repos/${REPO}/actions/runners" --jq '.runners[] | {name, status, labels: [.labels[].name]}'
