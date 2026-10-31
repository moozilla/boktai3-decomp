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

[ROADMAP.md](ROADMAP.md) has the plan: (1) draft, done; (2) match, nearly done;
(3) no concessions, current; (4) cleanup, where it comes naturally; plus the side
threads (RAM and data annotation, Studio, translation, tooling).

**Always aim for the final form.** When you touch a function, write C that matches
with no register pins, barriers or compiler overrides and reads like the original
authors wrote it. Pins were the phase 2 stopgap; new work never adds one.

**Work systematically, and stay until it matches.** Go through a subsystem in ROM order.
Don't sort for likely wins, and don't skip or defer the hard functions. Proving the
compiler decision that blocks a function is the halfway point; then find the C that
produces it. When an approach stops paying, change the approach or the tool, not the
function. A function becomes a holdout (best draft, note and matchsearch store, for a
stronger model) only after the extreme effort [WORKER.md](WORKER.md) "Stay on a function
until it matches" describes. Measure progress by exact matches and removed concessions,
not by fuzzy scores. Blind spelling sweeps don't count.

## Where things stand (2026-10-31)

Numbers live only in [PROGRESS.md](../PROGRESS.md) (regenerated at batch integration) and
the README badges. Use `python3 tools/progress.py` for the current figures and
`progress.py --list-pinned` for the phase-3 list.

| | |
|---|---|
| Build | `make` rebuilds the ROM byte-identical (`build/boktai3.gba: OK`, SHA-1 `2651c5e6…`) |
| Phase 1 | complete: every function has C |
| Phase 2 | the last guarded drafts (PROGRESS.md "Nonmatching drafts"): GPT-6 Pro's round 9 queue (scene actors 1, 2 and 4, misc) and 17 round 8 holdouts, which an Opus worker is retrying |
| Phase 3 | the Register-pinned share in PROGRESS.md; `scope.py FILE.c pins` and `tools/depin.py` remove them |
| Phase 4 | names, shared types, a real source tree and a self-placing build ([ROADMAP.md](ROADMAP.md)); so far the [EWRAM shared declarations](../notes/ewram-layout_2026-10-27.md) and naming as functions match |
| Data | fully relocatable; `make shifttest` compares 35 emulator screenshots after moving data. decomp.dev's data measure is ROM data that round-trips through an editable form (`tools/data_roundtrip.py`), by kind ([DATA_TRACKING.md](DATA_TRACKING.md)); RAM is tracked in PROGRESS.md |
| Report | every function in one of ~490 subsystem units under 11 categories plus Libraries ([REPORT_SUBSYSTEMS.md](REPORT_SUBSYSTEMS.md)); main-only CI publishes to decomp.dev |

Recent (details in [docs/history/](history/README.md) and the notes they link):

* **2026-10-31**: the finish-subsystem Codex sessions (scene actors 3, bosses, player,
  effects) and GPT-6 Pro round 8 are integrated. Round 8 left 18 holdouts with notes and
  matchsearch stores; an Opus worker matched `Ratatosk_UpdateDefeatSequence` (`081F4CF8`) from them and is continuing. scope's `why`
  now explains stack-slot reloads ([batch history](history/2026-10-31.md)).
* **2026-10-30**: rounds 6 and 7 (monster initializers and recovery) and the player cue
  batch; many command and animation tables are editable and browsable in Studio.
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

* The phases and "aim for the final form" above. No new pins, barriers or overrides.
* ROM order; no easy-first sorting; stay on a function until it matches (holdouts only
  after extreme effort); no per-function compile budgets; no blind spelling sweeps;
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
