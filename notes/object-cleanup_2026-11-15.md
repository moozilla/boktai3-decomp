# Object-model cleanup, items 1 to 5 (2026-11-15/16)

Tags: #cleanup #structs #engine #actors #tooling #object-model

Phase-4 cleanup from the list at the end of `docs/OBJECT_MODEL.md`, items 1 to 5. Every change is byte-identical: each unit that
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

* `struct Task` moved from `engine.h` to a new `include/engine/shared/task.h` (engine.h includes it), so
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

* `src/effects/large_scene_animation_controller/sub_08191508.c` (Nidhoggr_HidePartWithExplosion) uses the header's struct.
  `NidhoggrCollider` gains `flags` at +6 (bit 2 disables the collider) and
  `NidhoggrPartExtra` gains `flags` at +4 (bit 0 hides the part). The local `parts` were
  `rows` (flags at row+8), `partPhases` is `rowInitFlag` and `partCompleted` is
  `rowDoneFlag`.
* Also folded in: `struct BossParts` in `src/effects/large_scene_animation_controller/sub_08190C98.c`
  (Nidhoggr_ChangeCompletedPartState). Its `state[26]` at 0x71C2 is the header's new
  `partState` (it keys the part animation callback table in Nidhoggr_UpdatePartSequences).
* The unit's three index-scaled address computations stay as they were; each one
  written as a field access changes the code. They are not constant raw offsets, so they
  are not concessions.

## Doc errors found (fixed in docs/OBJECT_MODEL.md)

* `struct Task` is in `include/engine/shared/task.h` now (was engine.h).
* Item 2 counts: 116 candidates, 94 verified, not "about 80".
* Item 3: nineteen declarations (six of them with private `struct Player` definitions),
  not twelve with nine types; plus ~200 cast sites the doc did not count.
* Item 4: a third Nidhoggr view (`struct BossParts`) existed.
* Item 5: the `struct SceneActor` in scene_indicator.h was a gPlayers view (removed).

## Item 5: the registry actor base

Evidence: `notes/room-actor-layout_2026-11-15.md` (from the new `tools/struct_views.py`).
The result is described in `docs/OBJECT_MODEL.md` family 2; this is the log.

* `struct RoomActor` (include/link/registry_1745/room_actor.h, 0..0x3D4) is the base. `struct ChaseActor`
  (include/engine/shared/chase_actor.h) is RoomActor plus `struct SceneActorChaseGroup`, 0x624 bytes: the
  type the shared chase code takes. Chase classes embed ChaseActor (Skeleton, Ghoul, Mimic,
  Vorn, Golem, Mummy, Dog); the others embed RoomActor (Bee, Crow, Ghost, Slime, Tentacle).
* The views are gone: ResetView (it was ChaseActor plus family fields, which moved to new
  struct Vorn, Golem, Mummy and Dog), RegistryActor, and the animation, behavior (with the
  gate view), placement and terrain-collision views.
* `tools/retype_fields.py` does the rewriting: it compiles each unit and rewrites the
  member accesses agbcc rejects ("no member named"). It handles accesses inside macros,
  member names passed to offsetof-style macros, lines where only a trial compile tells
  which access is meant, subclass pointers passed for the base (`--upcast`) and casts to
  the base types (`--decast`). `tools/retype/room_actor.txt` holds every step; replayed on
  the pre-item-5 sources, it reproduces this tree.
* `tools/fold_views.py` (new, tested) folds a unit's private view of a class into the
  class: a member the class has is renamed, or the class takes the reader's name when its
  own only records the offset (field6AC -> pursuitCooldown, every other user follows); a
  member inside padding is added; past the end, appended; a signedness difference is tried
  and kept only if every user still matches. Results: Vorn 13 of 14 views, Mummy 10 of 11,
  Golem 5 of 7, Dog 6 of 8 plus the two attack copies by hand (a Dog's 0x624 is its bite
  collider, a `struct ContactCollider`), Skeleton 4 of 13.
* One `struct ChaseRouteProbe` replaced Skeleton's and Mimic's identical probes; Vorn, Golem,
  Mummy and Dog hold theirs as fields, and the init functions lost their byte offsets.
* Methods: the slots are named from their callers (update, destroy, onMessage, onUpdate,
  onDamage, onContact, targetQuery; the state handlers onStanding, onHidden, onEntry,
  onAlternateEntry, onReaction, onDefeat). update, destroy, onMessage and the state handlers
  are typed; class handlers with their own pointer type are installed with a cast. The real
  `SceneActorScript_OnCommand` takes (actor, command); its eighteen `(void)` prototypes were
  wrong. Typing adds no compiler warning (274 lines before and after, over 791 units).
