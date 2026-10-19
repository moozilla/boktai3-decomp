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

**Update (2026-10-19): no pins are needed for 8 or for the counter offsets.** Two macro
details caused them. (a) `do { } while (0)` around `KILL_KIND`/`CASE_BODY` is a loop to
`flow.c`, which adds the loop depth to every use inside, so the case flags outranked the
counter pointer. Use plain braces. (b) The saturating counter is an if/else with two stores
(`if (*p < 9999) *p = *p + 1; else *p = 9999;`): the second store gives the counter pointer
the uses it has in the original, and cross-jumping merges the stores back into one `strh`.
With both changes sub_08132D2C needs no pins. The same fix removed 18 pins each from
sub_080EE1B0 and sub_080BD1F0. (sub_08134398 is still a 90% draft: check it with
`check.py --full` on a copy without the `#else INCLUDE_ASM` branch, or the check compiles
the fallback asm and reports a false match.)

Only the register pins in 7 and 8 are concessions. `tools/depin.py` kept one of the five.

## sub_08134398 (sibling, 90% draft): the open problem is reload's spill set, not pseudos
Measured with `-dg` dumps (`build/w/dump.py`: preprocess like build.py, then `agbcc -dp -dg`, and read
`u.i.greg`). Every remaining difference is a **reload** register, so pins on pseudos can't fix it.
`rasolve.py` agrees: forcing the ROM's registers on all 96 differing pseudos makes it worse
(474 bytes).

* `reload1.c` builds one function-wide `spill_regs[]`: the sorted union of every register any instruction
  had to spill ("Spilling reg N" lines in `.greg`). `allocate_reload_reg` then rotates through it
  with a persistent `last_spill_reg`. One extra member shifts every later reload.
* The matched 2D2C spills {r0..r4}. r3 and r4 join only because of the inlined SetControl: `control`,
  `kind` and an address are live at once while the 0x2B2 and 0x2B0 offsets need reloads (insns 198 and 210).
* The 4398 ROM's reloads are in r5 and r7. Of 114 constant address offsets, 95 load into r5 and 8 into r7; the
  r1/r2 ones are probably C-level offset variables, which local-alloc places. So its spill set includes r5/r7. In
  its prologue, r3/r4 already hold two separate zero values (`movs r3, #0; movs r4, #0` before the
  `flags2D6` store), so the same SetControl pressure must spill r5 and r7 instead.
  The 0x2B2 offset is even inherited from the 0x2D6 reload (`subs r5, #0x24`).
* Still missing: C that gives two zero values that CSE doesn't merge, both born before the
  `flags2D6` store, without pins. These don't change the spill set (still {r0,r1,r2}):
  u8/u16/s32 zero types, and `asm("" : "+r"(zero))` barriers.
  Pins (asm-register variables) do produce r5/r7, but gcc then puts `actor` in r7/r8
  (unsafe; seen `adds r0, r7, r7`).
* Next idea: the ROM's very first reload (0x2D6) already uses r5. Find what makes r0..r4 unavailable
  at that insn. Diff `.greg` "Reloads for insn" between the draft and a hand-built variant.
* Also tried without effect (spill set stays {r0,r1,r2}): passing both zeros as parameters of an inline
  `Begin(actor, flags2D6, value, 0, 0)`, with or without typed locals at the call site. In the draft,
  CSE folds `zeroWord` into the `zeroByte` register and creates a separate HImode zero (`*movhi_insn`) for the `field2C8` store.
  The ROM stores r4 (SImode) into 0x2C8, 0x104 and 0x2AC, so one zero variable must survive CSE.
* sub_0813599C was matched by another worker (PR #281). The pieces that mattered: an inline `Fill(u32 *zero, void *dst)`
  with `zero` declared before the buffer, the HUD arguments loaded into locals in ROM order, and
  `asm("" : "+r"(flags2D0))` after the first clear (it forces the reload after the `flags2D6` store).
