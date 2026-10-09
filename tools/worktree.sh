#!/bin/sh
# Create an isolated worktree for one parallel worker:
#   tools/worktree.sh NAME [BASE]   -> ../wt/NAME on branch codex/NAME
# Shares the (gitignored) generated asm in gen/, the ROM and the built tools with
# the main checkout via symlinks; build/ outputs stay per-worktree.
set -e
MAIN=$(cd "$(dirname "$0")/.." && pwd)
NAME=$1; BASE=${2:-HEAD}
DIR=$(dirname "$MAIN")/wt/$NAME
git -C "$MAIN" worktree add -q -b "codex/$NAME" "$DIR" "$BASE"
ln -s "$MAIN/baserom.gba" "$DIR/baserom.gba"
ln -s "$MAIN/gen" "$DIR/gen"
mkdir -p "$DIR/build"
ln -s "$MAIN/build/tools" "$DIR/build/tools"
[ ! -d "$MAIN/build/venv" ] || ln -s "$MAIN/build/venv" "$DIR/build/venv"
# check.py needs a verified ELF to resolve calls; copy it so worker builds
# never overwrite a shared output. Full build verification is still required.
[ ! -f "$MAIN/build/boktai3.elf" ] || cp "$MAIN/build/boktai3.elf" "$DIR/build/boktai3.elf"
[ -f "$MAIN/tools/emu/harness" ] && ln -s "$MAIN/tools/emu/harness" "$DIR/tools/emu/harness"
echo "$DIR"
