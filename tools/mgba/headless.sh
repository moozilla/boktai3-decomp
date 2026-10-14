#!/bin/sh
# Run a Lua script against the game in the patched headless mGBA (no window).
#
#   tools/mgba/headless.sh SCRIPT.lua [ROM] [SAVE]
#
# ROM defaults to baserom.gba, SAVE to tests/saves/ShinBok2.sav (use "none" for
# a fresh boot). Breakpoints are on; the clock is fixed like the harness
# (BOKTAI3_RTC). The script runs unthrottled until it calls os.exit().
# Example: a script that does dofile("tools/mgba/centaur.lua"), replay(...),
# capture("x"), then os.exit(0).
set -e
REPO=$(cd "$(dirname "$0")/../.." && pwd)
HEADLESS=${HEADLESS:-$HOME/git/mgba/build/mgba-headless}
SCRIPT=$(cd "$(dirname "$1")" && pwd)/$(basename "$1")
ROM=${2:-$REPO/baserom.gba}
SAVE=${3:-$REPO/tests/saves/ShinBok2.sav}
TMP=$(mktemp -d)
cp "$ROM" "$TMP/game.gba"
[ "$SAVE" = none ] || cp "$SAVE" "$TMP/game.sav"
cd "$TMP"
CENTAUR_HEADLESS=1 MGBA_SCRIPT_DEBUGGER=1 "$HEADLESS" --script "$SCRIPT" game.gba 2>&1 | grep -E "^Scripting:" || true
rm -rf "$TMP"
