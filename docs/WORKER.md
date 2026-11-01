# Worker playbook: matching with scope

Tags: #matching #process

How to take a function from the ROM to C that compiles byte for byte to it. Read
[ROADMAP.md](ROADMAP.md) for the phases and [HANDOFF.md](HANDOFF.md) for where things stand. The matching loop itself
is [tools/scope/WORKER.md](../tools/scope/WORKER.md); this file is the frame around it.
The previous playbook (wip/ files, explain.py, family rounds) is kept in
[history/WORKER_2026-10-24.md](history/WORKER_2026-10-24.md).

## Aim for the final form

The project is in phase 3 (ROADMAP.md): phase 2 is nearly done, and the work now is
matching the last drafts and removing the register pins from existing matches. **Aim for
ordinary C**: no register pins (`register T x asm("rN")`), no `asm("")` barriers, never a
compiler override, with struct fields, real types and names, written the way the original
authors would have. Pins are heavily discouraged: each one is phase 3 work someone has to
undo, and the C that gives the same allocation has again and again been easier to find
than it looked. The landing commands refuse a match that adds pins or barriers unless you
pass `--allow-pins`.

Work systematically: in ROM order through a subsystem, staying on a function until it
matches. Don't sort for likely wins, don't skip the hard ones, don't hop between
functions when one gets difficult. When progress stalls the next step is more
diagnosis or a different approach, not another function.

## Stay on a function until it matches

A function is finished when it matches. Finding the compiler decision that blocks it is
progress, not a reason to stop: the next step is the C that produces that decision. Hard
functions take hundreds of compiles and hours, and many that one model left as hopeless were
matched later (`Ratatosk_UpdateDefeatSequence`, `081F4CF8`, after a round where GPT-6 Pro
set it aside). There is no compile or time budget.

When one approach stops paying, change the approach, in roughly this order:

1. `why` on the first real difference of every hunk, confirmed with a knob, and the C
   spellings the mechanism suggests (tools/scope/WORKER.md).
2. Recover what the original source had: the struct, macro or inline helper a matched
   relative uses (`scope.py like`, `tools/relatives.py --c`, `scope.py fields`).
3. Rewrite the function, or the stuck part of it, fresh from the ROM instead of patching
   the draft.
4. A persistent search: `tools/matchsearch` keeps every hypothesis, the best candidates and
   the compiler evidence across a long search, and keeps sources that are structurally right
   even when their byte distance is worse.
5. Read the agbcc source behind the decision (`build/scope/agbcc/gcc`; scope names the
   function and pass).
6. Fix or extend the tool so the next step becomes visible, and record the gap.

**Pinned matches and holdouts.** Settle for less than ordinary C only after extreme effort:
all six steps above, every remaining hunk explained by a mechanism proved with a knob, the C
for each tried and recorded in the function's matchsearch store, and no well-founded idea
left. Then:

* If pins or barriers make it match, land it with `--allow-pins`. A pinned match keeps
  progress moving and is better than a holdout. Its note says which allocation decision the
  pin forces, the knob that proved it, and the pin-free C tried, so phase 3 starts from there.
* If it still won't match, it is a **holdout**: keep the best ordinary-C draft in its
  `#ifdef NONMATCHING` branch, write the function's note (the differing instructions, the
  mechanism, what was tried and what was ruled out, and the path of the matchsearch store),
  list it as a holdout in your final message, and go on to the next function. A holdout is handed to a stronger model with your store and notes, so they
are the most valuable thing you leave. Being stuck for a while, a hard-looking diff or a
long function is not a holdout.

**Don't stop the run.** Keep going until every function in your queue is matched or a
holdout. A quota floor or the owner's stop is a checkpoint: commit what matches, keep the
best draft of the current function and write its note.

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
5. A long search: `build/venv/bin/python -m tools.matchsearch init build/matchsearch/FUNC
   SRC FUNC` starts a persistent store; `propose` complete C sources as hypotheses and
   `request` diagnoses, moves and the frontier ([tools/matchsearch/README.md](../tools/matchsearch/README.md)).
   Keep using the store for the function; it is what a later session resumes from.
6. On MATCH: `scope.py land FUNC [--name Prefix_Action]` (from a `new` workspace), or
   in a subsystem run `campaign.py land FUNC .`. Either merges into the unit, runs
   `make format` and checks the unit, and refuses a match that adds pins or barriers
   unless given `--allow-pins` (then it adds the concession marker). Commit one
   function per commit (`Match FUNC`).
7. Pins in an existing match (phase 3): `scope.py FILE.c pins FUNC` gives the cost of
   removing each, alone and grouped, and writes `FUNC.no_pins.c` with all of them gone;
   rematch from that draft with the loop above (the free removals `tools/depin.py` made
   are used up). Removing the last pin of a unit also removes its concession marker.

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
* A private inline helper that copies a shared one: `tools/shared_inline.py FILE.c` (or
  `--queue QUEUE.md`) lists each static inline that resembles a header's. A pinned private
  copy of the display-sequence step is common; calling `PlayerCue_StepSequence*` or the
  object's header step instead dropped every pin in EnemyD459_Update, Ooze_BeginAttack and
  VulcanCannon_InitializeFromScript.

What does not count as diagnosis: scripts that sweep C spellings blindly. Every variant
should test a hypothesis that scope's output gave you. When a hunk cannot be explained,
that is a scope gap: record it (below) so the tool gets better.

## Rules

* Matching means byte-identical, with the stock compiler. **No hand-written instructions
  in `asm()`** and no compiler overrides, ever: scope's knobs (`AGBCC_PRI`, `AGBCC_SPILL`,
  ...) find a mechanism, and the landed C must match without them. Register pins and empty
  `asm("")` barriers only as above, with `--allow-pins`; the unit carries
  `// MATCHING_CONCESSION: register-pinning` and `progress.py` reports it as
  Register-pinned. Take pins out of a unit you touch.
* An empty `do { } while (0);`, or `DEBUG_NOP()` (`include/global.h`), which expands to one, counts as a barrier. Its loop notes end the first cse pass
  and weight register refs the way a barrier does. A statement macro wrapped in
  `do { ... } while (0)` with a body is ordinary C only when the original plausibly had
  it, and the usual evidence is that several functions share it (twins computing the same
  thing). One used at a single site, written only to double its statement's refs, is a
  priority knob: the tools count it as a barrier (`tools/statement_macros.py`) and land
  refuses it without `--allow-pins`. An empty one is accepted as ordinary C
  only with evidence that the original had a compiled-out macro at that spot, for example
  the same statement in every caller of a shared helper. The tools count it and land
  refuses it without `--allow-pins`.
* An `if` whose body the original compiled out is written with `DEBUG_PRINT()`
  (`include/debug.h`, expands to nothing): `if (TakeFlag(f)) { DEBUG_PRINT(); }`. Matched
  code has 13 such sites, nearly all on `Take*` flag calls. Use it only where the original
  plausibly logged something: the condition is a call or a test worth reporting. An `if`
  with a bare empty body (`{ }` or `;`) is a fence; the tools count it as a barrier and
  land refuses it without `--allow-pins`.
* A `goto` loop is ordinary C, accepted for now where loop.c would rewrite the structured
  form (a counter reversed to count down, for example); phase 4 revisits them. So is a
  non-void function that falls off the end, leaving its argument in r0, when the ROM does
  that.
* The SDK's `SystemCall(n)` (`include/gba/syscall.h`) is the one inline assembly that
  counts as ordinary C: the original authors wrote it that way.
* No selector macros: a `#define` a file sets before including a shared header to change
  the header's code. Shared code is a shared inline helper or macro the callers use as is.
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
