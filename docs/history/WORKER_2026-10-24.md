# Worker brief: family-oriented matching

Work solo unless the current user explicitly authorizes delegation. When
workers are authorized, each has its own build outputs and a non-overlapping
assignment. Prefer a related subsystem/family with a known seed, observed
layouts, actual remaining body bytes and a concrete hypothesis over arbitrary
easy-first address sweeps. One-function commits do not require independent
one-function reasoning.

Recover shared types, API contracts and evidence-backed names while matching
when they improve the cohort or future use. Keep unknown fields explicit;
record static consumers/callers or runtime evidence. Coverage alone does not
prove semantic ownership. Physical TU consolidation is optional and must not
be confused with architectural understanding. Read docs/CANDIDATES.md for the
compact checker and candidate checkpoint workflow, and docs/BACKLOG.md for
longer-term directions.

## Setup (already done for you)

Your worktree is a full checkout set up by `tools/worktree.sh` (if `gen/` or
`build/boktai3.elf` is missing, run `tools/worktree.sh --here` first: it takes
seconds). `baserom.gba`, `build/tools` (agbcc) and `build/venv` are symlinks;
`gen/`, `build/res/`, `build/text/`, the verified `build/boktai3.elf` and the
compiled objects are clones of the built base checkout (`../b3-base`). Run
everything from the worktree root.

Check with `tools/check.py FILE.c` (seconds). **Don't run `tools/build.py` or
`make`** in a worker: the full ROM build takes minutes and runs once per batch
at integration, which is where layout problems are caught.

## Loop

1. Confirm the target is not already matched; select a related cohort first.
   For an exploratory inventory:
   `build/venv/bin/python tools/worklist.py --range START END --max-insns 40 | head`
   Review jump-table boundaries; the flag alone is not a reason to skip a useful family.
2. Find its relatives: `python3 tools/relatives.py ADDR --c` lists the matched
   functions that share runs of instructions with it (an inline helper both
   expand, a sibling state handler, a family variant), how much of the target
   each explains, the instruction ranges, and their C. Start from that C when
   one covers most of the target; write a shared run as a static inline helper
   when it repeats inside the target.
3. Read it: `build/venv/bin/python tools/asmat.py ADDR 80` (asm) and
   `build/venv/bin/python tools/ghidra_c.py ADDR` (Ghidra pseudo-C, often wrong in detail
   but useful for structure).
4. Draft `wip/sub_XXXXXXXX.c`, one function per file; promote only after review and exact checks:
   ```c
   #include "global.h"

   extern u8 *gUnk_02000710;          // RAM: gUnk_<address>, the linker defines it
   extern const u8 gUnk_0860B810[];   // ROM data: use the label that exists in gen/data.s
   void sub_08120DAC(u8 *);           // callees: declare locally, keep sub_ names

   s32 sub_08033568(void) { ... }
   ```
   * Keep tentative declarations local. Promote a shared family header when
     multiple consumers establish its layout; check every changed consumer
     exactly. Do not invent a universal object type from matching offsets alone.
   * **No data definitions** (no initialized globals, `static const` tables or
     string literals). Reference existing ROM labels instead.
   * Keep the `sub_XXXXXXXX` name. If you're confident what a function does,
     add a line to `symbols/proposed/<your-worker-name>.csv`:
     `addr,name,confidence,evidence` (e.g. `08033568,Foo_SetFlag,medium,"sets flag 0x17D on gUnk_020000E0 object"`).
