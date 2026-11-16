# Object-model cleanup, items 1 to 4 (2026-11-15)

Tags: #cleanup #structs #engine #actors #tooling #object-model

Phase-4 cleanup from the list at the end of `docs/OBJECT_MODEL.md`, items 1 to 4 (item 5,
the registry-actor base rename, waits). Every change is byte-identical: each unit that
includes a changed header was checked with `tools/check.py` through the new
`tools/header_dependents.py --check`. No concession was added; one unit
(`CellGate_ClampPlayerEdge`) lost its `*(u16 *)` casts along the way.

## Item 1: the script command table

* `data/actor_registry.csv`'s `name` column regenerated from `symbols/functions.csv` by
  callback address (Thumb bit masked) with the existing generator
  (`tools/datafmt/check.py --extract actor_registry`): 673 stale `sub_` names down to the
  24 callbacks that are still unnamed. The generator was correct, it just never re-ran.
* So it stays current: `keyed_callbacks.refresh_names()` rewrites the column without the
  ROM, `tools/promote_names.py` runs it after each naming batch, and
  `tests/tools/test_actor_registry_names.py` fails when the committed table is stale.
* Consumers now call it the script command table: the manifest description, the
  `keyed_callbacks` format, `tools/assets/{actors,gclnames,gcx,script_targets}.py`, Studio's
  actor page (counts and labels say "script commands") and nav, the atlas comment,
  `gcl/README.md`, and docs DATA, DATA_TRACKING, ENGINE, ASSETS, CENTAUR, OBJECT_MODEL. The
  file keeps its name.
* Ran: the new test, test_gclroles, test_script_trace, test_actor_command_tables,
  test_actor_marker_studio, test_studio_search; `tools/datafmt/check.py` (all OK).
* Not fixed: `tools/promote_names.py --dry-run` already stops on main with
  "sub_0805CD60 has two names: ScenePanelSubject_TickInactive, ScenePanelSubject_Idle".

## Item 2: task headers

* `struct Task` moved from `engine.h` to a new `include/task.h` (engine.h includes it), so
  object headers can embed it without pulling in all of engine.h and its prototypes.
* 116 structs began with a 0x18-byte byte array. 94 now begin with `struct Task task;`,
  each verified from its creator in one of three ways: the `Task_Create` result is assigned
  to or cast to the struct; it is handed to an init function whose parameter is the
  struct; or the creator installs `Task_SetCallbacks` callbacks that take the struct.
  `struct Player` is one (`Player_Create`, `Task_Create(6, 0xCAC)`; and the link-core
  creator sub_08243B0C).
* Not task headers (left alone): `PasswordEntry` (a 0x90-byte heap block; the 24 bytes are
  decoded symbol codes) and `ScriptKeyframeEntry` (an element of the entry array that
  starts at 0x1C inside the keyframe-pool task).
* Not verified (left alone; no creator in C or in gen/code_sym.s ties the block to the
  type): ActorAttachmentManagerResources, EffectPartManager, HealthBurstTask,
  AreaEffectLinkState, MenuPromptPanel, ModifierActionOwner, StatModifierOwner,
  ModifierMenuActor, OrbitBackgroundSelection, ParticleEmitterDisplay, CloudSprite,
  ResourceStation, SceneCombatActor, SceneInputPanel, SceneStimulusState, SolarBankPanel,
  StorageScreen, SunDiscPool, TetherPaletteView, VanargandBodyAnimation. Following each
  creator's init call chain further would settle them.
* Checked: 2,424 dependents of the changed object headers plus the 352 other dependents of
  engine.h/task.h, all match.
* Two units define a private `struct Task` (sub_08039490.c, sub_082250FC.c); they include
  none of the changed headers, so nothing collides yet. They are the next to fold in.

## Item 3: gPlayers

* There were nineteen declarations, not twelve (see the corrected OBJECT_MODEL.md item 3).
  Now one: `extern struct Player *gPlayers[4];` in player.h.
* Views removed and their fields mapped onto existing `struct Player` names:
  * AreaEffectPlayer: stateTag -> kind, flags -> statusFlags, action/actionPhase ->
    tetherMode/tetherPhase. Its `struct AreaEffectPosition` was `struct SceneVec` under
    another name and is replaced by it.
  * CellGatePlayer (declared as a plain `struct CellGatePlayer *gPlayers`, so every use was
    of slot 0): flags@0x1C -> kind, transitionMode -> tetherMode, direction ->
    coordinateAxis. New field `gateCount` at 0x4E3 (sub_08156FE0 read it as unk4E1[2]).
  * InteractionMarkerPlayer: flags -> statusFlags, action -> tetherMode, busy ->
    actionLatch464.
  * LevelAdvancePlayer (behind an `#ifndef GUARD_PLAYER_H` guard, now gone): mode -> kind;
    position copies write the union's `world` member.
  * OrbitBackgroundPlayer: transition -> tetherMode, target -> cameraTarget, targetActive ->
    cameraOffsetActive, heightScale -> visualSlowdownTimer (0xAF2, comment added).
  * ChasePlayer: flags -> statusFlags; its `u16 *cell` at 0x40 is `followTarget`, read as
    `((u16 *)followTarget)[3]`.
  * scene_indicator.h's `struct SceneActor *gPlayers` (x at 0x30, y at 0x34): position.x
    and position.z of slot 0. The struct is gone, so only scene_actor_lifecycle.h has a
    `struct SceneActor` now.
  * Unit-local forms: `u8 *gPlayers[]` in sub_08137DE8, sub_0815F578, sub_0815F588,
    sub_0815F5A0 (now `&positionalEffectGroup`, `facing`, `unk2D`); `void *gPlayers` in
    sub_0819FE9C (Hresvelgr; its private `struct Player` went too); six units
    (sub_08030710/7D8/818/864, sub_081D0F28, sub_08226CF8) each with a private
    `struct Player` (position at 0x30) and its own `struct Vec`, now SceneVec.
