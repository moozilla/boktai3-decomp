#!/bin/sh
# Build an instrumented, optimized copy of agbcc in build/agbcc-instr for
# tools/rasolve.py and tools/agbserve.py. It applies
# tools/patches/agbcc-instr.patch to the agbcc source that tools/setup.sh
# fetched and compiles the compiler with -O2 (agbcc's own Makefile uses -g
# and no optimization; -O2 makes it about 1.8 times faster).
# Without its environment variables (AGBCC_PRI, AGBCC_FORCE, AGBCC_SPILL, AGBCC_GCSE_T, AGBCC_NOTIE, CSE_KEEP, CSE_NOCANON, CSE_DEBUG, CSE_PATH, CSE_NOFOLLOW, AGBCC_NOPRE, AGBCC_PRE_TRACE, AGBCC_NOSTRICT, AGBCC_NOINHERIT, AGBCC_NORCSE, AGBCC_NOEQUIVMOVE, AGBCC_WHYREG, AGBCC_PREF,
# AGBCC_DUMPPRI, AGBCC_SERVER) it emits the same code as agbcc: checked on
# every C file in src/ and the guarded drafts. The ROM build never uses it.
set -e
cd "$(dirname "$0")/.."
SRC=build/tools/agbcc/gcc
OUT=build/agbcc-instr
[ -d "$SRC" ] || { echo "run tools/setup.sh first"; exit 1; }
if [ -d "$OUT" ]; then mv "$OUT" "$HOME/.Trash/agbcc-instr-$$"; fi
mkdir -p "$OUT"
cp -R "$SRC" "$OUT/gcc"
(cd "$OUT/gcc" && patch -s -p1 < ../../../tools/patches/agbcc-instr.patch)
make -s -C "$OUT/gcc" clean >/dev/null 2>&1 || true
# -j1: the generated headers race in a parallel build
make -s -C "$OUT/gcc" -j1 BASE_CFLAGS="-O2 -std=gnu11 -Werror-implicit-function-declaration \
-Wno-error=incompatible-pointer-types -Wno-everything" >/dev/null 2>&1
echo "built $OUT/gcc/agbcc"
