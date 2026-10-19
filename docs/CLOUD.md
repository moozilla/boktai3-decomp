# Cloud workers

Tags: #tooling #matching

Claude Code cloud sessions (claude.ai/code) can work on the decomp in parallel with
local workers. Each one sets up its own machine once (about 35 minutes), then takes
tasks from a queue coordinated through git branches, so no two workers take the same
functions.

## One-time setup (the user)

1. Put the ROM somewhere a direct download link reaches, privately (for example a
   private file host link). **Never commit it** and never put it in the repository.
2. Create a cloud environment for `moozilla/boktai3-decomp` with:
   * environment variable `BOKTAI3_ROM_URL` = that link;
   * setup script: `sh tools/cloud_setup.sh`.

`tools/cloud_setup.sh` installs packages, builds the tools, downloads and verifies the
ROM (SHA-1 `2651c5e6…`), and builds once. If the environment has no setup-script
option, the worker runs it as its first step.

## What a worker does (the brief)

Start each session with this prompt (the main session can start them too):

> You are a decomp worker on moozilla/boktai3-decomp. Read CLAUDE.md and docs/CLOUD.md.
> If `build/boktai3.elf` is missing, run `sh tools/cloud_setup.sh` (about 35 min) and
> wait for `ready: build/boktai3.gba: OK`. Then loop until your time runs out:
> claim a task with `build/venv/bin/python tools/cloud_task.py claim --unassigned`, match its
> functions, and PR it.

Per task:

1. `tools/cloud_task.py claim --unassigned` prints the task: a subsystem's unmatched functions of
   150+ instructions, with addresses and sizes, and checks out branch `cloud/TASK`.
2. For each function, biggest payoff first: `tools/asmat.py ADDR 80`, then write C.
   If the subsystem has a unit (`src/<subsystem>.c` holding the function's
   `INCLUDE_ASM`), write it there; otherwise in `src/fn/sub_ADDR.c` (copy the shape of
   a neighbouring file). Use the shared headers in `include/`; add struct fields there.
3. Check with `build/venv/bin/python tools/check.py FILE.c` (seconds). Useful helpers:
   `tools/explain.py` (per-line mismatch report), `tools/repair.py` (fixes wrong
   constants and callees), `tools/regions.py` (structure score that ignores
   registers), `tools/rasolve.py` / `tools/translate.py` (register allocation),
   `tools/relatives.py ADDR` (similar matched functions). docs/PERMUTER.md and
   docs/FOUNDRY.md say when each helps.
4. Byte-identical only, no handwritten `asm()` instructions. A close miss (fuzzy ≥ 80%)
   may stay as a `#ifdef NONMATCHING` draft (docs/WORKER.md); otherwise leave the
   `INCLUDE_ASM`. Give every function you match a useful name: a file-local
   `#define Name sub_ADDR` alias.
5. Commit each match separately. When the task is done (or you stop), run
   `make format`, then one full build (`BUILD_JOBS=$(nproc) make` must print
   `build/boktai3.gba: OK`), push the branch and open a PR titled
   `Cloud task TASK: N matches in <subsystem>`. Report matched and drafted functions in
   the PR body. Then claim the next task.

Don't edit shared docs (HANDOFF, INDEX, history); put notes in `notes/`. The main
session merges cloud PRs after a combined build.
