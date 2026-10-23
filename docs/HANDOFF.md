# Handoff: current state and how to continue

Tags: #process

The entry point for a new agent or a thread after compaction. Read this file,
then `docs/WORKER.md` before matching. Everything else is looked up, not read:
[docs/INDEX.md](INDEX.md) maps every doc and note by topic, and
`python3 tools/docindex.py ADDR` lists what has been written about a function.
Dated progress reports live in [docs/history/](history/README.md); they are
records, not instructions. Keep this file short: it describes the present.

## Current state (2026-10-23)

Numbers live in one place: [PROGRESS.md](../PROGRESS.md) (regenerated at batch
integration with `python3 tools/progress.py --markdown PROGRESS.md`) and the
README badges (live from decomp.dev). Don't copy totals into other files.

| | |
|---|---|
| Build | `make` rebuilds the ROM byte-identical (SHA-1 `2651c5e6875ac60abff734510d152166d211c87c`, `build/boktai3.gba: OK`) |
| Data | fully relocatable; `make shifttest` compares 35 emulator screenshots after moving data |
| Report | every function in one of ~490 subsystem units under 11 game categories plus Libraries ([REPORT_SUBSYSTEMS.md](REPORT_SUBSYSTEMS.md)); main-only CI publishes to decomp.dev |

Recent work, newest first (details in [history](history/README.md)):

* **Vanargand initializer draft** (2026-10-23): improved the complete guarded
  initializer with script-flag and palette lifetimes. No new exact match.
  [Worker notes](../notes/cx-xhigh.md) retain the experiments and resume point
  at 080D2584 after the campaign wrap-up marker. Combined ROM validation passed.
* **Mummy damage and reaction drafts** (2026-10-23): matched the complete
  timer-driven damage callback with four register pins and improved guarded
  Golem, Mummy and Ghoul reaction drafts. [Worker notes](../notes/cx-xhigh.md)
  explain the reload diagnosis, safe minimization and wrap-up boundary.
  Both combined ROM validations passed.
* **Script combat actor family recovered** (2026-10-23): the initializer, actor
  update, projectile attack, collider setup and hit reaction now match exactly.
  Tagged NPC continuation details are in [worker notes](../notes/cx-sol02.md).

* **Mummy, reaction and Ivy reconstruction** (2026-10-23): checkpoint recovery and anchor approach match; complete guarded reaction and Ivy drafts preserve larger callbacks. [Worker notes](../notes/cx-sol05.md) identify the incomplete deep-pass resume point. Combined ROM builds passed.
* **State actor campaign continuation** (2026-10-23): three control, palette and target-turn matches plus complete guarded drafts; resume details in [worker notes](../notes/cx-sol11.md).
* **Effect constructors and actor colliders** (2026-10-23): sol02 recovers two
  Nidhoggr attack-effect constructors and the script combat actor collider
  initializer. Full draft work and remaining targets are in
  [worker notes](../notes/cx-sol02.md).

