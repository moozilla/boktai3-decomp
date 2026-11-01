# Ratatosk_InitializeFromScript (081FAE0C): exact match

Tags: #matching #ratatosk #compiler #cse #reload #defaults

**Exact match in ordinary C.** `Ratatosk_InitializeFromScript`, 2428 bytes,
compiles byte-identically (`tools/check.py src/fn/sub_081FAE0C.c`: MATCH, 0
differing bytes). It has no register pins, barriers, inline assembly or compiler
overrides. Commit `c73f7b151` on `codex/opus-ratatosk-defeat`. The starting
points were the unit's plain draft (84.4%) and GPT-6's round 8 witness (93.5%,
`build/handoff/gpt6-round8-ratatosk/`). About 400 variants were compiled, most
of them through `scope.py ... try`. They are kept in `build/scope/drafts/ae/`
(worktree scratch, not committed).

## What the ROM shows

The keyword blocks parse three to nine u16 parameters or store defaults. Every
field is at least 0xE9C bytes into the actor, so each address is `actor +
offset`, with the offset loaded by reload. The defaults look like this:

```
ldr  r3, =0xEB4      @ offset reload
adds r1, r7, r3      @ address, r1
movs r0, #0x1e       @ value, r0, after the address
strh r0, [r1]
ldr  r0, =0xEB6      @ the rotation lands on r0: the value must be loaded again
...
```

The offset reloads rotate over r0, r2 and r3, because the address holds r1.
A default value is a short pseudo in r0, set after its address and dead at its
store. When two neighbouring defaults are equal, the second load is missing
unless the next offset reload took r0. The 0x73 block (all six values 30)
loads 30 three times for this reason. A single shared pseudo would keep r0
busy and push those offset reloads to r2. Two other cases differ:

* The 0x52 block shares one 30 across the 5 between its stores, so the
  address is in r0 and the value in r2.
* The late reset shares one 0 (r1) and one 3.

The hurt collider's first two defaults (30, 15) are not loaded like the
others. reload rematerializes them into the rotation's register (r1, r3), both
in the 0x41 default and in the late reset.

## The C that produces it

```c
u32 defaultHurtX, defaultHurtY;  /* 30 and 15, set at entry */
u32 param;                       /* function scope */
u32 value;
u16 *slot;
...
if (Script_SeekToKeyword(0x41)) {
    ...
    actor->hurtDamage = Script_GetValue();
    value = Script_GetValue();
    slot = &actor->hurtKind;
} else {
    actor->hurtX = defaultHurtX;
    actor->hurtY = defaultHurtY;
    slot = &actor->hurtZ;
    param = 0x1E;
    *slot = param;
    slot = &actor->hurtAttack;
    param = 0x28;
    *slot = param;
    slot = &actor->hurtDamage;
    param = 0x1E;
    *slot = param;
    slot = &actor->hurtKind;
    value = 0x1E;
}
*slot = value;
```

* **0x41, 0x63, 0x53:** `slot = &F; param = C; *slot = param;`. The address
  comes first. `param` gets a new value at every store, so no earlier register
  holds the constant. The last parameter goes through `value` and one store
  after the join, and jump2 cross-jumps that tail. In 0x53, `value` also
  carries alternateY, the first default that has to be loaded again.
* **0x42:** three pointer stores, `slot = &F; *slot = 0x1E;` (HImode constants,
  one shared value).
* **0x73:** pointer, field, pointer, pointer, then `value` for the last two.
  14 of 256 combinations of plain, pointer and do-while forms match; this is
  the one without a do-while.
* **0x52, 0x47, 0x58, 0x74, late reset:** plain field stores.
* **Camera:** `RatatoskInit_SetCameraOrbit(camera, 140, 255)`, an inline
  helper like `VampireInit_SetCameraOrbit` in `sub_08028F78`, then
  `camera->duration = 30`. The ROM loads both values before either store.
