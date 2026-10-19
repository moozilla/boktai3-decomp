# Defeat-statistics handlers: how sub_08132D2C (5740 bytes) was matched

Tags: #matching (cloud task 08132748, 2026-10-19)

`src/fn/sub_08132D2C.c` (`SceneActorReaction_DefeatRecordStats`) matches. It is about 2400 instructions, counting the tails that `notes/far-branch-tails.md` merges back in. Close siblings use the same template:
* `08134398` (in progress on this branch);
* `080BD1F0` (with tail `080BEEE8`);
* `080BFCC4` and `080BF0E4` (the 0x592/0x270F tails);
* probably the other owners in the far-branch list.
**Start any of them from this file.**

## Shape
* Zero health: control kind 17 to the defeat target, then the state reset.
* Link mode (`gUnk_030053F4 & 0x4800`): `Player_NotifyFirstSelectedSlot`, then `goto done`.
* Otherwise, if the owner bit is set (`field2E0 & 1`), not `flags2C0 & 0x80`, and category is not 5/6/7:
  * `STAT_INC(0x87C)` and `STAT_INC(0x538)`;
  * a 25-way `switch` on `category - 1`. Each case is `flags = collider+0x38`, `STAT_INC(per-category counter)`, then `KILL_KIND(flags)`;
  * then `sub_08159164` and `Player_RefreshLifeState`.
* Finally `RecordPair` unless a mask is set, then `colliderA.flags |= 4`.
* The case bodies are identical, so agbcc cross-jumps them. Write each case out in full and order the cases by ROM address, not by value.

## Counters (both from the matched `CannonTurret_HandleDefeat`)
* `STAT_INC(n)`: `H = H < 9999 ? H + 1 : 9999` gives `ldrh; ldrsh; cmp 0x270E; bgt`.
* `STAT_ADD(n)`: `H = clamp(H + 1)` gives `ldrsh; adds 1; cmp 0x270F; ble/bge 0`.

## The non-obvious fixes (each one checked against the ROM)
1. **A `return 1` that is a far `bl` to a shared block.** Write `goto done;` to the single final `return 1`. A local `return 1` duplicates `movs r0, #1`.
2. **Health test.** Write `if (health <= 0) { ...whole body... } return (u8)Callback_InvokeActor(...)`, so the callback path is laid out last.
3. **`movs rZ, #0` before the previous `strh`.** Use `u16 value = mask & *p; u32 zero = 0; *p = value; next = zero;`.
4. **A pointer stored to a high register after its first use** (`mov sb, r1` after `ldrh`). Use an inline that tests and writes the pointer out: `Test2(&flags2C0, actor, mask)`. With the plain form, the `mov sb` comes first.
5. **`adds r2, #0x30` then `ldr [r2, #4]` with no folding.** Access through a struct pointer: `struct { u32 a; u32 flags; } *link = (void *)(group + 0x30); link->flags |= one;`.
6. **A pointer computed again on both edges** (`adds r6, r7, #0x134` before two branches). This is GCSE/PRE. Assign `collider` inside each condition block, using gotos to a `record:` label.
7. **Copy after the switch index.** The collider copy comes after `subs r0, #1`: use `u32 index = *category - 1; collider = hit; switch (index)` with 0-based case labels. The temp gets r3 through a pinned offset: `register u32 offset asm("r4") = 0x134; asm("" : "+r"(offset)); hit = (u8 *)actor + offset;`.
8. **Case flags in r4.** Use `register u32 flags asm("r4")`. The last test is destructive: write `else if ((flags) &= 0x800)`.

Only the register pins in 7 and 8 are concessions. `tools/depin.py` kept one of the five.
