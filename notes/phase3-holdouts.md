# Phase 3 holdouts

Tags: #matching #phase3 #holdouts

One section per worker. Each row: the function, its best pin-free draft (a snippet to
`scope.py FILE.c put`), the differing instructions, the mechanism and the knob that matches,
what was tried, and the next idea.

## field-effects

### Scene_SpawnDirectionalDust (src/scene_object.c, 08004B64): resolved

Resolved without concessions by the dp-field Codex pass; see
[cx-dp-field_2026-11-13.md](cx-dp-field_2026-11-13.md). The earlier investigation follows.

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
Tried monsters-bosses' class-head lever (Hresvelgr_UpdateRisingProjectile, an early mention of
the copy): an early `savedActor = 0`/`-1` or `index = 0` at the top, `savedActor = actor` then
`index = savedActor`, or saving through `s32 *saved = &savedActor`. All stay at 11/9 or get worse. The
extended block starts at function entry, so no mention can come before it; the copy needs a last
use past the block end (the loop label at `active`), and that use would merge the r5 copy instead.

### DirectionalController_CheckRemaining (src/directional_interaction.c, 080098CC): resolved

Resolved by the dp-field Codex pass: the accepted branch returns without a value (fact 160), preserving the call result in hard r0. Both concessions are removed.


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

### sub_0801404C (ParticleGroup_UpdateBouncing, src/fn/sub_0801404C.c): deferred

Draft: `notes/phase3-drafts/field-effects/sub_0801404C.c` (38 ROM / 40 draft; with the four
barriers simply removed it is 45 / 51). `ParticleRandom_ReadSample(table)` in place of
`ReadDirectSample` puts the table in sl as the ROM has it (the direct form leaves the
REG_EQUIV table unallocated and rematerialized). Still open, in order:
* `scope.py ... probe`: PRE reuses the CellIndex load of position.x for the line-88 update
  (`AGBCC_NOPRE=1` gives 34 / 38). The barrier's "memory" clobber hid it. A pointer local for
  the update (`pos->x += ...`) is much worse (65 / 85).
* `resource` is r8 in the draft and caller-saved r3 in the ROM (spilled around the call);
  `sprite` (g + 20) is a stack slot in the draft and sb in the ROM; gRandomIndex's address is
  sb in the draft and r8 in the ROM (`AGBCC_PRI=144:+517` helps).

### ParticleGroup_UpdateFrozen (src/fn/sub_08014330.c, 08014330): deferred, one zero-extend

Draft: `notes/phase3-drafts/field-effects/ParticleGroup_UpdateFrozen.c` (3 ROM / 1 draft). There
is no `type` variable: plain `g->type` reads at the top and in the late test. gcse PRE then
produces the ROM's shape by itself: the top copy (`ldrb r0` / `adds r3, r0`) and the
`ldrb r3` it inserts at the end of the two arms that call FindCellOverride (`scope.py ... pre`).
The one difference: the ROM zero-extends the PRE register before `cmp #8`, while combine drops
the extension in the draft because every set of the QImode reaching register is a load or a copy
of one (nonzero_bits 0xFF). The ROM's register must have had a set with unknown upper bits (a
lowpart subreg of an SImode value counts, since ARM has WORD_REGISTER_OPERATIONS). A u8/u32/int
local at the top is worse (it changes the expression, so PRE stops), and so are casts on either
test.

### sub_08014634 (ParticleGroup_SpawnBouncing, src/fn/sub_08014634.c): deferred, rewrite started

The pinned source imitates register pressure: a `struct SpawnArgs` copy of count, options and
position with an `asm("" : "+m")`, plus 15 pins. In the ROM those three arguments are simply
spilled pseudos. `str r0/r1/r2, [sp, #0/4/8]` at entry, and `mov r1, sp; ldrb r1, [r1, #4]` is
reload reading the low byte of spilled `options` for `g->type = options`. Draft:
`notes/phase3-drafts/field-effects/sub_08014634.c`, plain C with ordinary arguments (132 / 116,
mostly register shifts). The inner loop counts down in the ROM (loop.c reversed `i`, then reloads
`count` as i's final value). Open: in the draft `options` stays in r6 and gcse hoists
`options * 4` for `gUnk_08604B90[options]` (the ROM recomputes it from the stack slot each
pass); the ROM keeps both `pool` (spilled, sp+0x10) and `g` (r6) and holds velocity in r8;
`rng` is caller-saved r3 around sub_08217E40.

### AnimatedParticleManager_Update (src/fn/sub_08015160.c, 08015160): resolved by dp-effects

The shared DisplaySequence_StepSprite draft now matches without concessions.
Writing p->timer++ in each playback-result branch gives p the extra references
needed to win r5 over the command pointer. jump2 shares the two increments into
the ROM's single tail; no user label is needed. Stock whole-unit check: MATCH,
540 bytes. The former diagnosis p:+206 was correct; the ordinary branch-tail form
supplies its effect. The earlier field-effects draft remains as historical evidence.

### AnimatedParticle_PriorityFromFlags (src/fn/sub_080154D0.c, 080154D0): deferred, DEBUG_NOP

Without the `DEBUG_NOP()` it is 7 ROM / 5 draft. regmove ties `flags & 0xf000` to the
constant's register instead of to `flags` (`AGBCC_NOTIE=13` matches). The empty loop's notes
after the AND make the AND's result live across a block boundary, which changes that tie.
Tried with no change: the value or the constant first (fact 33), `flags &= 0xf000`, a separate
`priority`, u32 parameter and casts, a ternary, `p >= 1 && p <= 4`, `p < 4`, a switch (worse),
and `result = 2` before or after.
Next idea: real code between the AND and the shift that ends the block (a test the original
had), or evidence for `concession-ok(debug-nop)` from a twin with the same compiled-out check.

### AnimatedParticles_Create (src/fn/sub_080154EC.c, 080154EC): deferred

Draft: `notes/phase3-drafts/field-effects/AnimatedParticles_Create.c` (6 ROM / 2 draft, all four
concessions removed). Two separate problems:
* jump2 cross-jumps the `movs r3, #0; bl Anim_SetSequence` tails of the type 5 and 6 arms into
  the type 7 arm's copy. The ROM keeps all three (arms 5 and 6 are even identical except for
  the palette bank). The pinned source keeps them apart with `asm("" : : "r"(a), "i"(N))`.
  The original's tails must have differed in RTL (a different call or argument form) or
  jumped to different labels at jump2 time.
* After the arms the ROM copies the state pointer to r8 and back (`mov r8, r4; mov r4, r8`):
  a second pseudo for the step. DisplaySequence_StepSprite is not this step: it reads
  `state->commands` where this function reads `p->anim.data` (`ldr r1, [r5, #0x68]`), and
  using it gives 101 / 124. A local inline step with a parameter copy, called with `saved`,
  gives 7 / 3 but still no copy back. Without a separate `saved` it is 101 / 124 again, so
  the second pointer is real.

### FadingParticleGroup_Spawn (src/fn/sub_08015F60.c, 08015F60): deferred

Draft: `notes/phase3-drafts/field-effects/FadingParticleGroup_Spawn.c` (57 ROM / 59 draft; with
the two pins simply removed it is 64 / 66). The random read goes through
PARTICLE_RANDOM_READ_SAMPLE (shared header), as in ParticleRandom_ReadSample. Open: the table
(ROM r6) loses to `rng` (3401 vs 2975; both are REG_EQUIV constants with doubled live
lengths; `AGBCC_PRI=table:+427` gives 43 / 45), and the sine table pointer wants r2
(`AGBCC_FORCE=sine:2` gives 43 / 43). Moving the `rng` set, or dropping `rng` for the header's
ReadSample/ReadDirectSample, is no better.

### FadingParticleGroup_Update (src/fn/sub_08016118.c, 08016118): deferred

Draft: `notes/phase3-drafts/field-effects/FadingParticleGroup_Update.c` (the memory barrier
removed: 43 ROM / 43 draft; the `raw` pin is still in it). Without the barrier, cse2 reuses the
`p->velocity.y` loaded for `position.y += velocity.y` in `p->velocity.y -= 2` (insn 177), where
the ROM loads it again. The position stores go through the same base at other offsets, so cse
sees no alias. A pointer local for the position or velocity (fact 60's lever) changes
allocation across the whole loop (87 / 89), and one only for the `-= 2` is no better (43 / 43).
The rest is allocation and one reload rotation (`AGBCC_SPILL=8:1` helps).

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
* The ROM keeps a dead `ldrb` of p->mode (0x182) before ScriptProc_Start.
  The later dp-menus investigation below disproves the original volatile-only conclusion: identical
  dispatch arms preserve the read until jump2 removes their test. A `vu8 mode` field changes the earlier
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

### Wireless_ReceiveParentUniPackets (0803548C, src/fn/sub_0803548C.c)

Draft: `notes/phase3-menus-link/Wireless_ReceiveParentUniPackets.c` (19 ROM / 19 draft, shape 6,
no pins or barriers). Parked to revisit, not finished.

- Struct: named per-slot arrays (`peers[4]` 4-byte packets at 0x80, `lastSequence`, `repeats`,
  `behind`, `ahead` at 0xD0-0xDF, then `sendStalls`, `receiveStalls`, `received` and `sent` at
  0xE4-0xE7).
- Loop: a do-while with a packet pointer stepped by 4 and `offset = i << 2` for
  gRfuSlotStatusUNI. These match the ROM through the loop, except which registers copy the held
  zero for the two `flags = 0` stores (sb into r2/r3).
- Tail: the ROM keeps two copies of `scratch[2]` (r3 for `& sent`, r1 for the full mask) and builds
  the matched mask in r2. Single-set variables tie the and to the dying `received`. The original's
  multi-set `got` (`got = received; ... got = expected;`, the old pinned form) or a separate `all`
  variable lowers i's global priority below a 23-ref pseudo. The whole loop then allocates with one
  high register fewer: the draft is 260 bytes against 272, at 55 / 49.
- Next: find which pseudo the multi-set tail promotes (`why` on 22 says it is a doubled constant
  register), or a tail form that keeps i ahead.

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

### ContactActor_UpdateRetreat (src/fn/sub_0804373C.c, 0804373C): deferred

Draft: `notes/phase3-drafts/actors/ContactActor_UpdateRetreat.c` (95 / 95, no pins). The ROM's
dead `ldr r3, =0x0403B714; mov r8, r3` before the enter block's first random draw is the
project's known random origin: `PARTICLE_RANDOM_CAPTURE_ORIGIN(origin)` plus a
`ParticleRandom_ReadRelativeSample(origin, table)` draw (monster_particle_random.h; scope
README fact 94; 48 functions in build/asm show the literal). The draft reads the steering timer
that way, and the literal comes back. Left is call-crossing allocation: the ROM has slot r7,
center sl, origin r8, position r6. The draft has slot r6 and position r7, swaps origin (sl)
with center (r8), and keeps a spill slot (sp #0xc against #8). `AGBCC_PRI=position:+4000`
gets to 55 / 55. Next: find which draws use the relative read (it decides origin's live
range), then the position/slot order.

### ContactSwarm_UpdateTracking (src/fn/sub_08044290.c, 08044290): deferred

Draft: `notes/phase3-drafts/actors/ContactSwarm_UpdateTracking.c` (25 / 27, the pinned file
with plain `jitter = *(const u16 *)(RANDOM_TABLE_ADDR + i * 2) & 15`). The pinned file's
single pin (`mask` in r2) is what keeps the jitter's `movs r2, #0xf` fresh. Without it, cse
pass 1 replaces that 15 with the pseudo that `stateFrame & 15` / `index &= 15` set in the first
block. Its path runs from there with the phase block skipped (AROUND, cse.c
cse_end_of_basic_block), the Collider block next, then the `blo` not taken. The pseudo then
crosses three calls and takes r8, so the prologue and everything after differ. A path break
needs a label with a use inside the phase block (the scan stops at the join) or a second jump to
the join; the ROM's phase block has neither. Tried: the shift form `(x << 28) >> 28` (combine
keeps the shifts, 2 / 2), `% 16`, ParticleRandom_ReadSample (do/while), and swapping the
frame test's arms (worse). Next: a mask whose bits combine narrows to 15 through nonzero_bits
(the 15 would be a new const after cse).

### ActorAttachment_CreateValueSpriteSlot (src/fn/sub_08065FD0.c, 08065FD0): PRE slot order only

Draft: `notes/phase3-drafts/actors/ActorAttachment_CreateValueSpriteSlot.c` (15 / 15, every
difference a PRE stack slot; `AGBCC_GCSE_T=127` or `=133` gives MATCH). Three levers got it
from 126 there:
(1) The sprite tail (Sprite_Init .. `placed->position`) is an inline helper. integrate.c folds
the large offsets 0x134 / 0x10E / 0x104 to const form there (fact 55), so gcse hoists them above
the frame loop as the ROM does.
(2) The frame-select loop is a label/goto loop. With while/for/do, NOTE_LOOP_BEG keeps the else
label and the loop label apart in jump1, so LCM inserts into the else block and loop.c hoists
`4` and the copies.
(3) The burst pointer starts after Mod and steps inside the body, with the draw written as
`*(u16 *)(RANDOM_TABLE_ADDR + gRandomIndex * 2)`.
Left: N = 258 real insns into gcse (T 129). The ROM's order (action, resourceKind, active,
sprite, sequences, unknownA) needs N 252..255 or 264..267 (`preorder`). About 30
code-preserving spellings keep N at exactly 258: the switches, IsAlternateMode, the goto
shapes, found/i types, burst loop forms, inline boundaries and dead results. A manager-fetch
inline moves N (e3: 263) but changes the code. Next: a structural difference that later passes
remove (jump threading of a second null test, a return-label shape). Or a shared inline with
CreateSpriteSlot / the other Create* slots once they are matched.

### ActorAttachment_CreateSinglePartSlot (src/fn/sub_08066278.c, 08066278): PRE slot order only

Draft: `notes/phase3-drafts/actors/ActorAttachment_CreateSinglePartSlot.c` (24 / 24, pin-free,
every difference a PRE stack slot). It is the same family as CreateValueSpriteSlot: a tail inline
`ActorAttachment_SetupPart` (const-form 0x134 / 0x10E / 0x104 hoisted above the clearing loop),
`parts = slot->parts` read before the null check, and the SDK `CpuFill32(0, offset, 8)` macro.
Its `vu32 tmp` gets a slot at declaration, below the inline's addressof'd xOffset / zOffset as in
the ROM, so it needs a `concession-ok(volatile)` note citing the SDK macro. Left: `preorder` wants
slot's pseudo +4 (r34/r35, by declaration order) together with N 184..187 (T 93). The draft has
N = 182, and the loop forms and declaration orders tried all keep it.

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

### sub_0815E034 (PlayerContact_ComputeCost), 3 pins kept
Draft: notes/phase3-drafts/monsters-bosses/PlayerContact_ComputeCost.c (the matched form with the
pins removed: 24 ROM / 22 draft differ). The ROM has common (63) in r6, player in r7 and source in r8.
The what-if `AGBCC_PRI=source:-1500` (source 2142 -> 642, below player's 689) fixes every
difference except one: in the cueDefenses switch, the offset constant takes r0 where the ROM has
r1. So source must be the lowest-priority long-lived pseudo, but its six visible uses
(unknown3C, three Accumulate calls, two Anim calls, plus the dead-`ands` arm pair, fact 57) give
it at least 8 refs over 126 insns. Things that changed nothing or made it worse:
- per-case `player->cueDefenses[k]` (player then outranks common, so common lands in ip);
- an inline helper for the two arm tails (its parameter copies are propagated away);
- every input/source split over the seven uses (128 variants);
- assigning source late.
Next: look for a local pseudo that takes r8 before global alloc (pass 0 reuses registers already
in use, README fact 13), so that source picks r8 without being last.

### PlayerContact_ApplyResponse (sub_0815E1F0), 1 pin and 1 operand barrier kept
Draft: notes/phase3-drafts/monsters-bosses/PlayerContact_ApplyResponse.c (13 ROM / 22 draft differ).
In the 0x80000 contact branch the ROM computes `r3 = power + 60` and `gEngineFlags & 0x1000` and
uses neither, then calls Player_RequestTimedReset(p, power + 60). The flags test is the fact-57
pattern: an if whose two identical arms jump2 merged. The draft's two arms (each a call plus the
positionResetCounter store) differ after reload only in the register the 0x4DA offset gets (r2
vs r4, reload rotation), so cross-jumping merges just the store. The dead `duration` set in r3
is still unexplained: every arm pairing of `duration` and `power + 60` scores worse. So does a
DEBUG_PRINT body, which flow deletes together with the test.
Next: find a duration use that survives flow and goes away after reload (a no-op move as in
fact 81, or a REG_EQUIV substitution), then fix the rotation with `spill`.

### TargetRegistry_FindById (sub_0815F5B4), 1 pin kept
Draft: notes/phase3-drafts/monsters-bosses/TargetRegistry_FindById.c (5 ROM / 5 draft differ). With the r3 pin
removed, requestedId (3 refs: the copy plus one compare weighted 2 in the loop, live 22, priority
1363) sorts below the hoisted sceneActor mask (3846), so the two swap r3/r4. Giving each arm its
own compare (refs 5, priority 4347) puts every register where the ROM has it. But jump2 then
cross-jumps the wrong way: the collider arm's tail jumps into the actor arm's copy, while the
ROM keeps the collider tail and sends the actor load back to it with `b`. That happens with
if/else, with a goto to an actor label after `break`, and with an early collider block.
Next: find the arm layout in which the actor tail is the one replaced (it would have to fall
into, or come before, the label the collider tail jumps to), or another source of two id refs.

### SpriteColliderGroup_ActivateParts (sub_0818E9B8), stride pin and r8 clobber kept
Draft: notes/phase3-drafts/monsters-bosses/SpriteColliderGroup_ActivateParts.c. Without pins, every insn
matches except the r8 save and restore: the ROM pushes r8 and never uses it. Some pseudo got r8
in allocation and lost all its uses afterwards. An explicit `zero` byte for the action store is
the nearest form so far: it takes r7, which pushes count to ip and the stride to r8. In the ROM,
count is in r7, the stride in ip, and the store takes cse's jump-equivalence zero (the loaded
`active` byte, r0). What is needed is a pseudo that sorts last (gets r8) and whose set and use
disappear after reload (reload_cse, a no-op move, or a substituted equivalence). None of these
help: u32 active, p->count instead of the count pointer, do/while, or indexing p->parts[i].

