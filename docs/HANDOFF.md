# Handoff: current state and how to continue

Tags: #process

The entry point for a new agent or a thread after compaction. Read this file and
[ROADMAP.md](ROADMAP.md), then [docs/WORKER.md](WORKER.md) and [tools/scope/WORKER.md](../tools/scope/WORKER.md) before
matching. Everything else is looked up, not read: [docs/INDEX.md](INDEX.md) maps every doc
by topic, and `python3 tools/docindex.py ADDR` lists what has been written about a function.
Keep this file short and about the present. The previous version (with the October recent-work
list) is [history/HANDOFF_2026-10-23.md](history/HANDOFF_2026-10-23.md); older records are in
[docs/history/](history/README.md).

## The goal: four phases, aiming at the last one

[ROADMAP.md](ROADMAP.md) has the plan: (1) draft, done; (2) match, done;
(3) no concessions: 3a, nothing addressed to the compiler, done; 3b, no steering, current;
(4) cleanup, alongside 3b; plus the side
threads (RAM and data annotation, Studio, translation, tooling).

**Always aim for the final form.** When you touch a function, write C that matches
with no register pins, barriers or compiler overrides and reads like the original
authors wrote it. Pins and barriers are gone (phase 3a) and none comes back. The work now
(phase 3b) is the linter's warnings, `python3 tools/concessions.py`: errors stay at zero,
warnings go down, and a change adds none (`scope.py land` prints the ones it adds).

**Work systematically, and stay until it matches.** Go through a subsystem in ROM order.
Don't sort for likely wins, and don't skip or defer the hard functions. Proving the
compiler decision that blocks a function is the halfway point; then find the C that
produces it. When an approach stops paying, change the approach or the tool, not the
function. A pinned match, or a holdout (best draft, note and matchsearch store, for a
stronger model), comes only after the extreme effort [WORKER.md](WORKER.md) "Stay on a function
until it matches" describes. Measure progress by exact matches and removed concessions,
not by fuzzy scores. Blind spelling sweeps don't count.

## Where things stand (2026-11-25)

Source and headers use the reviewed placement tables in `symbols/`.
[SOURCE_LAYOUT.md](SOURCE_LAYOUT.md) maps the directories and explains where
new files belong and how to find a function.
The [runtime layout audit](../notes/unit-identities_2026-11-25.md) ties the
revised names to checkpoint replays and the Monster Book.

Numbers live only in [PROGRESS.md](../PROGRESS.md) (regenerated at batch integration) and
the README badges. Use `python3 tools/progress.py` for the current figures and
`python3 tools/concessions.py --summary` for the phase-3 list.

| | |
|---|---|
| Build | `make` rebuilds the ROM byte-identical (`build/boktai3.gba: OK`, SHA-1 `2651c5e6…`) |
| Phase 1 | complete: every function has C |
| Phase 2 | complete: every function matches; no `#ifdef NONMATCHING` drafts are left |
| Phase 4 | the object model: [OBJECT_MODEL.md](OBJECT_MODEL.md) describes tasks, the registry actor and pool slots. Monsters derive from `struct RoomActor` (`include/link/session/room_actor.h`) or `struct ChaseActor` (`include/monsters/chase_actor.h`), with named and typed method slots and `enum MonsterCategory`; the per-header views and the `SCENE_ACTOR_CHASE_*` selector macros are gone. The eleven registry classes (Spider, Octopus, Ivy, Lun, Bat, Axe, Dog, Mummy, Vorn, Golem, Skeleton) are class units in `src/`; the functions that match only alone are explained in [notes/unit-context-sensitivity_2026-11-16.md](../notes/unit-context-sensitivity_2026-11-16.md) |
| Phase 3 | 3b in progress: 3 linter errors (volatile: the two scene link-list insertions and the scene camera global) and a use-weighted steering-debt subtotal in the linter summary. The [flag-order survey](../notes/flag-accessors_2026-11-25.md) gives the evidence for accepting a shared flag operation. The 2026-11-22 run recovered boss, Player, monster and menu types by subsystem and cleared two hard sites; its [report](../notes/claude-warning-run_2026-11-22.md) says what worked (type recovery first, plain loops), what the count hides (constants moved into accessor arguments) and the leads not taken. Hard sites: [hard-sites_2026-11-23.md](../notes/hard-sites_2026-11-23.md). Earlier: the [round-15 report](../notes/round15-priority-recovery.md) and [union review](../notes/round15-union-review.md). A falling count is not evidence of plausible original source. |
| Data | fully relocatable; `make shifttest` compares 35 emulator screenshots after moving data. decomp.dev's data measure is ROM data that round-trips through an editable form (`tools/data_roundtrip.py`), by kind ([DATA_TRACKING.md](DATA_TRACKING.md)); RAM is tracked in PROGRESS.md |
| Report | every function in one of ~490 subsystem units under 11 categories plus Libraries ([REPORT_SUBSYSTEMS.md](REPORT_SUBSYSTEMS.md)); main-only CI publishes to decomp.dev |

