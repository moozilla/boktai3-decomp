# fable-holdouts: what is still open, grouped by cause

State on 2026-11-15, after 55 functions cleared. One line per function is in
`fable-holdouts.tsv`; this file groups the open ones. "Open" means a concession is still
in the source (`python3 tools/progress.py --list-pinned`).

## Levers that cleared functions late in the run (try these first on the open ones)

* A block-local temporary takes r4 in local-alloc and pushes a long-lived pointer to r5:
  pass `index + delta` as a call argument and name it again afterward
  (Scene_TryStepDown).
* A test combine folds from known zero bits (`if (byte < 0) return 0;`,
  `value < 0 ? -1 : 0`) leaves a block boundary or an unknown zero until jump2. regmove
  then copies the mask instead of tying the operand, and cse cannot share the zero
  (Scene_TryStepDown, WirelessGameItems_UpdateIncomingItem). Guessed wording.
* A two-sided clamp ends the cse path, so constants after it are loaded again.
* `ParticleRandomTableSource` halves (include/monster_particle_random.h) keep the random
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
levers above): sub_080399F4, ParticleGroup_UpdateFrozen, Vorn_UpdateDefeat,
sub_08012B34, FadingParticleGroup_Spawn, FadingParticleGroup_Update, sub_08014CA4.

Global-alloc order off by a few priority points between two registers
(refs/life near tie): Collectible_UpdateEntry, ProximityEmitter_InitializeFromScript,
NamedObject_RegisterFollowCells, Hresvelgr_UpdateBoundaryBarrage,
SceneActorContacts_ReleaseInvalidOwners, TentacleActor_AlignSegmentsToTerrain.

local-alloc choice inside one block (which value takes r0): sub_081D3620,
sub_0803198C, sub_08217430.

cse operand order or sharing (a constant cse must not see, or must not share):
GameState_SelectRamBase, SceneInput_TickChoice, TetherActor_BlendPalette,
TetherActor_UpdateBounceReturn. The folded-test lever above is untried on these.

jump2 cross-jumping of identical tails: AnimatedParticles_Create,
TentacleActor_UpdateRetraction.

Reload spill or rotation order: ScenePanel_TickRig, Mummy_ResumeSavedRouteAfterAnimation.

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
WirelessGameItems_UpdateIncomingItem (`value < 0 ? -1 : 0`), Scene_TryStepDown
(`if (referenceHeight < 0) return 0;`).

## Recipe: random-table register priority (scope README fact 231)

1. In `u.i.lreg`, if the table's (or another once-set constant's) set carries `REG_EQUIV` and `scope.py A.c vs B.c FUNC` shows its `live` at twice the real span, it is doubled and ranks one register too low. Drop the pin or barrier and make combine, not cse, form the constant: `table = (const u16 *)(RANDOM_TABLE_ADDR + (p->someU8Field >> 8));` (no stack slot; sub_080399F4, 8a3ff6351), or the address-taken halves union from include/monster_particle_random.h when the ROM frame has a spare 4-byte slot (sub_0801404C, c34073c60). Build it once, outside the loop, where the ROM's `ldr =0x203B400` sits.
2. Then settle any remaining near tie with ref weights only: `ParticleRandom_ReadDirectSample(table)` (2 per site in a loop) against `ParticleRandom_ReadSample` (3), and the extern `gRandomTable` rather than a numeric `#define` when the ROM hoists the address. Group still open: ParticleGroup_UpdateFrozen, Vorn_UpdateDefeat, sub_08012B34, FadingParticleGroup_Spawn, FadingParticleGroup_Update, sub_08014CA4, sub_08014634 (table must outrank velocity: 17/165 undoubled).

Both forms are guessed wording (an always-zero term; a pointer built in halves) and belong on the review list below with sub_080399F4 and sub_0801404C.