* **Destination:** `actor->destination = *(k + actor->points);`.
  `actor->points[k]` swaps the add operands (1 instruction).

## Mechanism (agbcc cse.c and reload1.c)

1. **cse merges equal constants by register class.** When cse records
   `(set (reg N) (const_int C))`, `insert_regs` makes N equivalent to a
   register of the same mode already in C's class (`make_regs_eqv`). Later uses
   of N are then canonicalized to the older register, which stays live from the
   first store to the last. Plain field stores (f1) and fresh temporaries per
   store (an inline `SetParam` helper, p1 and p2: 117 instructions differ) all
   merge this way.
2. **Which modes collide.** The forms leave different registers in the class:
   * A field store expands through `store_bit_field` (read, mask, ior). It
     leaves an HImode constant (the ior operand) and an SImode result.
   * A pointer store leaves an HImode constant.
   * `*slot = value` with a u32 value copies the value to HImode, and cse folds
     the copy to the constant.
   * An HImode constant also finds an SImode register through the wider-mode
     lookup (`src_related`), but not the reverse.

   In the k_ sweep (64 combinations for the 0x41 default), the HImode copy left
   by `*slot = value` merged with a later pointer store of the same value.
3. **Reassigning one variable avoids the merge.** A new value for `param`
   removes it from the old constant's class. A reassignment to the value it
   already holds is deleted in cse1 (f8: the `value = 0x1E` before the join was
   deleted). The store that has to load its value again therefore needs another
   variable: `value`, which lives past the cse block.
4. **The 0x63 zero needs `param` at function scope.** On the branch to the
   default, cse knows the Seek result is 0, so a zero becomes that register
   (`src_related (subreg:HI (reg 314))`). The Seek result then needs a copy
   (`adds r2, r0, #0`). With `param` at function scope, its first use lies
   before the cse block. `make_regs_eqv` then makes it the canonical register,
   and the store keeps `param`. A block-local temporary loses: 4 instructions
   differ (o1 against o2).
5. **Long-lived defaults are rematerialized.** `defaultHurtX` and
   `defaultHurtY` live from entry to the late reset and get no hard register.
   Their REG_EQUIV constants are loaded at each use into the register the
   rotation picks (`allocate_reload_reg`). `chain->used_spill_regs` is every
   spill register that no pseudo holds at that insn (`finish_spills`, "Mark any
   unallocated hard regs as available for spills").

## Ruled out

* Plain field stores everywhere (the unit's draft, 84.4%; f1, 56/52).
* `do { } while (0)` around each store (d1 to d3, 34 or 35 differ). cse1 stops
  at a loop-end note, but cse2 runs after loop and merges anyway.
* A u16 temporary: it is promoted to SImode and behaves like the u32
  (k_HHP = k_VVP).
* `param` in the 0x42 or 0x73 default (q3, q5, r2 to r4: 66 to 79 differ). It
  changes `param`'s global allocation, which moves it out of r0 everywhere.
* Plain constant stores for the camera (cam1, 2 differ). A three-argument
  orbit helper that also takes the duration fails as well (cam5, 2 differ).
* Not needed after all: the witness's `CameraOrbit` helper, the `do`/`while`
  wrappers, the `*(u16 *)(void *)&` casts, `found` for 0x74, `hpB` as the 0x65
  zero, the `work`/`randomIndex` locals, and the per-block braces with their
  local `slot` and `value`.

## Scope gaps

* `why` reports "cse pass 2 replaced 334 by 312" but not the reason. It could
  name the class merge at the set (`insert_regs` → `make_regs_eqv`), and say
  whether the new register would have become canonical (live past the block,
  or first used before it). That is the lever for both the 0x41 store and the
  0x63 zero.
* `land` refuses a draft that changes anything outside the function, such as a
  new inline helper above it, so this unit was landed by hand (draft copied,
  clang-format, check.py). It could accept added `static inline` definitions.
