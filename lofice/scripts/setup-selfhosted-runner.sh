#!/usr/bin/env bash
# lofice — GitHub self-hosted runner bootstrap (Linux)
# Usage: bash lofice/scripts/setup-selfhosted-runner.sh
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
cd "$ROOT"

LABELS="${LOFICE_RUNNER_LABELS:-self-hosted,linux,lofice}"

echo "==> lofice self-hosted runner bootstrap"
echo "    repo root: $ROOT"
echo "    labels:    $LABELS"
echo

if ! command -v gh >/dev/null 2>&1; then
  echo "ERROR: gh CLI required. https://cli.github.com/" >&2
  exit 1
fi

if [[ ! -f ./autogen.sh ]]; then
  echo "ERROR: not a lofice tree (autogen.sh missing)" >&2
  exit 1
fi

if [[ ! -f Makefile ]]; then
  echo "==> ./autogen.sh (first-time — may take a while)"
  ./autogen.sh
  echo "==> ./configure --enable-curl=YES"
  ./configure --enable-curl=YES
else
  echo "==> Makefile present — skip autogen/configure"
fi

echo
echo "==> Register GitHub Actions runner (interactive)"
echo "    Follow prompts; use labels: $LABELS"
echo
gh auth status || gh auth login
gh api repos/{owner}/{repo}/actions/runners/registration-token --method POST | \
  jq -r '.token' > /tmp/gh-runner-token.txt 2>/dev/null || true

cat <<EOF

Next steps (manual — GitHub UI):
1. Repo → Settings → Actions → Runners → New self-hosted runner
2. Download & configure runner on this Linux host
3. Add labels: $LABELS
4. GitHub Actions → "lofice Linux Build" → Run workflow

Verify locally:
  ENABLE_CURL=YES bash lofice/scripts/verify-lofice-build.sh
  LOFICE_SKIP_OPTIONAL=1 bash lofice/scripts/verify-lofice-build.sh

EOF
