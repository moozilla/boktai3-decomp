# Compiler debugger: first session (2026-10-23)

Tags: #matching #tooling #regalloc

The research question was whether matching can work like an expert with
perfect knowledge of agbcc: name the compiler decision that differs, then
write the C that makes agbcc decide the ROM's way. Use these tools on a
draft in this order:

| Tool | Question it answers | Cost |
|---|---|---|
| `tools/step.py FILE FUNC` | Which decision differs first? It follows each differing insn through the `-da` pass dumps and names operand swaps, ties, deletions by cse/gcse/loop, and pure ordering differences. | 1 compile |
| `tools/flip.py FILE FUNC` | Which pass is responsible? It flips one decision at a time (`-fno-cse-skip-blocks`, `-fno-strength-reduce`, `-fno-gcse`...) with the instrumented agbcc, and prints where that decision lives in the compiler source. | about 16 compiles, 10 s |
| `tools/rasolve.py` | If only allocation differs: which priority or reload change? | seconds |
| `tools/hint.py FILE [FUNC] [--line N / --insn U / --at ADDR] [--flips]` | Ask about one spot: the decision there (step.py), its rule, and the agbcc source lines that make it. `--about WORDS` asks about a decision with no draft. | 1-2 compiles |
| `tools/agbccsrc.py [TOPIC or words]` | The rule and the anchored compiler source excerpt for one decision (cse-skip, cse-swap, biv-final, move2add, loop-invariant, ...). | none |
| `tools/hint.py --causes`, `tools/causebook.py` | Which C edits are known to cause a given kind of difference? It mines 145k single-edit mutants of 3,000 matched functions. | offline |
| `tools/retrobench.py` | A benchmark of broken copies of matched functions, for measuring agents. | offline |

Held-out check of the causebook: the true edit type is in its top 3 for
79% of mutants of unseen functions; frequency alone gets 59%.

## Matches

* **sub_080EFEA0** (new function, about 15 compiles). Read the ROM's
  instruction order as source evaluation order:
  * the callback and state value are loaded before the first store, so they
    are inline-helper arguments;
  * the first address comes before the shared zero and one constants;
  * the mask is formed before the load.
* **Ratatosk_BeginDirectionalReaction** (skipped 98.9% draft). Two plain
  field stores replace a contrived pointer-and-shared-zero spelling.

## Rules found (each confirmed in the compiler)

* **No scheduler.** agbcc has no Thumb instruction scheduler. An
  instruction that the ROM has earlier or later than yours is a value
  evaluated at another point in the C. step.py reports this as `order`.
  The usual fixes:
  * a local initialized there;
  * a helper argument (arguments are evaluated before the inline body);
  * statement order.
* **cse orders operands, which picks the copy.** cse puts an operand it
  knows to be a constant second in a commutative operation (cse.c
  fold_rtx, around "place a constant integer as the second operand").
  regmove then copies operand 1 for a two-operand `ands`/`orrs`. If the
  ROM copies the constant-holding register, then cse did not know it was
  constant at that point.
  * Example: SceneHud_UpdateActionSwapSelection.
  * regdiag misreports this case as allocation.
* **Dead counters leave a zero behind.** When loop.c eliminates a dead
  induction variable, it emits `reg = 0` after the loop.
  * unroll.c final_biv_value returns const0_rtx for "biv dead after loop
    exit".
  * cse2 then reuses that register for a later zero, so the zero sits at
    the loop exit.
  * Example: sub_081C5B6C. Still open: no spelling found yet.
* **cse skip-blocks.** cse carries what it knows across `if (c) x;`. It
  skips the block when the branch label has one use and the block has no
  labels (cse.c cse_end_of_basic_block). With `-fno-cse-skip-blocks`,
  sub_080F0108 goes from 10 differing instructions to 2: the ROM keeps
  `p + 0x3D4` unfolded across the `if (p->field2B1) p->field2B1 = 0;`.
  * The spelling is still open. These did not change it: `&& 1`, an empty
    else, a goto, `do {} while (0)`, an unused label, early-return inline
    helpers, and SceneActorControl_TakeFlag.
* **Derived constants depend on what is still in a register.** reload's
  move2add turns `movs rX, #K` into `adds/subs rX, #d` when rX still holds
  K-d. sub_080EFF6C differs only in this. rasolve: one more reload before
  `count = p + 0x2AC` in the rotation leaves 5 bytes.

## Second session: the ask-driven hint on new and skipped functions