Recent (details in [docs/history/](history/README.md) and the notes they link):

* **2026-11-22 to 23, Claude warning run** (PRs 1056 to 1060): subsystem workers recover
  types (`struct Ratatosk`, Hresvelgr and Vanargand bodies, Player's embedded animation
  channels, Dog, Axe, link panel), the mechanical tools `tools/inline_twins.py`,
  `tools/rawfield.py` and `tools/fold_literals.py` land, two hard sites fall, and running the
  port against the ROM finds 97 short calls and 28 missing returns (the `UBFIX` rule in
  `CLAUDE.md`). New: `tools/recorder/` (a flight recorder with source-level `why`) and
  `tools/autopilot/` (the game plays itself on the port, verified against the ROM). See the
  [run report](../notes/claude-warning-run_2026-11-22.md).
* **2026-11-17, round 15 extended continuation**: ordinary priority recoveries,
  shared sequence/sprite APIs, native actor and HUD ownership, and three diagnostic
  tool fixes preserve the exact ROM. The remaining priority/error proposals have
  replayable source, hashes and explicit retained debt. See the
  [maintained report](../notes/round15-priority-recovery.md) and
  [union review](../notes/round15-union-review.md); allocation-only forms remain
  concessions even where the earlier round-13 scan considered its queue complete.
* **2026-11-16, round 14 batch 4**: indexed pool dispatch, shared downed/action
  operations and player-array ownership preserve the exact ROM. Independent
  review corrects the four Feather table interfaces; its scheduler boundary and
  older private views remain explicit. Three proved-zero false positives are
  explained without an exemption or count reduction. See the
  [checkpoint report](../notes/round14-batch4_2026-11-16.md).
* **2026-11-16, round 14 batch 3**: shared animation layouts, actual command
  arrays, IRQ field assignments and ordinary source operations remove further
  warning sites. The one new annotation is backed by the original libgcc source.
  Combined stock ROM and every changed-header dependent match; see the
  [checkpoint report](../notes/round14-batch3_2026-11-16.md).
* **2026-11-16, round 14 batch 2**: direct values, real field types and ordinary
  control flow replace more allocation scaffolding. Compiler diagnostic and
  declaration-pattern false positives are corrected with regression tests and
  unchanged code generation. The exact ROM is preserved; unresolved source
  decisions remain in the [checkpoint report](../notes/round14-batch2_2026-11-16.md).
* **2026-11-16, round 14 batch 1**: ordinary fields, shared manager layouts,
  particle allocation, and directional input preserve the exact ROM while
  removing file-level errors and warning sites. Remaining volatile declarations
  and all compiler-warning work stay explicit in the [checkpoint notes](../notes/round14-batch1_2026-11-16.md).
* **2026-11-15 to 16, the phase-3 pilot run** (PRs 1021 to 1039): pilot workers removed the
  concessions from most remaining functions (concession-free 95.042% to 98.870%; functions
  with a concession 219 to 36), and the object-model cleanup landed in the same batches
  ([history](history/2026-11-16.md)).
* **Cleanup tools**: `tools/retype_fields.py` rewrites member accesses after a struct's
  members move or get names, replayed from steps files in `tools/retype/` (`room_actor.txt`,
  `room_actor_names.txt`, `chase_group_names.txt`, in that order, on a branch that predates
  them). `tools/unselect.py` replaces selector macros with a file's own copies of the
  inlines. `tools/fold_views.py`, `tools/struct_views.py`, `tools/merge_resolve.py` and
  `tools/header_dependents.py --since REV --check` support class folds and unit merges.
* **ARM code**: `sub_08000430` (IWRAM, `src/engine/iwram_code.c`) matched without pinning
  through an instrumented ARM compiler (`agbcc_arm`; [notes/arm-08000430.md](../notes/arm-08000430.md)).
  scope compiles Thumb only.

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
| [tools/testbench](../tools/testbench/README.md) | stage a scene, create actors, drive menus and scripts, and record function coverage and ROM replay results |

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

* The phases and "aim for the final form" above. Pins heavily discouraged, never overrides.
* ROM order; no easy-first sorting; stay on a function until it matches (pinned matches
  and holdouts only after extreme effort); no per-function compile budgets; no blind spelling sweeps;
  never stop a run early.
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