### SpriteColliderGroup_ResetActiveParts (sub_0818EA3C), total pin kept
Draft: notes/phase3-drafts/monsters-bosses/SpriteColliderGroup_ResetActiveParts.c (2 ROM / 2 draft: the first
count load uses r6, the ROM r5). Without the pin, combine folds `total` (a zero_extend of p->count)
into the compare as `(subreg:SI (mem:QI))`. Before reload, register_operand accepts a SUBREG of a
MEM. Reload then loads it into its next spill register. The draft's spill set is {r0, r4, r6}, so
after r4 (the 0x5C3 constant) it takes r6. The ROM's set also holds r5, so either total is a real
pseudo there (sharing r5 with count, as the pin forces) or some ROM insn needed a reload while
r0-r4 were busy. Things that changed nothing or scored worse:
- the types s32, u32 and u8;
- `total > i`, an assignment inside the condition, or a second use of total;
- for/while loops with hoisted constants;
- p->count read directly.
Next: find what keeps combine from folding the load (a second live use of total that goes away
later) or the ROM insn with four busy low registers.

### sub_0819BB18 (Hresvelgr facing motion burst), 4 operand barriers kept
Draft: notes/phase3-drafts/monsters-bosses/sub_0819BB18.c (`union RandomTableView` re-set in the loop, no
barriers: 24 ROM / 23 draft differ). In the ROM the base 0x0203B000 stays in r7 for the whole loop.
The first sample's `table + 512` is computed as `r7 + 0x400` and hoisted to the stack, not folded
to 0x0203B400. The second and third samples recompute `r7 + 0x400` in the loop (one `adds`).
- Union re-set in the loop: cse folds the first sample (same block as the set) to the literal.
- Plain pointer set before the loop: cse2 folds the hoisted copy, and gcse reuses it for the
  later samples (28/22).
The ROM needs a table pseudo that cse never sees as constant in the first sample's block or in
the preheader, and that gcse does not treat as redundant for the later samples.
Next: a matchsearch store over where the table is set and what type it has (symbol vs integer
address, the 512 offset applied at the call site or in the helper).

### Hresvelgr_UpdateTurningCharge (src/fn/sub_0819FC34.c, 0819FC34): deferred

