# Ratatosk_UpdateKnockbackRecovery (081F5290): exact match

Tags: #matching #ratatosk #compiler #cse #combine #basic-blocks

**Exact match in ordinary C.** `Ratatosk_UpdateKnockbackRecovery`, 800 bytes,
now compiles byte-identically (`tools/check.py src/fn/sub_081F5290.c`: MATCH, 0
differing bytes). It has no register pins, barriers, inline assembly or
compiler overrides. Commit `4fd4db14b` on `codex/opus-ratatosk-defeat`. The
starting point was GPT-6 Pro's round 8 candidate (83.5%) in
`build/handoff/gpt6-round8-ratatosk/`. About 60 variants were compiled, kept in
`build/scope/drafts/kb/` (worktree scratch, not committed).

## The deciding source property

The particle spawn converts its two random samples to 20.12 fixed point with
**sign-symmetric, in-place if/else statements**:

```c
speed = randomA() & 7;
variant = randomA() & 1;      /* s32 */
angle = (u8)randomA();        /* u8 */
drift = randomA() & 15;       /* s32 */
position = &spawn;
spawn = p->position;
position->x -= 128;
position->y += 128;
if (variant < 0)
    variant = -((-variant) << 12);
else
    variant <<= 12;
if (drift < 0)
    drift = -((-drift) << 12);
else
    drift <<= 12;
Ratatosk_InitAnimatedParticleParams(&params);
Ratatosk_ConfigureAnimatedParticle(&params, (randomA() & 31) + 52, speed, angle, 1,
                                   variant, drift);
sub_081FC26C(&params, &spawn);
```

The ROM shows only `lsls r6, r6, #0xc; lsls r4, r4, #0xc`, so the tests look
absent. They are not: they are folded late, after they have shaped the
allocation.

## Mechanism, pass by pass

1. **cse cannot fold `v < 0`.** cse's `simplify_relational_operation` uses no
   `nonzero_bits` for signed LT/GE (it only knows nonzero base addresses for
   EQ/NE and the unsigned edge cases), so `cmp; blt` survives cse1 and cse2.
   Each if/else has a join label reached by a jump and by fall-through, which
   ends every cse path. The code after the conversions (the three calls and the
   final `flags |= 1`) is therefore a separate extended basic block in both cse
   passes, and flow and local-alloc see separate basic blocks.
2. **What the split produces** (all visible in the ROM):
   * the fifth random read reloads the cursor address, the 1023 mask and the
     table address (`ldr r2, =gEffectRandomTableIndex` ... at 0x2C8) instead of
     reusing the registers of the first four reads;
   * the Configure argument `1` and the final `|= 1` constant are fresh
     `movs`, so only `tick & 1` and `variant & 1` share the constant, in `ip`;
   * gcse PRE finds the later `&spawn` (the last call) and `&p->flags` (the
     final `|= 1`) redundant across the new blocks. It gives each a reaching
     register and inserts a copy at the end of an earlier block, which cse2
     turns into a register copy (gcse dump: `PRE/HOIST: end of bb 33, insn
     1098, copying expression 127 to reg 511`, and `end of bb 27, insn 1097`
     for the flags). These are the ROM's `adds r5, r2, #0` at 0x2BC and
     `mov sb, r1` at 0x1BC. The call must be spelled `sub_081FC26C(&params,
     &spawn)`; passing `position` gives 38/40 and fails. The flags copy is
     the same PRE copy as in `sub_08026FF0`.
3. **combine folds the tests.** combine has `nonzero_bits`: the samples are
   masked u16 values, so `v < 0` is false. The comparison becomes constant and
   jump2 deletes the dead arms and jumps, leaving the two `lsls`. The block
   boundaries had already done their work in cse, flow and local-alloc.

## Other spellings the match needs

Found while cleaning the candidate up, each checked by a one-change variant:

* `u32 mask = 12; p->flags |= mask;` and `{ u32 bit = 1; p->flags |= bit; }`.
  Plain `p->flags |= 12` (7 instructions differ) and `p->flags |= 1` (1
  differs) fail. These are the same idioms as `sub_08026FF0`.
* `u8 angle` for the third sample (the `r3` move comes last).
* The position pointer assigned after the four reads. Initializing
  `struct ContactPosition *position = &spawn;` at its declaration fails (33
  hunks).
* The scale decay reads the field twice, unsigned then signed, and copies the
  pointer after both reads; the copy is kept for the later zeroing:
  ```c
  u16 *value = &p2->scaleX;
  u16 old = *value;
  s32 current = (s16)*value;
  scaleX = value;
  if (current > 23)
      *scaleX = old - 24;
  else
      *scaleX = 0;
  ```
  `s16 current` fails. A `decay(u16 *)` helper fails: its parameter copy
  changes the allocation of the whole function. Without the pointer variables
  the zeroing recomputes `p2 + 0xE52`, because 0xE52 is no valid add immediate
  and gcse cannot reuse the address.
* `p2->phaseTimer = 8; p2->motion = 0;` and the plain zeros match. GPT-6's
  explicit `zero` variable is not needed, and with it 3 instructions differ.
* Facing through the shared `RATATOSK_SET_FACING` (include/ratatosk_facing.h),
  as in the sibling `sub_081F5120`. The inline `facing()` helper of
  `sub_081F4CF8` fails here (3 hunks). cse reuses the QImode zero of the byte
  store's read-modify-write expansion as the call's `0` argument. Why the
  macro's `do { } while (0)` avoids that was not traced.
* `polar(&body->motion, ...)` with the arguments written directly. Copying the
  angle into an arm-local variable first fails. `body` is `p + 0x48`, computed
  at entry as in the ROM.

## Ruled out

* An if/else with identical arms: jump1 merges it before cse.
* Single-arm ifs: cse's skip-blocks path continues across them.
* The `?:` form of the conversion: the result goes to a temporary, so the
  shifts are not in place.
* Caller-save or rematerialization of the cursor constants: they are fresh
  pseudos in a new cse block, not reloads.
* gcse PRE linking the fifth random read's cursor, mask and table to the first
  four: the gcse dump lists PRE only for the two copies above and the sine
  table in the knockback arms.
