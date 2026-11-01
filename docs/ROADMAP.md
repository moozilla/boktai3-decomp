# Roadmap: the four phases and the side threads

Tags: #process

The project's plan, as the owner sets it. [HANDOFF.md](HANDOFF.md) says where things stand
now; this file says where they are going. Numbers live in [PROGRESS.md](../PROGRESS.md).

## The four phases

Every function passes through all four, but **aim straight for the last**: when you touch a
function, write it the way it will finally be.

1. **Draft** (done). Every function in the game has C: an exact match, or a guarded
   `#ifdef NONMATCHING` draft with a fuzzy score.
2. **Match** (nearly done, about 96% of code). Every function compiles byte for byte to the ROM.
   Register pins and empty `asm("")` barriers were allowed in this phase as a stopgap, and about
   a quarter of matched code still has them (the Register-pinned category in PROGRESS.md). The
   last drafts aim for matches without them; a pin is now a last resort for the hardest
   functions (`land --allow-pins`), and phase 3 work for later.
3. **No concessions** (current). Every match is ordinary C: no register pins, no barriers, no
   compiler overrides, no `*(u16 *)((u8 *)p + 0x172)` access where a struct field fits.
   `progress.py --list-pinned` is the queue; `scope.py FILE.c pins FUNC` gives the cost of removing
   each pin, alone and grouped, and writes the pin-free draft a rematch starts from (the free removals
   `tools/depin.py` made are used up).
4. **Cleanup** (where it comes naturally; the main push comes after phase 3). Source that reads
   like the original and builds the same ROM:
   * **Names.** No `sub_XXXXXXXX` functions or `gUnk_XXXXXXXX` RAM objects left: every function,
     RAM object and ROM table has a real name (`Prefix_Action` for functions, see
     [REPORT_SUBSYSTEMS.md](REPORT_SUBSYSTEMS.md)), with `confidence=low` where it is a guess.
   * **Types.** Shared structs, enums and inline helpers in `include/`, one declaration each;
     comments that explain the game, not the matching.
   * **A real source tree.** Today most functions sit in their own `src/fn/sub_XXXXXXXX.c`. They
     move into units that cover contiguous runs of functions in ROM order, grouped by subsystem.
   * **The build places everything itself.** Code links in ROM order, and RAM objects are
     defined in sections whose order gives the original EWRAM/IWRAM addresses, so nothing in the
     source depends on an address. Today RAM objects are absolute symbols (`symbols/ram.csv`,
     [RAM.md](RAM.md)); the linker script and build tools will check that each object lands where
     the original put it. The design for this is still open.
   * **Studio.** One consistent, intuitive interface over the views added in the matching phases
     (several model-built views are functional but hard to read).

Phase 4 is also when the remaining `NONMATCHING` scaffolding, per-file helper copies and
compatibility declarations go.
It also revisits `goto` loops: phase 3 accepts one where a structured loop would be rewritten
by loop.c (a reversed counter, for example), and cleanup looks for the structured form the
original more likely had.

## Side threads

These run alongside the phases, and each feeds the others:

* **RAM annotation.** Every EWRAM/IWRAM object named, sized and typed, with one shared
  declaration ([RAM.md](RAM.md)); tracked in PROGRESS.md.
* **Data annotation.** ROM data decoded into editable forms (CSV, text, images) that
  round-trip in the build ([DATA.md](DATA.md), [DATA_TRACKING.md](DATA_TRACKING.md)).
  decomp.dev's data measure is this round-trip.
* **Studio support** (`tools/studio/serve.py`). Every decoded table, actor, script, map and
  asset browsable and, where it makes sense, editable.
* **Translation and modding**, which the decomp serves: GCL scripts ([gcl/README.md](../gcl/README.md)),
  cutscenes ([CUTSCENES.md](CUTSCENES.md)), assets ([ASSETS.md](ASSETS.md)) and the fan
  translation [boktai3trans](https://github.com/moozilla/boktai3trans).
* **Tooling.** scope and matchsearch get better with every gap a worker reports
  ([tools/scope/README.md](../tools/scope/README.md) "Gaps").
