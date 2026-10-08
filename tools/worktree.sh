#!/bin/sh
# Create an isolated worktree for one parallel worker:
#   tools/worktree.sh NAME [BASE]   -> ../wt/NAME on branch work/NAME
# Shares the (gitignored) generated asm in gen/, the ROM and the built tools with
# the main checkout via symlinks; build/ outputs stay per-worktree.
set -e
MAIN=$(cd "$(dirname "$0")/.." && pwd)
NAME=$1; BASE=${2:-HEAD}
DIR=$(dirname "$MAIN")/wt/$NAME
git -C "$MAIN" worktree add -q -b "work/$NAME" "$DIR" "$BASE"
ln -s "$MAIN/baserom.gba" "$DIR/baserom.gba"
ln -s "$MAIN/gen" "$DIR/gen"
mkdir -p "$DIR/build"
ln -s "$MAIN/build/tools" "$DIR/build/tools"
[ -f "$MAIN/tools/emu/harness" ] && ln -s "$MAIN/tools/emu/harness" "$DIR/tools/emu/harness"
echo "$DIR"
