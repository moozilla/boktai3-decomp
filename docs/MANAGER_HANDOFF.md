# Manager handoff (2026-10-19)

Tags: #process #tooling

For the next orchestrating session. It's what the previous main session was managing, how
the pieces fit, and what to do next. Machine-specific state (logs, PIDs) lives in
`~/git/b3-logs/`. That folder is outside the repo, and it is the only place the private ROM
URL is recorded (`rom_url.txt`).

## Where things stand

* **Progress 41.23%** (8,860 / 11,532 functions) on main at #290. The run from #232 to #262
  added about 1,500 named foundry matches. Cloud workers added #275–#290.
* **What's left is mostly large functions.** About 1,230 have 150+ instructions, roughly
  85% of the remaining code. About 386 have 50–150 instructions, and about 98 fewer than
  50 (mostly hard leftovers).
* **Measured yields:**

  | Who | Functions | Matched | Cost |
  |---|---|---|---|
  | Codex `gpt-6.1-sol` | under 50 | 82% | ~20k tokens per match |
  | Codex `gpt-6.1-sol` | 50–150 | 69% | ~20k tokens per match |
  | Sonnet headless | under 50 | ~81% | ~$0.8 per match |
  | **Opus** | **150–400** | **4 of 6** | **~$4.7 per match** |

  Opus is the best lead on the big functions.
* The 1,412 Codex matches covered 8.72% of the code for 28.8M tokens, about 74 points of
  the weekly Codex quota (docs/FOUNDRY.md).

## What was running, and its state

| Job | Where | State |
|---|---|---|
| Foundry, Sonnet 50–150 (`tools/foundry_run.py --backend claude --model sonnet --workers 3`) | research worktree `.claude/worktrees/permuter-research-30b195` | running detached; finds matches, does **not** integrate them |
| Foundry finisher (repair.py, autosolve) | same | may still be running |
| loadguard (`tools/loadguard.py`, memory/load guard plus pacing) | detached, log `~/git/b3-logs/loadguard.log` | running |
| Subsystem match runners (`tools/unitize.py run --order unmatched`, Opus x2) | `~/git/b3-organize`, worktrees `~/git/wt/unit-*` | **ended**: it ran into the usage limit and failed 472 subsystems instantly (now cleaned up) |
| Unit integrator loop | `~/git/b3-integrate` | ended 01:03 |
| Depin farm, deep search | depin worktree `.claude/worktrees/funny-albattani-8f6d29` | 82 functions unpinned (pinned 8.63% → 8.00%); deep search writes `wip/deep/` |
| Cloud workers 1–3 | claude.ai/code, environment "Ben Test" | claim tasks from the git-branch queue; PRs #275–#290 merged |

## Next steps, in order

1. **Integrate the foundry's finished matches.** Nothing since #262 has been integrated.
   Work in the research worktree:
   * `tools/foundry.py integrate --limit 200`
   * `build/foundry/scale/namefix.py`, then `tools/promote_names.py`
   * merge main, run the combined build (`BUILD_JOBS=4 make`, ~3 min), PR and merge.
2. **Verify the 19 verified and 14 real-work units.** Units whose worker did real work have
   long logs in `~/git/b3-organize/build/unitize/`. Run `tools/unitize.py run --recheck --only
   SLUG,...` for those not yet verified. Then integrate from `~/git/b3-integrate` with
   `BUILD_JOBS=3 tools/unit_integrate.py --batch 10 --state ~/git/b3-organize/build/unitize`.
3. **Fix unitize before the next big run.** Treat "You've hit your session limit" as
   transient (add it to `TRANSIENT`), and stop the runner when it sees one rather than moving
   on. That bug left 472 empty worktrees and 60 GB on disk.
4. **Put Opus on the 150–400 queue** (2 workers), from the research worktree:
   `tools/foundry_run.py --backend claude --model opus --workers 2 --batch 5 --bucket 150-400 --stop STOP-claude`.
   Do the same for subsystem matching: `tools/unitize.py run --order unmatched --backend claude
   --model opus --jobs 2`. Few strong workers do better than many weak ones on large functions.