* Props: the wooden crate and the floor switch are pool slots of their own managers, not
  registry actors. None of the eighteen registry classes is a prop.
* Compiler facts found along the way:
  * Skeleton_Init keeps two pointer variables for the one object. Passing the Skeleton
    pointer where the original passed the chase pointer changes the register allocation,
    so Skeleton_SetMovementControlPreset keeps a ChaseActor parameter.
  * A struct that held ResetView by value had to grow its padding by 0xC8: TentacleActor,
    AxeActor, and two Skeleton views. The byte check caught each one.
  * `counter304` must be s32: as u32 a `> 0` test becomes `!= 0`.
  * Typing onUpdate, onDamage, onContact and targetQuery as function pointers changes which
    function-address constant Axe_Init spills (its installer takes them as `void *`), so
    those four stay `void *`.
* Still open: see item 5 in `docs/OBJECT_MODEL.md`.

## Names and units (2026-11-16)

### A. Names for the base

* 33 RoomActor members named from their readers and writers. The steps are in
  `tools/retype/room_actor_names.txt`. All 792 dependents match. The evidence:
  * `defense` (9F): subtracted from a hit's power.
  * `contactReward` (A0): passed to `PlayerStats_AddContactHundredths`.
  * `experience` (AC): keyword 'X'; awarded on defeat.
  * `attachmentRestart` / `attachmentTimer` (A8/AA): keyword 'l'; the restart value
    is copied into the timer.
  * `hudKind` / `hudLabel` (B2/9A): keywords 'K' and 'V', the HUD's type and parameter
    (low confidence).
  * `pendingControl*` (104..114): what `MonsterInit_SetMotionControl` queues and
    `SceneActorBehavior_ActivatePendingControl` installs: the control, its data, the
    release callback, the kind, and a pending bit.
  * `flashTimer`, `paletteTimer`, `particleTimer`: the base's existing offset checks
    and readers.
  * `scriptId` (1EC): keyword 'R'.
  * `sinkCallback` and `reactionGate` (238/23C), `routeReset` (248): set by the
    MonsterInit setters of the same names.
  * `stateCallbacks` (27C): indexed by state2A8.
  * `stepDone` (2B0) and `controlKind` (2B5): written together with `target` when a
    control is installed.
  * `hitLockTimer` (2B8): set to 0x2D on contact.
  * `placementFlags` (2C0): keyword 'M'.
  * `frameCounter` (2CA): incremented every frame and taken modulo intervals.
  * `behaviorFlags`, `statusFlags`, `motionFlags`, `reactionFlags` (2CC..2D6): the
    names of the shared helpers' getters and setters.
  * `hitAttributes`, `hitReactionFlags`, `hitOwners` (2D8..2E0): what
    `SceneActorReaction_HandleColliderContact` copies from the hit.
  * `heightOffset` (2EA): the height above groundHeight (Bee's flight, and the saved
    control).
* Left offset-named, each with a note:
  * field9E, fieldA1, fieldA2, fieldA4: only the keyword reads.
  * field2C2: readers call it speed, height or scale.
  * field2C8, field240, callbacks280..2A0, field2E6, field2E8, field3CC.
  * The SceneActorChaseGroup offset names are not done.
* Flags: four bits have one meaning in every class: status 0x4000 (alternate
  palette), 0x20000 (inactive), 0x1000000 (interacting), and motion 0x1000 (holds a
  contact node). They are enums in room_actor.h, used in the shared status and
  availability helpers. The other bits are passed as raw masks through many helpers,
  and no single meaning has held for them yet.
* `category` stays a number. MonsterInit sets it from an argument that each creator
  passes in (the categories SceneActorRegistry_SendToCategory addresses). Naming it
  means mapping every class's value first.

### B. Monster classes as units

* `tools/merge_resolve.py` settles merge_unit's conflicts while keeping every input
  byte-identical:
  * A helper copy gives way to the first file's copy when the file still matches.
    Otherwise it is renamed after its function.
  * Prototypes, externs and variables now count as conflicts, and the resolver
    settles them: the header's signature first, then the definition's, then each
    variant. A candidate is kept only if every file that declares it still matches
    and the merged unit compiles (`--out`).
* Hand fixes (merge_resolve reports these as UNSETTLED):
  * A definition whose return type disagreed with its callers'
    (SceneActorPlacement_BeginContactRoute now returns u8).
  * A definition taking a private view (Octopus_GetAnimationStatus now takes the
    ChaseActor).
  * Prototypes naming a struct that is defined later in the unit (they get a
    forward declaration).
