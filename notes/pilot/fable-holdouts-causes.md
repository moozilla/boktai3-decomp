# fable-holdouts: what is still open, grouped by cause

State on 2026-11-15, after 55 functions cleared. One line per function is in
`fable-holdouts.tsv`; this file groups the open ones. "Open" means a concession is still
in the source (`python3 tools/progress.py --list-pinned`).

## Levers that cleared functions late in the run (try these first on the open ones)

* A block-local temporary takes r4 in local-alloc and pushes a long-lived pointer to r5:
  pass `index + delta` as a call argument and name it again afterward
  (PuzzleBlock_TryStepDown).
* A test combine folds from known zero bits (`if (byte < 0) return 0;`,
  `value < 0 ? -1 : 0`) leaves a block boundary or an unknown zero until jump2. regmove
  then copies the mask instead of tying the operand, and cse cannot share the zero
  (PuzzleBlock_TryStepDown, WirelessGameItems_UpdateIncomingItem). Guessed wording.
* A two-sided clamp ends the cse path, so constants after it are loaded again.
* `ParticleRandomTableSource` halves (include/engine/math/monster_particle_random.h) keep the random
  table register from being REG_EQUIV, so local-alloc does not double its life
  (sub_0801404C). With plain `ReadDirectSample` its priority then sits one point under
  the sprite pointer, as in the ROM.
* `ParticleRandom_ReadSample` (statement macro) against `ReadDirectSample` changes the
  table's ref count by one per site (sub_08017510).
* Arguments that the ROM reloads from the stack on every use are plain spilled
  registers, not a struct; stack slots follow pseudo number.
* Code after an `if` guard that looks like hand-written setup (table, pointer, mask) is
  what loop.c hoists out of a `for` loop; write the `for` loop. gcse cannot insert on
  the guard's edge, which is why an invariant such as `options * 4` stays in the loop.

## Open, by cause

Random-table register priority (REG_EQUIV doubling or ref count; try the two table
levers above): sub_080399F4, ImpactParticleGroup_UpdateFrozen, Vorn_UpdateDefeat,
sub_08012B34, FadingParticleGroup_Spawn, FadingParticleGroup_Update, sub_08014CA4.

Global-alloc order off by a few priority points between two registers
(refs/life near tie): Collectible_UpdateEntry, TetherRing_InitializeFromScript,
MovingPlatform_RegisterFollowCells, Hresvelgr_UpdateBoundaryBarrage,
SceneActorContacts_ReleaseInvalidOwners, Centipede_AlignSegmentsToTerrain.

local-alloc choice inside one block (which value takes r0): sub_081D3620,
sub_0803198C, sub_08217430.

cse operand order or sharing (a constant cse must not see, or must not share):
GameState_SelectRamBase, SceneInput_TickChoice, TetherActor_BlendPalette,
TetherActor_UpdateBounceReturn. The folded-test lever above is untried on these.

jump2 cross-jumping of identical tails: ImpactBurst_Create,
Centipede_UpdateRetraction.

Reload spill or rotation order: PileDriver_TickRig, Mummy_ResumeSavedRouteAfterAnimation.

In progress when this was written: sub_08014634 (ParticleGroup_SpawnBouncing). The
committed source uses a local struct, unions and pins. A plain rewrite with seven
scalar parameters and two nested `for` loops (`g = &manager->groups[j]`,
`p = &g->particles[i]`, `sub_08217E40(&p->flags, 0)`) gets the structure: arguments
spill as in the ROM and loop.c makes the setup code. Left: the table constant hoisted
into r7, `&gRandomIndex` hoisted out of the outer loop into r3, and the group base
kept in a stack slot beside r6. Drafts: build/w/Q3_b.c (not committed; rebuild from
this description).

Not started (below 08057E04): sub_08014CA4, FadingParticleGroup_Spawn,
FadingParticleGroup_Update, WirelessGameMenu_InitEquipmentChoices,
WirelessGameDisplay_UpdateRoundAnnouncement, WirelessResultPanel_Init,
ContactActor_UpdateRetreat, TetherActor_UpdateWorldPosition,
OrbitBackground_UpdatePosition, StackedActor_Initialize,
InteractionMarker_InitializeFromScript, SavedPositionMarker_InitializeFromScript,
DisplayTrack_Update, TerrainEdgeMarker_FindTerrainDirection,
LevelAdvanceDisplay_UpdateAnimation.

