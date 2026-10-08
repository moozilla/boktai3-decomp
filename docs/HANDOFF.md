# Handoff: state of the project and what to do next

Written at the end of the first working session (2026-10-08) so a new thread
can pick up without the chat history. Read `README.md`, `CLAUDE.md`,
`docs/WORKER.md`, `docs/ROM_MAP.md` and `docs/FINDINGS.md` too.

## Where things stand

* **Build:** the ROM rebuilds bit-identical (`make`), and all data is
  relocatable (`make shifttest` moves it 64 KiB and the emulator screenshots
  stay identical).
* **Decomp:** 1,430 / 10,986 functions in C (13.0% of functions, ~2.8% of code
  bytes; see `PROGRESS.md`). Mostly small functions in `src/fn/` (one file per
  function), plus `src/lib/m4a.c` (57 of 58 MP2K functions; `CgbSound` left).
  The function total grew from 7,890 to 10,986 when `tools/disasm.py` started
  seeding from function-pointer tables in the data region (session 2); the old
  byte percentages were inflated because undiscovered code was counted as part
  of the preceding function.
* **Names:** almost everything is still `sub_XXXXXXXX`. Proposed names sit in
  `symbols/proposed/*.csv` (mostly m4a) and have **not** been applied to
  `symbols/functions.csv` yet. Review them first.

## Rebuilding the environment in a new session

The ROM is never in git, so a new cloud session needs it uploaded again.
Then:

```
ln -s /path/to/rom.gba baserom.gba        # SHA-1 2651c5e6875ac60abff734510d152166d211c87c
apt install build-essential cmake binutils-arm-none-eabi libmgba-dev python3-numpy python3-pil
pip install capstone
make setup      # gbadisasm (patched), agbcc, armips, emulator harness
make disasm     # ~18 min: gen/code.s, then gen/code_sym.s + gen/data.s
make            # must print OK
```

Optional: the Ghidra export for `tools/ghidra_c.py` (`tools/ghidra/README.md`,
~25 min). Workers found it useful as a first draft.

## Parallel workers: what was learned

* Setup: `tools/worktree.sh NAME` gives each worker its own git worktree
  (`../wt/NAME`, branch `work/NAME`) that shares `gen/`, the ROM and the tools
  by symlink. Workers get disjoint address ranges (`tools/worklist.py --chunks N`)
  and follow `docs/WORKER.md`. The lead merges `work/*` branches into main.
  Merges were always conflict-free because each function is its own file.
* **Models:** Haiku 5.5 and Sonnet 5.5 both matched 8/8 on a pilot. Haiku used
  ~160–230k tokens per round of ~40 easy functions. Sonnet used ~140–190k per
  round of 40–55 and handled medium functions and the library lift.
  Second-round yields dropped for Haiku in some ranges (w3: 10, w5: 28),
  because the easy pool there is running out. Next rounds should use Sonnet
  for 30–80-instruction functions, Opus or a permuter for the skip lists.
* Each worker logs the functions it gave up on in `notes/wN.md`, with the
  reason. That's the backlog for stronger models.
* Lead-side cost was the expensive part (Opus, long context). Keep the lead
  thread short: merge, fix tooling, and delegate.

## Open tooling issues

* ~~Thumb function pointers in C~~: solved, they just work now (081F065C, 081FC61C matched).
* ~~Leaf functions starting `mov ip, r0`~~: solved, they come from struct array accesses (see WORKER.md).
* Jump-table functions are mostly unattempted (`JUMPTABLE` in worklist).
* ~~Per-unit check~~: `tools/check.py` (links one file at its address, diffs vs. ROM).
* ~~gbadisasm vs. Ghidra function counts~~: reconciled; data-table seeds bring gbadisasm to 10,986 (Ghidra: 10,686). Some code may still hide in `.incbin` blocks (no pointer to it anywhere): search for `push {..., lr}` prologues after returns.
* m4a: only `CgbSound` is left (`notes/cgb.md`, best attempt `notes/cgb_attempt.c`; register allocation only). `CgbModVol` and `m4aSoundVSync` are matched. Old note: `CgbModVol`, `CgbSound` (older SDK variant) and `m4aSoundVSync`
  (asm in the SDK) are still INCLUDE_ASM. `gMaxLines = 0` in
  `symbols/ram.ld` is an unverified placeholder.
