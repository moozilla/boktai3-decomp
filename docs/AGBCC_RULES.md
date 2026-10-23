# agbcc rules: why the compiler wrote it that way

Tags: #matching #regalloc #tooling

Each rule says which decision in agbcc produces a code pattern, and what in
the C source controls that decision. Read it backwards from a diff: find the
pass that made the instruction (`tools/why.py FILE FUNC` prints each
differing instruction's trail through the passes), then look up the pass
here. If no rule covers it, read that pass in `build/agbcc-instr/gcc/`. The
compiler is deterministic, and the source says exactly when it makes the
choice. When a fix is verified (it matched, or it flipped the instruction in
question), add a rule with the function as its example.

Allocation priorities and the allocator tools are in `docs/REGALLOC.md`.

## Loops (loop.c)

**Code order before a loop is evidence about the loop.** loop.c inserts
everything it makes in front of the loop start note, in this order:

1. Hoisted invariants (`move_movables`), such as a constant the loop uses.
2. Then strength reduction's inits: the pointer that replaces `a[i]`, and
   the count-down register when the loop counter is only used to count.

So `for (i = 0; i < 8; i++) rec[i].order = -1;` compiles to `movs r1, #0xff`
(hoisted), `adds r2, r4, #0` (pointer), `movs r5, #7` (count-down from 7),
then a loop with `subs r5, #1; cmp r5, #0; bge`. A hand-written pointer loop
(`p = rec; i = 7; do { ... } while (--i >= 0);`) puts its inits before the
loop start, so the hoisted constant ends up *after* them.

* Symptom: the ROM materializes a constant before a pointer and counter init,
  and your code does it after (or the reverse).
* Fix: write an indexed `for` loop and let loop.c make the pointer and the
  count-down.
* Example: Modifier_ApplyToStat. The farm and two agents failed on it; it
  matched on the first compile once this rule was applied.

Whether an invariant is hoisted at all is `threshold * savings * lifetime >=
insns in loop` (`tools/rtlprobe.py loop` prints each movable's numbers).

## Two-operand instructions: copies before `ands`, `orrs`, `adds`

Thumb ALU instructions write one of their inputs (`ands rD, rS`). When the
result can't reuse an input's register, the compiler copies first
(`adds rD, rX, #0; ands rD, rS`).

* regmove.c (forward pass): it gives the result the register of an input
  when all of these hold.
  * The input dies in this instruction.
  * The result dies before the next branch or label.
  * The result is not used in between.
  * The input is not a constant set in another basic block
    (`reg_is_remote_constant_p`).
* regmove.c (backward pass): it makes the input's producer write straight
  into the result.
* local-alloc.c `combine_regs`: it ties the two when both are local to one
  basic block and the input dies.

The ROM's copy (`adds r1, r3, #0; ands r1, r0`) means none of the three tied
here. `why.py` checks this with a probe: `AGBCC_NOTIE=uid` turns all three
off at one instruction, and if that gives the ROM's shape, this is the
decision.

* Levers: the input lives on past this instruction; the result lives past the
  following branch; or the values are used in more than one basic block,
  which leaves them to global-alloc, and global-alloc does not tie.
* The operand the ROM copies is operand 1 of the RTL `(and A B)`. The trail
  in `why.py` shows which value is operand 1 after combine.
* Open example: EffectPart_UpdateAnimation and its three siblings. The probe
  confirms the decision, but no source spelling has been found that blocks
  all three ties.

## How to add a rule

1. Find the decision: the trail, a probe, or `tools/rtlprobe.py dump` and grep
   of the insn number across the pass dumps.
2. Read the condition in the pass's source.
3. Find the C that makes the condition true, and verify it with
   `tools/check.py`.
4. Write the rule: symptom, decision, fix, example.
