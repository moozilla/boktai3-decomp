# Cloud task 08122DF4

* `08122DF4` (`SequencedSprite_StepAnimation`) and `08126170` (`DriftingSprite_Update`) both inline the
  display-sequence Step that `SceneParticleBurst_UpdateSlot` and `include/route_sprite_playback.h` also use.
  Start any `ldrh [rS, #8]; lsls #1; ldr [..]; ... ands #0x30; lsrs #4` function from those.
  Two details mattered:
  * the sprite pointer is a separate variable from the object pointer (`adds r5, r7, #0`);
  * the first `commands` read goes through the object (`[r5, #0x48]`), not the state pointer. Copy the inline
    and give it the extra pointer.
* **Technique: one pointer variable reused for several fields** (08126170). The `delay` and `riseSteps`
  pointers were global pseudos that got r1, while the ROM has r2. regsolve reported "class or call-crossing; no reordering
  reaches it". Writing `counter = &s->hidden; ... counter = &s->delay; ... counter = (u16 *)&s->riseSteps;`
  (one variable for all three) makes them one pseudo, which then takes r2 from the first use.
  The first use then needed `asm("" ::: "memory")` after the flag store, because the ROM reloads the counter there.
* `08125528` is a NONMATCHING draft. Instruction count and structure match; only allocation differs. Two hints for whoever resumes it:
  the original reaches the position through separate field pointers (x through `position`, y and z through their own
  pointers kept in r6/sb), and the CellHeight inline's position parameter is a separate copy (`adds r5, r7, #0`).
* `08123194` (8876 bytes, 4145 insns with its tails 08124240/08125138) is in progress in a NONMATCHING
  draft. Structural differences (`tools/anchordiff.py -s`) went 2875 -> 231 with the changes below; what is
  left is mostly allocation repeated across 16 inlined Seek loops.

## Techniques found on 08123194 (apply to any big function)

* **Score big functions with `tools/anchordiff.py`, not check.py's byte count.** check.py pairs lines by
  position, so one inserted instruction makes the rest of an 8 KB listing "differ", and difflib on the whole
  thing aligns one inlined copy against another. anchordiff cuts both sides at the calls they share and
  aligns each piece; `-s` masks registers so only instruction choice and order count.
* **Block order reveals the `if` polarity.** The ROM laid out the `gUnk_030053F4 & 0x800` path first. Ours
  was written `if (!(flag & 0x800)) {A} else {B}`; swapping to `if (flag & 0x800) {B} else {A}` moved 1648
  instructions back into place. When a large chunk is "deleted" in one place and "inserted" later, check
  which branch the ROM falls into first.
* **A mask that survives in the ROM but folds in yours means the constant was hoisted.** The ROM keeps
  `lsrs r2, r1, #4; ands r2, r6` (with r6 = 15 set before the loop). combine folds `(x >> 4) & 15` on a
  byte away, *unless* loop.c has already moved the 15 into a register. loop.c (move_movables) moves
  a constant only if `threshold * savings * lifetime >= insns_in_loop`. Here threshold =
  `(loop_has_call ? 1 : 2) * (1 + n_non_fixed_regs)` = 13 on Thumb, minus 3 for every movable already moved
  in that loop. savings counts the matching sets of the same constant, and lifetime is their summed luid
  span. Read the decision with `agbcc -dL` (`Insn N: regno R (life L), move-insn savings S
  moved to/not desirable`). The loop pass runs twice, and the second run often moves what the first
  rejected. Declaring the source byte `u8 flags` (narrowed arithmetic makes separate QI and SI 15s, so
  savings rises) got the 15 hoisted.
* **`movs rX, #imm` repeated at every use inside a loop is a hoisted constant that lost allocation.** reload
  rematerialises a spilled pseudo that has a REG_EQUIV constant, so the ROM shows `movs r3, #0xf; ands r2, r3;
  movs r0, #0xf; ands r0, r1` in one loop and `ands r2, r6` in the next. Same C, different allocation.
* **`adds rX, rH, #0; ands rX, rY` (copy of a hoisted register, then op) comes from regmove's backward
  pass** (`copy_src_to_dest`), not from C. That happens when neither operand can be tied to the
  destination: op1 is the loop-invariant register, and op2 does not die or is not tieable. If yours ties
  (`ands rY, rH`), the destination is a local temp that regmove's forward pass (fixup_match_1) could
  merge with the dying source. Making the result a variable that is set twice
  (`h = flags & 15; h <<= 8;`) reproduces the copy. The ROM additionally shifts into a different register,
  which is still open.
* **Inline argument order: sums are materialised last.** expand_inline_function evaluates each argument
  in order. An `&s->field` argument comes back as an unexpanded `(plus s k)` (EXPAND_SUM), and its add
  is only emitted in the later parameter-copy loop. So `CanEnter(&s->position, s->cell, next, angle)` emits
  ldrh(cell), then add 0x74, then the quadrant inside the body. Assigning `cellp`/`position`/`quadrant` after
  the call turns them into CSE copies (`mov sb, r2; adds r7, r6, #0; mov r8, r1`), which is exactly the ROM
  prologue. Passing the raw angle and computing `((angle + 0x20) >> 6) & 3` inside the inline put that
  computation after the position add, as in the ROM.
* **A vector difference stored through a pointer** (`add r2, sp, #4; strh [r2]; strh [r2, #2]; ...; ldrsh
  [r2, #0]`) is an inline `Delta(&delta, a, b)` writing `out->x = a->x - b->x`, not field assignments.
* **A direction computed before an unrelated `if`** (`asrs r4, r0, #6; ands` ahead of the player check)
  is a variable declared and initialised at the top of the block, not the `switch` expression.