14 matches in the 080E8000-080F8000 range, 4 of them skipped drafts, most
in 1-5 compiles. Rules confirmed along the way:

* **An empty `if` on a taking helper ends cse's block.**
  `if (SceneActorControl_TakeFlag(&p[0x2B1])) {}` (a debug print compiled
  out, probably) stops cse from carrying values across the flag test; a
  plain `if (f) f = 0;` lets it, and then cse folds `sub + 0x42` into
  `p + 0x416`. Found from flip (`-fno-cse-skip-blocks`) and the cse-skip
  excerpt. Used by sub_080F0108, sub_080EFF6C, sub_080EB8CC, sub_080EBA84;
  the helper Ivy_ResetPhase is in include/ivy_phase.h.
* **The ROM's epilogue says whether a value is returned.** `pop {r1}` with
  the argument copied out of r0 is a non-void function with no return
  statement (sub_080F1FE8). step.py now reports this as `frame`.
* **Loop invariants are hoisted in body order.** A value the ROM loads among
  a loop's hoisted constants was written inside the loop body at that point
  (PhaseObject_TickCountdown's callback).
* **Constant-then-address and address-then-constant are both source order.**
  A shared zero after the first address is `u8 *x = &...; u32 zero = 0;`;
  a bit loaded before its address is a local initialized first
  (sub_080F3B30, sub_080EBEE0, sub_080F4AB4's `u8 one`).
* An unfolded `+0x20` then `+8` is a pointer local stepped first
  (`u32 *d = ptr + 8; d[2] |= 1;`).

Parked, with what is known:

* sub_080F3420: the found block (moved before the loop) recomputes `i *
  0x194` twice with `muls`; every spelling tried lets cse reuse the loop's
  multiply. Drafts in build/exp/w.
* Sword_AccelerateReturnTurn (src draft, 10 bytes): loop.c hoists the
  0x119, 0x164 and 0x11E addresses in that order in the ROM; the draft
  hoists 0x11E before 0x164 (second-pass "not desirable" decisions in the
  loop dump).

## Drafts left in build/exp only (not committed)

These have their decisions named but no spelling yet:

* sub_080F0108: cse skip-blocks; 2 instructions with the flag.
* sub_080EFF6C: reload rotation; 7 instructions.
* sub_081C5B6C: dead-biv zero. An existing draft is in src.

## Third session: fix.py, the inverse step

`tools/fix.py FILE.c [FUNC] [--nonmatching] [--budget S]` turns the
diagnosis into edits. step.py names the decision at the first divergence;
`tools/spellings.py` maps each decision to C rewrites known to flip it, and
the generic rewrites/permuter passes follow. Every candidate is compiled
through the fork server and scored by differing instructions (branch targets
included); a beam keeps half its states by fewest differences and half by
latest first divergence, plus score-neutral spellings that can enable the
next one (if/else to ternary, then inline the temporary).

Spellings, each from a real case:

* `sp_specialize`: a helper called with constants becomes a helper with the
  constants in its body (then shared-constant spellings on it).
* `sp_helper_arg`: `E & K` becomes `H(K)` (helper arguments are evaluated
  before the body).
* `sp_local_to_arg`: a helper's first local (`a = &p->position`) becomes an
  argument (sub_081C8FB0).
* `sp_addr_before_next`: `L = E; zero = 0;` becomes
  `{ q = &L; zero = 0; *q = E; }` (sub_0823AE5C).
* `sp_split_offset`: `*(u32 *)(X + 0x28)` becomes `((u32 *)(X + 0x20))[2]`.
* `sp_goto_loop`: a loop written with goto is not optimized by loop.c: the
  ROM recomputes invariant addresses inside it and does no strength reduction.
* Also: address-first locals, index locals, comparison casts, call-result
  casts, ternaries, the empty-if TakeFlag, non-void without return.

Results on October 23:

* `tools/fix_suite.py` (tests/fix/: the day's drafts before their hand
  fix): 7/7 matched, 10 to 25 s each.
* Batch over the 421 single-function NONMATCHING drafts, 60 s each, CPU
  only: 8 matches; 246 improved (13,240 to 10,200 differing instructions);
  171 drafts rewritten with fix.py's version where the fuzzy score rose.
  Two more matched by one hand edit on fix.py's draft, each since added as a
  spelling.
* Old drafts' residue is mostly register allocation (often with pins).
  rasolve found no exact compiler-space fix on 8 sampled; that needs its own
  catalog (priority changes as C edits).