## Guessed wording to review

These match with no concession, but the C form was chosen for its effect and may not be
what the authors wrote: TextState_ProcessTag (`cmp = 0` dead store),
WirelessGameItems_UpdateItemMerge (per-arm zero, dead `count < 0`),
WirelessGameItems_QueueLocalItem and QueueIncomingItem (`clear = 0; if (x < 0) clear = x`),
WirelessGameItems_UpdateIncomingItem (`value < 0 ? -1 : 0`), PuzzleBlock_TryStepDown
(`if (referenceHeight < 0) return 0;`), sub_080399F4
(`RANDOM_TABLE_ADDR + (p->shakeTicks >> 8)`, an always-zero term), sub_0801404C (table built
through the halves union).

## Recipe: random-table register priority (scope README fact 231)

1. In `u.i.lreg`, if the table's (or another once-set constant's) set carries `REG_EQUIV` and `scope.py A.c vs B.c FUNC` shows its `live` at twice the real span, it is doubled and ranks one register too low. Drop the pin or barrier and make combine, not cse, form the constant: `table = (const u16 *)(RANDOM_TABLE_ADDR + (p->someU8Field >> 8));` (no stack slot; sub_080399F4, 8a3ff6351), or the address-taken halves union from include/engine/math/monster_particle_random.h when the ROM frame has a spare 4-byte slot (sub_0801404C, c34073c60). Build it once, outside the loop, where the ROM's `ldr =0x203B400` sits.
2. Then settle any remaining near tie with ref weights only: `ParticleRandom_ReadDirectSample(table)` (2 per site in a loop) against `ParticleRandom_ReadSample` (3), and the extern `gRandomTable` rather than a numeric `#define` when the ROM hoists the address. Group still open: ImpactParticleGroup_UpdateFrozen, Vorn_UpdateDefeat, sub_08012B34, FadingParticleGroup_Spawn, FadingParticleGroup_Update, sub_08014CA4, sub_08014634 (table must outrank velocity: 17/165 undoubled).

Both forms are guessed wording (an always-zero term; a pointer built in halves) and belong on the review list below with sub_080399F4 and sub_0801404C.

sub_08014634 update: best draft is seven scalar parameters, the extern `gRandomTable`
(no numeric `#define`), `rng = &gRandomIndex` before the outer `for`, `u32 mask = 0x3ff`
in the inner body, plain flags: 46 ROM / 44 draft. Left: the table must outrank velocity
(17/165 undoubled), the 0x3FF hoist must land after the giv inits, one reload choice.
The fact 231 forms do not apply directly: the ROM's table load is a loop.c hoist, and a
combine-formed constant written before the loops scores worse (52).


## Round 13 recipes (2026-11-16)

* An early dead origin loses every priority race but needs r3: use a distinct
  branch-local symbolic origin, then copy it after the angle draw. Local allocation
  gives the sources r3, and their surviving copies reserve r3 for the joined value
  (08012B34; a numeric literal folds the copies).
* A cached byte loses a repeated zero extension: normalize the source in a signed
  inline (`if (type < 0) type = -1`) before returning it to the cache. The sentinel
  contributes unknown bits before combine deletes the impossible branch, while the
  final return copy preserves the separate input register (08014330).
* A constant RAM symbol must be formed after CSE: `(u32)symbol & ~(byte >> 8)` or
  `& ~(halfword >> 16)` keeps the symbol and becomes constant only in combine
  (0821B084). These known-zero forms are allocation-only wording for phase 4 review.

* PRE creates extra invariant array bases and spills: add a byte-derived zero to
  the owner address at each indexed state write, so the base becomes invariant
  only in combine (08014CA4). The resulting allocation also fixes the table/group
  ordering; a late table constant then replaces the halves union (also 08014634).
* Two lifetime copies collapse: place `if ((s32)raw < 0) raw = 0` between the
  comparison snapshot and the time snapshot (08016118). For an unsigned-byte load,
  combine makes the branch unconditional; its boundary survives regmove and the
  dead arm disappears in jump2. A pointer self-identity with a byte-derived zero
  separately preserves the velocity reload formerly protected by a memory barrier.
* A high register is wrongly reused during reload: mention its earlier value in
  a late zero term, e.g. `mask & (actor->direction4 >> 8)` (081A0BC8). Flow records
  the longer live range; combine erases the term but the conflict still excludes sl.
