# Worker playbook: matching with scope

Tags: #matching #process

How to take a function from the ROM to C that compiles byte for byte to it. Read
[HANDOFF.md](HANDOFF.md) first for the phases and the goal. The matching loop itself
is [tools/scope/WORKER.md](../tools/scope/WORKER.md); this file is the frame around it.
The previous playbook (wip/ files, explain.py, family rounds) is kept in
[history/WORKER_2026-10-24.md](history/WORKER_2026-10-24.md).

## Aim for the final form

Every function goes through four phases (draft, match, no concessions, cleanup;
HANDOFF.md), but **aim straight for the end**: C that matches with no register pins
or `asm("")` barriers, with struct fields, real types and names, written the way the
original authors would have. A concession is a last resort, used only when a knob
proved an allocation decision you could not reproduce in C after real attempts, and
the report says so. Easy C that matches with pins is not the target; it is an interim
result that phase 3 has to redo.

Work systematically: in ROM order through a subsystem, staying on a function until it
matches. Don't sort for likely wins, don't skip the hard ones, don't hop between
functions when one gets difficult. When progress stalls the next step is more
diagnosis, not another function.

## Setup

* A worktree: `tools/worktree.sh NAME` (seconds; clones the built base `../b3-base`,
  which `sh tools/base.sh` keeps at origin/main). In a desktop-app worktree:
  `tools/worktree.sh --here`.
* scope's instrumented compiler lives in `build/scope/agbcc/`. In a new worktree link
  the main checkout's (`ln -s MAIN/build/scope/agbcc build/scope/agbcc`) or build it:
  `sh tools/scope/build_agbcc.sh`.
* Check with `tools/check.py FILE.c` (one unit, seconds) or scope. **Never run `make`
  or `tools/build.py` in a worker**: the full ROM build runs once per batch at
  integration. No whole-corpus sweeps either.
* Use `build/venv/bin/python` for tools that need the venv (scope re-execs itself).
  Scratch files go under `build/` in your worktree, never fixed `/tmp` paths.

## The loop

[tools/scope/WORKER.md](../tools/scope/WORKER.md) is the procedure. In short:

1. **No C yet** (`scope.py todo`): `scope.py new FUNC` opens a workspace
   (`build/scope/work/FUNC.c`); `scope.py rom FUNC`, `like FUNC` (matched functions
   with similar code), `fields OFF...` (structs with members at those offsets) and
   `tools/relatives.py FUNC --c` give you the program to write.
2. **A draft** (`#ifdef NONMATCHING` in src/): copy the unit to a scratch file and run
   `scope.py FILE.c FUNC`. It prints MATCH or the differing hunks with the C line,
   RTL insn and pass that produced each.
3. `why UID` / `why all` explains a difference from the compiler's own traces and
   names a lever. **Confirm with a knob** (`AGBCC_PRI=name:+N`, `AGBCC_FORCE`,
   `AGBCC_SPILL`, `CSE_NOCANON`, ... ) before changing C, then find C with the same
   effect. `try` scores several variant files of one hypothesis in one call.
   `probe`, `spill`, `hoist`, `giv`, `frame`, `life`, `vs`, `pins` cover the rest.
4. Changes interact: keep one `why` asked for unless it makes things worse, and stack
   the next. Matches usually take two or three together.
5. On MATCH: `scope.py land FUNC [--name Prefix_Action]` (from a `new` workspace), or
   in a subsystem run `campaign.py land FUNC .`. Either merges into the unit, adds or
   removes the concession marker, runs `make format` and checks the unit. Commit one
   function per commit (`Match FUNC`).
6. Pins in an existing match (phase 3): `scope.py FILE.c pins` gives the cost of
   removing each; `tools/depin.py FILE --write` drops the free ones.

The compiler facts scope has established are in [tools/scope/README.md](../tools/scope/README.md),
section "agbcc facts established with scope": grep it when a difference looks familiar.

## When stuck

scope is the main tool, not a cage. Use whatever helps, and write down what worked:

* [external/eds/matching-tricks.md](external/eds/matching-tricks.md) and
  [compiler-flags.md](external/eds/compiler-flags.md): the Yu-Gi-Oh! EDS decomp's catalog
  of GCC 2.95 tricks (same middle end; [what carries over](external/eds/README.md)).
* [AGBCC_TIPS.md](AGBCC_TIPS.md): the pre-scope patterns (struct access, flag helpers,
  typed zeros, masks, signedness...). [AGBCC_RULES.md](AGBCC_RULES.md): why agbcc chose it.
* Register allocation: `tools/rasolve.py FILE.c FUNC` (solves inside the compiler, then
  names a C change), `tools/regsolve.py`, `tools/autosolve.py` over many functions;
  [REGALLOC.md](REGALLOC.md).
* `tools/fix.py FILE.c FUNC --nonmatching --budget 120` compiles known spellings for the
  first divergence; `tools/hint.py` explains one decision. The permuter:
  `tools/permute.py` ([PERMUTER.md](PERMUTER.md)). 500+ instruction functions:
  [BIG_FUNCTIONS.md](BIG_FUNCTIONS.md).
* A matched relative's C (`scope.py like`, `tools/relatives.py --c`), or rewriting the
  function from the ROM instead of patching the draft.

What does not count as diagnosis: scripts that sweep C spellings blindly. Every variant
should test a hypothesis that scope's output gave you. When a hunk cannot be explained,
that is a scope gap: record it (below) so the tool gets better.

## Rules

* Matching means byte-identical. **No hand-written instructions in `asm()`**, ever.
  Register pins and empty `asm("")` barriers are counted concessions: the unit carries
  `// MATCHING_CONCESSION: register-pinning` (land adds it) and `progress.py` reports
  them as Register-pinned.
* Nonmatching C on `main` only as a guarded draft:
  ```c
  #ifdef NONMATCHING
  // fuzzy: 96.5%
  int Foo(struct Object *p) { ... }
  #else
  INCLUDE_ASM("asm/nonmatching", sub_XXXXXXXX);
  #endif
  ```
  `tools/check.py FILE --nonmatching --record` writes the `// fuzzy:` line;
  `scope.py land FUNC --nonmatching` does the same for a workspace.
* No data definitions in drafts (initialized globals, `static const` tables, string
  literals): reference the ROM labels that exist. agbcc is C89 (declarations first).
* One translation unit per change; a unit is a contiguous run of functions in ROM order.
* Names: `Prefix_Action`, one prefix per object (`scope.py rename`, `land --name`).
  Provisional names are welcome; mark guesses `confidence=low`.
* Comments in C stand alone: say what the code does, never what you tried.

## Recording what you learned

* Per function: what `why` said and which C change it led to; what still differs and
  the diagnosis; **where scope's output was confusing, wrong or missing, and what
  would have saved you time**. In a subsystem run this is `build/scope/campaign/FUNC/REPORT.md`;
  the coordinator folds the gaps into [tools/scope/README.md](../tools/scope/README.md) "Gaps".
* Batch summaries go in `notes/TOPIC_YYYY-MM-DD.md` with a `Tags:` line, then
  `python3 tools/docindex.py`. HANDOFF.md says how to record a batch.
