# Manager handoff (2026-10-19)

Tags: #process #tooling

For the next orchestrating session. It's what the previous main session was managing, how
the pieces fit, and what to do next. Machine-specific state (logs, PIDs) lives in
`~/git/b3-logs/`. That folder is outside the repo, and it is the only place the private ROM
URL is recorded (`rom_url.txt`).

## Where things stand (updated 2026-10-19, local work wound down)

* **Progress 43.26%** (8,950 / 11,532 functions) on main at #300. There are 434
  `#ifdef NONMATCHING` drafts (mean fuzzy 86.4%), each a head start for a later worker.
* **From now on all work runs in Claude Code cloud sessions** (environment "Ben Test").
  The user's machine runs no workers. Everything local is merged or pushed:
  * foundry: #296, #300;
  * subsystem units: #294 and earlier;
  * pin farm: #267, plus the deep-search follow-up by the depin session;
  * the unfinished work is on branches:
    * `foundry-wip-2026-10-19`: 272 attempts below 80%, each with its best C and notes;
    * `wip/unit-*`: interrupted subsystem units;
    * `wip/<name>`: early worker worktrees.
* **What's left** (docs/FOUNDRY.md, handover section; every attempt is in `docs/foundry_attempts.tsv`):

  | Functions | Left | Notes |
  |---|---:|---|
  | 150–400 instructions | 922 | never attempted |
  | 400+ instructions | ~300 | |
  | 50–150 instructions | 61 | |
  | under 50 instructions | 39 | |

  Large functions are about 85% of the remaining code.
* **Measured yields:**

  | Who | Functions | Matched | Cost |
  |---|---|---|---|
  | Codex `gpt-6.1-sol` | under 50 | 82% | ~20k tokens per match |
  | Codex `gpt-6.1-sol` | 50–150 | 69% | ~20k tokens per match |
  | Sonnet | under 50 | ~81% | ~$0.8 per match |
  | **Opus** | **150–400** | **4 of 6** | **~$4.7 per match** |

  Strong models on large functions, with the subsystem's context, is the plan.

## How the cloud work runs

* **Setup per session:**
  * The environment's setup script builds a cached base once (~3 min); each session's hook sets up from it in seconds (docs/CLOUD.md).
  * The ROM comes from `BOKTAI3_ROM_URL`. If the environment UI has no place for it, put the download step in the session's starting prompt. Never commit the URL.
  * The URL is on the user's machine in `~/git/b3-logs/rom_url.txt`. The file is a random-named folder under `/var/www/example.com/html/`; delete it when cloud work ends.
* **Queue:**
  * `tools/cloud_task.py claim --unassigned` takes the large functions outside subsystem ranges.
  * `claim` without the flag also includes subsystem ranges. Use it now that no local workers hold those ranges.
  * Claims are branches named `cloud/TASK`. Workers open one PR per task.
* **Merging:** the manager merges cloud PRs once the combined build (`BUILD_JOBS=$(nproc) make`) prints `build/boktai3.gba: OK`.
* **Worth trying next in the cloud:**
  * Opus on the 150–400 queue.
  * The match-first subsystem brief (`tools/unitize.py`'s BRIEF): it hands one strong worker a subsystem's unmatched functions plus its matched siblings and types.
  * Workers can start from the NONMATCHING drafts and the `foundry-wip-2026-10-19` attempts.

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

## If work ever runs locally again

* **The machine has 16 GB of RAM and 10 cores.** About ten build-heavy jobs at once caused a watchdog panic. Run
  `tools/loadguard.py` (memory, load and pacing through `~/git/b3-logs/pace.txt`), and keep to
  about 6–8 headless workers.
* **Worktrees:** use `tools/worktree.sh`, which clones the built base `~/git/b3-base`; refresh that with
  `tools/base.sh`. Workers use `tools/check.py`; the full build (2:41, parallel) runs only at integration.
* **Unit runner:** `tools/unitize.py` must treat "You've hit your session limit" as transient and stop
  on it. It doesn't yet: one run left 472 empty worktrees and filled 60 GB.

## Tools map (start here)

* **Matching:**
  * `tools/foundry.py` and `tools/foundry_run.py` (docs/FOUNDRY.md)
  * `tools/unitize.py` and `tools/unit_integrate.py`
  * `tools/regions.py`, `tools/repair.py`, `tools/explain.py`, `tools/relatives.py`
  * `tools/rasolve.py` and `tools/translate.py` (docs/PERMUTER.md)
  * `tools/fixsearch.py` (bounded)
* **Organization:** `tools/namer.py` (Codex naming), `tools/subsystems.py` (report units and categories from name prefixes), `tools/promote_names.py`.
* **Infrastructure:**
  * `tools/worktree.sh`, `tools/base.sh`, `tools/clean.py`
  * `tools/loadguard.py`, `tools/codex_quota.py`
  * `tools/cloud_setup.sh`, `tools/cloud_task.py` (docs/CLOUD.md)
* **Showcase:** v3's final cut is in `~/Movies/boktai3-showcase/boktai3-showcase-v3.mp4` (9:50, #274), with earlier drafts beside it.

## Known loose ends

* `~/git/example.com/share-file.sh` fails because `share/` on the server is owned by root. The ROM went to a random folder under `html/` instead.
* The user couldn't find where to set `BOKTAI3_ROM_URL` in the cloud environment UI. New cloud workers can be given the download step in their starting prompt instead. Never commit the URL.
* Integrators: units cut from an older main need `unit_integrate.py`'s refresh (renamed callees, new matches moved in). Shared-header collisions between units leave one unit to re-run fresh.