* A global near-tie needs more references: expose the naturally identical fallback
  stores or early-release returns before jump2 (080AA8C4, 08101B1C). Use actual byte
  or halfword field widths to tune the measured competing live spans; jump2 shares
  the tails back to the ROM. All these allocation-only forms require phase 4 review.


## Round 13 batch 3 recipes (2026-11-17)

* A shared random-index address hoists too early: change first-pass invariant
  moves so loop.c defers it to pass two; staged union complements and the first
  sample offset cleared sub_0819BB18 (fact 244).
* Call tails share incorrectly: preserve distinct post-call USE counts to keep
  them separate (080154EC), or equal counts to let jump2 merge them (080AB968).
  Pointer unions and bounded guards preserve the needed late boundaries (fact 245).
* A macro's removal changes constant loads: check reload before blaming CSE.
  Reuse a global completion scalar, then independent late-zero self-assignments
  of the cached spark values to recover priorities (080AEE08; facts 246-247).
* A near tie needs one definition: an impossible signed-negative arm over a
  bounded value retains weighted definitions through allocation. Tune the real
  competing lifetimes and, for equal priorities, declaration order (0815F5B4,
  0817AEC8; fact 248). These forms are recorded for phase 4 review.

* A callback needs an earlier address's register preference: use one union for
  the disjoint values and retain a dead read across the later CSE block, so their
  pseudo stays canonical (0803198C; fact 250). The first address's overlap with a
  fixed argument excludes r0 and leaves the callback in r1.


## Round 13 batch 4 recipes (2026-11-17)

* A needed low spill register is missing: a bounded unreachable arm can request
  it before jump2, while preserving the final real loads (0820EE34; fact 251).
* A table's copy survives but still loses its loop register: remove REG_EQUIV,
  then measure and restore its reference weight with independent late identities
  without changing direct sample modes (08234540; fact 252).
* An unused saved high register is missing: bounded dead global carriers retain
  a reload home. A nested condition distinguishes global quantities from local
  callee homes; dead low results can control scratch rotation (08038448; fact 254).
* A value needs one argument register: an unreachable call can supply its copy
  preference, provided its exit rejoins the original control flow (081A14FC; fact 255).
* PRE inserts the right expressions in the wrong order: inspect its stock hash
  bucket count. Two dead unsigned folds change 141 to 145 and restore Mummy's
  spill order; all folds vanish before output (080D1784; fact 256).
* Pins reserve registers across a large state machine: reuse disjoint values in
  unions, then retain a bounded exit lifetime and a cross-block zero reference.
  This preserves Skeleton's sb/r8/sl values and spills the shared origin (080BD1F0;
  fact 257). All listed forms are allocation-only phase 4 review items.


## Round 13 completion recipes (2026-11-17)

All 36 functions in the original concession queue now match with zero counted
concessions. The earlier open-member lists above are historical checkpoints;
[the completion report](../round13-batch5_2026-11-17.md) maps every function to
its matching commit.

* A saved pointer is forwarded into a callback too early: keep a typed dead
  store through regmove and retain the pointer's earlier CSE lifetime. The final
  store disappears in jump2 (0803B40C; fact 258).
* A timer needs a different argument register after reload: first admit the
  missing low scratch register with bounded signed loads, then supply the
  separate argument-copy preference without crossing the dead call (0805AC3C;
  fact 259). The rig initializer uses the existing late-table recipe (0805B0F0).
* A sample should remain local while its result is global: a bounded negative
  halfword guard after the coordinate sum preserves that allocation boundary,
  and cursor reuse keeps the next address in its required register (081D3620;
  fact 260).
* A dead preference arm disables the desired implicit GIV: make the real key
  cursor explicit, preserving the guard, initializer order and pointer-before-
  index increments (08217430; fact 261).
* A mask and later pointer need the same saved register: reuse a typed union
  across disjoint lifetimes and weight its references with single-pass loops.
  Unlike a dead branch, this leaves the original PRE boundary intact (0804373C;
  fact 263).
* Correct scalar spills become unwanted induction cursors: retain a bounded
  zero in the halfword index until after loop.c, then use explicit byte-offset
  pointer additions. Declaration order now controls the original six homes
  (08071F50; fact 264).

Every listed source form is named as allocation-only in its function commit for
phase 4. The ordinary stock compiler is the acceptance authority throughout.
