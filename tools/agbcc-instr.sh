#!/bin/sh
# Build an instrumented copy of agbcc in build/agbcc-instr for tools/rasolve.py.
# It applies tools/patches/agbcc-instr.patch to the agbcc source that
# tools/setup.sh fetched. Without its environment variables (AGBCC_PRI,
# AGBCC_FORCE, AGBCC_SPILL, AGBCC_DUMPPRI) it emits the same code as agbcc;
# the ROM build never uses it.
set -e
cd "$(dirname "$0")/.."
SRC=build/tools/agbcc/gcc
OUT=build/agbcc-instr
[ -d "$SRC" ] || { echo "run tools/setup.sh first"; exit 1; }
if [ -d "$OUT/gcc" ]; then mv "$OUT" "$HOME/.Trash/agbcc-instr-$$"; fi
mkdir -p "$OUT"
cp -R "$SRC" "$OUT/gcc"
cp -R build/tools/agbcc/include "$OUT/include" 2>/dev/null || true
(cd "$OUT/gcc" && patch -s -p1 < ../../../tools/patches/agbcc-instr.patch \
    && rm -f agbcc local-alloc.o global.o reload1.o stmt.o function.o integrate.o)
make -s -C "$OUT/gcc" -j4 >/dev/null 2>&1 || make -C "$OUT/gcc"
echo "built $OUT/gcc/agbcc"
