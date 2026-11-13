# Codex Cloud worker: common rules (Boktai 3 decomp, phase 3/4 push, 2026-11-13)

You are one of about a dozen Codex Cloud workers, running alongside ~20 local Codex workers. Each
worker owns a disjoint list of functions or units; the coordinator (a Claude session) applies your
task's diff onto `main` after the run. You run on branch `codex/cloud-run`, which is `main` plus this
`cloud/` folder of briefs. Never edit `cloud/`.

The project: a byte-identical decompilation of Boktai 3 (GBA, agbcc = GCC 2.95 Thumb, `-O2
-mthumb-interwork`). Every one of 11,810 functions already matches in C. Phase 3 (now) removes the
last concessions (register pins, asm barriers and the other kinds in `tools/concessions.py`).
Phase 4 (cleanup) makes the code read like the original: real structs and names, one translation
unit per subsystem instead of 10,000 one-function files in `src/fn/`.
Read `CLAUDE.md` (ground rules; they apply to you), then your assignment.

## Setup (first thing)

The environment's setup script built a base checkout in `/opt/boktai3/base`. Link it into this
checkout: `sh tools/cloud_setup.sh --session` (seconds; a rebuild may continue in the background,
`tools/check.py` waits for it). If `build/boktai3.elf` or `build/venv` is still missing afterwards,
run `sh tools/cloud_setup.sh` (about 3 minutes). Confirm with `build/venv/bin/python tools/check.py`
on one file of your assignment (must print MATCH) before changing anything.

## Committing

Commit with git as you go (`git add` the paths, `git commit -m`), one commit per function matched,
per unit consolidated, per tool change. End every commit message with the line
`Co-Authored-By: Codex (gpt-6.1-sol) <noreply@openai.com>`. Your task's final diff is what gets
integrated, so uncommitted-but-saved work still counts; but keep the tree consistent: every file you
touch must pass `tools/check.py` at every point you might be stopped.

## Rules

* Byte-identical with the stock compiler, from ordinary C. No hand-written instructions in asm(),
  no compiler overrides, no new concession of any kind (pins, `asm("")` barriers, volatile locals,
  DEBUG_NOP fences, single-use macros, `asm("Symbol")` aliases, raw offsets where a field fits).
  No selector macros. Never `--allow-pins`.
* Check every unit you touch with `build/venv/bin/python tools/check.py FILE.c` (MATCH / exit 0).
  Don't build the whole ROM. Don't edit `build/asm/*`, `PROGRESS.md` or `PROGRESS.json`.
* Stay inside your assignment's files. Others own the rest; `cloud/depin_files.txt` lists files
  owned by depin workers (cleanup workers never touch them). A shared header change must be small
  and layout-preserving, and every unit that includes it must still match (`grep -l` it).
* No PII in anything you write (real names, usernames, home-directory paths; write `~`). Never add
  the ROM or anything under build/. US spelling.

## Depth over breadth (read this twice)

This is a long task: you have until **10:10 UTC**, about an hour, and you should use all of it.
Ending early is a failure, and so is a list of shallow notes. The goal is landed matches:
work through your list in order and **stay on each function until it matches** (land with
`scope.py land FUNC`, or the unit with `tools/check.py`). A holdout note is only for a function you
spent at least 20 minutes of real C attempts on, with the scope evidence that blocked it. Never
write drafts or notes for functions you haven't seriously attempted. Before you end, check
`date -u`: if it's before 10:00 UTC, you are not done; go back to the next function.

## Time and reporting

Work until **10:10 UTC** (check `date -u` between items), then make sure everything is committed and
end. If your platform's task time limit comes first, the same applies: commit after every item so
nothing is lost. Never stop early: when your list is done, go back to what you set aside with what
you learned, then improve the tools.

Your final message is your report: what you landed (commit summaries), what decided each result,
tool changes, what's blocked and why, what you'd do next.
