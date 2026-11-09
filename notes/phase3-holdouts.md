# Phase 3 holdouts

Tags: #matching #phase3 #holdouts

One section per worker. Each row: the function, its best pin-free draft (a snippet to
`scope.py FILE.c put`), the differing instructions, the mechanism and the knob that matches,
what was tried, and the next idea.

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

### ScanlinePresentation_UpdateNoise (src/scanline_presentation.c, 0800D874): deferred

Draft: `notes/phase3-drafts/field-effects/ScanlinePresentation_UpdateNoise.c`. It is the plain
three-loop form, with `amplitude = &p->amplitude; delta = &p->delta;` after `i = p->start`, the
ROM's order. Score 75 ROM / 71 draft. With `AGBCC_FORCE=<loop 1's zero>:12` it is 31 / 27.
Diagnosis (fact 92):
* In the ROM, p is in r4 because its preferred class is LO_REGS. In the draft it is STACK_REG
  (`AGBCC_REGCLASS_TRACE=22`: LO_REGS 26 against STACK_REG 24), so global alloc takes p from
  the all-register alternate class, and pass 0 picks ip. That knocks loop 1's zero (the ROM's
  ip) and everything after it out of place.
* The ROM's adds of p are the draft's plus one: the noise loop's `p + 726` (p->amplitude),
  hoisted into ip at 0x68. In the draft, cse2 rewrites that hoisted add into a copy of
  `amplitude`, so p loses an odd-offset vote.
* With the pointers set at the top of the function (before loop 1), cse2 doesn't merge them,
  and p is LO_REGS (43 / 45). But the ROM computes them after `i = p->start`.
* The ROM's 726 constants are single-use (`ldr r1, =0x2D6; adds r1, r1, r4`, reload's
  pattern). The draft shares one constant pseudo (`adds r7, r4, r3`).
* The pinned version's per-iteration `end = p + endOffset` also gets the ROM's in-loop
  `p + r8` and the r8 constant.

Next idea: C in which amplitude's address and the loop's `p->amplitude` don't share a cse
equivalence at the preheader (a separate EBB, or a constant cse can't merge), so the hoisted
add survives.

### sub_08010C10 (src/group_objects.c, 08010C10): deferred, reload rotation only

Draft: `notes/phase3-drafts/field-effects/sub_08010C10.c` (2 ROM / 2 draft), the pinned
source without the `d` pin and barrier. The only difference is the scratch for the first
`ldrsh` of `-p->offset.x` (ROM r2, draft r1). `scope.py FILE.c sub_08010C10 spill` (fixed in
ea1d04af5 to shift a function's first choice) matches with `AGBCC_SPILL=0:1`: the ldrsh's
constant reload is the function's first reload choice, the spill set is {r1, r2}, and the ROM's
rotation is one step on, so the original made an odd number of reload choices before line 679
(or had r1 live there, which is what the pin imitated). The draft has none before it: m sits in
r8, but `m->actor` reloads share r0. Tried with no change or worse (on top of the depin worker's
~40 spellings): an `actor` local, no `active` local, `-offset->x` and mixes, `0 - x`, s32
casts, x and z locals, `ZeroFill8(&p->offset)`, union RandomTableView, the else branch first,
and `addPosition`/`addPositionN` for the offset step.

Next idea: find C before line 679 whose RTL needs one reload choice and still emits the same
code (a copy that jump2 merges after reload, fact 59), or that keeps a value live in r1 across
the first ldrsh.

### sub_08012B34 (src/debris_particles.c, 08012B34): deferred, the dead 0x0403B714 load

Draft: `notes/phase3-drafts/field-effects/sub_08012B34.c` (20 ROM / 20 draft). The ROM loads
0x0403B714 (PARTICLE_RANDOM_SAMPLE_ORIGIN) into r3 in both arms and never uses it. The same dead
constant is pinned in sub_0804373C (`rngSetup`) and sub_08059C48 (`observed`).
* How a dead load survives in plain C: set an ordinary local to the constant and use it in a
  later basic block in an expression that combine folds through nonzero_bits, e.g.
  `v->z = Debris_Scale(...) | (setup & 3);` (0x0403B714 & 3 is 0). flow has already run, so
  the set is never deleted and still gets a register. With the use after the v->z line,
  `AGBCC_PRI=setup:+2908` matches (GPT-6 Pro's knob, now without the barrier).
* Why it doesn't get r3: global priority. setup has 3 refs over 80 insns, and each of its
  two constant sets gets REG_EQUIV, and update_equiv_regs doubles the live length per set
  (x4, local-alloc.c). That gives priority 93. angle (6 refs over 39 insns) is at 3000, so
  angle is allocated first and takes r3, the head of the Thumb REG_ALLOC_ORDER (r3, r2, r1, r0, ...).
  Beating 3000 at live 320 needs about 25 refs. Moving the use earlier shortens the life but
  changes the v->y store order (the use must emit nothing).

Next idea: the original probably had the constant inside the "first sample" random inline
(all three functions load it just before a sample read from the literal table address). Find a
form where the constant isn't a REG_EQUIV (so the live length isn't multiplied), or where angle's
priority drops below setup's.

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

### StatusScreen_FadeAndExit (0800EC0C, src/status_screen_fade.c)

Draft: `notes/phase3-menus-link/StatusScreen_FadeAndExit.c` (3 ROM / 1 draft insns). The
`int index = i; i = index;` block in main's version is not needed; only DEBUG_NOP is.
* What blocks it: `int blue = color & blueMask;` (blueMask hoisted to sl). color dies at that AND,
  so regmove's fixup_match_1 ties the output to color (`ands r1, r0`). The ROM keeps the output
  apart (`mov r0, sl; ands r0, r1; adds r1, r0, #0; muls r1, r5, r1`). fixup_match_1's forward
  scan stops at a CODE_LABEL, a JUMP_INSN or a NOTE_INSN_LOOP_BEG/END. DEBUG_NOP's empty loop sits
  between the AND and the multiply, and that is what matches now. `AGBCC_NOTIE=89` on the draft
  matches.
