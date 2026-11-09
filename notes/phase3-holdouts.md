# Phase 3 holdouts

One section per worker. Each entry: the best pin-free draft, what blocks it, the knob that
matches (when one does) and the next idea.

## field-effects

### Scene_SpawnDirectionalDust (src/scene_object.c, 08004B64): deferred

Draft: `notes/phase3-drafts/field-effects/Scene_SpawnDirectionalDust.c` (117 ROM / 93 draft
insns differ; it relies on a local `u32 x:16` bitfield struct defined next to it in the unit copy).
The ROM writes the particle's x and z halves as SImode read-modify-write (`ldr; and #0xFFFF0000;
orr; str`), z through the particle pointer in r5, not through the stack. Probes:
* a struct of u16 fields gives `strh`;
* a `u32 x:16` bitfield struct gives the SImode RMW only while the struct is ADDRESSOF (its address
  is never stored in a pointer variable); once `q = &pt` exists the store becomes `strh`;
* `pt.x = pt.x op ...` written directly also narrows to `strh`; `t = ...; pt.x = t;` keeps the RMW.
The pinned version gets z's RMW from a union of words plus FIRST/LAST macros. Next idea: find
the struct the particle really has at that offset (a packed `u32` position word written whole by
its siblings) and recover it in `include/`.

### Scene_TryStepDown (src/scene_object.c, 08004CE4): deferred

Draft: `notes/phase3-drafts/field-effects/Scene_TryStepDown.c` (42 ROM / 40 draft insns differ,
allocation only). Fixed so far: parameters used directly (no `actor = input` copy) and the
sibling-style `neighbor = index + delta; dynamic = TerrainModifiers_FindCellOverride(neighbor, 1);
savedIndex = neighbor;` put actor in r5 and index in r4. Remaining: the ROM gives mask (0xF0) r4,
cell r6, type r7, direction r8; the draft gives mask r8 (pri 1875, 6 refs / 64), cell r4 (3243).
`AGBCC_PRI=mask:+1400` cuts it to 17/19 but does not match: the low-register choice in the
`heightByte`/`lowMask` block (ROM ldrb r1, movs r0 #0xf; draft r0/r1) and a `regmove` copy at
line 773 remain. The draft still has statement expressions and plain `register` locals from the
pinned form; strip them once the shape is right. The ROM's dead `subs r0, r7, #1` tail comes from
`type == 1 || type == 2` with both arms returning 0 (fact 57-style cross-jump).

Tags: #matching #phase3 #holdouts

One section per worker. Each row: the function, its best pin-free draft (a snippet to
`scope.py FILE.c put`), the differing instructions, the mechanism and the knob that matches,
what was tried, and the next idea.

### Trigger_RunScript (src/item_container.c, 08006F00): deferred

Draft: `notes/phase3-drafts/field-effects/Trigger_RunScript.c` (11 ROM / 9 draft, two mechanisms).
1. The ROM keeps two copies of the actor index: `ldrsb r1` (compared with -1), `adds r2, r1` (the
   table index) and `adds r5, r2` (saved for values[8]). cse merges the draft's `index = actor` into
   actor (make_regs_eqv: index does not live past the extended block, which runs to cuid 73 with
   jump following), so the draft has one register. A copy survives only if the new register
   becomes the class head (its last use past the block end, or its first mention before the block
   start), which none of the spellings tried gives: s8/u8/s16 actor, an inline `PlayerMask`, a
   nested `if (actor != -1)` form, separate `index` and `savedActor` blocks. `CSE_KEEP`/
   `CSE_NOCANON` on the pseudos change nothing (the head test fails before them).
2. `&args` must be a pointer variable set before `source = p->arguments` and allocated before
   `out`, so it takes r2. With 3 refs over 22 insns its priority (1363) is below out's (3000):
   `AGBCC_PRI=argsPtr:+2000` fixes it. Routing the count's word RMW through the pointer gives it 5
   refs and fixes the allocation, but the ROM does that RMW through sp (`ldr r0, [sp, #0x28]`).
Next idea: find refs for argsPtr that cse later folds back to sp, or the label/loop structure
that ends cse's block between the actor copy and its uses.

### DirectionalController_CheckRemaining (src/directional_interaction.c, 080098CC): deferred

