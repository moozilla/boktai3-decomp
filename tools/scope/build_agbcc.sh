#!/bin/sh
# scope's own instrumented agbcc (tools/scope/agbcc.patch -> build/scope/agbcc/gcc/agbcc).
# Separate from tools/agbcc-instr.sh, which other tools extend, so neither breaks the other.
# It applies the patch to the agbcc source that tools/setup.sh fetched and
# compiles with -O2 (about 1.8 times faster than agbcc's own -g build).
# Without its environment variables (AGBCC_PRI, AGBCC_FORCE, AGBCC_SPILL, AGBCC_GCSE_T, AGBCC_NOTIE, CSE_KEEP, CSE_NOCANON, CSE_DEBUG, CSE_PATH, CSE_NOFOLLOW, AGBCC_NOPRE, AGBCC_PRE_TRACE, AGBCC_NOGIV, AGBCC_GIV, AGBCC_LOOP_TRACE, AGBCC_NOSTRICT,
# AGBCC_DUMPPRI, AGBCC_SERVER) it emits the same code as agbcc: checked on
# every C file in src/ and the guarded drafts. The ROM build never uses it.
set -e
cd "$(dirname "$0")/../.."
SRC=build/tools/agbcc/gcc
OUT=build/scope/agbcc
[ -d "$SRC" ] || { echo "run tools/setup.sh first"; exit 1; }
if [ -d "$OUT" ]; then mv "$OUT" "$HOME/.Trash/scope-agbcc-$$"; fi
mkdir -p "$OUT"
cp -R "$SRC" "$OUT/gcc"
(cd "$OUT/gcc" && patch -s -p1 < ../../../../tools/scope/agbcc.patch)
make -s -C "$OUT/gcc" clean >/dev/null 2>&1 || true
# -j1: the generated headers race in a parallel build
make -s -C "$OUT/gcc" -j1 BASE_CFLAGS="-O2 -std=gnu11 -Werror-implicit-function-declaration \
-Wno-error=incompatible-pointer-types -Wno-everything" >/dev/null 2>&1
echo "built $OUT/gcc/agbcc"
