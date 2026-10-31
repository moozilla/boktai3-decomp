# Handoff: current state and how to continue

Tags: #process

The entry point for a new agent or a thread after compaction. Read this file, then
[docs/WORKER.md](WORKER.md) and [tools/scope/WORKER.md](../tools/scope/WORKER.md) before
matching. Everything else is looked up, not read: [docs/INDEX.md](INDEX.md) maps every doc
by topic, and `python3 tools/docindex.py ADDR` lists what has been written about a function.
Keep this file short and about the present. The previous version (with the October recent-work
list) is [history/HANDOFF_2026-10-23.md](history/HANDOFF_2026-10-23.md); older records are in
[docs/history/](history/README.md).

## The goal: four phases, aiming at the last one

The project owner splits the decomp into four phases:

1. **Draft.** Every function in the game has C (an exact match or a guarded
   `#ifdef NONMATCHING` draft with a fuzzy score).
2. **Match.** Every function matches byte for byte. Concessions (register pins,
   empty `asm("")` barriers) are allowed here as an interim.
3. **No concessions.** Every match is plain C: no pins, no barriers, no
   `*(u16 *)((u8 *)p + 0x172)` field access.
4. **Cleanup.** Human-readable and organized: names, shared structs and headers,
   enums, comments, a real source tree (contiguous units).

Phase 1 is done: every function has C. The work now is phases 2 and 3, subsystem
by subsystem, and phase 4 where it comes naturally.

**Always aim for the final form.** When you touch a function, try for C that matches
with no concessions and reads like the original authors wrote it. A pin is a last
resort, used only where a knob proved an allocation decision you could not reproduce
in C after real attempts. Easy matches with pins are interim work that phase 3 has to
redo.

**Work systematically, and don't spin wheels.** Go through a subsystem in ROM order.
Don't sort for likely wins, and don't skip or defer the hard functions. Stay on a function
until it matches, or until you can name the one compiler decision blocking it (proved with
a knob, with the C spellings for it tried). Then record that as a scope gap. Measure
progress by exact matches and removed concessions, not by fuzzy scores. Blind spelling
sweeps don't count. When an approach stops paying, change the approach or the tool,
not just the function.

## Where things stand (2026-10-30)

Numbers live only in [PROGRESS.md](../PROGRESS.md) (regenerated at batch integration) and
the README badges. Use `python3 tools/progress.py` for the current figures and
`progress.py --list-pinned` for the phase-3 list.

| | |
|---|---|
| Build | `make` rebuilds the ROM byte-identical (`build/boktai3.gba: OK`, SHA-1 `2651c5e6…`) |
| Phase 1 | complete: every function has C |
| Phase 2 | the remaining guarded drafts (PROGRESS.md "Nonmatching drafts"), by subsystem in PROGRESS.md; most left are Monsters and Scene actors |
| Phase 3 | the Register-pinned share in PROGRESS.md; `scope.py FILE.c pins` and `tools/depin.py` remove them |
| Phase 4 | [EWRAM declaration/layout worker batch](../notes/ewram-layout_2026-10-27.md) prepared with per-unit validation; protected-dependent objects deferred and combined ROM validation pending integration |
| Data | fully relocatable; `make shifttest` compares 35 emulator screenshots after moving data. decomp.dev's data measure is ROM data that round-trips through an editable form (`tools/data_roundtrip.py`), by kind ([DATA_TRACKING.md](DATA_TRACKING.md)); RAM is tracked in PROGRESS.md |
| Report | every function in one of ~490 subsystem units under 11 categories plus Libraries ([REPORT_SUBSYSTEMS.md](REPORT_SUBSYSTEMS.md)); main-only CI publishes to decomp.dev |

Recent:

* **Boss worker continuation** (2026-10-31): ten of the fifteen reserved
  Ratatosk, Vanargand and Hresvelgr functions match in ordinary C. Five guarded
  drafts remain; the latest recovery draft also corrects the successful-state
  byte. Work stopped at the owner's request to preserve the 20% quota reserve.
  The requested broader pin-removal and struct/data/Studio cleanup remains open.
  Per-unit checks and separate source commits are on `codex/finish-bosses`;
  combined validation, alias promotion and progress reports remain with the
  integrator. See the [boss worker note](../notes/finish-bosses_2026-10-30.md).
* **Round 7 recovery and continued matching** (2026-10-30): the interrupted
  run's four integrated matches and an unmerged Skeleton worker match were
  recovered. Ten further functions are exact; cumulative bundles and patches
  contain fifteen matches with complete stock checks. Turn and delayed-response operations are shared,
  three initializers embed the canonical actor prefix, and all eleven installed
  command tables are editable in Studio. The formatter interruption test now
  handles canonicalized temporary paths. See the
  [Round 7 note](../notes/round7-monster-recovery_2026-10-30.md) for accepted
  extents and preserved blockers. PROGRESS files remain unchanged.
