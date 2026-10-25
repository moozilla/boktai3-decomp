# Cloud workers

Tags: #tooling #matching

Claude Code cloud sessions (claude.ai/code) can work on the decomp in parallel with
local workers. A new session is ready in seconds (the environment caches a built
base checkout, see below), then takes tasks from a queue coordinated through git
branches, so no two workers take the same functions.

## One-time setup (the user)

1. Put the ROM somewhere a direct download link reaches, privately (for example a
   private file host link). **Never commit it** and never put it in the repository.
2. Create a cloud environment for `moozilla/boktai3-decomp` with:
   * environment variable `BOKTAI3_ROM_URL` = that link;
   * setup script:

     ```sh
     #!/bin/bash
     mkdir -p /opt/boktai3
     git clone -q --depth 1 https://github.com/moozilla/boktai3-decomp /opt/boktai3/base \
       && sh /opt/boktai3/base/tools/cloud_setup.sh --provision \
       || echo "boktai3 provision failed; sessions fall back to a full setup"
     ```

How setup works (about 3 minutes once, then seconds per session):

* The setup script (`cloud_setup.sh --provision`) installs packages, builds the tools
  (agbcc, gbadisasm, permuter), makes the venv, downloads and verifies the ROM (SHA-1
  `2651c5e6…`) and builds `origin/main` once, all in `/opt/boktai3/base`. It finishes
  well inside the five minutes after which the environment stops caching, so the
  filesystem is snapshotted and later sessions skip it. The cache is rebuilt when the
  setup script or network settings change, or after about seven days.
* The SessionStart hook (`.claude/settings.json` → `cloud_setup.sh --session`, cloud
  sessions only) links the base's tools, venv and ROM into the session's checkout and
  copies its `gen/` and objects (about 5 seconds). If the checkout differs from the
  base's commit it rebuilds in the background (up to ~2 minutes when the cache is a day
  or more old); `tools/check.py` and `tools/build.py` wait for that by themselves.
* Without the setup script (or if it had no ROM), the hook does the full setup in the
  background instead (about 3 minutes).
* What used to take 35 minutes was gbadisasm: its label lookups were linear scans,
  ~7 minutes per pass. `tools/patches/gbadisasm-fast.patch` indexes them (identical
  output, ~3 seconds per pass).

## What a worker does (the brief)

Start each session with this prompt (the main session can start them too):

> You are a decomp worker on moozilla/boktai3-decomp. Read CLAUDE.md and docs/CLOUD.md.
> The session hook sets the checkout up; if `build/boktai3.elf` is still missing, run
> `sh tools/cloud_setup.sh` (about 3 min). Then loop until your time runs out:
> claim a task with `build/venv/bin/python tools/cloud_task.py claim --unassigned`, match its
> functions, and PR it.

Per task:

1. `tools/cloud_task.py claim --unassigned` prints the task: a subsystem's unmatched functions of
   150+ instructions, with addresses and sizes, and checks out branch `cloud/TASK`.
   It takes the biggest open task. `list --unassigned --smallest` orders tasks by their
   largest function, and `claim --unassigned --task ID` takes a specific one.
   `--from-end` orders by address from the end of the ROM backwards (Claude workers use it
   to stay clear of Codex workers going forwards); `--max-insns N` keeps only functions
   below N instructions (pair with a lower `--min-insns` for small-function tasks).
   `--fresh` puts tasks with the most functions lacking a NONMATCHING draft first (fresh
   40–120 insn functions match far more cheaply than old near-miss drafts). Task ids
   depend on the size filters, so `claim --task ID` needs the same filters as the list.
   The claim commit lists the task's functions; `claim` skips any task that shares a
   function with an unmerged claim (task ids shift as main changes, functions don't).
2. For each function, biggest payoff first: `tools/asmat.py ADDR 80`, then write C.
   If the subsystem has a unit (`src/<subsystem>.c` holding the function's
   `INCLUDE_ASM`), write it there; otherwise in `src/fn/sub_ADDR.c` (copy the shape of
   a neighbouring file). Use the shared headers in `include/`; add struct fields there.
3. Check with `build/venv/bin/python tools/check.py FILE.c` (seconds). Useful helpers:
   `tools/explain.py` (per-line mismatch report), `tools/repair.py` (fixes wrong
   constants and callees), `tools/regions.py` (structure score that ignores
   registers), `tools/rasolve.py` / `tools/translate.py` (register allocation),
   `tools/relatives.py ADDR` (similar matched functions). For functions of 500+
   instructions read docs/BIG_FUNCTIONS.md first (`tools/condense.py`,
   `tools/variants.py`, `tools/rtlprobe.py`; score by check.py's `realigned:` line). docs/PERMUTER.md and
   docs/FOUNDRY.md say when each helps.
   If a function has no prologue (no `push`, or only `push {r5, r6, r7}`), reads a
   stack frame it never set up, or its neighbour has no epilogue, the split is wrong:
   it is a label inside another function. Remove its start from
   `symbols/progress_functions.csv` (then `make`): the split drops disassembler starts the
   inventory does not list. Match the whole parent as one function.
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