* **Every scene's procedures have working names** (2026-10-19):
  `tools/assets/gclroles.py` names scene entries by area (`white_forest_3880`)
  and 93% of their private procedures by role (`_stage`, `_doors`, `_enemies`,
  `_after_cutscene_198`, `_on_arrival`, `exit_to_3107`), from how they are
  referred to and what they call; the 59 most-referenced library procedures
  got names by hand in `proc.csv` (`ensure_scene_actor_manager`,
  `place_scene_npc`, `roll_item_drop`, `arm_defeat_count_event`...). The readable view and the Studio use them
  (confidence `auto`); `gcl/names/proc.csv` still wins
  ([gcl/README.md](../gcl/README.md#generated-names-for-scene-procedures)).
* **A showcase scene in the game** (2026-10-17): `tools/showcase/` writes new
  script procedures (a GCL assembler), dialogue, a cutscene table and stage
  paths into a ROM copy, and `video.sh` records the result with sound. It
  also corrected the cutscene dialogue decoding (each run is one message;
  docs/CUTSCENES.md).
* **Relatives: start from matched code that shares runs** (2026-10-17):
  `tools/relatives.py ADDR --c` lists the matched functions that share long
  instruction runs with an unmatched one (registers abstracted), how much each
  explains, and their C. `--port --keep` ported 10 functions to exact matches
  (clones whose matched sibling came later, and constant-only variants);
  [tasks/relatives.md](tasks/relatives.md) ranks 594 unmatched functions where a
  matched relative explains half or more. Step 2 of WORKER.md's loop.
* **Saves decoded; a save at every checkpoint** (2026-10-17): two files of
  three checksummed regions (script variables, game state, continuation;
  ENGINE.md section 14). `tools/assets/saves.py` reads saves and VBA-M
  savestates and writes saves; `saves.py bank --make` turns the 83-checkpoint
  savestate bank (`../boktai3trans/saves`) into battery saves that boot
  headless into their scene (all 83 checked). Story beats and scenes list
  their checkpoints in Browse.
* **Story progress and the first dungeon** (2026-10-17): story progress
  (`r708[0x3C]`) is the quest log, 173 values; `tools/assets/story.py` and
  Browse › Story list each beat with its scene, cutscene and the doors and
  scripts that test it. The Sealed Prison is worked through room by room
  ([gcl/examples/sealed_prison.md](../gcl/examples/sealed_prison.md)).
  Areas keep the labeling pass's numbers when scenes are added (`areas.number`).
* **Cutscenes decoded** (2026-10-17): every cutscene is a table of steps and
  messages (`gEventBytecodePtrs`, 240 of them) that the matched SceneMessages
  director plays; receivers are the player, NPCs, scene actors, camera,
  screen and sound, each with its own command switch. `tools/assets/cutscenes.py`
  prints them; the Studio's Browse › Cutscenes shows each with its timeline.
  The prologue is worked through end to end ([gcl/examples/prologue.md](../gcl/examples/prologue.md),
  `tests/prologue.txt`), and scene detection now finds 496 scenes (142 had
  `scene_start` flags other than 1). Decomp targets: `cutscenes.py --targets`
  (the player, NPC, scene-actor and camera dispatchers; [CUTSCENES](CUTSCENES.md)).
* **Scripts (GCL) became readable** (2026-10-15/16): the scene scripts are
  Konami GCL. `gcl/` holds recovered names (registry keys, commands,
  variables, defines, parameters, 199 procedures), the debug prints'
  translations and a worked example; the Studio's Scripts page shows a
  readable Python-style view, grouped into Konami's source files (scenes,
  library by category, shared; `tools/assets/gclfiles.py`) with single-use
  helpers inlined on request. See [gcl/README.md](../gcl/README.md).
* **Script decomp targets**: [tasks/script_targets.md](tasks/script_targets.md)
  ranks the native routines scripts call that are still unmatched (95 used,
  23 KB at the start). The campaign started 2026-10-16: thirteen batches matched 97
  of them, all of the original top 15 included (`trap`, 081D0C04 and
  `place_character`'s 081CE1BC too), plus shared callees such as
  AnimData_Load and Hud_SetMode
  ([notes](../notes/script-entry-points_2026-10-16.md)). The near-misses,
  FloorSwitch_Create and A4FC are guarded drafts; most of what is left is
  large and inlines the packed-command sprite stepper (an unpinned form is
  one register copy short; see the notes, batch 10). Regenerate the list with
  `python3 tools/assets/script_targets.py --markdown docs/tasks/script_targets.md`.
* **30% reached** (2026-10-15) with a credit correction: retained assembly
  falsely absorbed by small C helpers was removed from the count
  ([ASM_OUTPUT_QC.md](ASM_OUTPUT_QC.md), [SCALAR_APPROACH.md](SCALAR_APPROACH.md)).
* No permuter search or matching worker is running. Frozen searches and their
  candidates: [PERMUTER_HANDOFF_2026-10-15.md](PERMUTER_HANDOFF_2026-10-15.md).

## Before you start: don't redo work

1. `python3 tools/docindex.py ADDR` (or `grep -rn ADDR docs notes gcl`): earlier
   attempts, skip reasons, near-matches and names for that function or its range.
2. `python3 tools/worklist.py --range A B` shows what is unmatched;
   `grep ADDR symbols/functions.csv` the current name and its source.
3. A `#ifdef NONMATCHING` draft may already exist (`grep -rln ADDR src`); start
   from it and update its recorded score.
4. For a subsystem, find its doc in [INDEX.md](INDEX.md) (by tag or address
   range) before reading code.

## Standing rules and preferences

The rules are in `CLAUDE.md` and `AGENTS.md`; these are the user's standing
preferences gathered from the history:

* Batched integration: one function per commit on a working branch, one
  reviewed combined build (`build/boktai3.gba: OK`) per merge into `main`.
* Multi-worker matching campaigns need the user's explicit authorization
  (they used to be tied to an allowance floor). Tooling, naming and docs do not.
* Do not recheck the live decomp.dev page per batch; local reports and Actions
  suffice (2026-10-09).
* Report progress as a percentage and percentage-point gain, bytes second.
* Register pinning is an authorized, counted concession
  (`// MATCHING_CONCESSION: register-pinning`); hand-written instruction
  bodies are never allowed.
* Provisional names are welcome when marked tentative; don't block matching on naming.

## Setting up

The ROM is never committed (nor anything derived from it: `build/`, `gen/`,
`extracted/`). On the current Mac everything is set up: `baserom.gba`,
`build/venv` (`source build/venv/bin/activate`), `build/tools`, `gen/`.
On a fresh machine:

```
ln -s /path/to/rom.gba baserom.gba        # SHA-1 above (Japanese, U33J)
pip install -r requirements.txt            # plus arm-none-eabi binutils, cmake, libmgba
make setup      # gbadisasm (patched), agbcc, armips, permuter, emulator harness
make disasm     # ~15-20 min: gen/code.s, gen/code_sym.s, gen/data.s
make            # must print build/boktai3.gba: OK
```

README.md has the macOS details.

## Matching a function (the inner loop)

```
python3 tools/worklist.py --range 08000000 080492B0 --max-insns 80   # unmatched, easiest first
python3 tools/asmat.py 0800E08C 60      # the asm
python3 tools/check.py wip/sub_0800E08C.c   # side-by-side diff; ends MATCH/MISMATCH
python3 tools/build.py                  # combined build: must print OK
```

`docs/WORKER.md` is the playbook (agbcc tricks, data/RAM declarations, unit
rules, naming with `#define` aliases and `tools/promote_names.py`).
[REGALLOC.md](REGALLOC.md) covers steering register allocation;
[PERMUTER.md](PERMUTER.md) the order to try `tools/litsweep.py`,
`tools/regsolve.py` and `tools/permute.py FILE --run SECS --focus auto`.

Parallel workers (only when authorized): give each `docs/WORKER_ROUND.md`;
the earlier worktree layout, ranges, merge and clone-seeding procedure are in
[history: former section 5](history/2026-10-early-state.md#former-section-5-running-parallel-workers-what-worked).

## Tools

| Tool | What |
|---|---|
| `tools/build.py`, `tools/check.py`, `tools/worklist.py` | build, check one file, list unmatched |
| `tools/clones.py --port [--loose]` | copy matched C to identical clone functions; run after merges |
| `tools/progress.py` | numbers; `--json` for the decomp.dev report |
| `tools/docindex.py [ADDR / --tag T]` | docs index; who wrote about an address or topic |
| `tools/emu/harness`, `tools/mgba/` | headless mGBA scripted runs; desktop Centaur script ([CENTAUR.md](CENTAUR.md)) |
| `tools/studio/serve.py` | the Studio: text, sprites, maps, actors, scripts, code, live view (port 8642) |
| `tools/assets/gcx.py`, `gclview.py`, `gclfiles.py`, `gclnames.py`, `gcloptions.py`, `gclroles.py` | scripts: Konami-style and readable views, source files, hash names, option schemas, generated scene procedure names |
| `tools/assets/script_targets.py` | native code reachable from scripts, ranked unmatched targets |
| `tools/assets/stages.py`, `hp.py`, `sprites.py`, `text.py` | stage scripts, terrain, sprites, text bank |
| `tools/script_trace.py`, `tools/function_context.py`, `tools/coverage.py` | runtime evidence: script calls, screens a function runs on |

## Known-hard classes

Solved patterns are in `docs/WORKER.md`. Still open:

1. A constant derived from another constant's register instead of separate
   literals (081A6108, 081A6EF8, 0813BA24, 0814DF0C).
2. `movs r1,#3; ands r1,r0` with the result in the constant's register (081F612C family).
3. Stack `s16[3]` vectors addressed through a callee-saved register (081C43B0 family).
4. Loops agbcc hoists or strength-reduces where the original recomputes (0811E8B0, 080150D0).
5. Pure register-allocation swaps: the permuter, or register pinning.
6. `CgbSound`, the last m4a function: registers left (`notes/cgb.md`).
7. Consumed-return ABI boundaries: drafts with defined results only
   ([ASM_OUTPUT_QC.md](ASM_OUTPUT_QC.md)).

## Open work

* Decompile the script-facing routines ([tasks/script_targets.md](tasks/script_targets.md));
  each match also names its options automatically.
* Script naming: procedures that wrap still-unnamed natives; instance-name
  hashes (`chara TYPE NAME`); resource names, where numbered families need one
  confirmed member to anchor them ([gcl/README.md](../gcl/README.md)).
* Translation units: merge `src/fn/*.c` into contiguous units with shared
  headers and structs ([UNIT_WORKER.md](UNIT_WORKER.md)).
* Coverage: record playthroughs with Centaur; little code is exercised by the scripted tests.
* Debug scenarios: `tools/mgba/scenario.lua` (docs/CENTAUR.md) warps, calls
  any procedure (spawners included), sets variables, moves the player and
  screenshots. Next: scenarios that reach the large-sprite palette variants the
  scene survey misses (attacks, bosses, menus; hook AnimData_SelectPaletteBank
  as `tools/mgba/palette_survey.lua` does).
* Stitched maps: the Studio's Dungeons tab places an area's room maps by their
  doors. Next: backdrop-aware drawing for outdoor areas, floors stacked
  separately, and the whole world from the area links.
* Translation (separate repo [boktai3trans](https://github.com/moozilla/boktai3trans)):
  inserter with box-width checks, VWF via `Text_DrawGlyph` (`08218D1C`).

## Recording your work

* Per-batch details (accepted sources, skips with reasons, near-matches) go in
  `notes/TOPIC_YYYY-MM-DD.md` with a `Tags:` line; subsystem knowledge in its
  `docs/` file (also with `Tags:`).
* A short dated summary goes at the top of the day's `docs/history/YYYY-MM-DD.md`
  with a `Tags:` line under its heading; then update "Current state" and
  "Recent work" here, replacing what they say rather than appending.
* Mention function addresses as 8-digit hex (`0821B858`) and ranges as
  `A–B`, so `docindex.py` and grep find them.
* Run `python3 tools/docindex.py` after adding or renaming docs.