* No unit needed its old declaration form. CellGate_ClampPlayerEdge first mismatched:
  the ROM reloads `gPlayers[0]` after storing the clamped coordinate. With the old scalar
  pointer, the `*(u16 *)&...position.x` store could alias it. With the array, agbcc
  keeps the pointer in a register. Writing the store as a field (`position.x = ...`)
  brings the reload back, and the read as `(u16)position.x` keeps its ldrh. That form
  matches with no cast, so no comment was needed.
* Field names conflict between views (0x457 is "action", "transitionMode", "transition" or
  `tetherMode`); `struct Player`'s existing names were kept and are all still provisional.
* Still open: about 200 sites read gPlayers through a cast, 23 view types (17
  `SceneActorChasePlayer`, 7 `LiftPlayer`, `ScenePanelInteractionPlayer` behind the
  `SCENE_PANEL_PLAYER` macro, many `(*(u8 **)gPlayers)[0x418]`). Same job, larger; a scripted
  pass like item 5.
* Drafts under notes/phase3-drafts that still name `struct AreaEffectPosition` will need
  `struct SceneVec` when they are next compiled.

## Item 4: Nidhoggr

* `src/fn/sub_08191508.c` (Nidhoggr_HidePartWithExplosion) uses the header's struct.
  `NidhoggrCollider` gains `flags` at +6 (bit 2 disables the collider) and
  `NidhoggrPartExtra` gains `flags` at +4 (bit 0 hides the part). The local `parts` were
  `rows` (flags at row+8), `partPhases` is `rowInitFlag` and `partCompleted` is
  `rowDoneFlag`.
* Also folded in: `struct BossParts` in `src/fn/sub_08190C98.c`
  (Nidhoggr_ChangeCompletedPartState). Its `state[26]` at 0x71C2 is the header's new
  `partState` (it keys the part animation callback table in Nidhoggr_UpdatePartSequences).
* The unit's three index-scaled address computations stay as they were; each one
  written as a field access changes the code. They are not constant raw offsets, so they
  are not concessions.

## Doc errors found (fixed in docs/OBJECT_MODEL.md)

* `struct Task` is in `include/task.h` now (was engine.h).
* Item 2 counts: 116 candidates, 94 verified, not "about 80".
* Item 3: nineteen declarations (six of them with private `struct Player` definitions),
  not twelve with nine types; plus ~200 cast sites the doc did not count.
* Item 4: a third Nidhoggr view (`struct BossParts`) existed.
* Item 5: the `struct SceneActor` in scene_indicator.h was a gPlayers view (removed).

## Item 5: the registry actor base (in progress)

Evidence: `notes/room-actor-layout_2026-11-15.md` (from the new `tools/struct_views.py`).

* `struct RoomActor` (include/room_actor.h) is the base, 0..0x3D4. ResetView and the eight
  monster structs embed it as `base`.
* `struct ChaseActor` (include/chase_actor.h) is RoomActor + `struct SceneActorChaseGroup`
  (0x624 bytes), the type the shared chase code takes. `SceneActorRegistryResetView` is
  gone: it was ChaseActor plus a tail of family fields, which moved to new struct Vorn,
  Golem, Mummy and Dog (19 units). ResetView was also the parameter type of base-only code
  and of the non-chase classes' code (Bee, Crow, ...); that code takes ChaseActor for now
  and is to be narrowed to RoomActor.
* `SceneActorAnimationView` folded into RoomActor (restart and nextDirection came from it).
* Skeleton is `struct Skeleton { struct ChaseActor chase; ... }`. Its two private views are
  folded in. skeleton.c's functions take a struct Skeleton.
* `tools/retype_fields.py` does the rewriting: it compiles each unit and rewrites the
  member accesses agbcc rejects ("no member named"). It handles accesses inside macros,
  member names passed to offsetof-style macros, lines where only a trial compile tells
  which access is meant, and subclass pointers passed for the base (`--upcast`).
  `tools/retype/room_actor.txt` holds every step so far; replayed on the pre-item-5
  sources, it reproduces this tree.
* Compiler facts found along the way:
  * Skeleton_Init keeps two pointer variables for the one object. Passing the Skeleton
    pointer where the original passed the chase pointer changes the register allocation,
    so Skeleton_SetMovementControlPreset keeps a ChaseActor parameter.
  * A struct that held ResetView by value had to grow its padding by 0xC8: TentacleActor,
    AxeActor, and two Skeleton views. The byte check caught each one.
* Still to fold: BehaviorView, PlacementView, TerrainCollisionView, RegistryActor; the
  Golem view in sub_080CCD7C.c; the Dog unions in sub_080E25FC.c and sub_080E5744.c; and
  Mummy's raw `(u8 *)arg0 + 0x6F4` offsets.
