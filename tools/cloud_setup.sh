#!/bin/sh
# Setup for a fresh Linux machine (a Claude Code cloud environment, CI, a VM).
#
#   tools/cloud_setup.sh              set up this checkout in place (about 3 minutes)
#   tools/cloud_setup.sh --provision  cloud environment setup script: the same, in the
#                                     cached base checkout $B3_CACHE/base (docs/CLOUD.md)
#   tools/cloud_setup.sh --session    SessionStart hook (.claude/settings.json): set this
#                                     checkout up from the base in seconds
#
# In place: installs packages, builds the pinned tools (agbcc, gbadisasm, the permuter),
# makes build/venv, fetches and verifies the ROM (BOKTAI3_ROM_URL, or put baserom.gba in
# the repo root first), then disassembles and builds once. Afterwards tools/check.py works
# in seconds and `make` only recompiles what changed.
#
# The cloud environment snapshots the filesystem after a setup script that finishes in
# about five minutes, so --provision runs once per environment (and again when the cache
# expires). --session then links the base's tools, venv and ROM, copies its generated
# asm and objects, and finishes any rebuild in the background; tools/build.py and
# tools/check.py wait for that rebuild by themselves.
#
# The ROM is never committed (.gitignore); nothing generated from it is either.
set -e
cd "$(dirname "$0")/.."
git config core.hooksPath .githooks 2>/dev/null || true  # tools/pii_check.py hooks
SHA=2651c5e6     # first 8 hex digits of baserom.gba's SHA-1 (README.md has the full value)
CACHE=${B3_CACHE:-/opt/boktai3}
BASE=$CACHE/base
PIDFILE=build/.cloud-setup.pid
LOG=build/cloud-setup.log
MAKELOG=build/make.log
JOBS=$(nproc 2>/dev/null || echo 4)

packages() {
  command -v apt-get >/dev/null || return 0
  # everything present already (a cached environment): skip apt entirely
  if command -v arm-none-eabi-as >/dev/null && command -v cmake >/dev/null && python3 -c 'import ensurepip' 2>/dev/null; then
    return 0
  fi
  SUDO=$(command -v sudo || true)
  $SUDO apt-get update -qq
  $SUDO apt-get install -y -qq build-essential cmake git curl binutils-arm-none-eabi python3-venv python3-dev >/dev/null
}

rom() {  # rom FILE: make sure FILE is the verified ROM
  if [ ! -f "$1" ]; then
    [ -n "$BOKTAI3_ROM_URL" ] || { echo "baserom.gba missing: set BOKTAI3_ROM_URL or copy the ROM here" >&2; return 1; }
    curl -fsSL "$BOKTAI3_ROM_URL" -o "$1.part" && mv "$1.part" "$1"
  fi
  sha1sum "$1" | grep -q "^$SHA" || { echo "$1 has the wrong SHA-1" >&2; return 1; }
}

in_place() {  # in_place [--tools-ok-without-rom]
  packages
  mkdir -p build
  [ -e build/venv ] || python3 -m venv build/venv
  build/venv/bin/pip install -q -r requirements.txt
  MINIMAL=1 sh tools/setup.sh >build/tools-setup.log 2>&1 || { tail -20 build/tools-setup.log; exit 1; }
  if ! rom baserom.gba; then
    # A setup script may not see the ROM URL: keep the tools, the session hook builds.
    [ "$1" = --tools-ok-without-rom ] && { echo "tools ready; no ROM, so no base build"; exit 0; }
    exit 1
  fi
  BUILD_JOBS=$JOBS make PY=build/venv/bin/python >"$MAKELOG" 2>&1 \
    || { tail -20 "$MAKELOG"; exit 1; }
  git rev-parse HEAD >build/.base-rev
  grep -q 'build/boktai3.gba: OK' "$MAKELOG" && echo "ready: build/boktai3.gba: OK"
}

background() {  # background MODE: run this script detached; build.py/check.py wait for it
  mkdir -p build
  B3_SETUP_BG=1 nohup sh tools/cloud_setup.sh "$1" >"$LOG" 2>&1 </dev/null &
  echo $! >"$PIDFILE"
  echo "Boktai 3 cloud setup: finishing the build in the background (log: $LOG)."
  echo "tools/build.py and tools/check.py wait for it automatically; everything else works now."
}

session() {
  # Already set up (a resumed session, or the hook ran before): nothing to do.
  if [ -f build/boktai3.elf ] && [ -f gen/code_sym.s ]; then
    exit 0
  fi
  mkdir -p build
  if [ ! -f "$BASE/build/boktai3.elf" ] || [ "$(cd "$BASE" && pwd -P)" = "$(pwd -P)" ]; then
    # No cached base build (no setup script, or it had no ROM): full setup here,
    # reusing the base's compiled tools and venv when they exist.
    [ -x "$BASE/build/tools/agbcc/agbcc" ] && [ "$(cd "$BASE" && pwd -P)" != "$(pwd -P)" ] && {
      [ -e build/tools ] || ln -s "$BASE/build/tools" build/tools
      [ -e build/venv ] || ln -s "$BASE/build/venv" build/venv
    }
    background --bg-in-place
    exit 0
  fi
  REV=$(cat "$BASE/build/.base-rev")
  link() { [ -e "$2" ] || [ -L "$2" ] || ln -s "$1" "$2"; }
  link "$BASE/baserom.gba" baserom.gba
  link "$BASE/build/tools" build/tools
  link "$BASE/build/venv" build/venv
  # Generated asm and compiled objects (with their content-hash stamps): a copy, so
  # builds here never touch the base. About 300 MB, a few seconds.
  [ -e gen ] || cp -a "$BASE/gen" gen
  for p in asm src res text c_labels.ld units.txt link.ld boktai3.elf; do
    [ -e "build/$p" ] || [ ! -e "$BASE/build/$p" ] || cp -a "$BASE/build/$p" "build/$p"
  done
  # The Makefile decides by mtime, and a fresh clone is newer than the copied gen/.
  # When gen/'s inputs match the base's commit, the copy is current: mark it so.
  git cat-file -e "$REV^{commit}" 2>/dev/null || git fetch -q origin "$REV" 2>/dev/null || true
  if git diff --quiet "$REV" -- symbols data tools/disasm.py tools/symbolize.py tools/m4a.py \
       tools/romlib.py tools/datafmt 2>/dev/null; then
    touch gen/*
  fi
  if git diff --quiet "$REV" 2>/dev/null && [ -z "$(git status --porcelain --untracked-files=no)" ]; then
    echo "Boktai 3 cloud setup: ready (base $(echo "$REV" | cut -c1-8)); tools/check.py works now."
    exit 0
  fi
  # This checkout differs from the base: rebuild what changed (usually under a minute).
  background --bg-make
}

case "$1" in
  --provision)
    # The setup script clones into $BASE and runs this from there.
    in_place --tools-ok-without-rom ;;
  --session)
    session ;;
  --bg-in-place)
    trap 'rm -f "$PIDFILE"' EXIT
    in_place ;;
  --bg-make)
    trap 'rm -f "$PIDFILE"' EXIT
    BUILD_JOBS=$JOBS make PY=build/venv/bin/python && echo "ready" ;;
  "")
    in_place ;;
  *)
    echo "usage: $0 [--provision | --session]" >&2; exit 2 ;;
esac