Pin: `register u8 *address asm("r1")` in the SetFlags helper, which ORs a mask into `flags` (+0x9c).
It is used twice: unconditionally after the `pending` block, and again under `PlayerCanMove` in case 0.
Draft: `notes/phase3-drafts/monsters-bosses/Hresvelgr_UpdateTurningCharge.c` (`p->flags |= mask;`),
16 ROM / 14 draft. The only difference is gcse's PRE: expression `(plus actor 156)` is fully redundant
at the second site, so the second site becomes a copy of the first (`adds r7, r4, #0` inserted before
the callback test), and the first address lives in r4 rather than r1. `scope.py pre`:
`AGBCC_NOPRE=1` gives an exact MATCH. The ROM computes `actor + 0x9c` again at both sites.
The offset is below 256, so fact 55's body/inline split (`(plus p (reg K))` against
`(plus p (const_int K))`) does not apply. Every spelling reaches gcse as the same expression:
a local address that is set twice, `q = p` inside the inline, a `u32` cast, `OrFlags(mask, &p->flags)`, and value/mask locals.
cse and cprop fold the copies back to `actor`. The coordinator's displaced-address form (`end = (u8 *)&actor->flags + 4; *(u32 *)(end - 4)`)
does not help either. If both statements are in one block, cse1 folds them back into
`(plus actor 156)`, and PRE copies it again. If `end` is set in an earlier block (top of the
function, or the start of case 0's else), gcse no longer merges the address and the first site
matches, but the round trip then stays as `adds r7, r5, #0xa0` ... `subs r0, r7, #4` (7/9).
Next: find a base the original might have used
that gcse sees as another operand at one site (a multi-set pointer, or a pointer from a call
or a load), or show that actor is reassigned in the original.

### Hresvelgr_PlayDescent (src/fn/sub_081A09C4.c, 081A09C4): deferred

Concession: `asm("" : "+r"(actor))` right after `actor = p`. The ROM keeps both: p in r7 (used
only by the last `Hresvelgr_RefreshTarget(p)`), the copy actor in r6 for the body, tick in r8.
Draft without the barrier: `notes/phase3-drafts/monsters-bosses/Hresvelgr_PlayDescent.c`
(11 ROM / 5 draft). cse merges actor into p (fact 4): p's last use is the else-if call, after
every use of actor in insn order, so actor never becomes the class head. This is the same family as
Trigger_RunScript's actor copy (field-effects). Tried: the body as an inline taking the actor
(`Descend(p)` returning whether it landed, or a void inline with `actor = p` inside), and a
loop-scoped copy. None keeps two registers. Next: find a form where the copy's uses
sit in a cse block that the copy does not reach and cprop leaves alone (cprop propagates a copy
that dominates its uses), or a reason the original's p was not a plain pseudo.
Fact 85 (RisingProjectile) does not carry over: there the ROM's copy is used only outside cse1's
path, but here the copy (r6) is used from the first block on, which needs it to be head. Being head
needs its last mention after the final `Hresvelgr_RefreshTarget(p)`, and the ROM places that call
last (the else arm follows the then arm).

### Hresvelgr_UpdateBoundaryBarrage (src/fn/sub_081A0BC8.c, 081A0BC8): deferred

Concession: `asm("" ::"r"(normalCollisionMask));` at the end, which keeps the 0x2f0 mask (sl) live
through case 1. Draft without it: `notes/phase3-drafts/monsters-bosses/Hresvelgr_UpdateBoundaryBarrage.c`,
2 ROM / 2 draft. Only the random index mask 0x3FF (pseudo 220) differs: ROM ip, draft sl. Global alloc
first gives 220 r5, but reload needs r5 for spills and re-places it with retry_global_alloc. In pass 0,
sl is in use and does not conflict with 220 (the mask's only use is in case 2, so it is dead in case 1),
so 220 takes sl. The ROM's pass 0 found sl busy (a conflict, or a conflicting pseudo that prefers sl),
and pass 1 took ip. Tried: the literal 0x2f0 in case 2 with no variable; `action` computed from
the mask (the ROM shares the 0x2f0 constant between `&actor->action` and the mask); the case-2 store
through SetCollisionMask. None changes it. Next: find a pseudo live in case 1's random block
that prefers sl, or a reason the mask is live after the switch in the original.

### sub_081A14FC (src/fn/sub_081A14FC.c, 081A14FC, 1764 bytes): deferred

Pins: `table` in r6, `sampleY` in r3 and `rawY` in r0 in case 1's random spray, plus an
`asm("" : : "r"(valueY))` barrier. Draft: `notes/phase3-drafts/monsters-bosses/sub_081A14FC.c`
(86 ROM / 82 draft). With the cast-constant table (`(const u16 *)RANDOM_TABLE_ADDR`) every sample
reloads 0x203B400, and 0x3FF becomes a pseudo in ip (80/80). With
`union RandomTableView view = {gRandomTable}`, the table is a pseudo and 0x3FF is reloaded at each
use, as in the ROM. But the table (view, 7 refs over 164 insns, priority 853) ranks below
&gRandomIndex (8 refs over 208, priority 1153), so they swap r6 and r8. Separately, the ROM's frame is one word
larger (`sub sp, #0x64` against 0x60), so every PRE spill slot from line 120 on is 8 bytes higher:
the ROM spills one more pseudo, probably the one the table pin displaces from r6.
`AGBCC_PRI=view:+400` swaps the pair but makes the rest worse. Next: fix the table/index ranking
(fact 110's double-counted sampler use, or fewer &gRandomIndex refs), then find the ROM's extra
spill.

### Vanargand_UpdateCaptureSetup (src/fn/sub_081A8E08.c, 081A8E08): deferred

Pin: `register s32 height asm("r4") = y;` in SetPositionAndGetHeight. The ROM has two zeros. One is
the pending-store zero (`movs r0, #0; mov ip, r0`), which is also case 2's sixth argument (ip). The
other is the position's y (`movs r4, #0` right after `movs r1, #0x80`), which is the sixth argument
in cases 0 and 1 (r4). Draft with literal zeros:
`notes/phase3-drafts/monsters-bosses/Vanargand_UpdateCaptureSetup.c`, 13 ROM / 9 draft. cse1 puts
every zero (14 pseudos) in the class of const 0, whose first member is the pending zero (pseudo 47),
so one register serves all of them (r4, 6 refs). Fact 85's lever does not apply: the zeros are
equal through the constant class, not through a copy. Tried: `u32 zero` / `u8 zero` locals, a
literal `p->pending = 0`, `animationFlags = 0` passed in as y, the inline returning `out->y`, and a
separate `height` local (with or without the `zero` argument). Next: a second zero that cse1 cannot
see as const 0 in SImode (a halfword-mode value, or a bitfield store's zero, as in fact 25), but
that still folds to `movs r4, #0` later.

### Vanargand_ApplyPendingDamage (src/fn/sub_081ACC24.c, 081ACC24): deferred

Pins: `partDamage` in sl, plus a barrier after it, and `mask` (1) in r2 for the count == 0 setFlags.
Draft: `notes/phase3-drafts/monsters-bosses/Vanargand_ApplyPendingDamage.c` (7 ROM / 7 draft).
partDamage takes sl without the pin. The barrier only added one insn to damage's live range: damage
(5 refs over 47 insns) and the health address (5 over 48) were 2127 against 2083, and the ROM gives
the health address r4. `if ((u8)p->part.flags & 6)` (or `(u16)`) adds that insn naturally and fixes
the pair (18 → 7). Plain spellings (`& (2 | 4)`, `!= 0`, an inline test) do not.
What remains: cse1 replaces the setFlags mask with pseudo 107, the display call's stack argument
`1` (line 101). Its path runs from line 98 through every skip down to the count == 0 block. 107 then
gets no register (priority 238), so the mask is reloaded after the `ldr` (r3, since count is in r2).
The ROM has a block-local mask in r2 loaded before the address, with count in r3. Tried: mask
locals before the call, after it, before the accumulated block, and `return mask`. Next: break cse1's
path between line 101 and the count block, or find a mask spelling that is not in const 1's class.

### PlayerParticleCloud_ScatterAnchors (src/fn/sub_081D3620.c, 081D3620): deferred

Pin: `register u32 random asm("r0")` in the loop.
Draft: `notes/phase3-drafts/monsters-bosses/PlayerParticleCloud_ScatterAnchors.c` (12 ROM / 12 draft, swap only).
Both scatter blocks sit in one basic block, so random and coordinate are local-alloc pseudos.
random has 8 refs (4 mentions, doubled by the loop) over 8 insns, which gives priority 30000.
coordinate has 12 refs over 8 insns, which gives 45000. coordinate is allocated first and takes r0.
`AGBCC_PRI=random:+15000,random#1:+15000` matches. A tie is enough, because random has the lower qty.
So random needs 6 mentions, or coordinate needs 4, with the same lives.
`(u16)origin + bias` still expands into two sets of coordinate.
Tried without effect:
- one-line sums, `ParticleRandom_NextSample`, the READ_SAMPLE macro
- u16/s32/u32 random, u16/u32 coordinate
- `coordinate += random & mask`, `random = *sample & mask`
- storing `coordinate + random` directly (a temp pseudo then takes r0)
- an inline helper returning the sum
Next: find a spelling where random's value passes through one more set before the add, without a new pseudo.

### Ratatosk_UpdateSunOrbRelocation (src/fn/sub_081F8988.c, 081F8988): deferred

Pins: an offset in r2 with a barrier and a `dest` pointer in r1, used for case 1's `p->sequence = 2`.
Draft: `notes/phase3-drafts/monsters-bosses/Ratatosk_UpdateSunOrbRelocation.c` (plain `p->sequence = 2;`,
8 ROM / 8 draft). It is only reload's rotation: `AGBCC_SPILL=15:1` matches.
The records (`R`/`X` lines in names.txt) show choice 13 (case 0's `sequence = 1`, r0).
Choice 14 is case 0's fading copy `p->finished = 1` (=388 in r2, with r1 held by the address pseudo).
Choice 15 is case 1's `sequence = 2`, which starts after r2 and takes r0. The ROM's copy also has r2 there.
So the ROM made one more choice, taking r0, between that copy and case 1's store, with identical output.
The candidates are the copy's stored `1`, or case 1's `q->input` address (now local pseudo 167 in r0).
Either one would have to be a reload rather than an allocated pseudo.
Tried:
- `goto fail` for the fading copy: the rotation is fixed, but the ROM keeps the copy.
- an inline copy for `slot < 0`
- inverting the fading test
- `goto done` before `++p->elapsed`
Next: find a spelling where one of those two values reaches reload unallocated.
### AreaEffect_AddSpriteFromScript (field-effects, open)
Plain draft without concessions in notes/phase3-drafts/field-effects/AreaEffect_AddSpriteFromScript.c:
19 insns differ, all register choice. Global alloc runs manager (priority 1604: 10 refs, live 187)
before the 0xA8 stride constant (1600). manager needs a live length of 188+ insns, or the stride
needs a 5th ref. With the item_container-style groundHeight/cellIndex inlines, the barrier is not
needed, and gcse puts &manager->count in r8 without the pin.

### TetherActor_UpdateBounceReturn (field-effects, partial)
19 -> 4 pins. Left: the incremented frame is zero-extended twice in the ROM (in plain C cse shares
the two; switch and mixed cast/memory forms don't help), and r6 holds 0x380 in the falling arm.

### VulcanCannon_UpdateFacingTurn (src/fn/sub_08207350.c, 08207350): deferred, one cse merge left

Landed: 12 concessions, a hand-unrolled step routine and VC_* selector macros for `vulcan_cannon_worker.h`.
Draft: `notes/phase3-drafts/monsters-bosses/VulcanCannon_UpdateFacingTurn.c` (5 ROM / 3 draft).
The draft is plain C: the shared `DisplaySequence_StepSprite` inline and `if (!p->facing) p->facing = 1; else ...`.
Everything matches from the step routine to the end.
What remains: `if (!p->allowTurn)` records allowTurn's pseudo (27) as equal to 0 on the fall-through.
cse1 (and cse2 again, so `CSE_NOCANON=27` alone does nothing) then stores the call's fifth argument `0` from 27.
That keeps 27 alive into r4. The ROM loads allowTurn into r1, lets it die at the `cmp`, and stores a fresh `movs r2, #0`.
Any non-equality test (`allowed <= 0` on an s32) avoids the merge but branches with `bgt`.
Tried:
- `(u8)allowed`, `& 0xFF`, `== FALSE`
- a zero local set before the test (it becomes r8 or folds back)
- a restart inline helper, and a result variable
Next: an EQ test whose operand cse cannot record, or a label in front of the not-allowed block (it starts a new EBB).

### EnemyParticle_UpdatePool (src/fn/sub_0820D2C4.c, 0820D2C4): deferred, StepSprite draft 12/16

Landed: 8 concessions in a hand-unrolled sequence step.
Draft: `notes/phase3-drafts/monsters-bosses/EnemyParticle_UpdatePool.c` (12 ROM / 16 draft).
It calls `DisplaySequence_StepSprite(&cursor->sprite, &cursor->anim)` and has no concessions in the step.
The ROM has three long-lived pointers: base in sb, an add-0 pointer in r8, and `ground` (base+0x54) in sl.
pool sits at [sp], i at [sp+4], and the `sprite = o` copy at [sp+8].
Inside the step, the first flag block and the command reads go through r8.
`sprite->frame` and the second block go through the spilled `sprite` (r6) and through base+0x18 (r4).
The draft's reads through r8 match, but its step pointer values come from a spilled `cursor`
(it takes [sp+4], which pushes i to [sp+8]), and `sprite` is dead.
Passing `sprite` or `&base->anim` instead changes loop's lead giv (base+0x18) and the whole allocation (29-72).
A single `base` pointer, a per-iteration `cursor = base` copy, and `&pool->e[i]` are all worse (41-116).
Next: work out whether r8 is a cursor biv or loop's reduced giv (try `hoist`), then the C that puts sprite/state on the base side
while the reads stay on r8.

### OrangeSpine_ApplyHitReaction (src/fn/sub_0820EE34.c, 0820EE34): deferred

Landed: 2 pins and 4 barriers (r0 and r1 dummies around two statements).
Draft: `notes/phase3-drafts/monsters-bosses/OrangeSpine_ApplyHitReaction.c` (36 ROM / 34 draft).
It is the matched DragonPuppy_ApplyHitReaction (0820AD84) form, which covers 193 of 196 insns.
That relative stores `reaction = 0x26` inside the 0x40 branch; OrangeSpine doesn't.
The ROM keeps r0 and r1 busy across `target->collider.responseValue = source->responseValue`:
the value goes to r2, the address to r1, and the 0x40 mask reload to r3.
So r3 joins reload's spill set (ROM r0-r3 plus r7; draft r0, r1, r2, r7), and every later choice rotates.
`AGBCC_FORCE=62:2` fixes the value register only; the spill set still lacks r3.
Tried:
- a `u16 reaction` temp
- a mask local
- `!= 0`
Next: find what the original kept live in r0 and r1 across that store. The pinned dummies stand in for it.

### sub_08217430 (src/fn/sub_08217430.c, 08217430): deferred

Landed: a `bank` pin in r3 and a barrier keeping `offset`/`base` live.
Draft: `notes/phase3-drafts/monsters-bosses/sub_08217430.c`
(`u8 *bank = gPaletteBankData + (gObjPaletteKeys[i] << 5);`, 8 ROM / 6 draft).
The copy `r0 = bank` before CpuFastSet gives bank (pseudo 33) a copy suggestion of r0.
Local-alloc's first pass hands it r0 ahead of the higher-priority `=gObjPaletteBuffer` temp (44), which then takes r2.
The ROM has bank in r3, the temp in r0, and `adds r0, r3, #0` before the call.
`AGBCC_FORCE=33:3` gives 2/2. What remains is the offset/base pair (global 39 in r1, ROM r0).
Tried:
- the expression inline in the call, `(u16 *)`, a separate dst local
- `ObjectFade_SourceBank(i)`
- inline copy wrappers (worse: the base address is no longer hoisted)
Next: C where bank does not die in the r0 copy (or is not a plain copy), so it gets no suggestion.

### GameState_SelectRamBase (src/script_virtual_machine.c, 0821B084): deferred, one operand order

Pin: `base` in r1.
Draft: `notes/phase3-drafts/monsters-bosses/GameState_SelectRamBase.c` (1 ROM / 1 draft).
cse swaps `base += off` to `(plus off base)`, so the draft prints `adds r1, r0, r1`; the ROM has `adds r1, r1, r0`.
update_equiv_regs doubles base's refs (its first set is a constant).
Tried, all with the same result:
- `= gGameStateStorage + off` and `&arr[off]` (worse: the base load moves late)
- `u32` arithmetic, `off + base`, `<< 3`, `% 128`, an offset local
Next: a first set of base that is not a constant equivalence, or a form cse leaves in order.

### Hresvelgr_EmitFacingMotion (src/fn/sub_0819BCB8.c, 0819BCB8): deferred

Pins: `quotient` in r1 with a barrier, and `angle` in r3 inside `Polar`.
`Polar` is shared with MotionPolar's users, and `Add` with Hresvelgr_SetPartApproachMotion.
Draft: `notes/phase3-drafts/monsters-bosses/Hresvelgr_EmitFacingMotion.c` (the pin-free landed form, 48 ROM / 43 draft).
A natural rewrite is much worse (70): `(rnd >> 3) % 2`, `% 1024`, `MotionPolar`, `Add`, `% 4`.
The ROM keeps the relative-sample origin (0x0403B714) live across the first two samples,
and the third sample reads relative to it. The landed C reproduces that with `origin`/`savedOrigin`.
Not worked on further this session.

## dp-bosses (Codex)

Tags: #matching #phase3 #player #bosses

### sub_0813D834 (0813D834): cse follow into a singleton case

The original two pins remain. The best new pin-free draft is
`notes/phase3-drafts/dp-bosses/sub_0813D834.c`: 0 ROM / 2 draft instructions
(the extra initial `cmp kind, #1; beq bonus`), versus the old 17/15 draft.
The entire body has the ROM's registers and instructions. Persistent search:
`build/matchsearch/sub_0813D834`, nodes `second-entry` and `case-zero-fallthrough`.

Confirmed `CSE_NOFOLLOW=2` gives MATCH. Read `cse_end_of_basic_block` in cse.c:
a case label with one incoming branch and a preceding BARRIER admits the
switch equality. Giving case 1 a second incoming branch blocks both passes;
the duplicate comparison survives. Making case 0 conditionally fall through
also blocks the follow, but its mode test survives (7/9). Repeating that test
in both call/skip arms reduces it to 0/2 plus two moved branch targets.

Tried direct and narrow parameter types, dispatch casts, union/array/address
forms, precomputing the scaled offset, reordered cases, if/else and separate
if dispatch, adjusted switch constants, default fallthrough and second entries
from case zero/default. Dispatch narrowing/offsets preserve the body but add
conversion/arithmetic instructions. Address-taking and ordinary inline readers
do not hide the switch equality. No one-pin partial found: removing the scaled
pin ties its shift to hard r3 and changes the earlier reload order.

Next: a genuine shared path into case 1 whose second edge becomes redundant
after cse, or an original helper/control-flow structure that puts a non-barrier
predecessor before the case label. Scope `why all` does not attribute the dead
argument copy and constant timer address to the switch equality; `probe` does.

### PlayerAction_RefreshFlagState (0813EDDC): one moved argument copy

The original seven concessions remain. New pin-free draft:
`notes/phase3-drafts/dp-bosses/PlayerAction_RefreshFlagState.c`, 1 ROM / 1 draft
plus one moved branch. The only difference is `mov r0, r8`: before the sign
test rather than immediately before GameState_GetConsumableDescriptor in
its nonnegative arm. All eight previous wrong registers are now correct.
Persistent search: `build/matchsearch/PlayerAction_RefreshFlagState`, node
`joined-choice` (baseline also retained).

The original pin-free draft's AGBCC_SPILL=5:1 gives MATCH, reconfirmed. Read
allocate_reload_reg in reload1.c and the R/X trace: four choices build the
clear-loop addresses/mask, choice 4 reloads 0x418 into r2, choice 5 then
chooses r0 for the alternate test; the ROM chooses r1. Assigning a `choice`
local from alternate on both passing p/non-p guard paths creates the extra
choice that starts the correct rotation, but its argument copy is too early.

Tried sign-bit/shift comparisons, null-safe and plain inline mode/slot
accessors, label and reversed conditional forms, duplicate full alternate
selection, initialization before the guard, separate test edges, field vs
pointer bonus stores, array and SI zeros, a separate bonus-loop counter,
union/addressable/narrow choice, call wrappers and call-only/test-only choice
uses. No exact subset of the existing choice/zero/current pins tested here
matched, so no partial landed. The copies' order is not explained by `why`;
it reports no supported causal summary, while the trace shows the source
copy survives into reload. Next: keep that reload choice but delay or delete
the copy after allocation, or recover the missing original access around the
mode/slot guard. Do not add a fence to manufacture the choice.

### sub_0815E034: direct defense addresses isolate the mask priority
Tags: phase3, player, sub_0815E034, allocation

The new pin-free draft is `notes/phase3-drafts/dp-bosses/sub_0815E034.c`:
6 ROM / 4 draft instructions differ, 440 versus 444 bytes. All long-lived
registers and all defense switch cases now match. It reuses `common` in the
blocked-mask arm, then assigns it again for the vulnerability mask. The extra
references raise its priority to 2500. Remaining differences are the first
mask's register/order and regmove tying the final mask test to r6.

A stronger diagnosis: starting with the old draft, replace each switch offset
with `attribute = &player->cueDefenses[k]` and remove the post-switch pointer
addition. `AGBCC_PRI=common:+1000` makes this direct-address draft **MATCH**.
Player's nine references give priority 2231, source 2061, and common 1500;
common must outrank both. Its REG_EQUIV constant doubles live length 10 to 20.
This also fixes the offset/attribute register issue through natural reload and
cross-jumping, so it is one allocation decision rather than two unrelated pins.

Tried narrow/wide common and vulnerable types, masked load temporaries, inline
mask helpers, union/array mask initializers, mask setup in both blocked arms,
separate mask assignments and precomputed element tests. Narrow common types
fold to the same RTL. Wide vulnerable types change allocation adversely.
Repeated initialization in an identical branch gives 7/7 but moves the mask
past the 0x40 test; it is not a good final form. The direct-address draft with
all existing source pins removed still needs common's priority raised.
Source-pin-only partials with typed offsets, helper reads, indexing and union
pointer initialization remain 8/8; no source change landed.

Next: give common a short-lived extra real reference that disappears after
allocation, or prevent REG_EQUIV doubling without an artificial fence. A mask
helper with the original aggregate representation may supply that reference.

### PlayerContact_ApplyResponse: shared duration selection, 6/6
Tags: phase3, player, PlayerContact_ApplyResponse, cross-jumping

New draft: `notes/phase3-drafts/dp-bosses/PlayerContact_ApplyResponse.c`.
Compute `duration = power + 60`, assign `power = duration` in the engine-flag
arm and `power += 60` in the other arm, then make one shared reset call and
counter store. This keeps the dead engine AND and reduces 13/22 to 6/6, with
the original 1100-byte size. Only the duration section differs; later code is
exact. The ROM keeps a dead r3 duration computation and computes power+60
again into r1; the draft keeps the first computed value in r2 and copies it.

`probe` tried all 16 canonical choices, 23 jump follows and one PRE replacement
without improving the count. `why` forcing power to r2 improves to 5/3, but
does not reproduce the second addition. Moving the counter store outside two
calls still leaves the calls unmerged. Duration in either call arm, separate
selected locals, redundant reassignment, inverse subtraction, a union duration
and rereading source power all retain the same issue or collapse the dead test.
The old source with only its pin removed is 8/6; u16/s32 power and declaration
order do not improve it. Original source retains both concessions.

Next: find the C form that lets the initial duration value die only after
allocation while the argument still uses the original power. The key is the
second addition, before considering reload rotation.

### TargetRegistry_FindById: 5/5 allocation-only; duplicate compares still cross-jump backward
Tags: phase3, dp-bosses, TargetRegistry_FindById

Draft: notes/phase3-drafts/dp-bosses/TargetRegistry_FindById.c. AGBCC_PRI=requestedId:+2484 makes the single shared comparison MATCH. The earlier duplicate-comparison draft already allocates correctly but jump2 shares the collider tail into the actor tail. Explicit found/missing labels, returns versus breaks in each arm, and an actor block outside the loop do not reproduce the ROM layout. Narrow flag/mask types, boolean flag extraction, aggregate flag temporaries and inline comparison helpers do not solve allocation. Union/struct/array mask temporaries fold to the same RTL. Original source retains its one pin. Next: an ID reference that survives through flow while preserving the shared comparison, or a real flag accessor that prevents the constant mask tie and REG_EQUIV lifetime doubling.

### sub_0819BB18: improved from 24/23 to 4/4; only invariant order remains
Tags: phase3, dp-bosses, sub_0819BB18

Draft: notes/phase3-drafts/dp-bosses/sub_0819BB18.c. Build the bank through ParticleRandomTableSource halfword stores before the loop, then read it through ParticleRandom_GetTable. This retains the 0x0203B000 base and computes the first rebased table in the preheader, leaving the later rebasing in the loop as the ROM does. CSE_NOCANON=229 removes an extra copied base in the second sample; ordinary C table++; table += 511; in the sampling inline reproduces that effect. The entire body is exact. Four differences remain: the gRandomIndex address is hoisted before the reduced counter initialization instead of after it, changing the two temporary reload registers. The result is the original 416-byte size. Symbol/union loop initializers, direct bank indexing, table temporaries, alternate shared readers, countdown/do/goto loops, early counter initialization and a seed argument do not reproduce the ordering. The source retains four barriers. Next: control the relative placement of the loop-invariant address and reduced counter initializer without changing the exact body.

### Hresvelgr_EmitFacingMotion: origin survives naturally; remaining allocation is not isolated
Tags: phase3, dp-bosses, Hresvelgr_EmitFacingMotion

Draft: notes/phase3-drafts/dp-bosses/Hresvelgr_EmitFacingMotion.c. Build the origin through ParticleRandomTableSource halfword writes and retrieve it with the shared accessor. This retains the otherwise folded origin and gives 44/44 differences versus the earlier 48/43; size is 368 bytes. Replacing either or both early direct reads by shared relative reads folds to exactly the earlier RTL; ReadDirectSample also does, while ReadSample changes allocation adversely. Union origin/quotient temporaries do not help. All 16 CSE canonical choices, five jump follows and five PRE choices fail to improve this draft. The original still has two pins and one barrier. Partial angle-pin removal with reordered Polar parameters, a mask local and a plain x pointer also fails. Next: isolate quotient lifetime before the origin copy and the saved actor copy; these affect global register pressure independently of the later polar-angle pin. Helpers shared with Hresvelgr_SetPartApproachMotion must be checked with the whole unit if changed.

### Hresvelgr_UpdateTurningCharge: 10/8; PRE still reuses the first flags address
Tags: phase3, dp-bosses, Hresvelgr_UpdateTurningCharge

Draft: notes/phase3-drafts/dp-bosses/Hresvelgr_UpdateTurningCharge.c. AGBCC_NOPRE=1 makes the original pin-free draft MATCH. Duplicating the first real flags update in both pending arms improves 16/14 to 10/8 but PRE still preserves the computed address across the callback. A separately assigned owner and an inline initialization helper with two real returns both fold to the original baseline. Reconstructing only the second owner pointer from its two halfwords defeats PRE and leaves 3 ROM / 9 draft, but adds six reconstruction instructions; it is scratch evidence rather than the preferred draft. The ordinary branch-local update draft is retained. Next: another real base representation that becomes the original pointer after gcse but before allocation, or the original owner reassignment. Original source retains the address pin.

### Hresvelgr_PlayDescent: 11/5; actor/input copy still propagates away
Tags: phase3, dp-bosses, Hresvelgr_PlayDescent

Draft: notes/phase3-drafts/dp-bosses/Hresvelgr_PlayDescent.c. The prior ordinary draft remains best at 11 ROM / 5 draft. Returning the actor from an inline descent body, either assigning or ignoring its result, does not preserve the copy: expansion drops the unused return. Putting the final refresh arm before the landing stores changes the branch layout and still merges input and actor. The ROM needs input in r7, actor in r6 and tick in r8. The source retains its one operand barrier. Next: find a real aggregate owner representation or control-flow join that survives copy propagation but folds before allocation; moving only the final use order is insufficient.

### Hresvelgr_UpdateBoundaryBarrage: 2/2; random-mask retry allocation still selects sl
Tags: phase3, dp-bosses, Hresvelgr_UpdateBoundaryBarrage

Draft: notes/phase3-drafts/dp-bosses/Hresvelgr_UpdateBoundaryBarrage.c. The prior pin-free 2/2 draft remains best. Shared ReadDirectSample produces the same RTL and difference; ReadSample increases register differences. Building the random mask through halfwords substantially changes the loop code, and rebuilding only the normal collision mask changes other allocations without fixing the retry choice. The source still retains its end-of-function mask operand. Next: expose why retry_global_alloc considers sl available despite the earlier collision/action mask using it, and find a genuine later use of that value or another pseudo that naturally conflicts with it in case 1. Keep this distinct from simple priority order: initial global allocation already chooses the intended low register before reload evicts it.

### sub_081A14FC: improved 86/82 to 30/32 with one shared sampler draw
Tags: phase3, dp-bosses, sub_081A14FC

Draft: notes/phase3-drafts/dp-bosses/sub_081A14FC.c. Use ParticleRandom_ReadSample only for rawY, retaining the other direct draws. Its extra counted table reference raises the table above the random-index address, fixes the frame size/slots and improves the pin-free draft from 86/82 to 30/32. Using it for every draw overshoots badly; direct shared reads fold to the baseline. Each individual draw was checked against the table-priority hypothesis, and increasing the sampler count to two through five is worse. Keeping the sampleY/rawY pins and barrier while removing only the table pin reaches 11/13, but removal combinations of those three remaining concessions do not match. The principal remaining difference is a resident 0x3ff mask in ip, while the ROM reloads it and uses ip for oy; gcse also moves the index-address copy later. Original source retains all four concessions. Next: displace the index mask naturally while retaining the corrected table priority; then revisit sampleY/rawY lifetimes.

### Vanargand_ApplyPendingDamage: partial landed, one mask pin remains
Tags: phase3, dp-bosses, Vanargand_ApplyPendingDamage

Removed the partDamage/sl pin and its operand barrier. The existing u8 narrowing
of the part flags supplies the necessary live-range instruction, and retaining
only the original r2 mask pin gives an exact whole-unit MATCH. The full pin-free
draft is saved under dp-bosses and remains 7/7. Halfword/array mask construction,
a shared return-mask local and count-label forms do not fix it. Splitting the
real song call between the special/non-special arms changes the CSE path and
gives 5/8, but leaves duplicated call/layout instructions. Next: reproduce that
path break while restoring the shared song call.

### sub_081D3620: 2/2; correct registers but bias addition attaches to random
Tags: phase3, dp-bosses, sub_081D3620

The unchanged source retains its single random pin. The old exact-shape draft allocates coordinate before random; AGBCC_PRI=random:+15000,random#1:+15000 confirms the allocation diagnosis. Applying bias to the masked random instead gives every ROM register naturally, but moves each bias addition from coordinate before the mask to random after it (2/2). Both signed and unsigned 16-bit random, unsigned coordinate, separated coordinate/bias statements, and an accumulator random retain or worsen allocation; 16-bit coordinate adds truncations. A union assembled from masked halfwords adds reconstruction instructions. Next: a real random-use form that increases its flow references while combine restores the coordinate-first bias addition. The draft changes no behavior: only the low halfword of the sum is stored.

### Ratatosk_UpdateSunOrbRelocation: 8/8; reload choice 15 confirmed, failure-tail sharing fixes allocation
Tags: phase3, dp-bosses, Ratatosk_UpdateSunOrbRelocation

All three original concessions remain. Confirmed AGBCC_SPILL=15:1 matches: choice 14 loads the finished offset in r2; choice 15 consequently starts after r2 and takes r0 instead of ROM r2. Replacing the early fading failure store/return by goto fail removes choice 14 and fixes every later register, but jump2 removes four instructions that the ROM duplicates. Writing out the negative-slot failure adds two instructions without changing allocation. A shared terminal return label changes nothing; a common final finished store changes layout substantially. Using q for the early finished store adds one register difference. A halfword-built finished constant adds 14 instructions. Next: retain the early failure tail while changing its reload inheritance, or prevent jump2 from sharing the second failure tail after arranging the right reload rotation.

### sub_08217430: 3/3; widened bank breaks the argument preference, base still coalesces
Tags: phase3, dp-bosses, sub_08217430

The original pin and operand barrier remain. Confirmed forcing baseline bank pseudo 33 to r3 gives only 2/2: the loaded base remains tied to bank instead of r1. Widening the completed address to u64 before truncating it for the call restores the missing argument copy naturally and gives a same-size 3/3 draft. This is diagnostic rather than a convincing original C form: the doubleword occupies r2/r3 and its low word coalesces the loaded base. Copy helpers returning source or destination, a source/control descriptor, union assignment and initializer, and a range-copy helper all collapse to the baseline. Splitting the offset changes shape; widening the offset or doing the addition in u64 worsens it. Integer-address addition and the retained-bank-pin variants did not match. Next: a source representation that prevents both the argument-copy preference and the base/result tie. Scope also failed to identify the doubleword holder of r3 because its holder search compared only the starting hard register.

### GameState_SelectRamBase: 1/1; cse deliberately moves the known constant base to the second operand
Tags: phase3, dp-bosses, GameState_SelectRamBase

The single original base pin remains. Read fold_rtx in gcc/cse.c: after obtaining equiv_constant for each operand, it explicitly places a constant second in commutative operations. This is operand ordering, not the allocation conflict that scope why reports. Array-of-eight-byte slots and u64 slot pointers retain the 1/1 difference. A union initializer or one-element pointer array changes allocation to 15/15; widening the address gives 28/28. Reassembling a union pointer from two halfword reads hides the constant from CSE and fixes the add order and all downstream registers, but five reconstruction instructions plus alignment remain. Next: a nonconstant first definition that combine reduces after CSE without reconstruction instructions. Scope incorrectly printed cse replaced nothing by nothing and recommended swapping allocation for a pure commutation; this is a diagnostic bug to fix.

## dp-effects (Codex)

Tags: #matching #phase3 #effects

### sub_08012B34 (08012B34, src/debris_particles.c): partial, origin pin remains

Removed the operand barrier using `ParticleRandom_ReadRelativeSample(setup, gRandomTable)`
for subsequent draws. The complete unit still matches; only the existing r3 origin pin
remains. The sample origin is now named `PARTICLE_RANDOM_SAMPLE_ORIGIN`.

Best ordinary-C source: `notes/phase3-drafts/dp-effects/sub_08012B34.c` (complete unit),
20 ROM / 20 draft, shape 0. It follows DirectionalParticle_Spawn and
FragmentEffect_SpawnRandomParticle: symbolic origin `(u32)&gHeapHead +
RESOURCE_REGISTRY_ENTRIES_ADDR`, a saved copy after the x-product, and shared relative
sampling for the vertical component. `AGBCC_PRI=setup:+2791` gives MATCH. The source's
angle wins r3 first (priority 2926); setup is 136. The two constant sets multiply setup's
flow live length by four in `update_equiv_regs`, as confirmed in local-alloc.c.

The earlier `setup & 3` draft also remains 20 / 20 and matches with
`AGBCC_PRI=setup:+2908`; the new draft uses real sampling arithmetic instead.
All later draws through the relative sampler extend setup's life but cse shares its
parameter copies, leaving only three counted refs. A first-sample-only relative use
plus the saved origin shortens the life, but does not change the allocation order.
Branch-local origins and the existing capture macro collapse into the same global
pseudo. Duplicating the full tails gives separate origins, but gcse changes random
cursor stores and jump2 leaves excess code. A split-half union keeps too much insert
code; zero-initializing it lets cse2 rebuild the same constant and its equivalence.
Constructing the origin within the first-sample helper, including union views, lets
cse eliminate the dead load before flow. Byte and halfword angle forms add or reorder
instructions without fixing the allocation.

Persistent evidence: `build/matchsearch/sub_08012B34` (source hypotheses and compiled
results); experiments: `build/cx/work/sub_08012B34/`. `why all` and the matching priority
knob confirm that the other differences are consequences of origin/angle placement
and reload rotation. Next idea: recover a first-sample helper whose short-lived origin
use survives until combine, allowing local allocation to reserve r3 before the angle.
No new concession or compiler override was landed.

### sub_0801404C (0801404C, src/fn/sub_0801404C.c): partial, table barrier remains

Removed three of four barriers. Reading coordinates through `CoordinateBits` keeps
PRE from sharing the early signed x load with the later motion load. A local graphics
struct names the resource tile field. Splitting the two presentation fallback paths,
repeating their tails and jumping to one `age` label gives resource 10 references
across two calls. GCC's caller-save profitability test is `4 * calls < refs`: the
original ordinary form has only 8, so resource takes r8 instead of caller-saved r3.
The repeated tails cross-jump after allocation. The complete unit matches with the
original table barrier and direct sampler; no other barrier remains.

Best pin-free unit: `notes/phase3-drafts/dp-effects/sub_0801404C.c`. Shared
`ParticleRandom_ReadSample` retains the table but leaves only 7 ROM / 7 draft
instructions different: sprite and table swap sb/sl. `AGBCC_PRI=sprite:+34` matches.
At this point resource is r3 (10 refs, life 204, priority 1470), random-index address
is r8 (13 refs, doubled life 570, priority 684), table is sb (10 refs, doubled life
576, priority 520), and sprite is sl (7 refs, life 287, priority 487).

Tried: direct/value coordinate helpers and by-value vectors; only the bit helper
keeps the ROM load order. Duplicating presentation tails without the join label
leaves extra tails. Sprite cursor derivation from the particle or indexed group
changes induction/initialization. Typed records and a union cursor give the same
7/7. Repeating cursor advancement in two or three arms raises it above random-index
and leaves unmerged additions because reload rotates their scratch registers.
Repeating particle advancement too changes loop control. A for loop is unchanged;
a goto loop loses useful hoists. One direct sample reduces table references enough
that the hoisted -256 mask beats it and the table is rematerialized. The barrier plus
table-first samples over-prioritizes table; the barrier plus direct samples matches.

Persistent evidence: `build/matchsearch/sub_0801404C`; all source experiments in
`build/cx/work/sub_0801404C`. Next: add one sprite reference that disappears after
flow, or reduce table density slightly while keeping the -256 mask below it. A
sampler or cursor helper with a useful late-eliminated copy may supply that effect.

### ParticleGroup_UpdateFrozen (08014330, src/fn/sub_08014330.c): deferred, combine removes byte extension

Best complete ordinary unit: `notes/phase3-drafts/dp-effects/ParticleGroup_UpdateFrozen.c`,
3 ROM / 1 draft, shape 2. It preserves the earlier field-effects result: PRE creates
one QImode reaching register, with the initial r0-to-r3 copy and reloads after both
terrain-call paths. Combine deletes the final unsigned-byte shift pair and compares
r3 directly instead of converting to r0 first. Confirmed exact with
`AGBCC_COMBKEEP=104`, the left-shift result. `why 268` formerly gave misleading
allocation advice; the scope improvement in fact 172 identifies this deleted conversion.

Read combine.c `set_nonzero_bits_and_sign_copies`, `nonzero_bits`, and the existing
COMBKEEP instrumentation. A PRE byte loaded/copied in every arm has known-zero upper
bits. The original barrier supplies an unknown upper-bit definition. Byte-return,
narrowing, signed-byte-return, and union-byte helpers do not preserve the extension;
a word-return helper also changes the initial compare. Header bitfields and a signed
field with unsigned casts are unchanged. Explicit byte caches lose the initial copy.
Explicit wide/signed caches retain the late extension but add a halfword/signed load
before the top byte load (2/2 or 2/4). Word/byte union representations, partial union
insertion, and halfword unions either lose PRE or add loads. Repeated helper returns
add control flow. Explicit shift reuse changes the initial compare and still deletes
the final conversion. Packing priority or the invalid cell into unused upper bits
keeps unwanted arithmetic. These are compiler hypotheses, not candidates to land.

Persistent source evidence: `build/matchsearch/ParticleGroup_UpdateFrozen`.
Scratch: `build/cx/work/ParticleGroup_UpdateFrozen`. Next: a genuine header/cache
representation that makes an SI-to-QI copy with unknown upper bits before combine,
but whose wide producer becomes the same byte load after combine. No source changed.

### sub_08014634 (08014634, src/fn/sub_08014634.c): partial, 11 pins and one barrier removed

The stock compiler matches the complete 664-byte unit after removing eleven of
fifteen pins and the entry-offset barrier. Reusing an ordinary option scalar for
both masking and the later tile lookup first removed the mask, manager, and entry
pins. Typed union views then reused that scalar for the count clamp, count header,
and both position reads (five pins); reused the completed manager lookup for the
options header (two pins); and reused priority/bound storage for the particle
cursor (one pin). Keep initialization order when separating C89 declarations.
These ordinary reuse forms carry phase-4 cleanup debt; no concession was added.

Four pins remain: small/large resource selection, counter, and table; barriers
remain on options memory, resource temporaries, and table/group. Removing both
resource pins and barriers leaves 4/4, shape 0: small uses r0 instead of r2 and
large uses r2 instead of r0. CSE2 substitutes the spilled resource for the reused
option view. The spill sweep found no matching rotation; forcing resource to r2
changes allocation throughout the function. Repeated post-join initialization in
each branch either keeps the swap or introduces an outer-loop entry check. An
explicit resource/pool struct becomes a multiword register value and changes the
whole allocation.

Removing only the counter pin leaves 19/19, shape 0. Exact diagnostic:
`AGBCC_PRI=i:+864`. Its initial zero doubles the measured lifetime: 32 refs,
flow life 190 (effective 380), priority 4210, versus group 26 refs/life 205,
priority 5073. A nonconstant first definition or seven additional counted refs
would reverse their order. Removing the group barrier makes i correct but swaps
group/table and adds a table copy (14/14, shape 2). Repeating the particle tail
changes unrelated allocation. Repeating inactive cleanup in both bound arms gets
all registers correct, but the zero-count path simplifies differently and leaves
an extra cleanup loop (1/13), so jump2 cannot share it.

Best fully ordinary alternative: `notes/phase3-drafts/dp-effects/sub_08014634.c`
(71/67, shape 58), based on the matched radial spawner sub_080148CC: inline wrapper,
ReadScalar/ReadOrigin helpers, and a wide origin copy. Table priority +1400 improves
it to 41/39 but is not an exact diagnosis. The table constant has doubled lifetime
and loses r7 to velocity; table-first/shared samplers, halfword return, table-half
construction, union initialization, count-up loops, scalar argument reordering,
and struct argument views did not recover the full match. Removing all remaining
concessions from the latest partial gives 90/86, shape 40; preserving the simpler
relative-based rewrite is more useful than its extra unions.

Persistent graph: `build/matchsearch/sub_08014634`; scratch candidates and compiler
traces: `build/cx/work/sub_08014634`. Next: shorten the counter lifetime without an
extra early set, or recover the resource helper whose separate branch destinations
survive CSE2. Then solve table equivalence and the options/count reload boundaries.

### sub_08014CA4 (08014CA4, src/fn/sub_08014CA4.c): partial, output pin and initialization barrier removed

The complete 704-byte unit matches after removing the lifetime-output pointer pin
and the three-operand initialization barrier. A shared output pointer in the fixed
and randomized lifetime branches receives r1 naturally. Split-half random-table
construction removes constant equivalence and recovers table allocation without
its barrier. Its uninitialized old-input USE initially created an extra sp+0x20
slot, shifting every stack parameter by four bytes. Reusing a typed union first
for the loop-bound test and then for the table halves removes that extra slot.
Facts 173/174 and the new scope frame explanation record this result.

Four pins and two barrier statements remain. Removing the speed-range pin and
barrier gives 17/17, shape 0; `AGBCC_SPILL=20:2` is exact on `spread_free.c`.
CSE replaces the named spread with the incoming speedRange pseudo, and the first
reload after the lifetime branches takes r0 instead of r2. Repeating the following
range load in each arm (with and without a join label) is unchanged. Repeating the
whole speed tail changes allocation and leaves duplicate code. A shared lifetime
byte/spread scalar, and using spread in the modulo call, still canonicalize to the
parameter. This remains a source-level reload-history problem.

After the initialization fix, removing the lifetime cursor pin leaves 11/11,
shape 0. The cursor naturally receives sl, but CSE replaces the low output copy
with that high-register pseudo, changing reload choices in both lifetime arms.
The spill search improves to 6 differences but is not exact. Reusing the output
for the source-byte address makes lifetime address-taken and adds a stack copy;
using it for a later tile-table view is unchanged. Advancing life from out changes
loop allocation. Removing the fixed lifetime byte pin/barrier needs both its
byte access and separate output allocation; direct byte, QI/SI, and union views
retain either wrong local registers or unwanted shared tails.

Removing the base pin enables PRE for base+32 and base+44 (initialized/enabled
arrays). The isolated base-pin draft is 73/73 before the table fix. NOPRE=1,2
improves it to 35/31; table:+50 fixes its table/base priority swap but does not fix
PRE. Direct field-address unions do not prevent PRE; reconstructing the pointer
through halves leaves shifts/ORs. An indexed-array rewrite gets the table into r7
but loses the original outer cursor initialization. Shared table-first sampling
raises the random-index address too far; direct sampling remains mismatched.

Best complete ordinary draft: `notes/phase3-drafts/dp-effects/sub_08014CA4.c`,
43/45, shape 46. It combines defined-input split-half table construction with a
per-iteration group view. There is no exact full-draft knob yet. The table and g
still compete closely, and two field expressions create extra frame values.
Zero or aggregate table initialization restores constant equivalence. Reversing
the half stores keeps the old-input spill. Reading the union directly for every
sample prevents combine from folding its construction.

Persistent hypotheses: `build/matchsearch/sub_08014CA4-v2`; scratch and exact
partial candidates: `build/cx/work/sub_08014CA4`. Compiler source checked:
combine.c distribute_notes preserves a lost REG_DEAD input with a USE at a label
for caller-save tracking. Next: a low-register output copy that survives CSE
without a cursor pin, or a legitimate field view whose address forms only after
PRE. The fixed initialization and scope explanation are landed independently.

## dp-menus (Codex)

Tags: #menus #hud #dialogue #matching #compiler

### Presentation_SetItemGraphic (08019ECC, src/selection_menu.c)

Best ordinary-C draft: `notes/phase3-drafts/dp-menus/Presentation_SetItemGraphic.c`.
It has the original 76-byte extent, 6 ROM / 6 draft differing instructions. The
source match remains unchanged with two pins and two operand barriers.

The ROM loads 0x121 into r0, copies it to r2, loads the graphic byte into r3,
then adds r3 to r2. In the no-pin unit, cse1 rewrites insn 47 from
`resource + tile` to `tile + base`; combine substitutes the 289 constant, and
regmove/local allocation tie the result to the byte. `AGBCC_COMBKEEP=32` on
`build/cx/work/Presentation_SetItemGraphic/base.c` preserves the constant load
before the byte load (5 / 5), but not the ROM's copy. Adding `AGBCC_NOTIE=47`
or `CSE_KEEP=33` does not match. No complete matching knob was found; the copy
is a separate unresolved decision. `probe` finds only one cse jump follow,
whose suppression does not help. Allocation force knobs do not override the
block-local quantities involved.

Tried typed table indexing; u8/u16/s16/u32 channel/index values; a one-element
array and packed field; address-taking and inline readers; full-width and
halfword union views; a base constructed from its two halves; initialization
before the inventory call or before the existing test; separate inline helpers
for the base, frame addition and keyframe call; and partial removal of each
existing pin/barrier. None matches or removes a concession for free. This
extends the earlier menus-link effort rather than restarting its variants.
The half-constructed union adds a live incoming value and a high-register save,
so it is worse and was not retained as the recommended source.

Persistent proposals: `build/matchsearch/Presentation_SetItemGraphic-v2`,
including the typed-table, address, aggregate, inline and lifetime hypotheses.
The guarded statement-order/copy/temporary slate is empty for this source.
Next: expose cse's constant-copy substitution as a separate diagnostic decision,
then find a real frame-index representation whose base remains noncanonical
through that substitution. A shared statement wrapper invented just to fence
this one initialization would be a concession, so none was added.

Tool result: scope now resolves names in COMBKEEP, NOEQUIV and canonical-register
knobs. Previously `AGBCC_COMBKEEP=base` silently did nothing; it now reports that
the optimized-away name is unavailable and requires its RTL pseudo number.


### sub_0803198C (0803198C): volatile read removed, callback pin remains

Tags: #menus #dialogue #matching #compiler

Landed partial: replace the volatile mode read with a switch on `p->mode`, with
both arms starting the same script. Jump2 merges the startup tails and removes
the test after flow; the dead byte load remains. `tools/check.py` reports MATCH
for the 948-byte unit. This corrects the earlier claim that only volatile can
produce that load. The callback pin is the only remaining concession.

Best completely ordinary draft: `notes/phase3-drafts/dp-menus/sub_0803198C.c`.
Two instructions differ: callback literal and store use r0 instead of r1.
`AGBCC_FORCE=261:1` matches on the mode-switch draft. Local allocation sees a
short block-local callback with no suggestion after the wait store kills both
r0 and r1, so it chooses r0. The desired address/store order already matches.

Additional C experiments: initialize callback before the wait store (gets r1,
but loads too early); move initialization across the value-list branch; duplicate
completion statements in both branches; use a shared callback variable or union
across the join; aggregate the wait and callback settings; reuse a union for the
wait and callback; evaluate the wait address before that union; use a nested wait
assignment; compare dispatch forms; and use direct or conditional callback stores.
The full-width union gets the callback in r1 but also puts the wait constant in
r1 before its address calculation (3 / 3). Address-first and nested-assignment
forms restore the wait sequence but lose the callback register again. A halfword
union creates partial-value preservation and extra instructions. No new partial
removes the remaining pin.

Read `update_equiv_regs` in local-alloc.c: it sinks a global constant's single
initialization next to its use when replacement fails; a same-block local does
not take that path. Across-branch spellings changed register pressure and code
order instead of producing the needed r1 suggestion. Persistent store:
`build/matchsearch/sub_0803198C`, with lifetime, join, aggregate and union proposals
and the exact diagnostic force result. The guarded move slate offers field-store swaps and temporary rvalues,
but none at the callback store; there is no useful move for that alias lifetime. Next: identify a real completion-state representation
that keeps r0 live through the callback store without adding a surviving read,
or introduces an r1 copy suggestion only at that store.


### TextState_ProcessTag (0803232C): five concessions removed, speaker alias remains

Tags: #menus #dialogue #matching #compiler

The queue calls this TextVm_ProcessTag, but the current symbol inventory and
source use TextState_ProcessTag. Landed the previous menus-link recovery with
only the existing r6 speaker alias pin retained: two other pins and all three
barriers are gone; the 1816-byte unit checks MATCH. Repeated complete per-arm
actions preserve the original cross-jump labels. No new concessions were added.

Best ordinary draft: `notes/phase3-drafts/dp-menus/TextState_ProcessTag.c`, still
3 ROM / 3 draft, identical extent. Insn 1190 initially copies name (144) into
alias (202), but cse replaces both alias uses by name at insns 1195 and 1219,
then drops the copy. `why 1195` proves this is cse canonicalization, before
allocation; raising name's priority would change all comparisons, not restore
the local copy. CSE_KEEP=202 has no effect because 202 is never the class head.
The retained alias pin is the exact diagnostic known to restore the copy.

Extended the previous field/name combination and inline experiments with a
pointer union, one-element pointer array, pointer-to-array view, separate
PLAYER pointer, and SI/DI aggregate views. All scalar views canonicalize away;
a DI initializer adds unrelated instructions. Identical late branch assignments
from name produce the copy at the correct address, but leave a dead length load
and the first call still uses r4. Identical field-address branches instead make
PRE insert the copy after the early NONE comparison. Reusing the previous
comparison result avoids a new load, but either folds the branch too soon or
changes name/value live ranges throughout the comparisons. Bit-test joins have
the same early PRE copy or substantially worse allocation. No complete knob
match is known beyond the original register constraint.

Persistent store: `build/matchsearch/TextState_ProcessTag`, with the baseline,
aggregate/lifetime/join proposals and name allocation trace. A fresh probe also
checks cse follow/canonical choices. Next: source that gives the alias a distinct
value representation only for the two comparisons, or a PRE insertion whose
latest placement is the 0x40C join; avoid revisiting simple scalar aliases.


### TextWindow_Destroy (08033778): typed links landed, allocation tie remains

Tags: #menus #dialogue #matching #compiler

Landed a small cleanup: both raw 0x1C8 pointer calculations now use the existing
`previous` field. The 132-byte unit matches; one pin and one barrier remain.
The best ordinary draft is `notes/phase3-drafts/dp-menus/TextWindow_Destroy.c`
(the previous menus-link recovery, 3 ROM / 5 draft, 136 bytes).

Confirmed again: `AGBCC_NOTIE=87` exactly matches. Cse replaces source-address
offset pseudo 44 with the destination's offset 41 (=456), then local allocation
ties source address 45 to that dying r0 constant. The ROM needs address 45 in r1,
which would let jump2 cross-jump the final slot copy with the other arm. `why`
reports that the source-load result 46 occupies r1 in the current tie order.

Tried typed previous-link accessors, a setter and getter, an inline unlink
returning the previous slot, an output-pointer unlink helper, naming both link
addresses, a nested assignment, source-first access, a globally shared source
slot, and a real nested next/previous link structure. Setter/getter variants
change allocation more broadly. Nested assignment and inline output forms have
the correct instruction shape but different registers. The nested structure
alone leaves the same tie; operating through link subobjects keeps additional
address arithmetic. Retaining the old join barrier alone does not remove the
pin for free. Unbound register declarations have no effect.

Persistent store: `build/matchsearch/TextWindow_Destroy`, including the exact
NOTIE counterfactual. Next: an ordinary source form that keeps the offset alive
past the arm-1 source-address add, or makes that address global without moving
its calculation ahead of the next-link address. Simple field accessor inlines
have been ruled out.


### ModifierMenu_SelectPendingSource (0803FCC4): separate scale, final sum allocation

Tags: #menus #matching #compiler

Best ordinary draft: `notes/phase3-drafts/dp-menus/ModifierMenu_SelectPendingSource.c`.
Separate category and index inputs from the scaled intermediate. This keeps all
instructions and the 160-byte extent; four instructions differ, the two sums and
their halfword stores using r0 rather than r1. Source remains unchanged with two
pins. The earlier draft's 4 / 4 result is reproduced.

Exact diagnostic: on `build/cx/work/ModifierMenu_SelectPendingSource/base.c`,
`AGBCC_NOTIE=166,220 AGBCC_FORCE=73:1,94:1` MATCH. Neither part alone suffices:
NOTIE alone still allocates the now-separate output in lowest free r0; FORCE
alone propagates the forced quantity backward through the multiply chain and
changes byte loads/address registers. The combined knob identifies both the
quantity split and its final choice. Local-alloc.c block_alloc first ties dying
input/output pseudos, then allocates quantities by suggestions/lifetime.

Tried signed/unsigned narrow result and displacement types, packed bitfields,
full-word/halfword union result views with and without initialization, index-first
compound sums, reusing category for the final result, separate tripled/scaled
values, typed cursor-index and output-pointer inlines, a sum inline, and identical
joins controlled by category/index bits. Plain narrow and union forms leave the
4 / 4 allocation; unsigned narrow scaling adds instructions; result reuse ties
backward and changes both multiply/load groups. Missing return keeps incoming r0
live and changes dispatch/epilogue, so it does not match here.

Persistent store: `build/matchsearch/ModifierMenu_SelectPendingSource`, including
the exact combined counterfactual. Next: a meaningful result representation
that both separates the final add quantity from the scale and prefers the old
category register r1. A simple scalar assignment only achieves one side.


### ModifierMenu_UpdateDescription (08041020): index pin removed, table arithmetic remains

Tags: #menus #dialogue #matching #compiler

Landed a partial: per-arm lookup sources and indexes remove the r4 index pin;
complete repeated lookup tails cross-jump back to the original calls. The unit
checks MATCH (264 bytes). One category pin and three barriers remain.

Best ordinary draft: `notes/phase3-drafts/dp-menus/ModifierMenu_UpdateDescription.c`,
5 ROM / 3 draft, 260 bytes. All differences are the mode-0 source table address:
the ROM adds 7 to category, scales by 4, computes menu + 188 in r1, then adds the
offset. Typed indexing instead combines 7*4 + 188 into 216 and computes the
address in r0. All other arms, loads, multiplies and calls match. Insn 79 starts
as category + 7; combine moves the constant through the shift and folds it with
188. `AGBCC_COMBKEEP=40,47` preserves category + 7 but still computes the byte
offset plus 188 before adding menu; it is not a complete matching knob.

Tried separate lookup tails, block-local sources/indexes, direct typed indexing,
local table without sharing sourceLocation, signed narrow index, commuted byte
addition, a byte pointer, compound pointer increments, pointer-to-array and array
address views, and typed slot/table accessors. Raw byte forms recover the ROM's
arithmetic shape but change category/source allocation in the other arms (11-18
register differences). Indexed forms fix those allocations but reassociate the
address. Keeping only the old table barrier or category pin does not match;
keeping all three old barriers plus category pin does, with the index pin gone.

Persistent store: `build/matchsearch/ModifierMenu_UpdateDescription`. The cse/PRE
probe finds no single decision improving the best draft. Next: preserve a typed
table base as a distinct address through combine without extending its lifetime
across other arms; inspect why the raw and indexed forms change category's
local/global quantity split before revisiting scalar table casts.


### ModifierMenu_LoadSavedSelection (08041C30): row address and call argument

Tags: #menus #matching #compiler

Landed typed modifierIdsBytes access in place of raw member-offset arithmetic;
the unit remains MATCH at 224 bytes with its two existing pins.
Best ordinary draft: `notes/phase3-drafts/dp-menus/ModifierMenu_LoadSavedSelection.c`,
8 ROM / 6 draft instructions, 220 bytes. The ROM loads state in r0, shifts the
index into r1, keeps the row address in r1, then loads the signed ID into r0 and
copies it into call argument r1 before setting argument r0. The draft ties the
row address to state in r0 and gives ID its call-preferred r1, deleting that copy.

Allocation evidence: row-field insns 165 and 171 tie the row/address chain.
`AGBCC_NOTIE=165,171 AGBCC_FORCE=83:0,87:1,85:1,91:1,88:0` reduces the difference
to three instructions: commuted add operands and the argument-copy order. This
is NOT an exact or semantically valid counterfactual: forcing ID into r0 leaves
its argument copy after the pending argument overwrites r0. The existing hard
pin influences expand_call early enough to protect that value; a late allocation
force does not. No complete matching knob was found. The cse/PRE/loop probe has
no improving decision. Read local-alloc.c's quantity tying/forced allocation and
global.c's find_reg preference logic. Scope initially accepted abbreviated knob
keys without applying them; those calls are discarded, and the tool now rejects
such keys with the correct spelling.

Tried direct indexed halfword access, state-relative typed rows, index-first and
offset-first sums, narrow and unsigned ID variables, halfword arrays, full-word
and halfword union views, a pointer union, repeated ID loads, typed getter and
add-selection inlines, whole-loop inlines with void/nonvoid result, casts and a
narrow parameter declaration, continue/goto/while/preincrement loops, and a
nonvoid outer signature. None improves the best ordinary draft. Restoring either
existing pin alone does not match. Keeping both pins allows the typed field
cleanup. Explicit goto rotation changes code placement; narrow/union forms are
optimized back to the same quantities.

Persistent store: `build/matchsearch/ModifierMenu_LoadSavedSelection`. Next: a
meaningful value boundary that makes expand_call precompute the signed ID before
placing pending in r0, together with a row-address form that ties to the scaled
index instead of state. A late forced register is insufficient evidence here.


### LevelAdvanceDisplay_UpdateParticle (0805704C): four pins removed, retained counter conversion

Tags: #menus #hud #matching #compiler

Landed a partial removing four X/Z position pins. Direct sums with the player
coordinate first, `player->position.world.x + particle->offsetX` (and Z), produce
the original load order and register allocation. Complete unit MATCH at 112 bytes.
Three counter pins remain: the loaded value, incremented frame and final phase.

Best ordinary draft: `notes/phase3-drafts/dp-menus/LevelAdvanceDisplay_UpdateParticle.c`,
6 / 6 differing instructions, same extent. Exact diagnostic on no-aliases.c:
`CSE_NOFOLLOW=1` MATCH. Cse follows the frame-expiration branch and reuses
`frame << 16` in the later phase calculation. The draft retains that shifted
value in r4 and deletes the second shift. The ROM retains the incremented frame
in r4 and shifts afresh at each test. Removing redundant input/resource aliases
is necessary for the knob to match; otherwise disabling the follow also retains
an extra particle copy and changes allocation. Cse.c's follow path requires a
single-use target label preceded by a barrier.

Tried direct and promoted increments, signed/narrow frame types, halfword/word
union views and initializers, a halfword array snapshot, reload from the frame
field, typed phase/expiration helpers, mask/shift comparisons, early returns,
separate conditionals, a boolean switch, explicit active goto, reversed arms,
and active/expired loop layouts. Most recover the same shared shift; the signed
phase changes shift semantics, switch/helper forms add branches, and reversed
arms preserve the wrong layout. Retaining only the frame/value pins leaves five
phase-allocation differences; all three retained counter pins match. Scope's
fresh pin audit confirms none of those three is free alone.

Persistent store: `build/matchsearch/LevelAdvanceDisplay_UpdateParticle`, including
the exact CSE_NOFOLLOW counterfactual. Next: an ordinary control-flow or value
boundary that prevents the first cse follow while preserving the ROM arm order.
The player-first direct sums should be kept; separately named coordinate values
raise pressure and obscure this one remaining issue.


### LevelAdvanceDisplay_UpdateAnimation (08057158): three pins removed, height copies and tail topology

Tags: #menus #hud #matching #compiler

Three partial commits remove the frame-sample, shared coordinate, and random-index
pins. A mask-first local inside the successful frame-range check fixes the sample
AND. Separate X/Y/Z coordinate locals fix their allocation. The matched staged
effect sibling supplies an index pointer: `cursor = &index; value = table[*cursor]`.
Taking the sample's address too leaves four X-axis differences; a normal sample
assignment matches. Complete unit MATCH at 468 bytes; three pins and seven
barriers remain.

Best ordinary draft: `notes/phase3-drafts/dp-menus/LevelAdvanceDisplay_UpdateAnimation.c`,
39 ROM / 25 draft instructions, 440 bytes. The solved sample and random regions
are exact. Remaining differences include timer allocation, kind/frame address
lifetimes, jump2 merging the two 30*frame tails, and constant-height copies.
The ROM builds 400 in r1 then copies it to r0 before loading Y in r1. The ordinary
draft folds that copy and changes load order. COMBKEEP=left does not recover it.
The cse/PRE/loop probe gives no single improvement; there is no complete matching
knob for the fully ordinary draft.

Isolated right-operand barrier diagnosis: rise-return.c keeps other existing
concessions and replaces that barrier/goto with a complete store and return.
`AGBCC_CROSS_LIMIT=228:2 AGBCC_NOCROSS=473` MATCH. Jump2 first shares its two store
tail instructions, then shares the multiply with the other rise arm. Limiting
the first jump alone lets the reverse merge happen. Read jump.c's find_cross_jump:
its source-jump UID and cumulative shared count explain both choices. This
counterfactual proves only that isolated barrier, not all remaining concessions.

Tried complete first/second-arm returns, duplicated rise tails, narrow signed and
unsigned rise amounts, wider result casts, separate frame-pointer groups, direct
global random draws, signed/narrow samples and coordinates, separate index loads,
index arrays/unions, sample capture helper, typed height helpers, a repeated
height-assignment macro, addressable height scalar, array and union initializers.
The latter height forms all fold the constant copy. Narrow rise amounts prevent
the tail merge but add two conversions. Full repeated rise code fails to merge
the larger random region. The macro trial was not landed. Preserve the normal
sample assignment and separate coordinates when revisiting the index pointer;
the sibling's `*(&sample)` spelling is worse here.

Persistent store: `build/matchsearch/LevelAdvanceDisplay_UpdateAnimation`, including
the exact isolated cross-jump counterfactual. Next: recover the source-level
height operation that materializes the constant copy, then reconstruct the rise
arm label topology with both multiplies retained. Kind/frame pins should be
retested together once the left/right scalar lifetimes are ordinary.


### ScenePanel_UpdateTimePalette (08057E04): spill layout and reload rotation

Tags: #menus #palette #matching #compiler

No source change landed. Best indexed draft:
`notes/phase3-drafts/dp-menus/ScenePanel_UpdateTimePalette.c`, 119 ROM / 129 draft
instructions, shape distance 118. The original fully depinned form reproduces
142 / 152, shape 116. Both are far from matching; no complete matching knob.
The ordinary frame grows from 24 to 32 bytes because panel (pseudo 23) and the
second target (175) spill into extra slots. Raising their priorities does not
recover the ROM (136 / 154 and 744 bytes). CSE_NOCANON=22 improves the base to
131 / 145 by preserving the panel copy; no single probe matches.

An isolated exact diagnosis: oldframe.c removes only the last oldFrame pin and
barrier. `AGBCC_SPILL=7:1` MATCH. Reload folds the frame load into the compare,
then chooses r1 instead of r3 because the previous reload took r0. The ROM needs
a preceding choice in r1/r2. Reversed comparison with the old pin retained gets
the load right but emits cmp r3,r0 instead of cmp r0,r3. Direct scalar, narrow,
array, union, addressed scalar, signed/unsigned comparison, XOR, negated operands,
and a delta/store formulation do not match. This knob proves only the isolated
pair, not the whole function.

Other close isolated groups: removing the previous-pointer step pin/barrier costs
two instructions (step in r0 instead of r5). Removing both channelMask pins costs
four instructions elsewhere: the palette-base literal changes r3 to r5 and the
second high-register pointer step changes r0 to r5. This is reload allocation,
not a different channel result. The existing channel helper worsens those groups.
Pointer-to-local source forms do not remove the high-register source constraints.

Tried direct row/column output, a conventional shared color helper, an exact-stage
pointer helper, addressable scratch pointer and one-element scratch array,
source-pointer references, narrow/signed mask and step variables, scalar/array
step storage, and frame/palette-base expression forms. Helpers and scratch
pointers substantially change allocation or stack traffic; array scratch removes
some barriers but still differs. Read the reload round-robin explanation and
frame slot records; the next useful task is recovering the original loop value
layout before chasing individual masks. Keep indexed output as an alternative,
but retain the original depinned baseline in the persistent store too.

Persistent store: `build/matchsearch/ScenePanel_UpdateTimePalette`, including the
exact isolated reload-rotation counterfactual. Next: remove the input/panel copy
class-head lifetime inflation in ordinary C, then eliminate the second target
spill while preserving the six intended scratch slots.
## dp-scene1-b (Codex)

### SceneActorRegistry_TestNearestPlayerBounds (08070C24)

Tags: `08070C24` `phase3` `register-allocation` `reload`

Two redundant pins removed and checked MATCH. Seven pins and three barriers
remain in the landed source. The saved pin-free draft uses existing route
members and direct bounds expressions; it differs in 40 ROM / 39 draft
instructions (shape 27), chiefly allocation and stack slots. Bounds spills,
nearestIndex takes a high register, and bestDifference spills instead of
occupying r6/r7. Moving vector subtraction to an inline helper and replacing
the pointer loop with indexing did not resolve those lifetimes; a natural for
loop worsened instruction shape.

The closest broader partial replaces the two route offset temporaries with
`SceneActorRegistryResetView.group.homeRoute` members and removes the initial
player-table pin/barrier. Only the signed delta-x load differs: its zero index
uses r1 instead of r2. Scope identifies reload choice 6; diagnostic
`AGBCC_SPILL=6:1` makes that partial MATCH. The preceding player-table reload
chooses r0. Early category returns, table-first declarations, and direct table
assignment do not change the decisive rotation. This partial remains only in
scratch because it is not a stock-compiler match.

Removing the final limit pin swaps component and limit registers. Separate
block locals, narrower types, and assigning the negated limit still swap them.
Writing the lower test as `component + limit < 0` fixes allocation but emits
an add and compare in place of cmn. The component is a multi-set global while
the limit is local; increasing the limit priority alone did not resolve it.
Next: recover the original aggregate/vector helper lifetime to keep bounds
and bestDifference in registers, then revisit the signed bounds predicate.
Scope's hunk matching also pairs unrelated same-form stack stores/moves in
this function; investigate value-aware alignment before trusting those why
messages as allocation evidence.

### SceneActorRegistry_RestoreActorDefaults (080718D8)

Tags: `080718D8` `phase3` `reload` `snapshot`

Landed seven pin and eight barrier removals (47 to 32 concessions), checked
MATCH. Shared terrain EnterState and InstallControl replace the initial reset
block; removing the five offset2 bindings/barriers together restores the later
0x5D9/0x5DA reload choices. Removing any one alone obscured that interaction.

Best pin-free draft is now 15 ROM / 15 draft instructions different, shape 2
(previous raw-offset draft was 158 / 156). The entire no-object branch becomes
exact with direct member copies and stores. In the other branch, deleting an
unused destination-address assignment before field2B5 restores its value load
allocation, reducing 19 differences to 15. Fully converting the other branch
or the tail to members shifts many preceding allocations and was worse.

Remaining gate flags load uses r1 rather than r4; diagnostic
`AGBCC_SPILL=55:3` fixes those five instructions, leaving ten differences.
The hitReactionLatch, motion5B8, field5C4 and field5C2 addresses have subsequent
reload rotation differences. ResetResponseState's unused index calculation
moves before the destination copy, leaving the shape distance of 2. Wrappers
with halfword index parameters and an argument expression do not change it.
Keeping the existing gate and destination barriers reduces the draft to nine
register-only differences, but is not concession-free. Keeping more original
second-branch pins interferes with the now-exact first branch.

Next: recover a snapshot/reset helper boundary that gives the gate and tail
address constants independent reloads; examine the call's unused forwarded
arguments and original helper signature. Keep the direct first branch intact.

### SceneActorPalette_BeginTransition (08071E00)

Tags: `08071E00` `phase3` `palette` `allocation`

No stock-compiler removal landed. Saved pin-free member/index draft differs
83 ROM / 81 draft instructions (shape 54), against 90 / 92 (shape 62) for the
mechanical pin removal. Prior round11-c work already tried 95 removal groups
and 24 search variants. This pass used the matched Axe initializer as a fresh
array-loop model, pointer byte stores, natural do loops versus goto loops,
member channel pointers, typed channel getters, channel-address helpers, and
red work repeated in both sampling arms. None restored the original loop's
allocation or its separate mask lifetimes. Narrowing the red scratch also
failed to replace its existing barrier without instruction changes.

Scope proves transition is allocated before base (3428 versus 2941 priority),
taking r6 instead of r7. Diagnostic `AGBCC_PRI=base:+488` flips the ordering
and improves to 75 / 73 differing instructions, but is not a full matching
knob. Duplicating red work in both arms changes allocation but worsens the
shape. global.c's allocno_compare uses refs * floor_log2(refs) / live length;
source variables alone do not create independent channel-pointer lifetimes
because CSE folds them. Halfword array indexing improves shape but joins the
byte and halfword induction values; the ROM keeps independent i and 2*i.
The color mask is cached across five uses where the ROM has separate masks.

Persistent evidence: build/matchsearch/SceneActorPalette_BeginTransition;
member/index root and arm-red hypothesis retained. Next: recover the original
RGB extraction/update helper so independent masks and induction values arise
from real types; then revisit transition/base density and final flag reloads.
The full difference is not explained by one knob yet. Source remains matching
with its original concessions.

### SceneActorPalette_ApplyTransition (08071F50)

Tags: `08071F50` `phase3` `palette` `allocation`

Landed the flagSet barrier removal with the byte-valued Test helper in each
category arm, retaining the original goto order. Check MATCH, 468 bytes.
The source retains 18 pins and three barriers. The pin-free base with that
helper has 102/104 differing instructions, shape 40. A do loop improves it
to 88/90, shape 40. Repeating the manager flag write in each category arm
further improves allocation (actor r7, manager r8 and category r9) but leaves
duplicate tails: saved draft 80/86 differences, shape 30.

Confirmed priority decisions: i loses to actor (1846 versus 2258);
AGBCC_PRI=i:+413 improves the base to 98/100. manager loses r8 to category;
AGBCC_PRI=manager:+160 also improves to 98/100. A real do loop and repeated
arm writes reproduce parts of those changes. Remaining frame growth comes
from colorCursor spilling; channel address construction and blueSum allocation
remain wrong. The direct member loop, ordered pointer declarations, s16 index,
and mask-first predicate did not recover the original allocation. Direct
actorFlags member access alone differs in six register instructions. The full
difference is not explained by a single matching knob.

Persistent evidence: build/matchsearch/SceneActorPalette_ApplyTransition.
Next: recover the channel pointer setup and signed-byte update helper; the
ROM uses byte load/shift for red and green, indexed ldrsb for blue. Preserve
the byte flag helper and category arm order that already remove a barrier.

### sub_08072204 (08072204, active palette binding)

Tags: `08072204` `phase3` `cse`

One pin remains; no landing. Pin-free draft differs in four instructions,
shape 2. CSE changes the manager load destination to transition, deleting the
initial r1-to-r3 copy; category23 then adds from r3 instead of r1. All later
instructions match. CSE_NOCANON=25 restores r1 but emits the copy only in the
category10 arm (100-byte function versus 96); it is a diagnosis, not a match.

Tried union/struct pointer initializers, reversed initialization/declaration
order, separate reads of the same global, structured category branches, a
transition-selection helper, and a u32 manager predicate. None preserves the
original early copy. Selector helper additionally reads category before the
manager null check, so it is not the saved candidate. The saved draft keeps
original control flow and removes the duplicate local name alias.

Next: inspect CSE copy elimination/REG_EQUIV and find the original ownership
or view helper that keeps manager and transition independent until selection.

### SceneActorContacts_ReleaseInvalidOwners (08072D34)

Tags: `08072D34` `phase3` `loop` `regmove`

No landing; best pin-free draft improves from 38/38 differences, shape 26,
to 11/9 differences, shape 2. Reuse the clear-result variable for cooldown 120
in each arm to prevent loop hoisting (goto-loop form independently produces
the same improvement). Return the state-field address through Unavailable's
output parameter to recover flags r4/state r1 allocation. This reproduces the
improvement of diagnostic AGBCC_PRI=state:+8664 on the preceding draft.

Remaining: regmove ties each clear AND to the dying flags input, losing the
ROM's mask copy into r0. Zero/valid then use r0 rather than r1. Scope proves
fixup_match_1 at insn196; AGBCC_NOTIE=196 restores the copy but changes later
allocation (13/13 differences), so no full matching knob yet. Tried byte and
halfword masks/results, union byte reads, shared clear/valid temporaries,
reversed AND operands, clear-owner helper, and retaining subsets of original
pins. None matched. Retaining final value/valid pins changes earlier allocation;
removing the actor group alone does not independently match.

Next: recover a typed flag-update helper that blocks the dying-input tie while
keeping the final valid zero available for the owner store. Preserve the two
ordinary-C lifetime fixes in the saved draft.
## cdp-bosses (Codex)

This pass rechecked all nine queue entries against the existing pin-free drafts and preserved the
best ordinary-C state under `notes/phase3-drafts/cdp-bosses/`. The blocking decisions and next
experiments are below; the detailed prior experiments remain in the `monsters-bosses` sections
above.

### Hresvelgr_PlayDescent

Best draft: `notes/phase3-drafts/cdp-bosses/Hresvelgr_PlayDescent.c` (11 ROM / 5 draft). CSE makes
the input pointer the class head because its final use follows every use of the actor copy. The
barrier is the only current way to retain both registers. Next: introduce a real control-flow block
that prevents copy propagation without changing the final else-arm layout.

### Hresvelgr_UpdateBoundaryBarrage

Best draft: `notes/phase3-drafts/cdp-bosses/Hresvelgr_UpdateBoundaryBarrage.c` (2 / 2). Removing the
keep-alive lets retry global allocation put the random mask in `sl` rather than `ip`; the collision
mask is no longer live in case 1. Next: recover a natural case-1 use or conflict for `sl`.

### sub_081A14FC

Best draft: `notes/phase3-drafts/cdp-bosses/sub_081A14FC.c` (86 / 82). The random-table view loses
`r6` to `&gRandomIndex`, and the ROM has one additional spill and a four-byte-larger frame. Next:
reduce index-address references using the shared sampler shape, then identify the displaced pseudo.

### sub_081A8E08 (Vanargand_UpdateCaptureSetup)

Best draft: `notes/phase3-drafts/cdp-bosses/Vanargand_UpdateCaptureSetup.c` (13 / 9). CSE merges the
pending-store zero and position-y zero into one constant class. Next: produce the y zero in a
narrow mode or bitfield operation so it only folds to zero after CSE.

### Vanargand_ApplyPendingDamage

Best draft: `notes/phase3-drafts/cdp-bosses/Vanargand_ApplyPendingDamage.c` (7 / 7). Narrowing the
flags test fixes the damage/health-address allocation. The remaining constant-one mask is made
canonical with the earlier display-call argument across CSE's extended path. Next: split that path
with a natural return/branch form before the count-zero block.

### sub_081D3620 (PlayerParticleCloud_ScatterAnchors)

Best draft: `notes/phase3-drafts/cdp-bosses/PlayerParticleCloud_ScatterAnchors.c` (6 / 6). Narrowing
the random sample to `u16` at the mask gives the ROM's `r0` sample allocation in both halves. The
remaining first-half difference is only scheduling: the draft delays the sample load until after
the coordinate load and bias; the second half additionally leaves the coordinate in `r2` rather
than `r1`. Splitting the coordinate initialization, using a `u16` sample local, adding an
intermediate narrowed local, and zero-valued dependencies did not move the load earlier. Next:
find a natural sample-read helper shape that keeps the narrowing's priority effect but schedules
the load at the address calculation, before the coordinate load.

### Ratatosk_UpdateSunOrbRelocation

Best draft: `notes/phase3-drafts/cdp-bosses/Ratatosk_UpdateSunOrbRelocation.c` (8 / 8, register-only).
`AGBCC_SPILL=15:1` matches: the ROM makes one additional reload choice between the fading copy and
case 1's sequence store. Next: make either the stored one or the input address reach reload rather
than global/local allocation.

### sub_08217430

Best draft: `notes/phase3-drafts/cdp-bosses/sub_08217430.c` (8 ROM / 6 draft). Local allocation gives
the source bank the call-copy suggestion `r0`; the ROM retains it in `r3`, and forcing that leaves
the offset/base pair. Next: keep the bank live beyond the call copy through a real subsequent use.

### GameState_SelectRamBase

Best draft: `notes/phase3-drafts/cdp-bosses/GameState_SelectRamBase.c` (1 / 1). CSE canonicalizes the
addition as offset-plus-base, reversing the ROM's operand order. Next: form the storage base through
a non-constant-equivalence initializer, potentially a union initializer whose clobber prevents the
canonicalization.
## cdp-effects (Codex)

### BackgroundFlash_UpdateScrolling (src/fn/sub_08047770.c, 08047770): deferred, 6/6

Draft: `notes/phase3-drafts/cdp-effects/BackgroundFlash_UpdateScrolling.c`. Removing the frame r1 pin leaves only a register swap: `frame` has 3 references across 18 insns (priority 1666), while the short-lived target-speed pseudo has 3 references across 4 (priority 7500) and takes r1 first. `AGBCC_PRI=frame:+5835` matches. An inline mask helper raised frame to 5 references but changed the second AND's tie/canonicalization; nested wrappers folded away. Next: a natural frame-test helper that supplies two more pre-flow references while retaining the constant-first `odd &= frame` form.

### RepeatedEffect_TickUpperTransition (src/fn/sub_0804F9C4.c, 0804F9C4): deferred, 77/71

Draft: `notes/phase3-drafts/cdp-effects/RepeatedEffect_TickUpperTransition.c`. The seven pins cover the repeated RNG-index, x/z sample, and result-value allocation in two symmetric transition arms. Removing them produces 77 ROM / 71 draft rows and one moved branch; the older recovery also found the first decisive mismatch at the halfword store after the upper-transition test. Next: factor the two arms through the table-first `ParticleRandom_ReadSample` form while preserving a separate post-join result value.

### OrbitBackground_UpdateVisibility (src/fn/sub_08050910.c, 08050910): deferred, 26/22

Draft: `notes/phase3-drafts/cdp-effects/OrbitBackground_UpdateVisibility.c`. Removing the background/selected pins and status barrier changes the ordering of the final special-mode load/test relative to the enabled test (seven moved branches overall). Direct ordinary condition spellings retained the wrong scheduling. Next: repeat the post-join status statement in each arm so jump2 cross-jumps the tails while keeping the special-mode load on the ROM side of the join.

### OrbitBackground_UpdatePosition (src/fn/sub_080509E8.c, 080509E8): deferred, 53/53

Draft: `notes/phase3-drafts/cdp-effects/OrbitBackground_UpdatePosition.c`. Ten constraints steer the angle correction, game-state reload, two symmetric signed approach blocks, and loaded blend frame. The unsigned-halfword blend-frame form removes its binding, but signed correction allocation and reload ordering remain; removing all constraints yields 53/53 and twelve moved branches. Next: unify the two approach arms behind a signed step helper, then duplicate the post-join store in each arm.

### DisplayTrack_Update (src/fn/sub_08054EC0.c, 08054EC0): deferred, 31/31

Draft: `notes/phase3-drafts/cdp-effects/DisplayTrack_Update.c`. The 27 constraints are concentrated in palette interpolation (period/divisor/frame pointers and color temporaries), the descending cleanup loop, and the sound-event tail. Despite their count, removing all leaves only 31/31 rows and one moved branch, indicating allocation rather than missing operations. Next: recover a typed palette-interpolation helper with the period pointer as its loop invariant and repeat the final packed-color store in the two interpolation arms.

### DisplayTrack_AddFromScript (src/fn/sub_08055268.c, 08055268): deferred, 12/18

Draft: `notes/phase3-drafts/cdp-effects/DisplayTrack_AddFromScript.c`. Four constraints steer the initial script value, zero halfword, event, and has-sequence result. Removing all changes 12 ROM / 18 draft rows and one branch; the extra draft instructions point to value lifetime/CSE rather than pure allocation. Next: use a union-zero initializer for the halfword state and return directly from the sequence-present arms so the result is not live across the join.

### Presentation_UpdateDropShakeBlink (src/fn/sub_081E2910.c, 081E2910): deferred, 46/34

Draft: `notes/phase3-drafts/cdp-effects/Presentation_UpdateDropShakeBlink.c`. The three destination pins as a group cost only 6/6, while removing the long-lived table r6 pin causes most of the 46/34 difference and three moved branches. The typed departure-layer layout is retained. Next: index the input table directly in each state arm (rather than advancing a shared table pointer), then address `layers[1].position.y` through a single typed destination local.

### SpriteTrail_CreateFixedEffect (src/fn/sub_081F28C4.c, 081F28C4): deferred, 4/4

Draft: `notes/phase3-drafts/cdp-effects/SpriteTrail_CreateFixedEffect.c`. The pin-free code has identical instruction shape; reload keeps `pool` in sl and spills the render position, while the ROM spills `pool` at sp+4 and keeps the position in sl. Moving declarations and replacing the two position-x field expressions were tested; declaration order is neutral and the field rewrite worsens allocation. Next: find the original render-loop inline/helper boundary that raises the position pointer's density without changing the emitted body.

### SpriteTrail_CreateMotionEffect (src/fn/sub_081F2A08.c, 081F2A08): deferred, 4/4

Draft: `notes/phase3-drafts/cdp-effects/SpriteTrail_CreateMotionEffect.c`. As in the fixed twin, only the equal-live pointer placement differs: pool is in sl in the draft, while the ROM spills it and keeps renderOrigin in sl. The already-separated motion setup eliminated the earlier second binding. Declaration reordering is neutral; direct origin-x spelling perturbs loop allocation. Next: solve the fixed twin's shared render-loop pointer-density form and apply it here.

### RisingActor_AdvanceHeight (src/fn/sub_081FDAF4.c, 081FDAF4): deferred, 5/3

Draft: `notes/phase3-drafts/cdp-effects/RisingActor_AdvanceHeight.c`. Removing the zero r4 and state r2 pins together leaves five ROM / three draft rows in this 76-byte leaf. The verified Orbit Volley state/resource layout is retained. Next: express the height update as a returned value (including the missing-return lifetime lever) so zero remains live in r4 and the state destination naturally occupies r2.

### sub_081FDCB0 (src/fn/sub_081FDCB0.c, 081FDCB0): deferred, 38/30

Draft: `notes/phase3-drafts/cdp-effects/sub_081FDCB0.c`. The source r1 pin costs only 4/4; the zero-use barrier and especially the memory barrier control the steering-stage schedule and account for four moved branches. The resource and 0x38 trail record types agree with their callers. Next: split steering calculation from trail emission into an inline returning the updated position, so the real call/data dependency replaces the memory fence.

### HomingProjectile_UpdateDefaultState (src/fn/sub_081FF454.c, 081FF454): deferred, 47/49

Draft: `notes/phase3-drafts/cdp-effects/HomingProjectile_UpdateDefaultState.c`. Four constraints steer a saved r5 value, y destination r2, constant one, and delay. Removing them changes 47/49 with five moved branches; the typed projectile lifetime/state layout remains. Next: return the state-transition result directly from each arm and repeat the delay store before each return, shortening both constant lifetimes and eliminating the join allocation.

### ProjectilePool_Update (src/fn/sub_08202730.c, 08202730): deferred, 20/20

Draft: `notes/phase3-drafts/cdp-effects/ProjectilePool_Update.c`. Only two empty operand barriers remain, on the sprite object and velocity; removing both preserves length but rotates 20/20 allocation rows. The live slot (0x128), configuration (0x5c), shared command reader, signed motion, and collider layout are retained. Next: use the shared sprite-step helper with the object and velocity passed as distinct typed arguments, supplying genuine references across the command dispatch.

### SpiralBurst_InitParticles (src/fn/sub_08234540.c, 08234540): deferred, 49/47

Draft: `notes/phase3-drafts/cdp-effects/SpiralBurst_InitParticles.c`. The signed cosine-table/direct sine sampling already removed the old barrier. The remaining two pins keep separate effect-table and random-table aliases in r5/r6; removing the random-table pin dominates and introduces three moved branches. Next: convert all draws to the table-first `ParticleRandom_ReadSample`/`ReadDirectSample` helpers while keeping the effect table captured outside the loop.
## cdp-field (Codex)

### TetherActor_UpdateBounceReturn (partial)
Dropped the destination pointer's `r1` pin: ordinary allocation already matches. The saved pin-free draft still differs by 23 ROM / 16 draft instructions plus two moved branches. The remaining costs isolate `frameValue` (4 rows), `incremented` (2 rows), and especially the `0x380` height offset (25 rows). Next: recover the original height-store expression so the constant is rematerialized in the ROM's allocation order.

### AreaEffect_AddSpriteFromScript
The pin-free draft differs by 38 ROM / 32 draft instructions plus eight moved branches. The count pointer's `r8` allocation is the largest isolated cause (30 rows), followed by the point pointer (19); removing the `first` barrier alone costs three rows. Earlier work identified the register allocation at entry and loop shape as the blocker. Next: compare the 55-instruction relative `sub_08063CAC` and duplicate the post-join point advance into its arms to adjust pointer density.

### AreaEffect_UpdateLinkPlayerResources
The pin-free draft differs by 40 ROM / 44 draft instructions plus 16 moved branches. Isolated costs show the loop counter pin/barrier pair changes 33 rows; the destination's previous value changes 41, the counter value 28, and the base pointer 40. Next: reconstruct the loop as the likely count-up source loop and let loop reversal create the ROM counter copy (scope facts 84 and 86).

### AreaEffect_UpdateLinkVisibility
The pin-free draft differs by 39 ROM / 37 draft instructions plus nine moved branches. The `~1` mask controls the largest allocation cascade (37 rows); the two count-pointer copies together cost 17 and the cursor barriers together cost six. Next: combine the two scans behind a shared typed helper and test whether repeated arm tails supply the missing references without barriers.

### ResourceStation_CheckInteraction
The pin-free draft differs by 36 instructions on each side. The trigger offset pin is the dominant isolated decision (31 rows); the offset/barrier group costs six and the nearby-offset group three. Next: replace constant byte offsets with a layout-preserving station struct and member addresses, so CSE and reload derive the constants naturally.

### ResourceStation_UpdateDigits
The pin-free draft differs by 32 ROM / 36 draft instructions plus one moved branch. The two loop-counter pins together cost 41 rows, while the read temporaries cost ten and field temporaries eight. The branch target shift points to loop source shape. Next: express both countdowns as count-up `for` loops and compare the 51-instruction relative `sub_08016C04`.

### DirectionalForce_Tick
The pin-free draft differs by 34 ROM / 30 draft instructions plus two moved branches. The random-table pin costs 14 rows, the countdown barrier 12, mask pin 11, and memory barrier eight. Next: use the shared table-first random-sample helper and rewrite the countdown as a count-up loop so loop reversal establishes the ROM lives.

### DirectionalForce_InitializeFromScript
The pin-free draft differs by 60 ROM / 61 draft instructions plus two moved branches. The status helper pin dominates the isolated result, while the random pointer/barriers form a 20-row group. Next: replace the private random reads with the shared table-first helpers, then revisit the value statement order that currently needs two barriers.

### WoodenCrate_CreateFromScript
The pin-free draft differs by 58 ROM / 46 draft instructions plus four moved branches. Fifteen of sixteen concessions are in the private `Step` helper; its group alone costs 50 ROM / 38 draft rows. Next: replace that private sequence stepper with `DisplaySequence_StepSprite`, then diagnose only the creator's remaining sprite allocation.

### WoodenCrate_ReactToHit
The pin-free draft differs by eight instructions on each side. The ROM preserves both the hit argument and a second crate copy; ordinary C reverses the first two copies, then uses the wrong saved copy after calls. Next: alter parameter-copy order with a plausible inline wrapper or callback-target view while retaining the helper's control flow.

### FloorSwitch_SetOccupant
The pin-free draft differs by eight ROM / six draft instructions. Priority instrumentation shows the `one` pseudo must be allocated before the function pointer so r6 is already occupied; `AGBCC_PRI=one:+11728` is the diagnostic. Pass 2 confirmed that duplicating `p->field10 = one` in both song-selection arms gives `one` the ROM's r6, but jump2 cannot merge the stores across the intervening calls and leaves two extra instructions. Duplicating the palette call with the store also gets r6 but leaves both four-instruction call sequences. Identity uses in the song argument (`+ one - 1`, `one & ~one`, shifted and remainder forms) fold before allocation and do not change the eight/six diff; XOR and OR identities survive but make the output worse. Reversing `one & f` does not affect allocation, while `f &= one` reduces the draft-only side to two instructions but increases shape distance. Next: find a real pre-call use of `one` that survives through global allocation and is removed by combine, or a branch layout whose duplicated stores are adjacent tails that jump2 can cross-jump.

### NamedObject_RegisterFollowCells
The pin-free draft differs by 17 instructions on each side. `AGBCC_PRI=object:+1027` matches exactly: the object pointer must outrank `w`; a smaller increase also fixes its order relative to the loop counter. Next: add natural object references by duplicating a post-join object-field statement into the registration branch arms, allowing jump2 to merge the emitted tails.

### ActorGroup_CreateFromScript
The pin-free draft differs by 79 ROM / 77 draft instructions plus six moved branches. All concessions are in the private `Step` helper, with the mask/barrier pair causing a 55-row cascade. Next: replace the helper with the subsystem's shared playback/sequence helper, preserving the setup pointer and embedded frame state.

### Collectible_StartDeath
The pin-free draft differs by 37 ROM / 33 draft instructions. Most concessions are a repeated private display-sequence stepper; the mask pin alone causes 52 differing rows. The output-only `d` barrier cannot simply be deleted because it is its only definition. Next: use `DisplaySequence_StepSprite` and write the actual duration value directly before removing the output barrier.

### Collectible_UpdateEntry
The pin-free draft differs by 46 ROM / 42 draft instructions plus one moved branch. The repeated private stepper accounts for most pins/barriers; the final duration and hit temporaries remain a separate allocation issue. Next: adopt `DisplaySequence_StepSprite`, then reconstruct the actual definition currently hidden by the output-only `d` barrier.

### SpriteDrop_UpdateEntry
The pin-free draft differs by 31 ROM / 29 draft instructions. Every concession is in the same private display-sequence pattern seen in the two collectible functions. Next: convert all three together to `DisplaySequence_StepSprite`; their shared C form should expose any caller-specific allocation difference.

### sub_0817AEC8
The pin-free draft differs by 53 ROM / 49 draft instructions plus three moved branches. Its single sine-table `r2` pin controls the entire allocation cascade. Older notes likewise identify allocation rather than instruction selection. Next: recover the angle/table expression from matched scene-volume relatives and test operand order and a typed sine lookup helper to generate the ROM preference naturally.
## cdp-link (Codex)

The 2026-11-13 pass recompiled every queue entry with all concessions removed. The best ordinary-C
starting points are archived under `notes/phase3-drafts/cdp-link/`; the row counts below are aligned
scope differences, not byte counts.

- **WirelessGameDisplay_UpdateShakeAnimation** — Replacing the two manual random-index sequences
  with table-first `ParticleRandom_ReadDirectSample` calls reduced the pin-free draft from 61 to 26
  ROM rows / 34 draft rows. The remaining blocker is allocation of the display owner to r7 rather
  than r6, which introduces the r8 save. Next try a shared display-step helper that shortens the
  owner's live range across both random reads.
- **WirelessResultPanel_UpdateOutcomeReveal** — 69 rows after removing eight pins. The first
  blocker is the drawing cursor's allocation across the outcome branches; repeat its post-join
  assignment in each arm before changing the result-counter loop.
- **WirelessResultPanel_Init** — 95 rows after removing fourteen pins. Several independent
  initialization phases are coupled by long-lived zero/one and object aliases; split the phases
  with branch-local member stores and diagnose each pin-removal variant independently.
- **EncodedRecord_XorRange** — 14 rows after removing two pins and one barrier. The current-value
  barrier controls the byte XOR loop allocation; next compare a pointer-advancing loop with an
  index loop and preserve the encoded byte in a narrow local.
- **sub_08108C94** — 60 rows after removing two pins. The table base and raw halfword allocation
  remain coupled; next use a typed table cursor and repeat the post-join raw assignment in both
  arms.
- **LinkMenu_WaitForReadyPeers** — 20 rows after removing the initialization pin and timer helper
  barrier. The timer helper's memory operand controls its caller's allocation; next inline the
  ordinary increment form into both timer branches.
- **LinkMenu_WaitSelectionMode** — 16 rows after removing the pointer pin/barrier pair. Allocation
  wants the pointer-to-pointer in r2 instead of r0; next test direct double indexing rather than a
  saved alias.
- **LinkSelection_WaitForEmptySlots** — One register-only row remains: cse1 canonicalizes the
  initial `i` comparison to the equal zero-valued `count` pseudo. Casts, zero spellings, declaration
  order, `i + 0`, and `i = count` do not change it; an explicit `0 < length` changes the compare
  shape. A second pass confirmed with `why 81` that pseudo 24 (`i`) is initially allocated to the
  ROM's r3, but cse1 substitutes pseudo 25 (`count`, r6) solely because both hold zero. Labels,
  gotos, `count ^ count`, `count - count`, signed/unsigned and narrow types, alias locals, and reuse
  of the session-ready result either leave that substitution intact or add instructions. Moving the
  count initialization into both sides of the initial branch preserves the desired comparison but
  adds two branches, so it is not a viable match. Next seek an ordinary expression whose zero is
  only exposed after cse1, or a natural preceding nonconstant definition of `i` that does not alter
  its otherwise-correct priority.
- **LinkSelection_ExchangeParticipantInput** — 14 ROM / 10 draft rows after removing three
  concessions. The initial 0x3c00 mask is built directly in r6 rather than in r1 then copied, and
  that changes the later mode tests. Next derive the mask through the first slot predicate so the
  copy remains live.
- **LinkSelection_UpdateParticipantScores** — 89 rows after removing fifteen pins. This is a broad
  allocation rewrite; isolate the zero aliases and participant cursor first, then retain only
  variants that improve the register-blind shape.
- **TimerDisplay_DrawRecord** — Eight rows after removing two pins and two barriers. The cell
  pointer barrier controls three register-only differences. `why 36` identifies the exact cause:
  each store block has three local quantities, triggering agbcc's small-set `qty_compare` ordering;
  it assigns the returned cell to r0 and the halfword constant to r1, while the ROM has the cell
  copy in r1 and the constant in r0. Typed return values, pointer/value aliases, wide or narrow
  constants, increment-built constants, and an inline store helper do not create the required
  fourth local quantity (the helper is instead emitted out of line). Next try a real fourth value
  arising from direct row/column indexing, rather than a dead steering local.
- **sub_0810FDDC** — 63 rows after removing two pins. The score-panel owner and tens digit compete
  across the drawing calls; next repeat the digit-base computation in the two branches.
- **LinkSession_UpdatePlayerValueAverage** — Ten rows after removing the `GetSelection` sum
  barrier. The inline helper's accumulator lifetime is the blocker; next use a returned aggregate
  or a caller-owned sum so the helper has no cross-iteration alias.
- **sub_081384A0** — 45 rows after removing the loop-index barrier. The barrier prevents induction
  canonicalization; next rotate the loop around its return/exit rather than using a while-form.
- **PositionEffectDisplay_UpdateSources** — 69 rows after removing three pins. Source cursor,
  flags input, and output object allocations are coupled; next duplicate the post-join source
  update in each flag arm.
- **Clock_CheckRtcStatus** — Five register-only rows (plus one moved branch) after removing two
  pins. The shifted byte is allocated to r4 instead of r5; declaration order, delayed
  initialization, scalarized two-word arrays/structs/unions, and zero-initialized aggregates did
  not reproduce the allocation. Splitting the shift-assignment, narrowing the second assignment,
  initializing `status` from the call before shifting it, and a one-member union all compile to the
  same five-row difference. Next recover the SDK RTC result aggregate type rather than using two
  independent u32 values.
## cdp-monsters (Codex)

The 2026-11-13 cloud pass rebuilt scope's instrumented compiler and ran `pins` on the complete
17-function queue. The saved drafts below are the best ordinary forms from the prior
monsters-bosses pass where those were closer, otherwise the newly regenerated all-concessions-removed
form. The detailed compiler investigations remain in the earlier function sections in this file;
this index records the complete queue and the current blocking decision after rechecking it.

* `TentacleActor_UpdateRetraction` — 20 differing rows after both barriers are removed. The unused
  absolute-value branch still needs identical post-reload arms so jump2 can delete the comparison.
* `sub_080ABB18` — 116 rows after seven concessions are removed. Loop-invariant field addresses
  still need to be produced by loop hoisting rather than manually pinned pointer temporaries.
* `Vorn_UpdateDefeat` — no pins or barriers remain, but eight single-use statement macros retain
  the loop-depth allocation effect. The next form must reproduce those reference weights with
  ordinary repeated statements.
* `sub_080B197C` — 26 rows after its barrier is removed; `AGBCC_PRI=active:+400` diagnoses the
  remaining r4/r5 allocation swap. It needs one natural reference to `active` or a shorter life.
* `Skeleton_TrySelectRetreatCell` — two rows after both map-slot barriers are removed. Reload
  choice 18 must naturally select r2 for the south signed-width load.
* `Skeleton_UpdateDefeat` — 273 rows after three pins are removed. The two `one` lifetimes and the
  random table allocation interact across the large state machine and must be separated first.
* `Golem_UpdateDefeat` — 92 rows after the random-table pin is removed. Use a table-first random
  sample form that preserves the current six-particle loop's allocation.
* `Mummy_ResumeSavedRouteAfterAnimation` — 22 rows after four concessions are removed. The saved
  mask and reaction/flags operands still require the ROM's stack and low-register placement.
* `Mummy_UpdateDefeat` — 110 rows after the table pin is removed. As with Golem, the next attempt
  should use the shared table-first random draw without changing the loop allocation.
* `Ivy_UpdateDefeat` — 14 rows after the table pin is removed. The helper form fixes allocation,
  but the table literal must be loaded before the random index while retaining the shorter life.
* `Axe_UpdateDamageSpin` — the closest ordinary draft has 11 differing rows; context and constant
  one need their priority order reversed without extending the doubled REG_EQUIV lifetime.
* `Terrain_ResolveActorMotion` — the closest ordinary draft has 10 differing rows. CSE must not
  fold the negated shifted offset back to the shift pseudo, without an operand barrier.
* `SpriteColliderGroup_ActivateParts` — six rows after both concessions are removed. The ROM saves
  unused r8, implying an allocation whose uses disappear after allocation/reload cleanup.
* `SpriteColliderGroup_ResetActiveParts` — two rows after the total pin is removed. Combine must
  retain a real total pseudo so the first count load occupies r5 instead of reload choosing r6.
* `VulcanCannon_UpdateFacingTurn` — the closest shared-step draft differs by five ROM rows versus
  three draft rows. An equality test must not let CSE reuse `allowTurn` as the later zero argument.
* `EnemyParticle_UpdatePool` — the closest shared-step draft differs by 12 ROM rows versus 16 draft
  rows. The step needs r8 as the loop cursor while sprite/state remain derived from the base side.
* `OrangeSpine_ApplyHitReaction` — the relative-based ordinary draft differs by 36 ROM rows versus
  34 draft rows. A natural value live across the response copy must occupy r0/r1 and add r3 to the
  later reload spill set.

Drafts: `notes/phase3-drafts/cdp-monsters/`.

### cdp-monsters pass 2 follow-up (2026-11-13)

* `SpriteColliderGroup_ResetActiveParts`: stayed on the two-row reload holdout. Algebraic
  second uses of `total`, duplicate identical arms, a copied total, and using `total` for the
  loop latch are either removed before combine or perturb the loop; none keeps the initial
  zero-extend pseudo alive. The original two-row diagnosis remains exact.
* `Skeleton_TrySelectRetreatCell`: `why 447` confirms choice 18 sees r0 occupied and r1/r2
  free after choice 17 selected r2. Expanding the successful south tail, the blocked south
  tail, or both prevents jump2 from recreating the ROM control flow; the best expanded-tail
  form is 8/10 rather than 2/2. The missing earlier reload must therefore come from another
  source-level lifetime, not duplicated south tails.
* `VulcanCannon_UpdateFacingTurn`: passing `p->allowTurn` as the final zero argument proves
  the equality-CSE diagnosis: it restores the missing four-byte instruction and exact branch
  layout, but reloads the byte through a retained offset base (4/4). Constant spellings,
  signed/unsigned zero tests, and a separate zero local all reproduce the 5/3 draft.
* `Ivy_UpdateDefeat`: the saved helper is closer than the prior index recorded (5/5). Reversing
  the table/index addition and pointer spellings cannot change machine order. An explicit local
  moves the literal early but returns the 14/14 allocation swap; duplicated-load arms inflate
  every inlined call and are far worse. The remaining form must load the literal early without
  creating a named table lifetime.

Additional pass-2 allocation attempts:

* `Vorn_SelectReactionControl`: duplicated boolean tests, zero expressions derived from `active`,
  split result/call forms, and signed/unsigned copy locals all optimize before global allocation
  or worsen control flow; they leave the exact r4/r5 swap unchanged. A source-level ninth use
  must survive through global allocation rather than being propagated by combine.
* `Axe_UpdateDamageSpin`: using the long-lived `context` as the branch-local `state`, or for any
  single state access, lets CSE merge the two pointer quantities and is substantially worse
  (34+ rows). The near match specifically requires two loads of the same actor context; the
  remaining +20 priority cannot come from sharing those source locals.
## cdp-scene2 (Codex)

Queue audit on 2026-11-13 found that 21 of the 26 assigned functions no longer contain a
concession on the current branch. Their queue scores predate the cleanup already present in
`src/actor_animation_controller.c`. The five remaining functions are retained below as ordinary-C,
pin-free drafts; `scope.py ... pins FUNC` verified the checked-in forms match and measured each
removal independently.

### Actor_UpdateBlinkBurstDisappearance: deferred

Draft: `notes/phase3-drafts/cdp-scene2/Actor_UpdateBlinkBurstDisappearance.c` (one missing
`movs r7, #0`, plus compensating alignment padding). The child-flags r2 pin was removed by using
the shared `DisplaySequenceControl` union form; this matched exactly and was committed separately.
The remaining pin initializes r7 after the controller pointer's final use, and the emitted zero has
no machine-level uses. Tried a scalar zero loop bound, reassigning the dead controller pointer,
zero-initialized scalar/union/array/struct counters, and using the zero in the loop bound. Either
GCC deletes the initialization or retains a compare and changes allocation. Next: find a natural
multi-set union whose initial zero survives RTL generation but whose eventual use folds away.

### Actor_UpdateFinishingAnimation: deferred

Draft: `notes/phase3-drafts/cdp-scene2/Actor_UpdateFinishingAnimation.c` (the same two-row zero
initialization/alignment difference as the blink-burst twin). The child-flags r2 pin was removed by
using `DisplaySequenceControl`; the ordinary union form matches exactly. The sole remaining pin is
the dead r7 zero. The scalar, union-counter, pointer-reassignment and loop-bound forms tried on the
twin either delete that zero or add a compare. Apply any natural surviving-zero form found for the
twin here as well.

### ActorDisplay_UpdateForegroundSprite: deferred

Draft: `notes/phase3-drafts/cdp-scene2/ActorDisplay_UpdateForegroundSprite.c` (78 differing rows:
62 ROM / 65 draft, with 11 moved branches). The remaining pin is the accumulated horizontal
coordinate in r1. The earlier typed-layout rewrite is already present and preserves the verified
halfword fields. Removing the binding changes allocation through most of the function, rather than
one isolated instruction. Next: split the horizontal and vertical sums into narrow typed helpers,
and compare reference density and copy preferences against the sibling foreground/accent display
updates.

### SingletonSlot7_UpdateEndingScene: deferred

Draft: `notes/phase3-drafts/cdp-scene2/SingletonSlot7_UpdateEndingScene.c` (81 differing rows:
73 ROM / 71 draft, with seven moved branches). Its four pins are two occurrences each of a constant
one and a byte zero. Individually, the zero sites cost 8 and 3 rows; each one site costs 32 rows.
Grouping equal-named locals does not recover allocation. Next: use a union/array initializer for
the byte zero and repeat each constant-one operation within the branch arms so cse and jump2 see
the same lifetimes as the ROM.

### SingletonSlot7_InitDisplaySprites: deferred

Draft: `notes/phase3-drafts/cdp-scene2/SingletonSlot7_InitDisplaySprites.c` (95 differing rows:
88 ROM / 80 draft, with two moved branches). Thirteen concessions remain: eight pins and five
barriers, seven of them inside the private `StepPlayback` helper. Independent costs range from 4
to 45 rows and the helper group costs 137 rows, showing that its C shape, rather than one caller
allocation, is the blocker. Next: replace the private playback implementation with the shared
display-sequence stepping form, preserving table-first packed-command reads; then retry the two
caller-side constant-one/duration groups separately.
## dp-field (Codex)

Tags: #phase3 #field #matching #compiler

### Scene_TryStepDown (08004CE4): one barrier removed, four concessions remain

The matching partial moves the type calculation after the existing mask barrier.
This removes the height-byte input barrier without changing any instruction.
Stock `tools/check.py src/scene_object.c` matches all 37 functions (7,236 bytes).

Best pin-free draft: `notes/phase3-drafts/dp-field/Scene_TryStepDown.c`
(18 ROM / 20 draft differing instructions, shape 6). It reads the cell byte into
`mask`, copies it to `heightByte`, computes `type`, then sets `mask = 0xF0`.
The nonconstant first set removes the constant-equivalence priority penalty:
mask takes r4, cell r6, type r7, direction r8 as in ROM. It leaves an extra copy
and shifts before setting 0xF0. Moving the mask assignment before the shift lets
CSE delete the first mask set and restores the earlier allocation failure.

The earlier field-effects draft remains useful as a second frontier. It matches
under the combined diagnostic knobs `AGBCC_PRI=mask:+1400`,
`AGBCC_FORCE=heightByte:1,current:2,referenceHeight:0`, and
`AGBCC_NOTIE=231,485`. Thus the remaining blockers are mask priority, allocation
of the two loaded bytes and two-address operand selection for both nibble ANDs.
These are diagnostic overrides only; they are not used by any landed source.
The saved draft also improves with `AGBCC_PRI=current:+544` (13/15 differing).

Tried: reusing index or the override pointer for mask; initial mask copies before
and after the shift; narrow mask and reference-byte types; union mask initializer;
union byte references; swapping AND operands; decrement helper; assigning the
low mask to remaining before the AND; and mutating lowMask directly. The latter
removes the unwanted remaining copy but gives lowMask the long-lived r4 instead
of ip. The helper/copy forms change CSE's canonical representative and spill
layout. Narrow union byte reads prevent ties but perturb allocation substantially.
Keeping the other original concessions while applying these forms did not remove
a second site. Parameter/neighbor cleanup alone does not preserve the partial's
allocation when the remaining pin is retained.

Next: a height/nibble representation that retains the nonconstant mask priority
without retaining its initial copy; or a shared nibble operation whose subreg
operands prevent both regmove ties without making lowMask the canonical result.

### Trigger_RunScript (08006F00): argument pointer pin/barrier removed

The matching partial uses `argsPtr->values = values`, removes the redundant
`argsPtr = &args` before Script_Call, and removes both the argsPtr input barrier
and r2 pin. The single pointer definition survives GCC's PRE at the correct
position, and its use for the values field gives the necessary allocation.
Stock `tools/check.py src/item_container.c`: all 30 functions MATCH (7,024 bytes).
One actor-output barrier and one actor-index pin remain.

Best pin-free draft: `notes/phase3-drafts/dp-field/Trigger_RunScript.c` (11/9,
shape 6). As in the earlier field-effects notes, its pointer-based word RMW gives
the right register but the wrong two memory operands. Direct `&args` for that RMW
matches with `AGBCC_PRI=argsPtr:+2000` except the actor-copy region (9/7, shape 2).
The matching partial's source becomes 23/23 when both remaining actor concessions
are simply removed: CSE merging also changes pointer and saved-value allocation.

New attempts: s8 union and array actor views; an inline active-player predicate;
a validation while/break form; indexed copying, indexed source, descending copy,
and destination-end loop. Union/array views compile like the baseline. The helper
changes the return/control region; the validation loop changes loop-note priorities.
The indexed and descending copies lower to the same loop as the baseline, so they
do not fix the argument-pointer priority. The destination-end loop changes shape.
The argsPtr partial is now resolved; the next pass should focus on a real boundary
that preserves a short-lived actor-index copy before the saved r5 value, then
reassess the fully pin-free pointer allocation with that corrected copy present.

### DirectionalController_CheckRemaining (080098CC): resolved

A value-less `return;` on the accepted branch, with `return 0` after SetState on
failure, exactly matches the ROM with no pins or barriers. The angle is used
only as `(u16)Math_Atan2(dx, dz)` for direction quantization. No angle pseudo
needs to survive the later blocks; expand_function_end's hard-r0 USE retains
the call result and makes local-alloc avoid r0 throughout the success path.
Both the explicit value-less return and a goto to a fall-through function end
match. The former reads more directly and is landed. This extends fact 95 to a
function that has an explicit zero return on its other path (fact 160).
Stock full-unit check: 21 functions MATCH (2,352 bytes).

### sub_08010C10 (08010C10): still deferred, first reload scratch

Best pin-free draft: `notes/phase3-drafts/dp-field/sub_08010C10.c` (2/2, shape 0).
Confirmed `AGBCC_SPILL=0:1`: MATCH. The first signed x load needs r2 for the
constant offset where the stock default scan picks r1; all other bytes match.
The newer why explanation corrects an inference in the earlier field-effects
note: shifting the initial scan does not prove a prior ROM reload choice.
Its spill set is r1/r2, both free, and there is no previous choice in this draft.

Additional forms: duplicate the post-join Vec16_Lerp in both active arms (5/19,
shape 20; jump2 does not fully merge the argument setup); signed-axis union
(6/8, extra extensions); inline reverseAxis for both coordinates (same 2/2);
zero-vector subtraction (same 2/2). These supplement the earlier documented
40-plus spellings rather than repeating their offset/actor/else-first searches.
No reduction landed. Next: a real shared vector operation that makes r1 live
at the x load, or an earlier setup whose RTL consumes a scratch before later
cross-jumping. A changed initial scan alone is not a C solution.

### sub_08017510 / Weather_UpdateTilemapScroll (08017510): allocation-only frontier

Best pin-free full-unit draft: `notes/phase3-drafts/dp-field/sub_08017510.c`
(48/48, shape 0; exact size 372 bytes). Replace the private Random macro and
literal table with `ParticleRandom_ReadDirectSample(gRandomTable)` through a
local table pointer. This fixes table-first instruction order, repeated reloads,
and cross-jump shape. ReadSample's statement macro instead changes shape (61/65
with the named table); literal table plus direct helper retains late loads (56/57).

Exact diagnostic: `AGBCC_FORCE=54:4,table:3` on this saved draft gives MATCH.
The index-address pseudo 54 and table 52 take r3/r6 in ordinary C; ROM r4/r3.
Those two decisions also put p in r6 as required. Raising index priority alone
by 11714 still leaves p in r4, because index takes r2 instead; this is register
preference/availability as well as ordering. p has 39 refs / 143 life (priority
13636), index address 5/52 (1923), table 4/62 (1290).

Additional attempts: make the existing Random cursor argument actually read and
write its pointer (same 56/57 fully pin-free); initialize table/cursor before the
scroll-delta call (54/54, shape 8); keep original pins but replace their barrier
with real cursor usage (18/18, shape 14; hard pointer aliases stop index reuse);
read only the first draw through cursor then write the global for subsequent
reuse (10/10, shape 2, but the separate global address takes r5). No partial
reduction landed. Next: a shared table/cursor capture whose real uses preserve
the ROM allocation without aliasing away the cached random index.

### TetherActor_BlendPalette (08048E0C): one moved copy remains

Best pin-free draft: `notes/phase3-drafts/dp-field/TetherActor_BlendPalette.c`
(1/1, shape 2). Initialize i before weight: weight's shorter life puts it in ip
and the component mask in r8 as ROM, but its `mov ip,r1` is then after the i=0
initialization instead of before it. The baseline (weight before i) is 6/6;
`AGBCC_PRI=weight:+26` gives MATCH. Priorities: componentMask 3068 (9/88),
weight 3043 (7/46); losing one weight live insn or adding one mask live insn
would flip their order without a large structural change.

Tried after diagnosis: weight/mask declaration orders; s16 weight (1/3, correct
allocation but extra sign extension); s16 inverse; u16/s32/u64 component mask;
union mask and weight aggregates; array/union loop counter; weight=64-inverse;
helper returning palette weight; direct channel extraction; indexed-vs-pointer
output; multiply instead of shift for packed colors; for/while/goto loop forms.
The aggregate counter keeps the one moved copy; while/for match the baseline;
the goto form changes loop allocation. The pointer output materially changes
subword stores. Next: shorten the baseline weight life by one pre-combine insn
without moving its first copy, or lengthen the mask life by one without moving
its emitted initialization. No reduction landed.

### TetherActor_StartInteractionEffect (08049D4C): resolved

Removed the r3 zero pin by repeating the final effectValue store and attachment
call inside each accepted branch. Reload sees a single branch-local straight
line and eliminates its redundant literal-zero argument load. jump2 subsequently
cross-jumps the repeated tails back into the ROM's shared sequence (fact 91).
Only duplicating the effectValue store did not suffice (17/25); the whole tail
was needed. Arrays/unions retained the extra literal zero or added extensions;
readback from effectValue folded back to the baseline. The original pin-free
baseline was 1/1, shape 2 (a second movs r3,#0 before the call).
Stock full tether-actor unit: 114 functions MATCH (20,516 bytes).

### TetherActor_BeginDirectedState (0804A15C): resolved

Removed the sample r0 pin by capturing the X difference with the existing
ARMOR_CAPTURE_COMPONENT_DELTA helper. The saved actor initially wins allocation
(5 refs / 36 live, priority 2777) over dx (3 / 11, 2727). Diagnostic
`AGBCC_PRI=dx:+51` gives MATCH. The shared capture form raises the real reference
density and produces the same allocation with the stock compiler. Applying it
to both components changes allocation again (28/28); the Z difference remains
a direct field expression. The heading now uses directedHeading instead of a
raw offset. No shared header changed.

Table-first random draws improved the baseline from 24/24 to 9/9 but moved the
table and heading setup. Splitting the motion tail (8/24) or angle assignment
(16/20) let CSE specialize the zero-delta path and did not fully cross-jump.
Stock full tether-actor unit: 114 functions MATCH (20,516 bytes).

### TetherActor_InitKindFromScript (0804A430): resolved

The joined kind value lost r0 to the selected-kind address: 7/9 differing, shape
2. Diagnostic `AGBCC_FORCE=selectedKind:1,36:3,38:2` gives MATCH; forcing kind
alone overlaps the already allocated pointer and is not a valid diagnosis.
Raising kind priority by 20000 does not evict the local address temporaries.

Repeat both the selected-kind store and actor-kind reload in each branch, with
a separately declared branch-local kind value. This prevents the early merge
that a single outer kind variable permits. Each call result stays in r0 during
allocation; jump2 then cross-jumps the identical tails. Stock unit MATCH.
Repeating the store alone is 4/6 with branch locals; repeating both with an
outer kind is still 7/9. Short/union values, union pointers, ternary selection,
inline store helper and unsigned slot did not fix the baseline allocation.

### TetherActor_InitDisplayFromScript (0804A600): resolved

Removed the display barrier by repeating SetPriority and the following keyword
read in both initialization branches. The baseline is 23/23, shape 0: display
priority 4363 (8 refs / 55 live) loses r4 to actor at 4639. Diagnostic
`AGBCC_PRI=display:+277` matches. Repeating SetPriority gives correct allocation
but an extra three-instruction call: the fall-through arm ends in flow's USE 0
which blocks cross-jumping. Including the keyword result assignment makes both
tails shareable and gives stock MATCH (full unit, 20,516 bytes).
Count-up palette copying and union display pointers retained 23/23. Hoisting
the display pointer or removing source's distinct lifetime changed shape.
Repeating the entire script/heading tail was too broad (23/35).

### TetherActor_UpdateWorldPosition (0804AF80): three concessions removed

Landed the typed ContactPosition x/y/z stores in place of u16 indexing: these
member stores preserve the display-pointer reloads, removing both memory
barriers. A u16 shake value removes its r0 pin while preserving the addition
operand allocation; narrowing coordinate too breaks it (26/24), while a signed
value leaves 7/9. The named displayOffsetY field also matches. Retain the
existing offset-variable assignments for now: deleting all of them changes
later move2add reuse and reload rotation (11/11). Eight pins and three barriers
remain. Stock full tether-actor unit: MATCH (20,516 bytes).

Best pin-free draft: `notes/phase3-drafts/dp-field/TetherActor_UpdateWorldPosition.c`
(25/24, shape 7), improved from the current simple removal's 43/38 and old
field-effects draft's 67/64. Its display region now matches; remaining problems
are terrain-store address reuse, previous-value registers and final copy scratch.

The final copy alone becomes a plain targetPosition struct assignment plus
named heading store with only 3/3 differing (offset scratch r3 versus ROM r0).
`AGBCC_SPILL=13:2` gives MATCH on that isolated tail-struct candidate. A local
pointer, pointer union, inline copy helper and u64 copy keep the same difference;
a temporary struct/union changes load/store shape (5/5), builtin memcpy is 10/10.
Removing the previous-height r2 pin alongside this tail is also just 3/3, so a
corrected scratch decision could remove that pin too. Keeping only the old
offset pin/barrier with the struct copy worsens to 10/10.

The full clean draft still uses a subtract of two for height's address where
ROM loads 0x129 in r4, then previousSlope/Height/Attributes need r0 destinations
and r4/r2/r4 samples. No combined matching override was established. Next:
change the terrain/previous-field C to make the needed reload rotation before
the final struct copy, then remove its six remaining copy/heading concessions.

### TetherActor_UpdateLockedShake (0804B43C): resolved

Removed the table pin with the shared PARTICLE_RANDOM_SAVE_ORIGIN capture and
ParticleRandom_ReadSample draws. The named-table sample-helper draft is 29/27,
shape 12; `AGBCC_PRI=randomTable:+293` gives MATCH. Table priority 1320 loses
to amplitude 1612 and index address 1400. The real table capture adds reference
density so table takes r6, amplitude r7 and index address r8, fixing all downstream
reload choices. The same origin helper already captures gRandomTable elsewhere.
A union table initializer retains 29/27; constructing the halves changes shape
(52/55). DirectSample is 31/29. No shared header edits. Full unit MATCH.

### TetherActor_BlockPlayerAcrossTerrain (0804B6A4): pointer PRE frontier

Best pin-free draft: `notes/phase3-drafts/dp-field/TetherActor_BlockPlayerAcrossTerrain.c`
(1/3, shape 2). Capture current=actor before the player-height call and use
current for heightPosition. This avoids the unwanted PRE reaching register but
leaves one actor copy into r6 plus padding; heightPosition then reads r6 instead
of r7. CSE_NOCANON=73 does not remove that copy.

Baseline without the barrier is 9/15, shape 14. `pre` proves that replacement 1
hoists actor+80 across the player-height calculation into an extra sb pseudo;
`AGBCC_NOPRE=1` gives MATCH. The gPlayers PRE replacements are required and
disabling them worsens the result.

Additional forms after the prior 24-variant pass: branch-local initial position
(2/4, but its address moves after the test); compute position after the height
join (13/15, shape 2, extra r4 copy and later allocation); no inner position
assignment; late inner assignment; union actor, array position and inline
position helper (same PRE as baseline); pointer copy before attributes or
initial position (8/14), versus after the player-height call (baseline); shared
surfaceHeight for actor (10/14 or 23/23); reusing position (5/3, removes ROM's
recomputed address and copy).

Restoring actor=current immediately after the player-height call removes the
extra copy but swaps actor/position r6/r7 (13/13, exact size). Raising position
priority by 181 corrects those registers but produces four extra instructions,
so that is not a complete diagnosis. Restoring after height/difference changes
shape; using position for final Z accesses removes needed address operations.
Next: a natural actor/position lifetime that keeps actor+80 non-anticipatable
through the height call and coalesces the saved actor copy. No reduction landed.

### TetherActor_UpdateAuxiliaryTransition (0804C748): two pins removed

Landed explicit low-nibble mask variables in both later frame branches, which
prevent regmove from tying their results to rawFrame. This removes rawFrame's
r2 pin. Capturing the display address with TETHER_CAPTURE_DISPLAY, used in both
visibility arms, gives activeDisplay enough real references to take r4 before
actor; frame can then reuse r4 without its pin. The capture performs the same
field-address operation in both arms and has no selector. Stock full unit MATCH.
One value pin remains at the hidden auxiliary-sprite flag update.

Baseline all-pin removal is 45/45, shape 0. Exact diagnostic on base.c:
`AGBCC_PRI=activeDisplay:+318 AGBCC_NOTIE=313,341 AGBCC_FORCE=240:2`.
Actor priority 3846 beats activeDisplay 3529; raising activeDisplay alone leaves
9/9. Raising frame instead worsens to 73/72. Moving display copies immediately
after calls is 11/11, shape 4; before calls is 47/43. An inline display getter
retains the allocation mismatch. The frame pointer cannot be reused for the
final increment without changing the ROM's address lifetimes (61/62).

Best fully pin-free draft: `notes/phase3-drafts/dp-field/TetherActor_UpdateAuxiliaryTransition.c`
(2/2, shape 2, exact size). An inline auxiliary-flag reader gets the ROM's r2
address, r0 value and r1 mask, but leaves `adds r0,r2` then loads through r0;
ROM loads r0 directly through r2. The extra copy consumes existing alignment
padding. Pointer-taking, local-pointer, signed and wide return getters do not
remove that copy; a one-word struct return is equivalent.

Other flag attempts: direct RMW, value-before-address, signed word, value array,
union word, union bitfield, complement/AND/complement (5/5); bitfield-pointer
store changes access width; u64 value changes shape; moving display's copy
earlier changes the register graph. Next: fold the helper's address-copy pseudo
into the load while retaining its local allocation, or another real visibility
helper that produces that load/store ordering.
