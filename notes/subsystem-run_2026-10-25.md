# Subsystem scope run, 2026-10-25/26

Tags: #process #tooling #matching

The first run of [tools/scope/subsystem](../tools/scope/subsystem/README.md): Codex gpt-6.1-sol
workers (reasoning high), each in its own worktree on a queue of one report category's
NONMATCHING drafts, matching with scope and landing with `campaign.py land`. Started 23:35 PDT
with one worker per subsystem (9). At 23:45 a second worker was added for 7 subsystems (16 in all),
taking the lower half of its subsystem's queue. The owner capped the run at 50% of the weekly Codex
quota left.

## Results

At 01:20 (about 1h45 in, quota 99% -> 62%): about 50 functions matched, 25 of them integrated in batch 1
([moozilla/boktai3-decomp#849](https://github.com/moozilla/boktai3-decomp/pull/849): 77.0% -> 77.6%
of code, 15 of 25 without pins). The rest follow in batch 2.

| Worker | Queue | Matched | Note |
|---|---:|---:|---|
| monsters (top half) | 86 | 12 | best rate: high-fuzzy drafts at the top of the queue |
| player (top half) | 19 | 7 | 1 on the first pass, 6 more on the second |
| effects (top half) | 29 | 6 | |
| menus (top half) | 11 | 5 | finished |
| engine | 11 | 3 | finished |
| field (top half) | 7 | 3 | |
| player2 | 19 | 4 | |
| menus2 | 10 | 2 | finished |
| actors (top half) | 56 | 2 | closed out functions with scripted spelling sweeps |
| actors2 | 56 | 2 | |
| bosses (top half) | 12 | 1 | finished |
| bosses2 | 12 | 1 | finished; the hardest bosses (95-950 differing insns) |
| effects2, field2, link, monsters2 | 28, 7, 6, 85 | 1 each | monsters2 swept 85 functions with scripts |

## What went wrong, and the changes made

* **Easy-first queues.** Queues were sorted likely wins first (high fuzzy per byte left). Nearly all
  matches came from queue tops, and the second worker of each subsystem got only the hard half. The
  owner wants the hard functions worked, not deferred: queues are now in ROM order
  (`queues.py`, and `campaign.py queue` in #850).
* **Per-function compile budget.** The brief said about 150 compiles per function, then move on.
  Workers read "don't give up" as applying to the run, not the function. The brief now has no budget:
  stay until MATCH or until the blocking decision is named and proved with a knob.
* **Scripted spelling sweeps.** monsters2 and actors wrote scripts that tried C spellings blindly,
  closing out about 200 functions with 2 matches. The brief now says sweeps are not diagnosis.
* **Transient API errors** (`Selected model is at capacity`, `429 Too Many Requests`) end a Codex
  session. watch.sh resumes it with `codex exec resume`; most workers needed 1 to 4 resumes.
* Codex reads its queue once: a re-sorted `queue.json` mid-run does not reach a running worker.

## scope gaps the workers reported

Grouped by how many workers hit them (the next tool changes):

1. **Reload and spill** (5): `spill` classifies but does not say how to get the ROM's scratch
   register; secondary reload choices and inherited rotation state are unexplained; local-alloc has
   no effective knob.
2. **PRE / cse copies** (4): diagnostics lack the source expression and insertion site; eliminated
   aliases' lifetimes are missing; some suggested knobs do nothing.
3. **Alignment of repeated blocks** (3): cross-block alignment produces misleading order
   differences, stack-slot pairings and `try` rankings.
4. **`why all` stops after eight hunks** (2); it needs address-range filtering, and full-unit
   analysis is slow.
5. **Shared fast-cache races** (2) when several commands run at once under one `SCOPE_RUN`.
6. One each: knob runs compiled with `-g` can report MATCH when the real build differs; `pins` can
   suggest deleting a scratch value's only definition; repeated inline-local names make knob
   targets ambiguous; no warning for uninitialized draft variables; `campaign.py land` runs
   `make format` over the whole tree (26 unrelated formatting edits); no documented override for a
   function another function's boundary swallows (0819BE28).

Each worker's per-function REPORT.md files are in `../wt/sc-*/build/scope/campaign/` on the owner's
machine (not committed).