5. Inner loop (default): `build/venv/bin/python tools/explain.py wip/sub_XXXXXXXX.c`
   compiles your function (agbcc fork server, with `-g`), aligns it with the ROM
   and prints each mismatching region under the C line(s) that produced it
   (~1 s, a few hundred bytes to 2 KB; exit 0 = MATCH):
   ```
   L15: *(u16 *)(p + 0xc6) = *a;
   L16: value = p + 0x360;
     selection: different instructions for this code
     ROM: adds r1, #0xc6; [strh r0, [r1]; adds r0, r4, #0; adds r0, #0xd4; ... (+9)]; adds r0, r4, #0
     you: adds r1, #0xc6; [ldrh r0, [r3]; strh r0, [r1]; movs r0, #0xd8; ... (+9)]; adds r0, r4, #0
     idioms for the ROM shape:
       ~0.97 p->fD4 = (u32)p + 0x360; | p->f178->f6 = p->fAC2; | ...   [src/fn/sub_0813D34C.c:33]
   ```
   (sub_08239CDC: that idiom's struct layout matched the function in two checks.)
   * Read it top to bottom and fix **structural** regions (selection, control,
     order, extra, missing) first; `frame` (saved registers, stack size) and
     register-only regions usually move with them. Each class has a one-line
     hint (ldrsh vs ldrh+lsl+asr means a signed vs unsigned field; branch sense
     means negate the condition and swap the bodies; ...).
   * **Idioms** are matched C from `src/` whose code has the ROM's shape at that
     spot ('=N' same shape, seen N times; '~0.8' nearest). Open the file:line to
     see the declarations (types decide ldrsh/ldrh, struct vs pointer decides
     addressing). `tools/idioms.py --shape "lsls R,R,#0x10; asrs R,R,#0x10"`
     looks up any instruction sequence, and `tools/idioms.py ADDR` shows the
     nearest matched C for every basic block of a function you have not drafted
     yet. The corpus is `build/idioms/corpus.pkl`: build it once with
     `python3 tools/idioms.py --build` (about a minute) and rebuild after large
     batches of matches.
   * On a function of 500+ instructions, trust check.py's `realigned:` line
     (tools/anchordiff.py), not its fuzzy score, and work the method in
     [BIG_FUNCTIONS.md](BIG_FUNCTIONS.md): batches of edits with
     tools/variants.py, pass dumps with tools/rtlprobe.py.
   * When only registers remain, go to tools/regdiag.py / tools/rasolve.py
     (below). `tools/check.py sub_XXXXXXXX` remains the byte-exact acceptance
     check (raw aligned listing, `--full`); run it once before committing.
   * When the only structural region left is one register copy (`adds rX,
     rY, #0` on one side), explain's hint is generic: also read
     `tools/check.py --full`, and try a second variable holding the same value,
     an early return or `goto` that changes block layout, or a scoped constant
     local. In the October benchmark ([MODEL_BENCHMARK.md](MODEL_BENCHMARK.md))
     Haiku and Sonnet workers matched 5–11 of 15 near misses within 10 checks
     (about 5 checks per match). explain.py did not beat check.py there
     (Haiku 6 vs 5, Sonnet 8 vs 11). It won where an idiom pointed at a struct
     type and lost on lone register copies. Stop after about ten checks without
     progress and keep the draft (step 6).

   When it says MATCH, commit the function on the working branch. The integrator
   runs one combined full-ROM build before merging the batch.
6. Matched: `git add src/fn/sub_XXXXXXXX.c symbols/proposed && git commit -m "match sub_XXXXXXXX"`.
   If attempts stop yielding useful hypotheses, retain the candidate under `wip/`, add a line to
   `notes/<your-worker-name>.md` with the address and what was off (register
   swap, branch order, ...), and move on. To keep a useful close candidate, commit it
   as a NONMATCHING draft (below); it gets a fuzzy score, never exact credit.

**NONMATCHING drafts.** Wrap the candidate so the default build uses the asm:

```c
#ifdef NONMATCHING
// fuzzy: 96.5%
int Foo(struct Object *p) { ... }
#else
INCLUDE_ASM("asm/nonmatching", sub_XXXXXXXX);
#endif
```

`tools/check.py FILE --nonmatching` compiles the draft and compares it with the
function's full size; `--record` writes the `// fuzzy:` line that `progress.py`
reports (ROM-free) to decomp.dev. Put a short comment in the draft saying what
still differs. Only commit drafts that compile and are worth resuming.

**Scratch files:** never use fixed paths under `/tmp` (other workers share it). Put helper scripts in your worktree under `build/` (gitignored) or a directory named after your worker.

## When stuck: let fix.py try first, then ask the compiler debugger (October 23)

Before working a draft by hand, and again before committing it as a
NONMATCHING draft, run

    python3 tools/fix.py FILE.c [FUNC] [--nonmatching] --budget 120

It names the decision at each divergence and compiles the C spellings known
to flip it (helper constants, address-first locals, split offsets, casts,
ternaries, the TakeFlag pattern...), keeping the best few in a beam. A match
is written to `build/fix/FUNC.c` (copy it over the draft, drop the
`#ifdef NONMATCHING` wrapper, `make format`, then `tools/check.py`);
otherwise `build/fix/FUNC.partial.c` holds its best version, often several
instructions closer. Its output is reprinted C: tidy obvious extra
parentheses before committing. It solved 7 of 7 of the October 23
hand-matched drafts unaided, in 10 to 25 s each (`tools/fix_suite.py`).
Differences that are only register allocation are its weak spot; use
`tools/rasolve.py` there.

Before trying spellings at random, ask which compiler decision differs.
Each call costs one or two compiles:

* `python3 tools/hint.py FILE.c [FUNC] [--nonmatching]` explains the first
  difference: the decision, its rule, and the agbcc source lines that make
  it. Aim it with `--line N`, `--at 0x080F3438` (a ROM address) or
  `--insn U`. Add `--flips` (about 10 s) to flip one compiler decision at
  a time; the flip that helps names the pass (cse skip-blocks, gcse,
  strength reduction...).
* `python3 tools/hint.py --about "cse skip"` gives a rule and its compiler
  source with no draft. `tools/step.py FILE --all` lists every difference.

How to read its notes:

* **order**: agbcc has no instruction scheduler, so the ROM's order is the
  source evaluation order. Move the value with a local initialized at that
  point, an inline-helper argument (arguments are evaluated before the
  body), or statement order. Loop invariants are hoisted in loop-body order.
* **frame** with `pop {r1}`: the function returns a value, possibly a
  non-void function with no return statement.
* **call result**: cast the callee's result to u8/u16.
* **comparison type**: make the compared value signed or unsigned.

Patterns that matched:

* Actor flag at 0x2B1: `if (SceneActorControl_TakeFlag(&p[0x2B1])) {}`
  with an empty body, not `if (f) f = 0;`. Pair it with a reset helper that
  takes the address before the shared zero:
  `u8 *state = &p[0x2A8]; u32 zero = 0; *state = zero;`. See
  include/ivy_phase.h.
* A mask loaded before its global is an inline-helper argument.
* An alias like `u8 *a = (u8 *)actor;` costs a `mov ip` copy; cast inline.

The rules and worked examples are in notes/compiler-debugger-2026-10-23.md.

## agbcc matching tips

* **Use real struct types, not `*(u16 *)(p + 0x20)` casts**, whenever the
  asm indexes arrays inside an object. Leaf functions that start with
  `mov ip, r0` (369 of them) come from struct/array-of-struct accesses like
  `p->elems[i].flags &= ~1;`. With pointer casts agbcc pushes r4 instead. See
  `src/fn/sub_0810C1E8.c`. Structs also fixed ordering problems elsewhere
  (`sub_081FC61C`).
* **Switch / jump tables work**: a plain C `switch` reproduces the table.
  Order the case bodies in the source by their order in the original asm,
  not by value; use `r = N; break;` + one `return r`; list `case 0: case 7:
  default:` explicitly when the table does. `switch` on `s32` gives
  `cmp; beq; cmp; bgt`, on unsigned `blo`. (The worklist JUMPTABLE flag can
  misfire on trailing data; a real one has `ldr; mov pc` and a `.4byte` table.)
* **Branch layout**: the original often has the opposite layout from the
  obvious C. Try testing the branch-taken condition first, `goto` for the
  true cases, `if (a || b) r = 0; else r = 1; return r;`, nested ifs instead
  of `&&`, and sequential early returns in the original's order.
* **Constant load order**: declare the constant as a local first
  (`u32 m = 4;`) to hoist its `movs`; declare it inside a block after a call
  to sink it. To get "mask constant before the ldr", use a static inline
  helper: `static inline u32 tst(u32 *a, u32 m) { return *a & m; }`.
* **Commutative operand order** follows the source: try `(i << 2) + (u32)tbl`
  vs `tbl[i]`, permute OR chains, `t = r; t += x;`.
* **Shadow register + MMIO**: `gShadow = expr; REG = gShadow;` (not chained).
  DMA: `struct Dma { vu32 src, dst, ctl; }` and read `d->ctl` afterwards.
* **Hidden args**: an argument register kept alive across a call, or an
  unused-looking parameter, usually means the callee takes more parameters.
  Declare them. `pop {r1}; bx r1` means it returns the callee's value.
* **Reloads**: if the original reloads a global pointer between stores,
  declare it `u8 *` and use casts; a struct-typed global gets CSE'd.
* **Loops**: descending loops with no pre-check are `do {} while`; where the
  pointer increment sits (`e++` in the body vs. the `for`) moves `adds`.
* C89: declarations at the top of a block only.
* **Per-file compiler flags**: a line `// CFLAGS: -O2` (replaces the default `-O2 -mthumb-interwork`) — e.g. library code that ends in `pop {r4, pc}` was built without interworking.
* **Redundant null check** (`bl f; cmp r0,#0; bne; movs r0,#0`): the function
  returns the address of the struct's first field:
  `if (!p) return NULL; return &p->unk0;` (src/fn/sub_08065C40.c).
* **Materialised bools** (`movs r0,#1; b; movs r0,#0; cmp r0,#0`): put the
  test in a `static inline u8` helper (`return TRUE/FALSE`) and call it in the
  `if`. An `int` result gets jump-threaded away (src/fn/sub_080F9374.c). For a
  `bne; movs 0; b; movs 1` return, `if (x & m) return TRUE; return FALSE;`
  works where the ternary doesn't (src/fn/sub_080FF7AC.c).
* **Clone families**: run `build/venv/bin/python tools/clones.py --port --range START END`
  on your range after a few matches; it copies matched C to identical
  siblings and keeps what `check.py` accepts.
* **Near relatives**: `python3 tools/relatives.py --port --keep` ports the C of
  every matched function that explains 90% or more of an unmatched one
  (registers abstracted, so it also finds clones whose matched sibling came
  later) and keeps the MATCHes; the rest stay in `wip/relatives/` with their
  fuzzy score as starting drafts. `docs/tasks/relatives.md` ranks the
  unmatched functions by how much a matched relative explains.
* **Wrong values in the right shape**: `build/venv/bin/python tools/repair.py
  wip/sub_X.c sub_X` takes the ROM's constants and symbols (field offsets,
  ids, arguments, callees, pool callbacks, a block of fields moved by one
  delta) and puts them in place of the C's, keeping each edit that brings the
  code closer. Run it first on any port or draft that is close in shape; it
  takes seconds. What it can't fix it lists as `ROM only` / `C only`
  instructions: a call, guard or store the C is missing, which is your job.
