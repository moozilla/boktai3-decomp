#!/bin/sh
# One-shot setup for a fresh Linux machine (a Claude Code cloud environment, CI, a VM).
#   BOKTAI3_ROM_URL=https://... tools/cloud_setup.sh     (or put baserom.gba in the repo root first)
#
# Installs packages, builds the pinned tools (agbcc, gbadisasm, the permuter), makes
# build/venv, fetches and verifies the ROM, then disassembles and builds once
# (about 35 minutes: disassembly and symbols are single-threaded). Afterwards
# tools/check.py works in seconds and `make` only recompiles what changed.
# The ROM is never committed (.gitignore); nothing generated from it is either.
set -e
cd "$(dirname "$0")/.."
SHA=2651c5e6     # first 8 hex digits of baserom.gba's SHA-1 (README.md has the full value)

if command -v apt-get >/dev/null; then
  SUDO=$(command -v sudo || true)
  $SUDO apt-get update -qq
  $SUDO apt-get install -y -qq build-essential cmake git curl binutils-arm-none-eabi python3-venv python3-dev >/dev/null
fi

if [ ! -f baserom.gba ]; then
  [ -n "$BOKTAI3_ROM_URL" ] || { echo "baserom.gba missing: set BOKTAI3_ROM_URL or copy the ROM here" >&2; exit 1; }
  curl -fsSL "$BOKTAI3_ROM_URL" -o baserom.gba
fi
sha1sum baserom.gba | grep -q "^$SHA" || { echo "baserom.gba has the wrong SHA-1" >&2; exit 1; }

[ -d build/venv ] || python3 -m venv build/venv
build/venv/bin/pip install -q -r requirements.txt

MINIMAL=1 sh tools/setup.sh        # agbcc, gbadisasm, permuter; skips armips and the mGBA harness

BUILD_JOBS=$(nproc 2>/dev/null || echo 4) make PY=build/venv/bin/python > build/cloud-setup.log 2>&1 \
  || { tail -20 build/cloud-setup.log; exit 1; }
grep -q 'build/boktai3.gba: OK' build/cloud-setup.log && echo "ready: build/boktai3.gba: OK"