* Units so far (each whole unit MATCH):
  * `src/monsters/spider/spider.c`: 45 functions, 15172 bytes.
  * `src/monsters/octopus/octopus.c`: 46 functions, 14980 bytes.
  * `src/monsters/lun/lun.c`: 50 functions, 16116 bytes.
  * `src/monsters/ivy/ivy.c`: 61 functions, 21464 bytes.
  * `src/monsters/ivy/ivy_child.c`: 11 functions, 2424 bytes.
  * `src/monsters/bat/bat.c`: Bat_Init and Bat_Create.
  * `src/monsters/bat/bat_restore.c` and `src/bat_defeat.c`.
  * `src/monsters/axe/axe_contact.c`: 7 functions, 2708 bytes.
  * `src/monsters/axe/axe.c`: 96 functions, 29148 bytes.
* More hand fixes, each keeping every file byte-identical:
  * Function-name aliases (`#define OldName Axe_Name`) hid conflicting prototypes.
    They give way to the real name.
  * Local copies of a header's struct or inline (RotatingDrawable, NormalizeAngle)
    give way to the header.
  * Two headers declared one function two ways. Both are unified, and every
    dependent still matches:
    * `SceneActorTerrain_RequestRetreatRoute` takes a ChaseActor.
    * `Collider_SetAttackProperties` takes u32 arguments.
* More units (each whole unit MATCH):
  * Dog: `src/monsters/dog/dog.c` (10356 bytes), `src/dog_scripted.c` (5028), `src/dog_approach.c` (6940).
  * Mummy: `src/monsters/mummy/mummy.c` (2900), `src/monsters/mummy/mummy_entry.c` (1788), `src/mummy_defeat.c` (7936),
    `src/mummy_pursuit.c` (1584), `src/mummy_path.c` (15236), `src/mummy_init.c` (4952).
  * `src/monsters/skeleton/skeleton_rise.c` (5508).
  * Vorn: `src/monsters/vorn/vorn.c` (8472), `src/monsters/vorn/vorn_main.c` (13216), `src/vorn_main_2.c` (11460).
  * Golem: `src/monsters/golem/golem_main.c` (20620), `src/monsters/golem/golem_main_2.c` (10964), `src/golem_main_3.c` (4248).
  * Skeleton: `src/monsters/skeleton/skeleton_main.c` (71 functions, 31956 bytes).
  * `src/dog_reaction.c` (3152).
* Header prototypes that disagreed with the definitions now state the definitions'
  own signatures. Every dependent matches:
  * `skeleton.h`: 28 prototypes, most of them `void`/`void *` where the function
    takes the ChaseActor.
  * `scene_actor_chase.h`: Skeleton_UpdateScriptWaitState.
  * `scene_actor_chase_step.h`: SceneActorChase_CellIndex is now defined whatever
    SCENE_ACTOR_CHASE_SCALAR_CELLS says. That is one branch less of a selector
    macro; the rest of the selector set (`scene_actor_chase_reaction.h`) still
    needs taking apart.
* Declarations brought into line with their definitions, each file still matching:
  * A definition that ignores trailing arguments its callers pass takes them as
    unused parameters (Vorn_SelectRecoveryAction, Vorn_OnDestroy, Golem_OnDestroy).
  * A definition returning a flag returns u8 like its callers expect
    (SceneActor_IsFacingNearbyTarget, SceneActor_DispatchContextAction).
  * Local `Heap_Alloc`/`Memory_Clear`/`Script_Call` prototypes give way to engine.h.
  * Golem files that copied `golem_charge.h` now include it; the copies are dropped
    by a scratch script that removes items identical to the header's.
* Compiler fact: some functions match alone but not inside a unit. They are left in
  `src/fn` for a matcher:
  * Bat 08087018 and 08087A70.
  * Dog 080E059C, 080E25FC and 080E4B34.
  * Mummy 080D2194, 080D2584, 080D5778, 080D6608, 080DA97C and 080DC228.
  * Vorn 080ACBE0 and 080B2580. 080ACCD4, a lone file between them, also stays.
  * Golem 080CF3E4 (it also still uses the scalar-cells selector) and Dog 080E0750.
  * Correction (2026-11-16): this is not agbcc state. merge_unit hoists the includes
    above a file's chase selector `#define`s (or an earlier file has already included
    the header), so the selector no longer applies. 080E059C was misattributed. See
    `notes/unit-context-sensitivity_2026-11-16.md`.
  * Units split at these functions, so a class can take two or three units.
