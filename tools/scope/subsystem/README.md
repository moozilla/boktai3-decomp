# Subsystem runs: Codex scope workers, one subsystem each

Tags: #process #tooling

A run gives each game subsystem (report category) a Codex worker in its own worktree. The worker
goes through the subsystem's NONMATCHING drafts in ROM order with `tools/scope` and commits one
function per match on its own branch; the coordinator integrates the branches in reviewed batches.
First used on 2026-10-25 (results: [notes/subsystem-run_2026-10-25.md](../../../notes/subsystem-run_2026-10-25.md)).

## Running one

From the main checkout, on an up-to-date `main`:

```
sh tools/base.sh                                   # the shared built base new worktrees clone (minutes)
python3 tools/scope/subsystem/queues.py build/subsystem/q --exclude src/iwram_code.c   # one queue per category
tools/scope/subsystem/launch.sh sc-bosses build/subsystem/q/bosses.json "Bosses"     # one per subsystem
nohup tools/scope/subsystem/committer.sh > build/subsystem/committer.log 2>&1 &      # makes the commits
tools/scope/subsystem/watch.sh 50 &                # resume transient failures; stop all at 50% quota left
tools/scope/subsystem/status.sh                    # progress and quota
tools/scope/subsystem/integrate.sh 2               # cherry-pick new matches onto claude/sc-batch-2
```

`--exclude` keeps units another effort owns out of the queues. Every launch needs the user's
authorization and a quota floor (`tools/codex_quota.py`); start with one worker per subsystem,
measure for 10 minutes, then decide. 16 workers burned about 25 points of the weekly quota per hour.

After `integrate.sh`: review the diff (no hand-written instructions in `asm()`, the
`MATCHING_CONCESSION` marker wherever there is a pin or barrier, no comments about the worker's
experiments), `make` must print `build/boktai3.gba: OK`, then
`python3 tools/progress.py --markdown PROGRESS.md`, commit both PROGRESS files, open the PR,
merge it, and append `build/subsystem/integrated.pending` to `build/subsystem/integrated.txt`.

## Pieces

| File | What |
|---|---|
| `BRIEF.md` | the worker prompt (placeholders filled by launch.sh) |
| `queues.py` | per-category queues of NONMATCHING drafts, ROM order |
| `launch.sh NAME QUEUE CATEGORY` | worktree `../wt/NAME` (branch `codex/NAME`), scope's agbcc linked in, worker started |
| `committer.sh` | the sandbox cannot write `.git`: commits each `build/scope/campaign/FUNC/commit` request |
| `watch.sh FLOOR [N]` | resumes workers that died on capacity/429/5xx errors (up to 5 times); stops all at FLOOR |
| `resume.sh NAME`, `stopall.sh`, `status.sh` | by hand |
| `integrate.sh N` | batch branch from the workers' `Match FUNC` commits |

Inside a worktree a worker uses `build/scope/campaign/FUNC/` (start.c, base.c, variants, result.json,
REPORT.md, best.c) and lands a match with `python3 tools/scope/campaign.py land FUNC .`, which merges
it into the unit, formats it and runs `tools/check.py` on the unit. Each worker keeps a one-line-per-
function `build/scope/campaign/LOG.md`. Its REPORT.md files record where scope was confusing or missing:
read them after a run, they are the next tool changes.

## Lessons from the first run (2026-10-25)

* **ROM order, no easy-first sort.** The first queues were sorted likely wins first and the second
  worker of a subsystem got the hard half; nearly all matches came from queue tops and the hard
  halves matched about 1 in 12. The user wants the hard functions worked, not deferred.
* **No compile budget per function.** A "~150 compiles, then move on" rule made workers close out
  functions they could have kept working. The brief now says: stay until MATCH, or until the one
  blocking compiler decision is named, proved with a knob, and the C spellings for it are tried.
* **Ban scripted spelling sweeps.** Two workers wrote scripts that tried C spellings blindly and
  closed out about 200 functions with 2 matches. Every variant must test a hypothesis from scope.
* Codex reads its queue once at the start: re-sorting `queue.json` mid-run does not reach it.
* Transient `Selected model is at capacity` and `429 Too Many Requests` errors end a `codex exec`
  session; `codex exec resume ID` continues it (watch.sh does this).
* Codex's MCP servers (e.g. a stale cloudflare-api OAuth token) log errors at startup; harmless.
