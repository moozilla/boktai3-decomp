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

### Lun_HandlePlayerContact (src/fn/sub_0809AE0C.c, 0809AE0C): deferred

The landed version uses two do/while(0) statement macros (LUN_FINISH_SPEED_CONTACT is used once).
The ROM keeps two dead reads of `context->flags3D4 & phase` (`ldr; ldr; ands r0, r7` with the
result unused) before each Lun_ApplyContactFlag, and Apply's mask is a fresh `movs r2, #2`.
Drafts: `Lun_HandlePlayerContact.c` (no macros, `if (Lun_ReadContextFlags(actor, phase))
DEBUG_PRINT();`, 29/25 differ) and `.macro.c` (only the two-use read macro, 4/4: the palette
pointer copy `adds r3, r0, #0`). Without the macros, cse2 replaces Apply's mask (2) with phase,
which is still known to be the constant 2 (`why 562`). phase's reassignments by the reads are gone
before cse2, and phase's set sinks to its first use. In the macro version the do/while keeps the
reassignment alive until flow. Tried: `phase = Read(...)` plain or inside the `if`, a separate
`contact` variable, u32 phase, a u32 mask parameter for Read, Apply with `|= 2`, and u8/u16/s8 masks
(those go to the stack). Next: a form where phase really changes (the read's result reused) or
some other reason the ROM's mask is a separate constant.

### Spider_UpdateWanderGoal (src/fn/sub_080A4018.c, 080A4018): deferred

Draft: `Spider_UpdateWanderGoal.c` (the no-pin draft, 58/62 differ). The ROM's allocation is input r4,
transform r5, table r6, heading r7, ctx r8, mask (0x3FF) sb, &gRandomIndex sl, with distance and
&input->0x3D4 spilled to the stack. sb's 0x3FF comes from reload's move2add (`adds r1, #0x2f` on the
r1 that held 0x3D0), so mask has to get sb. Two constant pointers have to be in registers with no
REG_EQUIV, as in Bat_UpdateDefeat. `AGBCC_NOEQUIV=31,32` (table, indexAddress) gives 58/58 with
shape 16, and the rest is the order of input, transform, table and heading. The halves union cannot
help here either: the target packing reuses 0xFFFF0000. Next: a general way to set a constant pointer
without a REG_EQUAL note at function entry; this is the same blocker as Bat.

### TentacleActor_SelectWanderRoute (src/fn/sub_080A8904.c, 080A8904): deferred

Draft: `TentacleActor_SelectWanderRoute.c`, every do/while(0) macro written out (46/48 differ, all
allocation). The macros put their statements one loop level deeper, and flow weights REG_N_REFS by
loop depth. So with them the scan mask constant has 21 refs (14 without), i 13 (11), n 9 (7) and
currentCell 10 (8). That reorders global allocation: the ROM has facing r8, count sb, the 3 mask r6.
The draft gives facing r6 and count r8. Next: C that really nests those statements (a loop the
original had, for example the neighbor scan as an inner loop) or other refs that raise the mask
and n by the same amount.

### TentacleBody_RefreshDirectionalAnimations (src/fn/sub_080A8F38.c, 080A8F38): deferred

Draft: `TentacleBody_RefreshDirectionalAnimations.c` (41/37 differ, against 83/79 for the plain
no-pin draft). Step is written as the shared DisplaySequenceState playback, with the flip tests
from SpriteEffectController_StepDrawable and ordinary `frame.profile[frame.kind * four]` /
`++frame.i` in the body. Remaining: loop.c hoists the constant 1, because three equal sets
(TestMask's `& 1`, the first flip bit and `duration = 1`) are combined in combine_movables;
`AGBCC_NOMOVE=99` keeps it in the loop. The register order is the other part: the ROM puts body in
sb, offset in sl and the segment pointer in r8 (seven saved registers); the draft uses r8/sb/r7.
Next: find what holds r7 in the ROM (another long-lived value the draft lacks, maybe a kept
`&body->speed` or a `four` constant), then split the 1s.