* **Round 6 initializer family completed** (2026-10-30): all 17 reserved
  initializers match in ordinary C, with one initial matching commit per function.
  They share canonical actor/body/attack/target/group/route records and common
  initialization, graphics and callback operations. Engine owner checks confirm
  the recovered body and resource records; four command tables now roundtrip
  through editable CSV and are browsable in Studio. Final reviewed unit checks,
  the combined ROM and RAM-literal checks pass. Two older chase selectors were
  removed; ten remain, along with several compiler-sensitive body variants.
  The [family note](../notes/monster-initializers_2026-10-30.md) records the exact
  extents, shared migrations and remaining work. The supplied monster queue has
  107 functions left. Aggregate reports are left for the integrator to regenerate.
* **Round 4 shared monster batch completed** (2026-10-29): all five reserved
  targets match in ordinary C: Mummy movement (`080DA97C`), Ghoul contact
  (`0808B3EC`), `SceneActor_UpdateStatus` (`08078EE8`), `Vanargand_Init`
  (`081AE0C0`), and Mummy displacement recovery (`080D2584`). Golem, Skeleton,
  Ghoul, and both Mummy reactions now share the chase records and operations;
  the status updater reuses the same component/control/terrain layer. Stock and
  complete-unit gates, old-consumer preservation, and the combined ROM/RAM-literal
  build pass. Remaining matching interfaces and the symlink-root landing fix
  are documented in [the batch note](../notes/shared-monster-round4_2026-10-29.md)
  and [shared-operation guide](SHARED_MONSTER_REACTIONS.md). This batch starts
  from the supplied PR 887 bundle; refresh aggregate reports after combining
  additional worker commits.
* **Earlier largest matches**: `Skeleton_ReactToContact` (`080C12FC`) and
  `Golem_UpdateHitReaction` (`080C9C04`) remain exact after shared-header cleanup.
  The [PRE ordering guide](../tools/scope/PRE_STACK_ORDER.md) explains the compiler
  rule and rejects applying it before physical instructions and stack identities
  are verified. Vorn `080AEB74` remains exact.
* **Second subsystem run with RAM layout workers** (2026-10-27): 29 more matches (to 79.7%) and RAM
  objects with one shared declaration 29% -> 75%; gpt-6.1-terra tried and dropped for Sol
  ([notes/subsystem-run_2026-10-27.md](../notes/subsystem-run_2026-10-27.md)).