Draft: `notes/phase3-drafts/field-effects/DirectionalController_CheckRemaining.c` (41 ROM / 43
draft). In the ROM, Math_Atan2's result stays in hard r0 from the call to the `bne` that jumps
straight to the epilogue, and every local temp in between (the u16 copy, the field loads, the
facing tests) uses r1-r5. local-alloc avoids r0 only when hard r0 is live across those blocks,
which the C gets only from the pin (`register s32 angle asm("r0")`): a pseudo for angle crosses
several blocks, so it is global, and local-alloc has already put temps in r0 by the time global
runs. `AGBCC_FORCE=angle:0` does not help (the temps clobber it). The ROM's `adds r1, r0` then
in-place `lsls r1, r1, #16` also says the narrowing reads a copy that combine left alone, which
is what a copy straight from the hard return register gives. Spellings tried: plain
`(u16)angle`, a `u16 heading` local, an assignment inside the expression. Next idea: a form in
which the RTL keeps the call's hard r0 as the returned value (no pseudo between the call and the
return), for example the call's result returned through an inline whose result the caller
returns, or a matched relative with the same "return the angle when facing" shape.

### EffectManager_UpdateAll (src/homing_effect.c, 0800A8E4): deferred, one insn from a match

Draft: `notes/phase3-drafts/field-effects/EffectManager_UpdateAll.c` (1 ROM / 1 draft: one
instruction in another place). Diagnosis: the 0x148 stride for `view` and `p` is not a register
in the ROM. `AGBCC_NOREG=<the stride pseudo>` on the depin draft (two explicit `view++; p++;`)
gives an exact match: reload loads 0x148 for the first add, the second add inherits it, and the
reload rotation then gives the ROM's r2 for -1 and r0 for i. In plain C, cse1 merges the two
`(set (reg) (const_int 328))` that the addsi3 expander makes. The constant stays two
single-use pseudos (which combine folds back into each add, so reload loads it) only when one
increment is loop.c's: `view = p;` at the top of the body makes view a giv of p, and its reduced
increment lands before `p++` as in the ROM. What is left is the giv's initialization: loop.c
emits it just before NOTE_INSN_LOOP_BEG, after `i = 5`, and the ROM has `mov r7, r8` before
`movs r2, #5`. A reversed up-counter (`for (i = 0; i < 6; i++)`) does not help: check_dbra_loop
reverses it in pass 1 before the giv is reduced, so its `i = 5` comes first again. Making view a
giv of i (`view = &manager->effects[i]`) delays the reversal to pass 2 and fixes the init order,
but then the giv's increment lands before i's increment, after `p++`. Next idea: a counter that
check_dbra_loop can only reverse in pass 2 while view stays a giv of p.

## menus-link (Menus, HUD and dialogue; Link and wireless)

### StatusScreen_DrawClock (0800E5B8, src/status_screen.c)

Draft: `notes/phase3-menus-link/StatusScreen_DrawClock.c` (4 ROM / 4 draft insns).
* The pinned match keeps the digit-array address in a user variable `values` (pinned to ip).
  Without the pin it gets sb: two sets of `values = digits` each get a REG_EQUIV (the frame
  pointer is function-invariant), and update_equiv_regs (local-alloc.c) doubles the live length
  once per set, 48 -> 192, so its priority (625) falls below the hoisted `gBgRecords` constants
  (833), which take sb first; then pass 0 of find_reg picks sb over ip. Needs priority > 833
  (`AGBCC_PRI=values:+600`). The sibling NumericDisplay_DrawPair gets the same refs from an
  inline `do { row = 0; values = digits; } while (0);`, which the concession detector does not
  count (see the note to the coordinator).
* Indexing the digits (`int tile = digits[col] + 0x60;`) instead: loop.c's hoisted giv base
  lands in ip as in the ROM, and only the inner loop's shift and digit pointer swap r3/r6
  (shift 5 refs over 14 insns against the pointer's 5 over 13; `AGBCC_PRI=shift#2:+551`
  fixes the first loop). Tried: shift inline (hoisted by loop.c: right registers, but the
  `out = dst` copy then comes before it), `dst[col]` (giv order flips), `shift = row * 16`,
  the shift in the for init, operand orders of the add.
* Next: a form where shift lives two insns less (or the pointer two more) without moving the
  `out = dst` copy; or what made the original's `values` denser.

## actors

### SceneObjects_ScriptLower (src/fn/sub_0801E998.c, 0801E998): deferred

