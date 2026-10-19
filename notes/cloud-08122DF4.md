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
* `08123194` (8964 bytes with its tails 08124240/08125138) is a NONMATCHING draft at 99.6% (25 bytes).
  Every instruction matches. Left: stack slots of four spilled GCSE/PRE reaching registers, and six
  register choices from cse2 (see "Third round" below; they do *not* follow from the slots).
  Structural differences went 2875 -> 0 with the changes below.

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

### Second round on 08123194 (231 -> 19 differing instructions)

* **Read the byte twice instead of caching it.** `shape = (*p >> 4) & 15; height = (*p & 15) << 8;`
  produces ROM's `ldrb r1; lsrs r2, r1, #4; movs r0, #0xf; ands r0, r1`, with the `and` tied to the
  constant. A `flags` local (any type) gives `ands r0, r1` with the output tied to flags. The two loads
  are merged by CSE, but the RTL keeps the constant as the first `and` operand, so local-alloc/regmove
  tie it differently. That one inline, used *everywhere* (CanEnter and all 16 loops), took the draft
  from 231/1194 (structural/total) to 47/201. Inside loops the `& 15` is hoisted and survives; outside
  loops combine folds it. So one inline explains both shapes, and no separate slope variant is needed.
* **`x & 0xC3` on a u16 load ties the wrong way because C narrows it.** `*(u16 *)(p + 2) & 0xC3` is
  shortened to an HImode `and` and the output takes the constant's register (`ldrh r1; movs r0, #0xc3;
  ands r0, r1`). The ROM has `ldrh r0; movs r1, #0xc3; ands r0, r1`. A small accessor inline
  `static inline u16 CellAttributes(s32 i) { return *(u16 *)(Properties(i) + 2); }` (its return value is
  an SImode pseudo) or `*(u32 *)p >> 16 & 0xC3` both give the ROM form.
* **A value set before a call that only one path needs gets spilled.** `result = 2; if (!Seek(...)) goto
  stop;` kept `result` live across the inlined Seek loops, so it was spilled (frame +4, every
  `movs r4, #k` became a store). The ROM sets it after the call: `u8 found = Seek(...); result = 2; if
  (!found) ...`. Likewise `vx = s->vx; if (vx < 0) vx = -vx; vz = s->vz; ...` (abs before the next load).
* **`bne far; b other` vs `beq far; b other` for a shared check.** The ROM jumps from the 0x800 path to the
  shared `if (result < 0) goto stop;` (not to the deflect code after it), with the conditional branch
  targeting the check. An explicit `if (found) goto check; goto stop;` with a `check:` label before
  the test reproduces that. `if (found) break;` gets inverted.
* **A trailing `return 0` placed after the shared exit** comes from
  `} else if (!CanEnter(...)) { stop: s->state = 2; return 1; } return 0;`.

### PRE register numbering (what is left on 08123194)

GCSE's PRE creates one "reaching register" per partially redundant expression in `pre_delete`. It
walks the expression hash table bucket by bucket, so the new pseudos are numbered in hash order. When
several of them are spilled, reload gives stack slots in pseudo-number order (the frame grows up on
Thumb), and the slot offsets come out permuted. For `(plus (reg R) (const_int k))` the bucket is
`(13772 + R + k) % T`, and for `(plus fp (reg R))` it is `(14419 + R) % T`. T is the table size,
`(insns at GCSE / 2) | 1` (see `gcse.c: hash_expr_1`, `alloc_expr_hash_table`). Dump the table and the
`PRE: redundant insn ... reaching reg is N` lines with `agbcc -dG` (`build/w/gsum.py` in my session was a
40-line wrapper). With our numbers, the ROM's slot order needs T <= ~2365 (we have 2593, i.e. ~450
fewer insns at GCSE time) or a quite different pseudo numbering. About 180 QImode truncate+re-extend
triples come from C narrowing `u8` arithmetic (`*p >> 4`, `*p & 15`) and are removed later by combine,
so those are the likely place the original's RTL was smaller. I did not find C that removes them
without changing the code. Variable placement and declaration order only move T by at most ±16.

### Third round on 08123194 (24 -> 19 differing halfwords)

* **Diagnostic compiler.** A copy of agbcc's gcc/ with `GCSE_T` (overrides the expression hash table
  size in gcse.c `alloc_expr_hash_table`) and `CSE_DEBUG`/`CSE_NOCANON` (trace / veto cse.c
  `make_regs_eqv` making a new register canonical) separates the causes. Build: copy
  `build/tools/agbcc/gcc`, patch, `make`; run with `AGBCC=.../agbcc` (build.py honours it). Never for
  committed code.
* **Slots.** ROM order is expr 217 (0x800 path `cell+1`) @0x44, 1907 (`&found[preferred]`) @0x48,
  1178 (`cell-1`) @0x4c, 1075 (`cell+1`) @0x50. With `GCSE_T` anywhere in 2267..2347 the four slots
  match, nothing else changes. No pseudo renumbering at T near 2593 gives that order (hash:
  `(13773+a)%T`, `(14419+c)%T`, `(13771+b)%T` with a=cell(0x800 path), b=cell(else path), c=preferred;
  brute-forced), so the original had roughly 500..650 fewer insns at GCSE. Cutting only after
  `preferred` (the else-path Seeks) is pseudo-neutral; uniform cuts would need about 32%. Free cuts that keep the
  code: the CanEnter shape checks through `u32 b = *Properties(i); b >> 4` (-4 T per site). The same
  in Seek changes loop.c hoisting of the 15 (loops are 112 insns; the second 15 hoists below 90).
* **Two C fixes.** Seek needs `return 0;` at the end of the `cell < target` branch (in the
  `found = Seek(cell, cell - 1)` copy that branch is dead, and its exits then skip `found = 0`), and the
  found[] scan does `goto check` (ROM re-tests `result < 0`), not `goto deflect`.
* **The r9 choices (0xaae/ab4/aca, 0x1852/1858/187a).** In ROM, the uses of PRE's register for
  `cell+1` read Seek's `target` (r9). In ours, cse2's `make_regs_eqv` makes the reaching register
  canonical, because its last use (the dead `i = cell + 1` in the next Seek copy) is 58 insns after
  `target`'s last use (cuid 3011 vs 2953; 6052 vs 5995 for the else path). Vetoing that alone does not give the ROM:
  it makes `i` canonical instead, so the full set of equivalences differs. Still open.
