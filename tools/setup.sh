#!/bin/sh
# Fetch and build the pinned third-party tools into build/tools/.
# System packages needed (Debian/Ubuntu):
#   apt install build-essential cmake binutils-arm-none-eabi libmgba-dev python3-numpy python3-pil
#   pip install capstone
set -e
cd "$(dirname "$0")/.."
ROOT=$(pwd)
mkdir -p build/tools
cd build/tools

fetch() { # name url commit
    if [ ! -d "$1" ]; then git clone -q "$2" "$1"; fi
    git -C "$1" fetch -q origin "$3" 2>/dev/null || true
    git -C "$1" checkout -q "$3"
}

# gbadisasm (pret lineage): recursive-descent disassembler emitting reassemblable GNU as
fetch gbadisasm https://github.com/jiangzhengwenjz/gbadisasm.git 940def2c91d678ee671716b8b44a2223c0ff2c62
PATCH="$ROOT/tools/patches/gbadisasm-no-assert.patch"
if git -C gbadisasm apply --check "$PATCH" 2>/dev/null; then
    git -C gbadisasm apply "$PATCH"
else
    # An already-applied patch is fine; an incompatible source tree is not.
    git -C gbadisasm apply --reverse --check "$PATCH"
fi
# Indexed label lookups: identical output, a pass takes seconds instead of minutes.
PATCH="$ROOT/tools/patches/gbadisasm-fast.patch"
if git -C gbadisasm apply --check "$PATCH" 2>/dev/null; then
    git -C gbadisasm apply "$PATCH"
else
    git -C gbadisasm apply --reverse --check "$PATCH"
fi
make -s -C gbadisasm

# agbcc: the GCC 2.95-based compiler used by agbcc-era GBA games (matching decomp)
fetch agbcc https://github.com/pret/agbcc.git da598c1d918402c42c0c0d7128ba14567f3175e9
[ -x agbcc/agbcc ] || (cd agbcc && ./build.sh)

# armips: for translation hacks (patch assembly). MINIMAL=1 skips it and the mGBA harness.
if [ -z "$MINIMAL" ]; then
fetch armips https://github.com/Kingcom/armips.git 62adab4ef30da765f5cf22a451eb08a59c54dc8b
git -C armips submodule update -q --init --recursive
[ -x armips/build/armips ] || (mkdir -p armips/build && cd armips/build && cmake -DCMAKE_BUILD_TYPE=Release .. >/dev/null && make -s)
fi

# Optional matching search; its parser is bundled, and Python needs toml.
# The project patch adds agbcc passes, crash fixes and --focus support
# (docs/PERMUTER.md); an older version of the patch is replaced.
fetch decomp-permuter https://github.com/simonlindholm/decomp-permuter.git 8556c81d80d1c1af98a858c8f4dc951357f29139
PATCH="$ROOT/tools/patches/decomp-permuter-agbcc.patch"
if git -C decomp-permuter apply --check "$PATCH" 2>/dev/null; then
    git -C decomp-permuter apply "$PATCH"
elif ! git -C decomp-permuter apply --reverse --check "$PATCH" 2>/dev/null; then
    git -C decomp-permuter checkout -q -- .
    git -C decomp-permuter clean -qfd src
    git -C decomp-permuter apply "$PATCH"
fi
# the patch changes the parser grammar (statement expressions anywhere an
# expression goes): rebuild its tables when they predate that
grep -q "primary_expression -> LPAREN compound_statement RPAREN" decomp-permuter/perm_pycparser/yacctab.py ||
    (cd decomp-permuter/perm_pycparser && rm -f yacctab.py && python3 _build_tables.py >/dev/null)

# mGBA harness
[ -n "$MINIMAL" ] || make -s -C ../../tools/emu

echo "tools ready in build/tools"