* No decomp.dev report or CI yet. `tools/progress.py --json` is a start, and
  `report.json` can be generated without the ROM, as knidl's `gen_report.py`
  does.

## Coverage / "centaur" plan (next big lever)

1. **Record playthroughs**: `tools/mgba/centaur.lua` in desktop mGBA records
   inputs as harness scripts (`rec_start("name")`). The RTC and solar
   settings must match the harness defaults (see the script header). Replays
   feed `tools/runtime_ptrs.py` (pointer evidence) and per-screen coverage
   (`mark NAME` segments).
2. **Find the gaps**: list functions never executed in any run, grouped by
   caller and region, and map them to game features. Then play those parts
   deliberately.
3. **Force the rest**: add `poke ADDR VAL` and `call FUNC` commands to
   `tools/emu/harness.c`. Use them, and the Lua console (`poke32`, `watch`),
   to trigger unreached menus and scripts. The event bytecode keeps EUC-JP
   debug labels, which are good hints for debug menus and scenes.
4. **Name by context**: tag matched functions with the screens they run on
   and the data they read, and name subsystems in batches.

The current test runs cover only 1,709 functions (~6% of code). The
shiftability proof only covers those paths, so more coverage also hardens the
build.

## Cleanup backlog (later phase)

* Group `src/fn/*.c` into real translation units: contiguous files, shared
  headers, struct definitions instead of `*(u16 *)(p + 0x156)`.
* Apply reviewed names; add struct/RAM symbols (`symbols/ram.ld`).
* Classify the 1,483 song-table entries into BGM and SFX (`tools/m4a.py --list`).
* Split `gen/data.s` into named assets (graphics, tilemaps, palettes), with
  extraction to PNG and rebuild, as eds/knidl do.

## Session 2 (same day): what changed and what to do next

* **Tools added:** `tools/check.py` (one file vs. ROM, side-by-side diff,
  ~0.5 s), `tools/clones.py` (clone families: ports matched C to identical
  siblings; `--stats`, `--port --range`), `tools/permute.py` (decomp-permuter
  for agbcc), harness `poke`/`freeze`/`unfreeze`. `build.py` rejects raw ROM
  data addresses in C (they broke the shift test once; see FINDINGS).
* **Disassembly:** 10,986 functions (data-table seeds). Still missing: small
  leaf callbacks that don't start with `push` and are referenced from code
  literals only (e.g. `0806F54C`, `08070444`, `08070E44`). Extend
  `disasm.py` seeding to accept non-push entries referenced by code literals
  whose previous halfword is a return, then re-run `make disasm`.
* **Workflow that worked:** Sonnet workers on disjoint ranges, 60–90 matches
  per round at ~110–230k tokens each; merge, then run
  `tools/clones.py --port` over idle ranges after each merge (it found 77
  functions for free). Run `make shifttest` after big merges.
* **Unsolved codegen classes** (logged across `notes/s*.md`; good targets for
  Opus or the permuter):
  1. A big constant built from another constant's register (`movs r2,#0x94;
     lsls; ... subs r2,#4`) instead of separate literals.
  2. Unoptimized bool merge blocks (`beq; movs 1; b; movs 0; cmp r0,#0`):
     agbcc threads them away. Bit-allocator loops need this.
  3. "Copy constant to a second register" in fill loops (`adds r2,r1,#0`).
  4. `bl f; cmp r0,#0; bne; movs r0,#0` (redundant null check) folds to
     `return f()`.
  5. Pure register-allocation swaps (most skips): try `tools/permute.py`.
  These may hint at a slightly different compiler build or flags for some
  files; worth testing `old_agbcc` and `-O1` per class before brute force.
* **CgbSound:** last m4a function; best attempt + permuter result in `notes/`.

## Translation (separate repo)

The translation lives in [boktai3trans](https://github.com/moozilla/boktai3trans).
Its branch `claude/kind-shannon-ugkern` has the first-session text tooling
under `decomp/` (`tools/text_dump.py`, `tools/charmap.py`, `text/script.jsonl`
with Japanese + 2007 English per string). Translation-side ideas:

* a text inserter with box-width checks that uses the 1.47 MB of free space
* `{1F}{xx}` accents
* a variable-width font via `Text_DrawGlyph` (`08218D1C`)

Lan Hikari's 0.9 PR on that repo is still open and untouched. The earlier
recommendation was to merge it as-is for players and ask for the sources
behind the binary patches.