* **Subsystem scope run** (2026-10-25/26): 16 Codex gpt-6.1-sol workers, one or two per
  subsystem, matched 65 drafts in 4.5 hours for half the weekly Codex quota (77.0% -> 78.6%;
  [#849](https://github.com/moozilla/boktai3-decomp/pull/849),
  [#854](https://github.com/moozilla/boktai3-decomp/pull/854)).
  Lessons (ROM order, no per-function budget, no blind sweeps) and how to run one are in
  [tools/scope/subsystem/README.md](../tools/scope/subsystem/README.md); the results are in
  [notes/subsystem-run_2026-10-25.md](../notes/subsystem-run_2026-10-25.md).
* **ARM code**: `sub_08000430` (IWRAM, `src/iwram_code.c`) matched without pinning
  through an instrumented ARM compiler (`agbcc_arm`; [notes/arm-08000430.md](../notes/arm-08000430.md)).
  scope compiles Thumb only.
* **Yu-Gi-Oh! EDS wiki vendored**: [external/eds/](external/eds/README.md), a catalog of
  GCC 2.95 matching tricks from another agent-driven decomp.

## The tools that matter now

| Tool | What |
|---|---|
| `tools/scope/scope.py` | the matching workbench: diff, `why`, knobs, `try`, `probe`, `pins`, `new`/`land`/`rename`. Spec and findings: [tools/scope/README.md](../tools/scope/README.md); loop: [tools/scope/WORKER.md](../tools/scope/WORKER.md) |
| `tools/matchsearch/` | persistent model-directed hypotheses, guarded moves, value constraints, diverse frontier, and compiler evidence ([README](../tools/matchsearch/README.md)) |
| `tools/scope/subsystem/` | a Codex worker per subsystem, each in its own worktree; committer, watcher, batch integration ([README](../tools/scope/subsystem/README.md)) |
| `tools/scope/campaign.py` | the per-function variant: one Codex worker per draft in the main checkout, landed by `campaign.py integrate` |
| `tools/check.py FILE.c` | exact check of one unit (`--nonmatching --record` scores a draft) |
| `tools/worktree.sh NAME`, `tools/base.sh` | a worker worktree in seconds, cloned from the built base `../b3-base` |
| `tools/progress.py` | PROGRESS.md / PROGRESS.json; `--list-pinned` |
| `tools/codex_quota.py [--floor N]` | Codex weekly quota left, from Codex's own session logs |
| `tools/relatives.py`, `rasolve.py`, `fix.py`, `hint.py`, `permute.py` | when stuck ([WORKER.md](WORKER.md) "When stuck") |
| `tools/docindex.py [ADDR / --tag T]` | who wrote about an address or topic |
| `tools/emu/harness`, `tools/studio/serve.py` | headless mGBA runs; the Studio (text, sprites, maps, actors, scripts) |

## Running workers

Multi-worker campaigns need the user's explicit go-ahead and a Codex quota floor the user
sets (`tools/codex_quota.py`). Codex (gpt-6.1-sol, reasoning high) does the bulk work;
a Claude session coordinates, reviews and integrates. Start small (one worker per subsystem),
measure usage for about 10 minutes, then scale. 16 workers used about 25 points of the
weekly quota per hour. Codex sessions die on transient capacity and 429 errors:
`tools/scope/subsystem/watch.sh` resumes them. The Codex sandbox cannot write `.git`, so
commits are made outside it (`committer.sh`).

Integration is batched: one commit per matched function on the worker branches. The
coordinator cherry-picks them onto one branch, reviews the diff (no hand-written
instructions, concession markers, no experiment notes in comments), runs one full `make`
(`build/boktai3.gba: OK`), regenerates PROGRESS (`python3 tools/progress.py --markdown PROGRESS.md`,
commit both files), then opens the PR and merges it.

## Standing preferences (the owner's)

* The phases and "aim for the final form" above. Concessions are a last resort.
* ROM order; no easy-first sorting; stay on a function until it matches or the blocking
  decision is proved; no per-function compile budgets; no blind spelling sweeps.
* **Tooling first.** Every scope gap a worker hits is the next tool change. Workers
  report where scope was confusing, wrong or missing; fold that into
  tools/scope/README.md "Gaps" and fix the tool.
* **Over-explain.** The owner follows along to learn how decompilation works: say what you
  are doing and why, in plain terms.
* Batched integration; merge a batch PR right after opening it once the ROM build is OK.
  Tooling- and docs-only PRs skip the ROM build.
* Don't block on corpus-wide compile or equivalence sweeps; fix mismatches when they happen.
* Report progress as a percentage and percentage-point gain, bytes second; totals only in
  PROGRESS.md and the badges.
* Provisional names are welcome when marked tentative; don't block matching on naming.
* Never `rm -rf` (move to `~/.Trash`). US spelling. No personal information in commits
  (`tools/pii_check.py` runs in the hooks and CI).

## Setting up

The ROM is never committed (nor anything derived from it: `build/`, `gen/`, `extracted/`).
On the owner's Mac everything is set up: `baserom.gba`, `build/venv`, `build/tools`, `gen/`,
the built base `../b3-base`, scope's compiler in `build/scope/agbcc/`. On a fresh machine:

```
ln -s /path/to/rom.gba baserom.gba        # SHA-1 above (Japanese, U33J)
pip install -r requirements.txt            # plus arm-none-eabi binutils, cmake, libmgba
make setup      # gbadisasm (patched), agbcc, armips, permuter, emulator harness
make disasm     # ~15-20 min: gen/code.s, gen/code_sym.s, gen/data.s
make            # must print build/boktai3.gba: OK
sh tools/scope/build_agbcc.sh              # scope's instrumented agbcc
```

README.md has the macOS details.

## Beyond matching

The decomp serves the owner's translation and modding work. Those threads are documented
on their own: scripts are Konami GCL ([gcl/README.md](../gcl/README.md); readable views and
generated procedure names), cutscenes ([CUTSCENES.md](CUTSCENES.md)), saves and story
progress ([ENGINE.md](ENGINE.md) section 14, `tools/assets/saves.py`, `story.py`), assets
([ASSETS.md](ASSETS.md)), the Studio (`tools/studio/serve.py`), a showcase scene written
into a ROM copy (`tools/showcase/`), and the translation repo
[boktai3trans](https://github.com/moozilla/boktai3trans). Their open items are in
[history/HANDOFF_2026-10-23.md](history/HANDOFF_2026-10-23.md) ("Open work").

## Recording your work

* Per-batch details (what matched, what did not and why, the scope gaps hit) go in
  `notes/TOPIC_YYYY-MM-DD.md` with a `Tags:` line; subsystem knowledge in its `docs/` file
  (also with `Tags:`).
* A short dated summary goes at the top of the day's `docs/history/YYYY-MM-DD.md` with a
  `Tags:` line under its heading. Then update "Where things stand" here, replacing what it
  says rather than appending.
* Mention function addresses as 8-digit hex (`0821B858`) and ranges as `A–B`, so
  `docindex.py` and grep find them.
* Run `python3 tools/docindex.py` after adding or renaming docs.
