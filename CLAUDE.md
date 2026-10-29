# Working on the Boktai 3 decomp (notes for agents and humans)

Read `README.md` first, then `docs/HANDOFF.md` (current state and next steps). Look
things up in `docs/INDEX.md` and with `python3 tools/docindex.py ADDR` rather than
reading docs in full; `docs/history/` is a record, not required reading. The ROM (`baserom.gba`, SHA-1 `2651c5e6…`) is never
committed, and neither is anything generated from it (`build/`, `extracted/`).
Never commit personal information (real names, usernames, `/Users/<name>` paths;
write `~`). The hooks in `.githooks` run `tools/pii_check.py`.

## Ground rules

* New worktree: `tools/worktree.sh --here` (seconds; clones the built base in
  `../b3-base`, no build). Workers use `tools/check.py`, never a full build.
* Fast exact checks suffice for working-branch commits. Run one combined ROM
  build before batch integration into main (`build/boktai3.gba: OK`). Each batch
  integration PR also regenerates `PROGRESS.md` and `PROGRESS.json` with
  `python3 tools/progress.py --markdown PROGRESS.md` (see `AGENTS.md`). Rebuild or
  run relocation/emulator tests again only when changes or failures justify it.
* Matching means byte-identical. No hand-written instructions inside `asm()`
  to force a match. The one exception is source the original authors wrote as
  inline assembly: the SDK's `SystemCall(n)` (`include/gba/syscall.h`, an
  in-place `swi n`, as in Power_EnterSleep and m4a). It counts as a normal match;
  a native port implements `SystemCall` in its BIOS layer.
  `INCLUDE_ASM` is the placeholder for unfinished functions.
  Nonmatching C goes on `main` only inside `#ifdef NONMATCHING` with an
  `INCLUDE_ASM` `#else` branch (see `docs/WORKER.md`). The default build never
  compiles it; it is reported as a fuzzy score, never as exact progress.
* One translation unit per change/PR. A unit is a `src/*.c` file covering a
  **contiguous** run of functions in ROM order.
  The user-authorized orchestration exception is a reviewed batch integration
  PR: preserve separate per-function commits, validate the combined ROM,
  then merge once into `main` (see `AGENTS.md`).
* Don't edit generated files (`build/asm/*`). Names go in
  `symbols/functions.csv` / `symbols/data.csv`; pointer facts go in
  `symbols/pointers.txt` / `symbols/nonpointer_*.txt`.
* Report hierarchy (decomp.dev): category -> unit -> function. Name functions
  `Prefix_Action` with one prefix per object; after a naming batch run
  `make disasm && python3 tools/subsystems.py --regroup`. Placement rules are in
  `docs/REPORT_SUBSYSTEMS.md`.
* Use useful provisional names without a separate proof exercise. Mark guesses
  `confidence=low` and record the current interpretation; revise later.

## Decompiling a function

The project is in phases 2 and 3 of four (draft, match, no concessions, cleanup;
`docs/HANDOFF.md`). Aim for the final form: matching C with no pins, written like
the original. Go through a subsystem in ROM order and stay on a function until it
matches. The loop is `tools/scope/WORKER.md`; the frame is `docs/WORKER.md`.

1. Look at it: `python3 tools/scope/scope.py rom FUNC` (calls and literals named),
   `scope.py like FUNC` and `tools/relatives.py FUNC --c` (similar matched C),
   `scope.py fields OFF...` (structs with members at those offsets).
2. Compile a copy of the unit with scope: `python3 tools/scope/scope.py FILE.c FUNC`
   prints MATCH or the differing hunks with the C line and compiler pass behind each.
   `why UID` explains a difference; confirm with a what-if knob
   (`AGBCC_PRI=name:+N`, ...), then find C with the same effect.
3. Land a match with `scope.py land FUNC` (merges into the unit, formats, checks
   it); `tools/check.py FILE.c` is the exact check of one unit. Unfinished
   neighbours stay as `INCLUDE_ASM("asm/nonmatching", sub_XXXXXXXX);`.
4. Compiler facts: `agbcc -O2 -mthumb-interwork` (pret's agbcc). Leaf
   functions push `lr`, which agbcc does and old_agbcc doesn't. scope's
   findings: `tools/scope/README.md` "agbcc facts established with scope";
   the Yu-Gi-Oh! EDS catalog: `docs/external/eds/matching-tricks.md`.
5. Library code exists already. MP2K (`m4a`) functions match pokeemerald's
   `src/m4a.c` byte-for-byte (43 identified, see `symbols/functions.csv`),
   and `libagbsyscall` is standard. Use `tools/sigmatch.py OBJ.o` to find more
   library code (agb_flash, siirtc, libgcc/libc pieces).

## Mapping data

* Run the game headless: `tools/emu/harness baserom.gba tests/X.txt OUTDIR`
  (with `BOKTAI3_SAV=tests/saves/ShinBok2.sav` for a late-game save, or
  `BOKTAI3_SAV=build/saves/NNNN.sav` for any of 83 checkpoints across the story:
  `python3 tools/assets/saves.py bank --make` builds them from the savestate bank
  in `../boktai3trans/saves`, `saves.py bank` lists them). Commands are
  listed at the top of `tools/emu/harness.c`. `mark NAME` records coverage per
  segment; `dump`, `watch ADDR:LEN` and `shot` are available.
* `tools/coverage.py diff A.read B.read` lists what one screen reads that
  another doesn't. `tools/vram_map.py PREFIX` maps on-screen tiles to ROM.
* After new emulator runs, fold the evidence in:
  `python3 tools/runtime_ptrs.py RUN_DIR...`, then `make` and `make shifttest`.

## When the shift test fails

1. Dump RAM periodically on both ROMs (see `tools/ramdiff.py` docstring) and
   find the first word that differs by something other than the shift.
2. `watch` that address to find the instruction that writes it, then work
   back to the ROM data it came from.
3. The fix is almost always one of two things: data misread as a pointer
   (add a `nonpointer_ranges.txt` range or an emulator run that reads it), or
   a pointer the scan can't see (unaligned or in a structure; add it to
   `pointers.txt` or teach a walker like `tools/m4a.py`).

Local register pinning is user-authorized as a temporary matching concession.
Mark affected translation units `// MATCHING_CONCESSION: register-pinning`;
count exact bytes normally and report them in the Register-pinned category.
Retain readable C, fast exact checks, and batch validation; defer removing the
hints. This does not authorize handwritten instruction bodies.