Draft: `notes/phase3-drafts/actors/SceneObjects_ScriptLower.c` (3 ROM / 3 draft insns differ,
allocation only; a whole-file unit copy). Shape: a do-while inside `if (found < count)`, with
`slots`, `int zero = 0` and `int one = 1` set inside the if, and the callback pointer
`fn = SceneObject_UpdateLowered` at the top. The ROM keeps the constant 1 (variant store) in sb;
the draft gives sb the timer's HImode insertion-mask zero (facts 61/63; priority 306), which the
`b3a = 0` byte store then reuses through the wider-mode lookup. `one` scores 300 because the
hoisted `=0` set lengthens its life by one insn; the pinned original won on a tie (283 vs 283).
Tried without effect or worse: u8/u16 `one`/`zero`, a `u16 *` for the timer store, plain `0` for
timer and lift, swapped stores, `lift = timer`, chained `timer = lift = zero`, and more refs to
`one` (`found = one`, `b3b + one`) or `zero` (`b3a = zero`). Next idea: keep the timer's HI zero
out of the loop preheader (a different spelling of the timer member, or the slot struct the
siblings ScriptRaise/Append use, written as one HI store), or find the source of the tie.

## monsters-bosses (Monsters, Bosses, Player, Engine core)

### SceneGrid_ScanColumns (src/fn/sub_0806E7CC.c, 0806E7CC): deferred

Draft: `notes/phase3-drafts/monsters-bosses/SceneGrid_ScanColumns.c`, the matched ScanRows form
(`[row][column +/- count]`, `column <= 7`, returns `2 - cell` / `-(cell + 2)`). 14/14 insns differ,
only an r6/r7 swap of steps and row. `AGBCC_PRI=steps:+4` matches. steps is the stack parameter:
assign_parms gives it REG_EQUIV (mem), so update_equiv_regs doubles its live length (1411 against
row 1414). Undoubled it would outrank kind (2823 > 2523); the ROM wants row < steps < kind.
Tried: `!steps`, a limit variable, gridRow per arm, 1D indexing, parameter types, local copies
(a stepsInput copy gives 19 diffs and steps takes r3), a while loop (worse). Next: give steps
four or five more insns of life without changing the code (fact 17's `s16` conversion trick), or
reorder the parameter's first use.

### sub_0806F06C (src/fn/sub_0806F06C.c): deferred

Draft without the DEBUG_NOP: `notes/phase3-drafts/monsters-bosses/sub_0806F06C.c`, 23/23 differ.
`AGBCC_PRI=i:+920` matches. cse puts REG_EQUAL (const 0) on i's first set, which doubles its life.
In the landed version, the DEBUG_NOP's loop notes make jump.c's duplicate_loop_exit_test copy the
outer `++i <= 19` (a LOOP_BEG note followed by a simple jump), giving i 13 refs. Tried for
loops, `u32 i`, box-test restructurings: no change. Next: find another statement that leaves a
LOOP_BEG + jump shape (an inner loop that really exists in the original, for example), or give
i a non-constant first set.

### Bat_UpdateDefeat (src/fn/sub_08088CD4.c, 08088CD4): deferred

Pin: `register const u16 *table asm("sl") = gRandomTable;` for the six-spark burst. Drafts:
`Bat_UpdateDefeat.no_pins.c` (96/100 differ) and `Bat_UpdateDefeat.c` (halves union, 19/27).
`AGBCC_NOEQUIV=table` (pseudo 227) on the no-pin draft gives an exact MATCH. With the REG_EQUIV,
the table's life doubles to 208 (7 refs). Local-alloc fills r4/r5/r6/r8/sb/sl with
gRandomIndex, rangeB, 2, 20, 4 and 3. global.c's takeover of a sparsely used local register (the
K/E records in AGBCC_DUMPPRI) compares refs/live: position takes r6, 2 takes sl from 3 (7/184 >
7/192), and the table at 7/208 beats no one, so it is rematerialized at each use. Undoubled (7/104)
it takes sl as in the ROM. The halves union (082335F0's idiom) does not fold here: rangeB's
bitfield writes just before it put 0xFFFF0000 and 0xFFFF in cse-shared pseudos, so combine can't
merge the half insertions into one constant. Ruled out: a copy of an earlier-set `samples`
(cse/path following keeps the note), byte-wise union assembly, insertion order, moving the
insertion before rangeB, raising or lowering the constants' priorities (AGBCC_PRI, AGBCC_NOREG).
Next: a first set without a note that ends up in sl alone. That could be the relative
SceneActorReaction_BeginDefeatBurst's union reuse, if the ROM allows one more sl use, or
another rangeB write form that keeps the masks out of cse.
