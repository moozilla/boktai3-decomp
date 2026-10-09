# Boktai 3 decomp

A matching decompilation of **Shin Bokura no Taiyou: Gyakushuu no Sabata**
(Boktai 3, GBA, Japan: `BOKTAI3` / `U33J`). The goal is C source that compiles
back to the original ROM byte-for-byte. The English fan translation
([boktai3trans](https://github.com/moozilla/boktai3trans)) will be built on
top of it, but the translation lives in its own repo.

📈 **Progress: [decomp.dev](https://decomp.dev/moozilla/boktai3-decomp) · [PROGRESS.md](PROGRESS.md).**

**Status:**

| | |
|---|---|
| Disassembly | 11,025 functions (incl. callbacks found via data-region pointer tables), reassembles **bit-identical** (`make compare`) |
| Shiftable | every ROM pointer is a symbol; all 13.7 MB of data can move. Moving it by 64 KiB plays **pixel-identical** in every scripted test (`make shifttest`) |
| Compiler | identified as **agbcc** (pret's GCC 2.95 GBA compiler), `-O2 -mthumb-interwork` |
| C pipeline | `src/*.c` → agbcc → spliced into the ROM in place of the asm; `INCLUDE_ASM` for unfinished functions; **4,203 functions matched** (see PROGRESS.md); `tools/check.py` per-function diff, `tools/permute.py` for decomp-permuter |
| Libraries | 57 of 58 MP2K (`m4a`) functions in C (`src/lib/m4a.c`, from pret's pokeemerald plus older-SDK variants) (`tools/sigmatch.py`); `libagbsyscall` identified |
| Text | script bank format decoded (9,976 strings round-trip byte-exact); the text itself is handled by the translation repo |
| Sound | MP2K engine; 1,483 songs, 5,525 track streams, 81 voicegroups, 415 samples walked |
| Symbols | `symbols/` — named functions (incl. raphaelr's), data map, confirmed pointers |

See [docs/ROM_MAP.md](docs/ROM_MAP.md) for what lives where,
[docs/FINDINGS.md](docs/FINDINGS.md) for engine notes,
[docs/screens/](docs/screens/) for per-screen graphics sources, and
[CLAUDE.md](CLAUDE.md) for the decompilation workflow, and
**[docs/HANDOFF.md](docs/HANDOFF.md) for current state, open issues and next steps.**

## Setup

You need your own copy of the ROM:

```
baserom.gba   SHA-1 2651c5e6875ac60abff734510d152166d211c87c
```

Put it (or a symlink) at `baserom.gba` in this repository's root.
`BOKTAI3_ROM` also selects the input for the ROM-reading Python tools, but the
assembler and regression scripts expect the root-level file or symlink.
Nothing derived from the ROM is committed: generated assembly lives in `gen/`
and build output in `build/`, and both are gitignored. See
[THIRD_PARTY.md](THIRD_PARTY.md) for what comes from elsewhere. Our own code is
MIT ([LICENSE](LICENSE)).

System packages (Debian/Ubuntu):

```
apt install build-essential cmake binutils-arm-none-eabi libmgba-dev python3-numpy python3-pil
pip install capstone
make setup          # pinned gbadisasm, agbcc, armips, permuter, and mGBA harness
```

On macOS with Homebrew and the Xcode Command Line Tools installed:

```sh
brew install cmake arm-none-eabi-binutils mgba python
python3 -m venv build/venv
source build/venv/bin/activate
python -m pip install -r requirements.txt
make setup
make disasm
make
```

Activate `build/venv` in each new shell before using the Python tools. The
harness Makefile detects Homebrew's mGBA headers and library on macOS; set
`MGBA_PREFIX` if it lives elsewhere. All third-party source and build products
stay under the ignored `build/` directory.

For ROM-free GitHub Actions reporting and decomp.dev registration, see
[docs/PROGRESS_CI.md](docs/PROGRESS_CI.md).

## Building

```
make disasm         # generate gen/code.s (gbadisasm, ~18 min) and symbolize it into gen/
make                # split around src/*.c, compile C with agbcc, link, verify SHA-1
make shifttest      # move all data by 64 KiB and compare emulator screenshots
```

## Layout

```
asm/          hand-written asm (macros, top-level ROM for the pure-asm build)
include/      C headers
src/          decompiled C, one file per contiguous run of functions
symbols/      functions.csv, data.csv  -- names (source of truth for labels)
              pointers.txt             -- ROM words proven to be pointers (emulator)
              nonpointer_*.txt         -- ranges proven/declared not to hold pointers
gen/          generated asm (gitignored, shared by worktrees)
notes/        per-worker notes (functions that resisted matching)
tests/        input scripts for the emulator harness
tools/        everything that generates or checks the above
  disasm.py       run gbadisasm with seeds (BL targets, pointer-derived), iterate
  symbolize.py    make every ROM pointer symbolic; split data into labeled slices
  split.py        cut asm around C units; build.py compiles/links/compares
  m4a.py          MP2K sound walker (songs, tracks incl. unaligned GOTO/PATT, voices, samples)
  emu/harness.c   headless mGBA: scripted input, screenshots, code/data coverage,
                  word-load pointer evidence, RAM dumps, write watchpoints, fixed RTC
  runtime_ptrs.py turn harness evidence into symbols/pointers.txt + nonpointer_runtime.txt
  shift_test.sh   shiftability regression; ramdiff.py / shift_diff.py to debug it
  vram_map.py     which ROM bytes the tiles on screen came from
  sigmatch.py     find library functions (compiled .o) in the ROM, relocation-masked
  progress.py     functions/bytes decompiled (+ objdiff-style JSON)
  ghidra/         headless Ghidra import/analysis/export scripts
  mgba/centaur.lua  desktop mGBA script: record playthroughs as harness scripts, peek/poke/watch
  worklist.py     remaining functions with difficulty signals; --chunks for parallel ranges
  worktree.sh     isolated worktree for a parallel worker
  ghidra_c.py     Ghidra pseudo-C for a function (draft only)
  gfx.py, lz77.py, coverage.py, asmat.py
```

## Decompiling a function

1. `python3 tools/asmat.py 08033568` shows the asm.
2. Write C in `src/` (functions in a file must be contiguous, in ROM order;
   phase 1 uses one file per function in `src/fn/`, see [docs/WORKER.md](docs/WORKER.md)).
   RAM globals can be referenced as `gUnk_0200XXXX`; the linker defines them.
3. `make` — prints `OK` or the first mismatching address.

Matching notes so far: plain `agbcc -O2 -mthumb-interwork` reproduces leaf
functions including the always-pushed `lr`; `old_agbcc` does not (it omits the
push in leaf functions). Rename a function by adding it to
`symbols/functions.csv` — no re-disassembly needed.

## How shiftability was proven

Pointer detection is heuristic (aligned words in ROM range, table shapes,
MP2K structure walking) plus evidence from the emulator: the harness records
which ROM words the program loads with `LDR`/`LDM` (pointers) versus words it
only reads as plain data (halfword loads, DMA, BIOS copies). Bulk-copy loops are
discounted. The shift test then moves all data and diffs screenshots; RAM
snapshots (`tools/ramdiff.py`) pinpoint the first divergence when it fails.

The tests so far cover boot/setup, title, new game + intro cutscene, a
late-game save, every pause-menu tab and some field play — about 6% of the
code. Areas no test reaches may still hide a bad pointer guess; extending
`tests/` with more of the game is the way to find them.