* **Returns a value it never sets**: a non-void function that falls off the
  end (`u32 f(...) { ...; s->v = 0; }`) gives `pop {r1}; bx r1` with a stale r0.
* **Work in progress** goes in an untracked `wip/` directory
  (`check.py wip/sub_X.c` works); copy into `src/fn/` only on MATCH, so the
  full build never sees a non-matching file.
* Function pointers work: declare the callee (`void sub_081F05D0(void);`) and
  pass `sub_081F05D0`. The literal gets the Thumb bit (`src/fn/sub_081F065C.c`).

* Compiler: `agbcc -O2 -mthumb-interwork`. Leaf functions may end in
  `bx lr` with no push. Functions that call others end in `pop {r0}; bx r0`
  (void) or `pop {r1}; bx r1` (returns a value), so the return type shows
  in the epilogue.
* `movs r0, #0; str r0, [rX]; pop {r1}` at the end: write
  `return *(u32 *)(p + off) = 0;` with a `u32`/`s32` return type.
* Parameter widths matter: `lsls r1, #0x10; lsrs r1, #0x10` at entry means a
  `u16` parameter, and a missing truncation means `u32`/`s32` (a `u8` that
  should have been `u32` won't match).
* Trailing data after a function and labels other code points into are
  handled by the build (since commit "split: keep trailing data..."). If
  linking reports an undefined `_08XXXXXX` / `__addr` symbol, report it as a
  tooling bug in your notes instead of working around it.
* agbcc derives one RAM address from another (`subs r0, #0x2c`) when a
  function uses several nearby constants. Declaring each address as its own
  `extern T gUnk_<addr>;` gives each one a separate literal, as the original
  usually has. Assigning `p = (T *)CONST;` as a separate statement stops the
  offset being folded into the literal.
* Declaring a constant local first (`u32 m = 4;`) moves the constant load
  before the memory operations. A struct copy `*(struct P *)a = *(struct P *)b`
  produces paired `ldr`/`ldr [,#4]`.
* Register-allocation differences can come from declaration order or a
  missing or extra temporary. Try reordering locals, splitting or merging
  expressions, and `u8`/`u16`/`s16` types for loads (`ldrb`/`ldrh`/`ldrsh`).
* Branch order: `if (!x) return A; ...` and `if (x) {...} else return A;`
  produce different layouts. Try both. A single `return` with a result
  variable often matches better.
* `cmp rX, #n; beq; cmp rX, #n; bgt ...` is a switch statement.
* **Never write a raw ROM data address** (`0x08603300`) in C: it matches but breaks the shift test. Declare the gen/data.s label (`extern const u32 gUnk_08603300[];`). build.py rejects raw addresses >= 0x0824DAFC.
* Struct field offsets: define a local struct with observed padding and field
  types. Raw casts can change alias information, pointer reloads and address
  lifetimes; they are not always interchangeable with typed fields. The solo
  family tranche matched 0818D754 and 080F57E8 on their first typed checks.
  See `notes/solo-family_2026-10-09.md` for the layout and its limits.
* Multiplications by constants appear as shift/add sequences, and
  divisions as `bl __divsi3` / `Div` (svc 6).

## Asm-only functions from scratch (October 24)

PRs #634 and #636 wrote never-tried functions of 30 to 150 instructions
with a neighbouring family's structs: 30 to 50% matched exactly at first,
about 1 in 6 once the easy ones ran out. What worked, in order:

1. **Borrow a neighbour's spelling first.** Grep the callers and the
   functions on either side for the same offsets, callees or script ids
   (`grep -rn 0xC091 src`). A matched sibling's struct, inline helpers and
   local declaration order usually carry over verbatim (sub_08022790 from
   SceneActors_SpawnAmbientParticlePair, sub_0800F6F8 from
   StatusScreen_Initialize).
2. **Write it, then run `tools/translate.py FILE FUNC --natural --lazy
   --seconds 240 -j 1`** on the closest version. It searches spellings and
   types, not just registers, and finds things by hand rarely tries:
   it matched sub_08022790 by making the RNG index `u16`. Its `best.c` can
   be semantically wrong (it reorders statements past their uses and wraps
   blocks in `do {} while (0)`): read it before copying, and take only the
   idea.
3. **Known idioms**:
   * Shared RNG (`gUnk_03005308` index, `gUnk_0203B400` table): `u16 index`,
     locals `table`, `indexPtr`, `mask = 1023` declared in that order, and a
     `smallMask = 3` assigned just before its first use.
   * `a |= 4` on a global where the ROM loads the constant before the
     address: `s32 four = 4; gUnk_X |= four;`.
   * Two byte fields set to one value in the ROM's store order:
     `s32 v = t + 0x22; p[0x51] = v; p[0x50] = v;`.
   * A loop that recomputes `p + 0x110 + i * 4` on every pass (no hoisting,
     no strength reduction) was not optimized as a loop; a `goto` loop
     reproduces that (draft sub_08022624).
4. **Score a function inside a unit** by copying it into a scratch
   `src/fn/sub_X.c` with its declarations and running `check.py` there; the
   unit's own score covers every function in it. Delete the scratch file.

What still fails, and is worth knowing before starting: the ROM keeping a
value in a different callee-saved register than agbcc picks (translate's
"no compiler-space solution"), an inner-loop reload of a global that agbcc
merges with the outer loop's copy (sub_0800F6F8), and an incremented
counter kept untruncated in a copy (sub_08234DC8 at 98.4%).

## Finishing

Stop when your assigned budget is reached or your range has no easy
functions left. Do not run a redundant worker build when the integrator will validate the batch.
Report: functions matched (count + list), functions skipped and
why, and any naming insights.

## Big functions and near misses (October 19)

* Score a large function with `tools/anchordiff.py [RUN_DIR] [-s]` (the RUN_DIR is in
  check.py's `artifacts:` line). It realigns ROM and candidate at their shared calls, so one
  inserted instruction no longer makes the rest "differ". `-s` counts only structural
  differences. Get the structure to 0 first, then the registers.
* `tools/localsweep.py FILE.c` tries deleting `p = &s->field` pointer locals (late
  `adds rX, rY, #0` / `mov sb, rY` copies are GCSE/PRE reaching registers, so the original
  wrote the field at each use). It turned the sub_08125528 draft into a match unattended.
* A draft whose structure is right but whose registers differ: strip the `#else INCLUDE_ASM` branch
  and run the permuter for 5 minutes. It matched sub_081CC64C, where hand edits had failed.
* Write-ups of these, with the compiler-source reasoning behind them (loop.c hoisting
  thresholds, regmove operand ties, inline-argument order, PRE numbering), are in
  `notes/cloud-08122DF4.md`.

## Permuter (for "only registers differ" skips)

`build/venv/bin/python tools/permute.py wip/sub_X.c --debug` checks the base score first.
`build/venv/bin/python tools/permute.py wip/sub_X.c --run 600 --focus auto` then runs a
bounded search on all cores, stopping at an exact match, using the pinned
[decomp-permuter](https://github.com/simonlindholm/decomp-permuter) with the
project patch, installed by `make setup` (or set `PERMUTER` to another patched
checkout). Before a long run, try `tools/litsweep.py` (reload registers) and
`tools/regsolve.py` (which variable must outrank which); see
[PERMUTER.md](PERMUTER.md) for the workflow and measurements.
Candidates with better scores land in the printed
`build/permute/sub_X/run-*/output-*/` directory. Use it on your best non-matching
attempt when the structure is right and only register allocation or instruction
order differs. A zero score still needs `tools/check.py` and a full build;
see `docs/COMPILER_REFERENCES.md` for the scoring controls and limitations.

## Local orchestration safeguards

**Wrong extents: fix the boundary, don't skip.** If a listed function's extent
also holds other complete routines (its own `pop {pc}`/`bx` and literal pool,
then another `push {..., lr}`, often after raw `.byte` runs), register the
missing start instead of skipping the function:
`python3 tools/add_function.py START` adds it to the inventory
(`symbols/progress_functions.csv`), and `make` then splits it off. Check the
start with objdump and look for references first (see
`docs/SCENE_ACTOR_BEHAVIOR.md`), then match the shortened function. Commit the
boundary separately (`boundary XXXXXXXX: ...`); the full build validates it.

Before drafting, confirm the function is absent from `src/` (including library
units); matching an existing function earns no new coverage. Worker checkouts
share generated files read-only: invoke `build/venv/bin/python tools/build.py`
and `tools/shift_test.sh` directly, since Make prerequisite regeneration could
write shared `gen/`. Exact library evidence may justify `// COMPILER: old_agbcc`;
ordinary register differences alone do not. See `docs/LIBGCC.md`.

* **RTC SDK code** at `08248970–08249238` matches `agbcc -O0 -mthumb-interwork`.
  The r7 frame and repeated byte-local loads are useful clues, but require exact
  checks. Eight-bit `u32` bitfields reproduce its narrow status operations; see
  `docs/RTC.md`. Do not apply this setting broadly to ordinary register misses.

* **EEPROM SDK code** `08248634–08248970` matches `agbcc -O1 -mthumb-interwork`;
  source comparison and exact checks are in `docs/EEPROM.md`. Optimization levels
  can vary between linked libraries. Do not assume one global build flag.

* **RFU SDK code** uses `LIBRFU_VERSION 1024` and O2; its serial interrupt
  routines are ARM, selected with `// COMPILER: agbcc_arm`. Reviewed blob
  boundaries are applied without editing shared `gen/`; see `docs/RFU.md`.
  A function before an assembly blob does not make that blob matched C.

* **Audit permuter source changes**, including low-score outputs. Reject
  uninitialized reads and changes to distinct pointer lifetimes even if the
  assembly score improves. MGS round 5 found both; see `docs/MGS_GCL_MATCHING.md`.

* **A zero permuter score is not an exact match.** Sol round 6 found an
  unconditional branch-target difference with score zero. Only `check.py`
  resolved-byte equality and the complete ROM build establish acceptance;
  see `notes/round6-sol.md`.

## Current family workflow

Start with the best retained candidate and exact siblings. Review body/pool
extents, types, widths, aliases and callback signatures before allocation
experiments. Use compiler diagnostics for a concrete hypothesis; cap blind
permutation and preserve unsuccessful hypotheses. The prior solo pass already
tested prologue-bugfix without improving its selected residue.

Use tools/candidate.py with a reviewed --target-size to checkpoint meaningful
attempts, including errors and alternatives. Candidate-length equality, fuzzy
similarity, and zero permuter scores are not full-function acceptance. Review
source validity, check exact bytes; defer the complete build to batch integration.
Keep unmatched source outside production src/ and preserve it on a dedicated
WIP branch at session closeout. Report exact bytes separately from recovered
layouts, semantic evidence, candidate improvements and tool work.

## October 12 lean workflow (latest user preference)

Prioritize percentage gain over exhaustive justification. Fast checks authorize
working-branch commits; one combined full build gates the integration PR. No
repeated baseline/worker/per-commit rebuilds or regression suite for doc edits.
Use provisional descriptive names and comments; mark uncertainty, correct later.
Retain failed drafts cheaply. Report code percentage and percentage-point gain
first. Add diagnostics or tests only to answer an actual failure/question.

Local register pinning is user-authorized as a temporary matching concession.
Mark affected translation units `// MATCHING_CONCESSION: register-pinning`;
count exact bytes normally and report them in the Register-pinned category.
Retain readable C, fast exact checks, and batch validation; defer removing the
hints. This does not authorize handwritten instruction bodies.
Format C with `make format` (clang-format, pinned in requirements.txt) before
committing; formatting never changes agbcc output.
Before committing a hinted file, run `tools/depin.py FILE --write`: it
removes every pin/barrier the match does not actually need.

What counts as Register-pinned (`tools/progress.py`, 2026-10-21): any matched
function whose own body, or a static helper or function-like macro it calls (in
the unit or in a header), holds `register T x asm("rN")` or an empty `asm("")`
of any shape: plain barrier, `"memory"` clobber, `"+r"(x)` / `"r"(x)` operands,
or a register clobber `asm("" ::: "r3")`. The scan reads the source itself
(outside NONMATCHING drafts), so it counts a hint even when the unit lacks the
marker; keep adding the marker anyway. `progress.py --list-pinned` prints them
all (smallest first, with the hint kinds) as the cleanup list. Not counted:
`__asm__("Name")` symbol aliases on declarations (plain linkage names) and the
upstream `asm("swi 0x2A")` in `src/lib/m4a.c`.