5. **Cloud workers:** keep them on `tools/cloud_task.py claim --unassigned` (functions outside
   subsystem ranges), and merge their PRs after a combined build. When the cloud work is
   done, delete the ROM from the user's server: a random-named folder under
   `/var/www/example.com/html/`, whose name is in `~/git/b3-logs/rom_url.txt`.

## Budgets and the user's rules

* **Claude plan (Pro):**
  * Check it with the session tool `get_usage`. The 5-hour window and the weekly window are separate.
  * The user wants usage spread across the window, with room left for their own work. Don't pause work in progress.
  * Pace with `~/git/b3-logs/pace.txt`: the maximum number of running `claude -p` workers. Loadguard holds only workers that have just started, releasing matching workers before consolidation ones.
  * Fable has its own weekly bucket, but probably also counts against the shared limit. Treat it as expensive.
* **Codex:**
  * Check with `tools/codex_quota.py --floor 15` before any Codex work. Keep at least 15% of the weekly quota.
  * It was at 14% on 10-18, resetting Sat 20:30, so it's off until then.
  * Luna is cheap but names poorly. Sol is the workhorse.
* **Decomp progress comes first.** Organization (units, names, ranges) and tooling come second.
* PRs are merged as soon as the build is OK. No `rm -rf`: move to the Trash, or use `tools/clean.py`, which the user approved, and remove merged or empty worktrees. Never commit the ROM or anything generated from it.

## Machine limits and how work runs

* **16 GB of RAM and 10 cores.** About ten build-heavy jobs at once caused a watchdog kernel panic on 10-18. Keep loadguard running. Total headless workers should stay around 6–8.
* **Worktrees:** `tools/worktree.sh NAME`, or `--here` for an app-made worktree, clones the built
  base `~/git/b3-base` in about 5 s with no extra disk. `tools/base.sh` refreshes the base; the
  integrator runs it after merges.
  * Start worktrees at the base commit. Regenerating `gen/code_sym.s` takes 15 min on one core.
* **Builds:**
  * Workers only run `tools/check.py`, which takes seconds.
  * The full build runs once per batch at integration: `tools/build.py`, compiled in parallel (`BUILD_JOBS`, 2:41 from scratch).
* **Disk:** watch it. `tools/clean.py --runs --worktrees` is the scoped cleanup.
* **Context:** keep the orchestrator's context small. Hand detail to worker sessions, and compact when convenient.

## Tools map (start here)

* **Matching:**
  * `tools/foundry.py` and `tools/foundry_run.py` (docs/FOUNDRY.md)
  * `tools/unitize.py` and `tools/unit_integrate.py`
  * `tools/regions.py`, `tools/repair.py`, `tools/explain.py`, `tools/relatives.py`
  * `tools/rasolve.py` and `tools/translate.py` (docs/PERMUTER.md)
  * `tools/fixsearch.py` (bounded)
* **Organization:** `tools/namer.py` (Codex naming), `tools/families.py` (ranges from name prefixes), `tools/promote_names.py`.
* **Infrastructure:**
  * `tools/worktree.sh`, `tools/base.sh`, `tools/clean.py`
  * `tools/loadguard.py`, `tools/codex_quota.py`
  * `tools/cloud_setup.sh`, `tools/cloud_task.py` (docs/CLOUD.md)
* **Showcase:** v3's final cut is in `~/Movies/boktai3-showcase/boktai3-showcase-v3.mp4` (9:50, #274), with earlier drafts beside it.

## Known loose ends

* `~/git/example.com/share-file.sh` fails because `share/` on the server is owned by root. The ROM went to a random folder under `html/` instead.
* The user couldn't find where to set `BOKTAI3_ROM_URL` in the cloud environment UI. New cloud workers can be given the download step in their starting prompt instead. Never commit the URL.
* Integrators: units cut from an older main need `unit_integrate.py`'s refresh (renamed callees, new matches moved in). Shared-header collisions between units leave one unit to re-run fresh.
