#!/bin/sh
# Keep the shared base checkout (../b3-base) at origin/main and fully built, so new
# worktrees (tools/worktree.sh) can clone its gen/, build/res/, build/text/ and ELF
# instead of building. Run after merges (tools/unit_integrate.py does).
#   tools/base.sh            create or refresh, build, print the commit
set -e
MAIN=$(git -C "$(dirname "$0")/.." worktree list --porcelain | sed -n '1s/^worktree //p')
BASE=${B3_BASE:-$(dirname "$MAIN")/b3-base}
git -C "$MAIN" fetch -q origin
if [ ! -d "$BASE" ]; then
  git -C "$MAIN" worktree add -q --detach "$BASE" origin/main
  mkdir -p "$BASE/build"
  ln -s "$MAIN/baserom.gba" "$BASE/baserom.gba"
  ln -s "$MAIN/build/tools" "$BASE/build/tools"
  [ ! -d "$MAIN/build/venv" ] || ln -s "$MAIN/build/venv" "$BASE/build/venv"
  for d in gen build/res build/text; do                         # seed from the main checkout
    [ -e "$MAIN/$d" ] && cp -c -R "$MAIN/$d" "$BASE/$d" 2>/dev/null || true
  done
fi
cd "$BASE"
git reset -q --hard
git checkout -q --detach origin/main
make -j3 > build/base.log 2>&1 || { tail -5 build/base.log; exit 1; }
grep -q 'build/boktai3.gba: OK' build/base.log || { tail -5 build/base.log; exit 1; }
echo "base $(git rev-parse --short HEAD): OK"
