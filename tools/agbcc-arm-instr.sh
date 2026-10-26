#!/bin/sh
# Build an instrumented copy of the ARM compiler (agbcc_arm, pret's gcc_arm cc1)
# in build/agbcc-arm-instr. It applies tools/patches/agbcc-arm-instr.patch to
# the gcc_arm source that tools/setup.sh fetched. The knobs are the Thumb
# patch's (tools/patches/agbcc-instr.patch) plus loop/sched/gcse ones added
# while matching sub_08000430 (src/iwram_code.c):
#   AGBCC_PRI, AGBCC_PRI_LABEL, AGBCC_FORCE, AGBCC_FORCE_LABEL, AGBCC_DUMPPRI,
#   AGBCC_SPILL*, AGBCC_ADDSPILL, AGBCC_NOTIE       register allocation
#   AGBCC_NOMOVE, AGBCC_NOMOVE1, AGBCC_FORCEMOVE, AGBCC_FORCEMOVE2,
#   AGBCC_GIV, AGBCC_NOGIV, AGBCC_LOOP_TRACE         loop.c invariant motion
#   AGBCC_NOUNIT1=insn,...                           sched1 treats insns as unit-less
#   AGBCC_NOPRE=expr,..., AGBCC_PRE_TRACE,
#   AGBCC_GCSE_T=N, AGBCC_GCSE_DBG                   gcse PRE and its table size
#   CSE_KEEP, CSE_NOCANON, CSE_DEBUG, CSE_PATH(S), CSE_NOFOLLOW(_JUMP), CSE_WHY
# The patched sources document each knob where it is read. Without the
# environment variables it emits the same code as agbcc_arm. The ROM build
# never uses it. Run: build/agbcc-arm-instr/gcc/cc1 -quiet -O2 -mthumb-interwork X.i
set -e
cd "$(dirname "$0")/.."
SRC=build/tools/agbcc/gcc_arm
OUT=build/agbcc-arm-instr
[ -d "$SRC" ] || { echo "run tools/setup.sh first"; exit 1; }
if [ -d "$OUT" ]; then mv "$OUT" "$HOME/.Trash/agbcc-arm-instr-$$"; fi
mkdir -p "$OUT"
cp -R "$SRC" "$OUT/gcc"
# gcc_arm's Makefile reads ../libiberty, ../include and friends
for d in libiberty include ginclude config.if config.sub; do ln -s "../../build/tools/agbcc/$d" "$OUT/$d"; done
(cd "$OUT/gcc" && patch -s -p1 < ../../../tools/patches/agbcc-arm-instr.patch)
# the copied objects look up to date; rebuild them from the patched sources
rm -f "$OUT"/gcc/*.o "$OUT/gcc/cc1"
# -j1: the generated headers race in a parallel build
make -s -C "$OUT/gcc" -j1 cc1 BASE_CFLAGS="-O2 -std=gnu11 -Werror-implicit-function-declaration \
-Wno-error=incompatible-pointer-types -Wno-everything" >/dev/null 2>&1
echo "built $OUT/gcc/cc1"
