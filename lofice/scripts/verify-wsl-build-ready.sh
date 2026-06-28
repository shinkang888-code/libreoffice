#!/usr/bin/env bash
# lofice — WSL/Linux build prerequisite check (JSON to stdout)
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
cd "$ROOT"

check_cmd() {
  if command -v "$1" >/dev/null 2>&1; then
    echo "ok"
  else
    echo "missing"
  fi
}

has_makefile="no"
[[ -f Makefile ]] && has_makefile="yes"

gcc_s="$(check_cmd gcc)"
gpp_s="$(check_cmd g++)"
make_s="$(check_cmd make)"
autoconf_s="$(check_cmd autoconf)"

ready="false"
if [[ "$has_makefile" == "yes" ]]; then
  ready="true"
elif [[ "$gcc_s" == "ok" && "$gpp_s" == "ok" && "$make_s" == "ok" && "$autoconf_s" == "ok" ]]; then
  ready="true"
fi

cat <<EOF
{
  "root": "$ROOT",
  "platform": "$(uname -s)",
  "tools": {
    "gcc": "$gcc_s",
    "g++": "$gpp_s",
    "make": "$make_s",
    "autoconf": "$autoconf_s",
    "automake": "$(check_cmd automake)",
    "libtool": "$(check_cmd libtool)",
    "pkg-config": "$(check_cmd pkg-config)",
    "curl": "$(check_cmd curl)"
  },
  "makefile": "$has_makefile",
  "autogen_sh": $([ -f ./autogen.sh ] && echo true || echo false),
  "install_deps_sh": $([ -f ./install_deps.sh ] && echo true || echo false),
  "ready": $ready
}
EOF