* The same fence, right after the last channel mask, is in StatusScreen_FadeInPalette (an inline
  do/while(0) around r, g and `b = color & 31744`) and Palette_BlendRange
  (`do { fromG = greenMask & from; } while (0);`). The original probably had one construct
  we don't know yet. Solving it would likely free all three.
* Close but not right:
  - `int b = 31744; b &= color;` (FadeOutPalette's spelling) gives the ROM's AND and multiply,
    but b is set twice, so loop.c doesn't hoist 31744 into sl (17/13).
  - `int mask = 31744, b = mask; b &= color;` hoists it, but cse propagates the copy, which
    gives the tie again (3/1).
  - `color = b * factor;` reuses color: no tie, but color then has two deaths and becomes
    global, so r1 goes to the r product (20/20 registers).
  - A clamp `if (blue < 0) blue = 0;`: combine folds it and leaves a label, so there is no tie,
    but the multiply then ties (15/15).
* Also tried: narrow types (u16/s16/u32 blue, mask or color), casts on the AND input, operand
  order, the blue statement first, `continue`/`while`/DEBUG_PRINT clamps.
* Next: a C form where cse keeps a per-iteration copy of the hoisted mask (make_regs_eqv makes
  the new register the class head when it lives past cse_basic_block_end), or the construct
  the three siblings share.

### sub_0800F4EC (0800F4EC, src/numeric_overlay.c): StatusScreen_DrawClock's twin

Draft: `notes/phase3-menus-link/sub_0800F4EC.c` (4 ROM / 4 draft insns, only `mov ip, sp` /
`mov rN, ip` as sb). Same blocker as DrawClock: the pinned `values` (ip) must be allocated before
the earlier blocks' sb pseudos, which takes priority of about 1250. Notes:
* One `int *values = digits;` per block (each set once, so it is doubled only once) gives 652
  and 600 (3 refs over 46 and 50). `AGBCC_PRI=values:+600,values#1:+600` matches.
* Pointer form with `digit = digits` per row: loop.c doesn't hoist the frame address, which
  gives `mov r6, sp` (4/2).
* Indexed `digits[col]`: the giv base is hoisted into ip as in the ROM, but its init comes after
  the `out = dst` copy, and the r3/r6 roles swap (8/8).
* Indexed `dst[col]` and `digits[col]` in one statement: a stack slot spills.
* Extra mentions of values in the row loop (end test `digit < values + 2`, `values[col]`)
  change the loop (60+ insns).
* Next: a fourth ref, or about 6 fewer live insns, for the per-block `values`; or a set that
  doesn't get a REG_EQUIV note (3/23 = 1304 without the doubling).

### Presentation_SetItemGraphic (08019ECC, src/selection_menu.c)

Draft: `notes/phase3-menus-link/Presentation_SetItemGraphic.c` (6 ROM / 6 draft insns).
* The ROM keeps the 0x121 apart from the byte it is added to: `ldr r0, =0x121; adds r2, r0, #0;
  ldrb r3, [r3]; adds r2, r2, r3`. That needs a constant set, a copy and a register add that
  survive to reload. Every pin-free form tried lets combine fold the constant into
  `(plus tile 289)`, so the ldrb comes first and reload adds the literal (`ldrb r2; ldr r0;
  adds r2, r2, r0`). The pinned version gets there with `asm("" : "+r")` on base and on
  resource.
* Tried: the constant in the call or in its own statement, `base` then `resource = base`,
  u16/s16/u32 types, a `u8 *entry` with offset, and an inline helper modeled on its sibling
  Presentation_SetActorIconFrame (`*r + 0xf0`).
* Next: what stops combine from folding a constant into an add while keeping the set and the
  copy. A `scope` knob that forbids one combine (like AGBCC_NOTIE) would show whether that alone
  is enough.

### SelectionMenu_NavigateGrid (0801ACB8, src/selection_menu.c)

Draft: `notes/phase3-menus-link/SelectionMenu_NavigateGrid.c`: main's C without the
`asm("" ::: "memory")`, 2 ROM / 0 draft insns. The ROM reloads `*held` for the horizontal
tests (`mov r0, ip; ldrh r1, [r0]`). gcse's PRE finds that load fully redundant with the
vertical tests' load and deletes it, and cse2 then merges the copy. `AGBCC_NOPRE=1` on the draft
matches, so this is the only decision left.
* gcse's expr_equiv_p compares only code, mode and operands (MEM is "e0"), so alias sets and
  the /s flag don't separate the two loads. To survive, they need a different address register
  or a different mode.
* Different mode: `u32 h = *held;` for the horizontal tests keeps the load, but h dies in the
  last and, which ties (`ands r1, r0`, 2/2). A u32 for the vertical tests unties them but
  breaks their const-copy ands (5/5).
* Different address: a fresh `u16 *pad = gPadState;` or direct `gPadState[0]` keeps the load in
  ip, but the vertical read then uses the top's symbol register r1 instead of the ip copy (2/2).
* Also tried: 128 combinations of held vs gPadState for each of the nine pad reads, re-setting
  `held` between the tests, and inline vertical/horizontal helpers.

### ResultGame_UpdatePlayerMarker (0801C6CC, src/fn/sub_0801C6CC.c)

Draft: `notes/phase3-menus-link/ResultGame_UpdatePlayerMarker.c` (11 ROM / 11 draft, only key
and counter swapping r2/r3). The draft keeps velocity set in each arm, `u8 *counter` and
`u8 *table = gUnk_0824DB64;` before the index read (that order gives the ROM's load order), and
no asm.
* key (`1 & gPadState[1]`, also the zero for the else-arm stores) must be allocated before
  counter, whose priority is 5000 (4 refs, 16 insns). key has 7 refs, but its life is
  24 -> 50. regmove ties the and to the constant 1's pseudo, which carries a REG_EQUIV, so
  update_equiv_regs doubles it (fact 20's first-set doubling), and key's priority drops to 2800.
  Without the doubling it would be about 5600. `AGBCC_PRI=key:+2201` matches.
* Tried: `x & 1` vs `1 & x`, u8/u16/s32 key, no key variable (literal zeros), `p[0xb2]`
  instead of counter in the compare and/or the increment, and `AGBCC_NOTIE=22` (r3 still).
* Next: a spelling that keeps key from inheriting the constant's REG_EQUIV set, or that adds
  2 refs to key.

### sub_0803198C (0803198C, src/fn/sub_0803198C.c): message window open

Draft: `notes/phase3-menus-link/sub_0803198C.c` (2 ROM / 2 draft: the callback constant in r0
instead of r1). It has no pin; it keeps `(void)*(volatile u8 *)&p->mode;`.
* The ROM keeps a dead `ldrb` of p->mode (0x182) before ScriptProc_Start. Only a volatile read
  survives flow, so the original had volatile there too. A `vu8 mode` field changes the earlier
  `p->mode = 0` store (it adds a load), so the cast stays local to the read.
* `ldr r1, =MessageWindow_ClearTextCompletion; str r1, [r4]`: pseudo 261 is local to block 17,
  born after the `strh` that kills r0 (0xFFFF) and r1 (the wait address). It has no copy
  suggestion, so find_free_reg takes r0. `AGBCC_FORCE=261:1` matches. Spill rules out a reload
  (the reload rotation goes r2 -> r3 across it).
* Tried: a direct `p->fnDone =` store (worse: the fnSlot hoist is what gives the ROM's `r4`),
  the store before `p->wait`, `p->wait = -1`, a callback local set before the if or in each arm
  (global, gets r6), casts and `fnSlot[0]`, a `vu8` field with a bare `p->mode;`.
* Next: find what makes r0 busy (or suggests r1) across the store in the original block, e.g. a
  value that lives from the `strh` past the store.

### TextState_ProcessTag (0803232C, src/fn/sub_0803232C.c): one register copy left

Draft: `notes/phase3-menus-link/TextState_ProcessTag.c` (3 ROM / 3 draft, from 21 / 14 once
main's 3 pins and 3 barriers came out). It has no pins and no barriers.
* What closed most of it: each arm has its own store, `Text_FindChar` and `goto done`
  (or `goto findS`) instead of shared push/findAdvance/palette labels, and every close-path arm
  has its own `p->palette = 0;`. jump2 then cross-jumps each tail into the code before a label.
  do_cross_jump puts a new label there, and jumps to a new label are never searched in
  jump_chain (`INSN_UID < max_uid`), so the ROM's `movs r0,#2; b L` pairs stay apart. Jumping to
  an existing user label (`palette:`) let them merge, because of the jump-around rule that
  lowers the minimum.
* Left: the ROM copies `name` (r4) into r6 at 0x40C and uses r6 for the GARIGARIKUN (DDB8) and
  FUTARI (DDC4) compares, then r4 again for PLAYER (DDCC). The draft uses r4 for all three.
* Tried: `p->text` vs `name` for NONE, HATENA, DDB8, DDC4 and DDCC in all 32 combinations. With
  `p->text` at DDB8, gcse PRE inserts its reaching register at the end of the NONE block (copy at
  line 158, not 0x40C); `AGBCC_NOPRE=1` gives `adds r6, r5, #0x38` at 0x40C, the right place but
  computed instead of copied. Also tried an alias local set before the hasValue checks, after
  `name`, or in a scope before DDB8; an inline helper for the two tags; `name` assigned earlier.
* Next: find what gives a fresh pseudo at 0x40C that is a copy of name, not a recomputation.
  Reload's cse would copy r4 only within one block, so look for C that keeps 0x40C in name's
  block, or for a PRE copy placed after name's set (pre_optimal there).

### TextWindow_Destroy (08033778, src/fn/sub_08033778.c)

Draft: `notes/phase3-menus-link/TextWindow_Destroy.c` (3 ROM / 5 draft, no pins or barriers).
It reads `next = w->next; nextSlot = &w->next;` (the ROM's `adds r3, r0, #0` copy), and both arms
end in `prevSlot = &w->previous;`. jump2 cross-jumps the arms' identical `prevSlot = temp` copies
into the join, which gives the ROM's `adds r2, r1, #0` there.
* Left: in the `next` arm, local-alloc ties the `w + 456` temp to the dying 456 constant (r0);
  the ROM keeps the temp in r1, so its copy matches the other arm's and merges. `AGBCC_NOTIE=87`
  matches.
* Tried: natural `w->next->previous = w->previous` arms (13-16 differences); prevSlot set first
  in each arm (9); a global prevSlot set directly in arm 1 plus a `last` copy at the join
  (6, next in r1 vs r2; `AGBCC_FORCE=next:2` gives 4); `last` copies (cprop/cse fold them).
* Next: C that makes the arm-1 temp global or keeps the constant alive past the add.

### Inline `do { ... } while (0)` fences (for phase 4)

tools/concessions.py counts empty do/while(0) loops and single-use statement macros, but not an
inline `do { ... } while (0)` with a body. Its loop notes still stop regmove's fixup_match_1 scan
(it breaks at NOTE_INSN_LOOP_BEG/END, regmove.c), so it works as a fence the way DEBUG_NOP does.
The coordinator said to leave the scoring alone for now. These are the ones in src/ on
2026-11-09, 161 blocks in 122 files (`function:line`):

* `src/actor_animation_controller.c`: Actor_ConfigureInteractionChildFromScript:4331
* `src/background_controller.c`: Background_FindMapId:385
* `src/fn/sub_080192FC.c`: ClockEvents_Update:37
* `src/fn/sub_0801F4C4.c`: SceneObject_UpdateLowering:51
* `src/fn/sub_0801F5B8.c`: SceneObject_UpdateLowered:9, SceneObject_UpdateLowered:13
* `src/fn/sub_08020A60.c`: SceneMessages_RunScript:24
* `src/fn/sub_0802141C.c`: SceneActors_FindAvailable:21, SceneActors_FindAvailable:22
* `src/fn/sub_0802FCA8.c`: Actor_SelectPathNodeAction:93
* `src/fn/sub_08035958.c`: Wireless_OnParentUniSendComplete:48
* `src/fn/sub_08035F5C.c`: Wireless_TryStartParentNiSend:56
* `src/fn/sub_080362E0.c`: Wireless_ExchangeChildNiPacket:90
* `src/fn/sub_08038838.c`: WirelessGameMenu_UpdateSelectionExchange:42
* `src/fn/sub_08038AD4.c`: WirelessGameMenu_UpdateActiveRound:103
* `src/fn/sub_0803D8A8.c`: ScenePool_ResumeAfterIdle:9
* `src/fn/sub_080427FC.c`: ContactActor_ApplyHealthContact:33
* `src/fn/sub_080478A4.c`: BackgroundFlash_UpdateAppearing:18
* `src/fn/sub_080479B0.c`: BackgroundFlash_UpdateDisappearing:10, BackgroundFlash_UpdateDisappearing:17
* `src/fn/sub_08047F68.c`: ContactBurst_OnContact:16
* `src/fn/sub_080480A4.c`: ContactBurst_Update:70
* `src/fn/sub_0805386C.c`: InteractionMarker_TickIdle:27
* `src/fn/sub_08053DF8.c`: SavedPositionMarker_TickIdle:27
* `src/fn/sub_08054DC4.c`: DisplayTrack_SetSequenceFromScript:16
* `src/fn/sub_08055C04.c`: StageIconDisplay_TickReveal:4
* `src/fn/sub_08055DD4.c`: StageIconDisplay_InitAnimations:26, StageIconDisplay_InitAnimations:31
* `src/fn/sub_08056D24.c`: ScriptUtility_SetFlag800:8, ScriptUtility_SetFlag800:9, ScriptUtility_SetFlag800:10
* `src/fn/sub_0805DDE8.c`: SceneHud_DrawCenteredMeter:18
* `src/fn/sub_0805E270.c`: SceneInput_InitGlyphPair:26
* `src/fn/sub_0805E394.c`: SceneInput_GetGridPosition:17
* `src/fn/sub_0805E7D8.c`: SceneInput_GetNavigationKeys:14
* `src/fn/sub_08060A70.c`: SceneInventory_BeginConsumableSwapPair:39
* `src/fn/sub_0806B8DC.c`: SpriteEffectController_UpdatePool:17, SpriteEffectController_UpdatePool:18
* `src/fn/sub_0806E130.c`: RouteSpritePool_Tick:33
* `src/fn/sub_08070FA8.c`: RegistryCurrentGame:25
* `src/fn/sub_08072E50.c`: SceneActorContacts_SetDistanceGate:27
* `src/fn/sub_080764C8.c`: SceneActorAnimation_SelectDirectionFlagsVariant:29
* `src/fn/sub_080838AC.c`: SceneActorControl_TickGroundBob:82
* `src/fn/sub_08086A2C.c`: Ghoul_UpdateDyingState:57
* `src/fn/sub_08089800.c`: Ghoul_UpdateScriptedAction:32, Ghoul_UpdateScriptedAction:34
* `src/fn/sub_0808CE50.c`: Ghoul_UpdateSpecialActionControl:61, Ghoul_UpdateSpecialActionControl:63
* `src/fn/sub_0808CF54.c`: Ghoul_UpdateAnimationElevenControl:61, Ghoul_UpdateAnimationElevenControl:63
* `src/fn/sub_0808D058.c`: Ghoul_UpdateAttackControl:61, Ghoul_UpdateAttackControl:63
* `src/fn/sub_080A8144.c`: SceneActorPlacement_UpdateFlaggedResponse:27, SceneActorPlacement_UpdateFlaggedResponse:29
* `src/fn/sub_080AA8C4.c`: TentacleActor_AlignSegmentsToTerrain:81
* `src/fn/sub_080B713C.c`: SceneActorTerrain_FindOpenNeighborDirection:11
* `src/fn/sub_080BB4DC.c`: Skeleton_UpdateDownedState:57
* `src/fn/sub_080C61AC.c`: Golem_UpdateDying:57
* `src/fn/sub_080D48F4.c`: Mummy_UpdateScriptedAction:60, Mummy_UpdateScriptedAction:62
* `src/fn/sub_080DA5C8.c`: Mummy_ResetResponseState:30
* `src/fn/sub_080E0390.c`: Dog_UpdateDefeatDelay:57
* `src/fn/sub_080E3790.c`: Dog_UpdateScriptedAction:60, Dog_UpdateScriptedAction:62
* `src/fn/sub_080F34C8.c`: ChildGroup_WaitForInactiveMembers:45
* `src/fn/sub_080F3B60.c`: ChildGroup_UpdateTrailingMembers:95
* `src/fn/sub_080F4FA8.c`: Sword_UpdateHorizontalWaveReturn:20
* `src/fn/sub_080F508C.c`: Sword_UpdateDepthWaveReturn:20
* `src/fn/sub_080F5B68.c`: TargetMotion_AdvanceFollowSegment:41
* `src/fn/sub_080F645C.c`: Sword_UpdateDamage:56, Sword_UpdateDamage:74
* `src/fn/sub_080FC894.c`: Axe_CopyAuxiliaryVector:15
* `src/fn/sub_081004F8.c`: FloorSwitch_Release:43
* `src/fn/sub_081029F8.c`: NamedObject_UpdatePlayerAttachment:73
* `src/fn/sub_08103110.c`: NamedObject_CreateFromScript:209
* `src/fn/sub_08109040.c`: SelectionSlots_ClearValue:23
* `src/fn/sub_0810B3BC.c`: SceneMenu_UpdateRecords:45
* `src/fn/sub_0810D60C.c`: PanelText_DrawDecimal:17
* `src/fn/sub_0811A030.c`: MotionActor_DispatchRequestedState:72
* `src/fn/sub_0811B094.c`: Collider_ScheduleUpdates:49
* `src/fn/sub_0811BD30.c`: Actor_UpdateShadowSpriteParts:84, Actor_UpdateShadowSpriteParts:88
* `src/fn/sub_0811D270.c`: PanelText_DrawDigits:17
* `src/fn/sub_0811E728.c`: StatModifier_ScaleForGlobalMode:12, StatModifier_ScaleForGlobalMode:18
* `src/fn/sub_0811E77C.c`: StatModifier_AddForGlobalMode:11, StatModifier_AddForGlobalMode:17
* `src/fn/sub_0811E7C8.c`: StatModifier_AddForOwnerMode:11, StatModifier_AddForOwnerMode:17
* `src/fn/sub_08121110.c`: ActorAction_RunActiveCallbacks:31
* `src/fn/sub_081296D0.c`: FeatherEffectPool_UpdateTask:36
* `src/fn/sub_0812F3CC.c`: SceneActorReaction_BeginContactResponse:86
* `src/fn/sub_08136F58.c`: sub_08136F58:95
* `src/fn/sub_0813BAD8.c`: sub_0813BAD8:132
* `src/fn/sub_0813C5B4.c`: ConvergingEffects_EmitFarParticle:57
* `src/fn/sub_0813FE30.c`: sub_0813FE30:50
* `src/fn/sub_08140F40.c`: Player_GetForceDirection:93, Player_GetForceDirection:94
* `src/fn/sub_081559A4.c`: Player_UpdateTimedChargeAction:95
* `src/fn/sub_08165488.c`: SceneHud_DrawDecimalDigits:16
* `src/fn/sub_0816F6B4.c`: PauseMenu_BeginTypeThirteenItemUse:33
* `src/fn/sub_0816F8AC.c`: PauseMenu_BeginTypeFourteenItemUse:33
* `src/fn/sub_08173C0C.c`: SceneHud_TickEquipmentToggle:37
* `src/fn/sub_08175008.c`: Presentation_UpdateAllocationSelection:150
* `src/fn/sub_0817A57C.c`: SceneActors_SpawnConvergingParticle:57
* `src/fn/sub_0817B5E4.c`: update_facing:13
* `src/fn/sub_081A55D4.c`: Vanargand_BlendPaletteBanks:16
* `src/fn/sub_081A8820.c`: ActorSoundPhase_UpdateConditionalStop:39
* `src/fn/sub_081B9BC0.c`: Registry2388_BlendPaletteBanks:18
* `src/fn/sub_081BD854.c`: DarkLoan_CreateHiddenDisplay:19
* `src/fn/sub_081C0478.c`: ScenePaletteFade_BlendPaletteBanks:16
* `src/fn/sub_081C6094.c`: sub_081C6094:24
* `src/fn/sub_081D010C.c`: sub_081D010C:76
* `src/fn/sub_081D07D8.c`: SceneNpc_EnterProjectedIdle:59
* `src/fn/sub_081D58B4.c`: MonsterBook_UpdateHighlightPulse:7
* `src/fn/sub_081D71CC.c`: ScreenTransition_FadeBlendOut:26
* `src/fn/sub_081EB8E0.c`: sub_081EB8E0:180
* `src/fn/sub_08210C5C.c`: Collection_UpdateHighlightPulse:7
* `src/fn/sub_0821211C.c`: ForgingBook_UpdateHighlightPulse:7
* `src/fn/sub_08213294.c`: ForgingBook_UpdateCursorPulse:7
* `src/fn/sub_08214B44.c`: Sprite_RenderScaledList:54
* `src/fn/sub_08214D20.c`: sub_08214D20:43, sub_08214D20:44
* `src/fn/sub_08217430.c`: sub_08217430:86
* `src/fn/sub_08219C40.c`: Heap_Alloc:16
* `src/fn/sub_0821A184.c`: ResourceRegistry_AllocateEntry:8, ResourceRegistry_AllocateEntry:10
* `src/fn/sub_082210B8.c`: Random_InitValueTable:10
* `src/fn/sub_08221D40.c`: ScalarApproach_ApplyProfileArray:13
* `src/fn/sub_082237A0.c`: RfuManager_StartChildConnect:46
* `src/fn/sub_08228DB8.c`: SunClock_GetPhase:52, SunClock_GetPhase:62, SunClock_GetPhase:99, SunClock_GetPhase:109
* `src/fn/sub_082392E8.c`: sub_082392E8:70, sub_082392E8:73
* `src/fn/sub_08239480.c`: Player_EmitConvergingParticle:57
* `src/fn/sub_0823A028.c`: ActorEffect_SelectFlashingValue:15
* `src/fn/sub_0824AC74.c`: Libgcc_MultiplyInt64:61
* `src/golem_2.c`: Golem_UpdateScriptedAction:393, Golem_UpdateScriptedAction:395
* `src/group_objects.c`: BoardPiece_Wander:493, BoardPiece_Wander:494, BoardPiece_Wander:495, BoardPiece_Wander:496
* `src/modifier_selection_and_stat_previews.c`: ModifierMenu_DrawSecondSourceStats:697, ModifierMenu_DrawSecondSourceStats:716, ModifierMenu_DrawBackground:863
* `src/palette_controller.c`: Palette_BlendRange:69, Palette_InitializeFromScript:461, Palette_SetBrightnessFromScript:709
* `src/password.c`: Text_CopyWithoutTags:9, Password_DecodeEntry:174
* `src/status_screen.c`: NumericDisplay_DrawPair:22, NumericDisplay_DrawStatusValues:68
* `src/status_screen_fade.c`: StatusScreen_FadeInPalette:98
* `src/tether_actor.c`: TetherActor_BeginAuxiliaryState:1122, slopePush_AdjustSlopeMovement:1183, TetherActor_UpdateWanderingState:3125
* `src/wireless_game_items_and_display.c`: WirelessGameItems_UpdateTemporaryItem:887, WirelessGameItems_StartOutgoingItems:1292

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

### ScenePool_StateLaunchArc (src/fn/sub_0803E5F4.c, 0803E5F4): deferred

Drafts: `notes/phase3-drafts/actors/ScenePool_StateLaunchArc_owncalls.c` (5 ROM / 2 draft: every
state block written in full with its own Memory_Clear, break, `return 0` after the switch) and
`notes/phase3-drafts/actors/ScenePool_StateLaunchArc.c` (13 / 7, but the ROM's layout: a
`history` set in each case, one shared `Memory_Clear(history, 0x100)` after the switch, case 3's
state 1 behind a goto after it). In both, the random read (`RANDOM_TABLE_ADDR` literal, fact 21)
and the launch half match. The ROM: cases 0, 1 and case 2's state 1 keep their own stores
(`strb r1, [r0]; adds r0, #8`) and jump to the `movs r1, #0x80` of case 3's state 2. Case 3's
state 1 keeps its own call with `adds r1, #0xff` (reload's move2add). Two blockers:
(1) With own calls, move2add (which runs before jump2) turns case 2 state 1's `r1 = 256` into
`r1 += 255` because r1 = 1 there. Its tail then equals case 3's state 1, so jump2 can't share
the call at 190 as the ROM does. Case 0 has the same problem once its 1 lands in r1. So at
reload_cse time the ROM had no `r1 = 256` in those blocks, or a label before it: the call
was shared before reload.
(2) With a shared call, those three blocks end in identical `[strb r1, [r0]; adds r0, #8]`
tails that jump to the same label, and jump2 merges them. The ROM keeps them apart, so their
jumps must have targeted labels with separate jump chains. One idea, not yet built: two labels
around the `r0 = history` copy, which delete_noop_moves removes only after mark_all_labels has
canonicalized labels. Case 2 state 1's 1 lands in r0 because its block has exactly three local
qtys (fact 38); `AGBCC_FORCE=151:1` on the shared draft shows the merges that follow.
Later session: the label shape can't work, since reload already deletes the `r0 = history`
noop copy (only move2add's own `(set r r)` noops reach jump2). Third draft,
`notes/phase3-drafts/actors/ScenePool_StateLaunchArc_returnsplit.c` (16 / 2): own calls, case 3's
state 1 ends in `return 0` while every other block breaks, and the function ends
`} else entry->stateFrame++; return 0;`. With that split, case 3's state 1 is out of the break
label's jump chain, so case 1 and case 2's state 2 now share exactly as the ROM does, and case 3
keeps both blocks. Left: (a) case 0's 1 and address swap registers (fact 38), so case 0 merges
8 insns into case 3 state 2; (b) case 2 state 1 keeps its call: its `adds r1, #0xff` can match
case 3 state 2's `r1 = 256` only through the REG_EQUIV notes, and case 1's cross-jump has already
stripped that note (scope README fact 111). In ROM order the plain copies go first, so the ROM
had no move2add copy in case 2 state 1: its call was shared in source. But a source-shared call
there is jumped to by the redirected case 0/1 jumps too (do_cross_jump reuses the label), and
case 1 then merges into case 2 state 1. The pinned file's empty asm in case 1 is what separates
them. Next: find what else besides an asm makes case 1's tail end differently at jump2 (a USE or
CLOBBER insn stops the compare), or a helper that returns history (the pinned file's
Prepare/PrepareFirst/PrepareZero inlines).

### ScenePool_StateLaunchShortArc (src/fn/sub_0803E7B4.c, 0803E7B4): deferred

Draft: `notes/phase3-drafts/actors/ScenePool_StateLaunchShortArc.c` (the LaunchArc own-calls
form; kind in r8, the fire logic and the launch half match). Its switch tail is byte for byte
the LaunchArc one and has the same residual (5 ROM / 2 draft). Solve LaunchArc first; then the
return-split shape carries over.

### ScenePool_StateEmitBursts (src/fn/sub_0803E998.c, 0803E998): deferred

Not drafted separately: its kind switch ends in the same shared Memory_Clear tail as LaunchArc
(cases 0, 1 and case 2's state 1 jump to case 3 state 2's `movs r1, #0x80`; case 3's state 1
keeps `adds r1, #0xff`). The front half (the bursts) is ordinary. Do it after LaunchArc.

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

### TentacleBody_ResetDirectionalAnimations (080A915C) and TentacleBody_RestartSegmentAnimations (080A93F4): deferred with Refresh

Same family as TentacleBody_RefreshDirectionalAnimations: the same Step helper and the same
`frame` stack struct and segment loop. With Refresh's draft Step and ordinary body C, Reset scores
54/52 (`TentacleBody_ResetDirectionalAnimations.c`); Restart is not tried yet. Solve Refresh first,
then move the shared Step into `include/tentacle_segment_animation.h` and port it to all three.

### TentacleActor_RemoveSegmentColliders (src/fn/sub_080AA688.c, 080AA688): deferred

Draft: `sub_080AA688.c` (`--trail->activeCount;`, 25/24 differ). The ROM rebuilds `trail + 0x8EA` in
the loop; the draft hoists it. `hoist`: in loop pass 2 the address (insn 148) moves at 13 x 1 x 3 >=
37, after pass 1 moved the mask 1 (as the ROM does too). Tried `--trail->activeCount`,
`activeCount--`, a pointer local and `= activeCount - 1`; all are hoisted. Next: a form whose pass-2
threshold or lifetime fails, or a count address that is not invariant (derived from the segment).

### TentacleActor_AlignSegmentsToTerrain (src/fn/sub_080AA8C4.c, 080AA8C4): deferred

Draft: `TentacleActor_AlignSegmentsToTerrain.c`, 36/38 differ, all of it allocation. It passes
`&segment->position` to Height (the landed code passes `&trail->segments[i].position`, which costs
recomputation). Left: the ROM copies Height's result (`adds r2, r1, #0` at the switch join) and
keeps Height's `p` in r6 copied from r7 before the override call. The draft coalesces the result
and strength-reduces the copy (insn 703, tagged loop). `why all`: 234 vs p allocation order
(`AGBCC_PRI=234:+1636` gives 28/30).

### Six-spark burst family: Bat_UpdateDefeat, TentacleActor_SpawnSparkBurst (080AB294), Vorn_UpdateDefeat

All three spawn six sparks from `gRandomTable` kept in a callee-saved register. The ROM's table
pseudo has no REG_EQUIV; the C pins it or uses an asm barrier. In SpawnSparkBurst the halves union
(`TentacleActor_SpawnSparkBurst.c`) already gives the ROM's allocation (table in r7), 4/10 differ:
only the half insertions are left unfolded. The `struct VAB v` bitfield writes before it leave
0xFFFF0000 and 0xFFFF in cse-shared pseudos, so the insertions' `and` has a register operand and
combine has no constant to simplify. Ruled out here: the two-statement offset form
(`off = (u8 *)gRandomTable + k; table = (u16 *)(off - k);`, sub_08000430), where cse still
records REG_EQUIV (const (plus sym k)) and the result is worse, and halves placed before the vector
writes. Next: a table source that cse does not see as constant and combine folds, with no mask
constant in common with a bitfield write in the same block.

### TentacleActor_UpdateRetraction (src/fn/sub_080AB968.c, 080AB968): deferred

The ROM computes |secondaryMotion.x| and |secondaryMotion.z| (`adds r3, r1, #0; ...; rsbs r3, r1, #0`)
and never uses them. The landed C keeps them with a volatile asm. So in the original they fed
something deleted after flow. Draft `TentacleActor_UpdateRetraction.c`: `if (ax > az) Advance(...);
else Advance(...);` (7/12). The compare survives, because reload materializes 0x6400 differently in
the two arms and jump2 cannot cross-jump them. Tried: an empty `if` with DEBUG_PRINT (flow deletes
the abs, 20/2), branching inside Advance, and a `speed` variable set before the branch. Next: two
arms that stay identical after reload (all arguments set before the branch), or a switch on the
abs values.

### TentacleActor_RecoverFromSegmentLoss (src/fn/sub_080ABB18.c, 080ABB18): deferred

The ROM computes 15 field addresses of p before the segment-removal loop's first test and spills
most of them to the stack. That looks like loop.c hoisting the invariants of a loop that contains
the final stores. Draft `sub_080ABB18.c` (stores inside a `for (;;)` exit branch, 139/107) has
loop.c hoist only a few of them (savings threshold). The landed C precomputes the pointers by hand,
with 5 pins and 2 barriers (all removed: 107/107). Next: decide between the two shapes with
`hoist` (which addresses the ROM's loop moved), then match the allocation.

### Vorn_SelectReactionControl (src/fn/sub_080B197C.c, 080B197C): one barrier left

Landed c725f2610: the value pin is gone (`mask &= value` in HasFlag). The `asm("" : : "r"(active))`
remains. Without it (draft `sub_080B197C.c`, 26/26, an r4/r5 swap only) a (18 refs / 94 live, pri
7659) outranks active (8 / 33, 7272). `AGBCC_PRI=active:+400` matches. Passing `active - 1` to
SceneActor_DispatchContextAction fixes the allocation but changes the code. `!active` and
`active == 0` fold before regclass counts. Next: a ninth natural ref of active, or two insns less
of its life.

### Ivy_UpdateDefeat (src/fn/sub_080EE1B0.c, 080EE1B0): table pin, one load position from a match

Six-spark family, but here the table is a local-alloc qty, not a REG_EQUIV spill. Block 24's
call-saved qtys (AGBCC_DUMPPRI L records) in priority order: =gRandomIndex 2010 r4, a pointer
1163 r5, =1#2 769 r6, size 760 r8, then the position `&actor->body.position` (7 refs, life
58-252, 721) and the table (7 refs, 36-236, 700). The ROM has the table in sb and the position in
sl, so the table must come first: its life must be 185-193, or the position's over 200.
Draft `Ivy_UpdateDefeat.c` (5/5, no table variable): a `NextRandom(wrap, bits)` helper that reads
`gRandomTable` itself gives the constant pseudo its birth at the first `add` (48, pri 744), and
the allocation is the ROM's. The only difference left is that the `ldr =gRandomTable; mov sb`
pair comes after the index computation instead of before the gRandomIndex load. Next: a form that
sets the table before the first index but ends its life (or its refs) as the helper form does.
Passing gRandomTable straight to RandomFrom (no variable) gives the same 14/14 as the pinned-out
draft; the union RandomTableView forms are no better.

### Axe_UpdateDamageSpin (src/fn/sub_080F9974.c, 080F9974): pin and one barrier left

Landed: the flags2D6 `mask` barrier is gone (`OrStatus(0x20, &actor->flags2D6)`, an inline
whose constant parameter is set before the address). Draft `Axe_UpdateDamageSpin.c` has no
concessions: `state[0xED6] = 1; *(u16 *)(state + 0xED4) = 0;` gives the ROM's code, including
reload's `subs r1, #2` (move2add). It is 11/11, with one swap left: context (7 refs / 170 live,
pri 823) and the constant 1 (4 refs, flow life 48, doubled by REG_EQUIV to 96, pri 833) take r8
and sb the wrong way round. `AGBCC_PRI=context:+20` matches. One more flow-time insn inside the
1's range (SetState's field2B1 store to the ED6 store) would flip it (both lives grow, the 1's
counts twice), and so would three fewer insns in context's range outside it. Tried without
effect: local temps for angle/zero/speed/one, a shadow pointer, a struct copy through pointers,
inline helpers for the ED6/ED4 stores (constant parameters are substituted), and simpler
mask forms (those change the code).

### Terrain_ResolveActorMotion (src/fn/sub_0811AE9C.c, 0811AE9C): 6 concessions, pin-free draft 10/10

Draft `Terrain_ResolveActorMotion.c` (no concessions) rewrites the middle part:
`clearance = Terrain_GetCellClearance(next.coordinates); y = next.coordinates[1];
floor = clearance - Offset(next.coordinates[2]);` (the Offset inline keeps `z + 96` from being
reassociated into `clearance - 96 - z`), and `shape = 0xF00; shape &= flags; flags &= 0xF000;`.
That also removes the z-clamp cross-jump difference: both arms now keep their own deltaZ store.
Two differences are left. (1) In the else arm the ROM negates a copy of offset for
`Abs(offset) >> 12`. cse's fold_rtx turns `-x`, with x known to be `-(difference << 12)`, back
into the shift's pseudo, so the draft keeps the shift alive in r2 and the ROM's local r0/r1
pair is lost. Splitting it as `offset = difference << 12; offset = -offset;` gives the right
negation (11/11, shape 0) but puts both in one register. The original's C must hide the
`neg` from cse (the pinned version did that with `asm("" : "+r"(offset))`). (2) The high-byte
result and shape sit one register higher (r3/r4, ROM r2/r3), because offset's pseudo takes r2.
Fixing (1) probably fixes (2).

### sub_0813D834 (src/player_3.c, 0813D834): 2 pins, one cse follow from a match

Draft `sub_0813D834.c` (no pins) is 17/15. Every difference comes from cse following the
switch's `beq` into case 1 (`probe`: `CSE_NOFOLLOW=2` matches). cse then knows kind == 1 and folds
the timers path's `kind * 2` into `ldr =0x532`, where the ROM computes `lsls r0, r3, #1` (kind
kept in r3). cse_end_of_basic_block follows a jump only to a label with LABEL_NUSES == 1 that is
preceded by a BARRIER, so the original's case 1 label must have had a second use or no barrier
before it. No effect: an if/else chain (worse), `u32 kind`, computing from `inputKind`, a
`goto` to the timers block from the flags test.

### PlayerAction_RefreshFlagState (src/player_3.c, 0813EDDC): 7 concessions, pin-free draft 8/8

Draft `PlayerAction_RefreshFlagState.c` drops all seven (`if (alternate < 0) override = 0; else
override = GameState_GetConsumableDescriptor(alternate);` and `override->slot` read directly).
What is left is reload rotation: `scope.py spill` matches with `AGBCC_SPILL=5:1`. Between reload
4 (the 0x418 actionMode offset, r2) and reload 5 (alternate from r8 for `cmp`), the ROM made one
more reload choice that took r0 and left no visible insn. Reload 5 then got r1, and override's
reloads in the slot loop follow. Writing the actionMode/slot test as separate ifs, through a
`mode` local or pointer, `!= 0`, or a `u32 slot` local changes nothing, and neither does a
ternary for override (worse). Next: find the ROM's hidden reload (a pseudo with a MEM REG_EQUIV
replaced at its use, or a copy deleted after reload).
