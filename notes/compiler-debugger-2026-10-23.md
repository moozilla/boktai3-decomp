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
| `tools/hint.py`, `tools/causebook.py` | Which C edits are known to cause a given kind of difference? It mines 145k single-edit mutants of 3,000 matched functions. | offline |
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

## Drafts left in build/exp only (not committed)

These have their decisions named but no spelling yet:

* sub_080F0108: cse skip-blocks; 2 instructions with the flag.
* sub_080EFF6C: reload rotation; 7 instructions.
* sub_081C5B6C: dead-biv zero. An existing draft is in src.
