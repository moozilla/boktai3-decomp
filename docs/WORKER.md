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

Your worktree is a full checkout with `baserom.gba`, `gen/` (generated
asm, shared, **read-only**) and `build/tools` (agbcc) symlinked in. Run
everything from the worktree root.

## Loop

1. Confirm the target is not already matched; select a related cohort first.
   For an exploratory inventory:
   `build/venv/bin/python tools/worklist.py --range START END --max-insns 40 | head`
   Review jump-table boundaries; the flag alone is not a reason to skip a useful family.
2. Read it: `build/venv/bin/python tools/asmat.py ADDR 80` (asm) and
   `build/venv/bin/python tools/ghidra_c.py ADDR` (Ghidra pseudo-C, often wrong in detail
   but useful for structure).
3. Draft `wip/sub_XXXXXXXX.c`, one function per file; promote only after review and exact checks:
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
4. Inner loop: `build/venv/bin/python tools/check.py sub_XXXXXXXX` compiles just your file,
   links it at its real address and prints original vs. yours as compact aligned mismatch hunks,
   with `!` on differing lines (`--full` expands the listing) (~0.5 s; exit 0 = MATCH). Literal pools show up
   as junk instructions; that's fine. When it says MATCH, commit the function on the working branch. The integrator
   runs one combined full-ROM build before merging the batch.
5. Matched: `git add src/fn/sub_XXXXXXXX.c symbols/proposed && git commit -m "match sub_XXXXXXXX"`.
   If attempts stop yielding useful hypotheses, retain the candidate under `wip/`, add a line to
   `notes/<your-worker-name>.md` with the address and what was off (register
   swap, branch order, ...), and move on. Never commit a nonmatching file to `main`. Dedicated WIP branches may
   preserve candidates outside `src/`; they receive no exact progress credit.

**Scratch files:** never use fixed paths under `/tmp` (other workers share it). Put helper scripts in your worktree under `build/` (gitignored) or a directory named after your worker.

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

## Finishing

Stop when your assigned budget is reached or your range has no easy
functions left. Do not run a redundant worker build when the integrator will validate the batch.
Report: functions matched (count + list), functions skipped and
why, and any naming insights.

## Permuter (for "only registers differ" skips)

`build/venv/bin/python tools/permute.py wip/sub_X.c --debug` checks the base score first.
`build/venv/bin/python tools/permute.py wip/sub_X.c --run 60 -j 2` then runs a bounded search
using the pinned [decomp-permuter](https://github.com/simonlindholm/decomp-permuter)
installed by `make setup` (or set `PERMUTER` to another checkout).
Candidates with better scores land in the printed
`build/permute/sub_X/run-*/output-*/` directory. Use it on your best non-matching
attempt when the structure is right and only register allocation or instruction
order differs. A zero score still needs `tools/check.py` and a full build;
see `docs/COMPILER_REFERENCES.md` for the scoring controls and limitations.

## Local orchestration safeguards

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
Before committing a hinted file, run `tools/depin.py FILE --write`: it
removes every pin/barrier the match does not actually need.
