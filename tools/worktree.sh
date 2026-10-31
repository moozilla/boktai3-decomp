#!/bin/sh
# Set up a worktree for a worker in seconds, without building anything:
#   tools/worktree.sh NAME [BASE]   new worktree ../wt/NAME on branch ${BRANCH_PREFIX:-codex}/NAME
#   tools/worktree.sh --here        set up the current worktree (e.g. one the desktop app made)
#
# The slow, ROM-derived parts come from the shared base checkout ($B3_BASE, default
# ../b3-base; tools/base.sh keeps it built at origin/main):
#   gen/  build/res/  build/text/  build/boktai3.elf  build/src/  build/asm/  are APFS clones (cp -c: instant, no
#       extra disk until a file changes), so a worker's own build never touches the base;
#   baserom.gba  build/tools/  build/venv/  build/scope/agbcc/  tools/emu/harness  are symlinks (read-only).
# Workers check with tools/check.py (one function or unit, seconds); the full ROM
# build (tools/build.py, minutes) runs once per batch at integration.
set -e
MAIN=$(git -C "$(dirname "$0")/.." worktree list --porcelain | sed -n '1s/^worktree //p')
SHARED=${B3_BASE:-$(dirname "$MAIN")/b3-base}
[ -f "$SHARED/build/boktai3.elf" ] || SHARED=$MAIN        # no built base yet: use the main checkout
BASEREV=$(git -C "$SHARED" rev-parse HEAD)
if [ "$1" = "--here" ]; then
  DIR=$(git rev-parse --show-toplevel)
else
  # default start: the base's commit, so the cloned gen/ and objects match this tree exactly
  NAME=$1; BASE=${2:-$BASEREV}; PREFIX=${BRANCH_PREFIX:-codex}
  DIR=$(dirname "$MAIN")/wt/$NAME
  git -C "$MAIN" worktree add -q -b "$PREFIX/$NAME" "$DIR" "$BASE"
fi
clone() {  # clone SRC DEST: APFS copy-on-write, else a plain copy
  [ -e "$1" ] || return 0
  [ -e "$2" ] && [ ! -L "$2" ] && return 0                   # already set up
  rm -f "$2" 2>/dev/null || true                              # a stale symlink
  mkdir -p "$(dirname "$2")"
  cp -c -R "$1" "$2" 2>/dev/null || cp -R "$1" "$2"
}
link() { [ -e "$1" ] && [ ! -e "$2" ] && ln -s "$1" "$2"; return 0; }
git -C "$DIR" config core.hooksPath .githooks  # tools/pii_check.py hooks
mkdir -p "$DIR/build"
link "$MAIN/baserom.gba" "$DIR/baserom.gba"
link "$MAIN/build/tools" "$DIR/build/tools"
link "$MAIN/build/venv" "$DIR/build/venv"
link "$MAIN/tools/emu/harness" "$DIR/tools/emu/harness"
mkdir -p "$DIR/build/scope"
link "$MAIN/build/scope/agbcc" "$DIR/build/scope/agbcc"  # scope's compiler: tools/scope/build_agbcc.sh
clone "$SHARED/gen" "$DIR/gen"
clone "$SHARED/build/res" "$DIR/build/res"
clone "$SHARED/build/text" "$DIR/build/text"
clone "$SHARED/build/boktai3.elf" "$DIR/build/boktai3.elf"
# compiled objects with their content-hash stamps: a build here recompiles only what changed
clone "$SHARED/build/src" "$DIR/build/src"
clone "$SHARED/build/asm" "$DIR/build/asm"
HERE=$(git -C "$DIR" rev-parse HEAD)
if [ "$HERE" != "$BASEREV" ] && ! git -C "$DIR" diff --quiet "$BASEREV" "$HERE" -- symbols data tools/symbolize.py; then
  echo "note: symbols differ from the base ($(git -C "$SHARED" rev-parse --short HEAD)); gen/ labels may be stale." >&2
  echo "      Either start from the base commit, or run 'make gen/code_sym.s' here once (slow)." >&2
fi
echo "$DIR"
