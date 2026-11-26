# The registry actor base: merged layout (2026-11-15)

Tags: #object-model #structs #actors #registry #enemies

Evidence for item 5 of `docs/OBJECT_MODEL.md`: one type for the object every entry of the
actor registry allocates. The tables come from `tools/struct_views.py`, which probes each
struct with agbcc's layout and lists members by offset:

    python3 tools/struct_views.py SceneActorRegistryResetView SceneActorRegistryActor \
        SceneActorAnimationView SceneActorBehaviorView SceneActorPlacementView \
        SceneActorTerrainCollisionView Skeleton --to 0x3D4 --only-disagreements --markdown

## Whole-object views and component views

A whole-object view starts with `void *object; void *drawable;` and covers the actor from
offset 0. Fifteen structs do:

| struct | size | header |
|---|---|---|
| SceneActorRegistryResetView | 0x6EC | scene_actor_state.h (2,717 uses in 578 files) |
| SceneActorRegistryActor | 0x5F8 | scene_actor_registry.h |
| SceneActorAnimationView | 0x3D4 | scene_actor_animation.h |
| SceneActorBehaviorView | 0x3D0 | scene_actor_behavior.h |
| SceneActorPlacementView | 0x45C | scene_actor_placement.h |
| SceneActorTerrainCollisionView | 0x2E8 | scene_actor_terrain.h |
| Skeleton, Ghoul, Mimic | 0x74C, 0x678, 0x664 | their own headers |
| Bee, Crow, Ghost, Slime, TentacleBody | 0x490, 0x43C, 0x62C, 0x40C, 0x934 | their own headers |

The eight monster structs are copies of ResetView's member list (same names, same types)
up to 0x3D4. The other views named in item 5 are components, not whole-object views:

| struct | size | what it is |
|---|---|---|
| SceneActorRegistryControl | 0x1C | a control record (bitfield parameter/radius, timer, mode, target position) |
| SceneActorRegistryRoute | 0x14 | one route entry (route point pointer, current point, branches) |
| SceneActorRegistryNode | 0x8 | a list node: next, actor |
| SceneActorRegistryMessage | 0x8 | a message: receiver id, data pointer |
| SceneActorRegistryTileDrawable | 0x6C | the object `drawable` points at (flags, position, palette) |
| SceneActorRegistrySequenceDrawable | 0x34 | a sequence drawable (flags, position, animation, palette bank) |
| SceneActorAnimationTileDrawable | 0x2C | another view of `drawable` (the tile at +0x20); 85 of its casts are `(...)actor->drawable` |
| SceneActorRegistryControlGroup | 0x74 | the chase group at 0x3D4 (its `field43C` is named by absolute offset) |
| SceneActorRegistryCallbackBlock | 0x74 | the same group, only its fallback at +0x70 |

These stay separate types. The drawable views become one drawable type and the two group
views fold into `struct SceneActorChaseGroup`, but neither is part of the actor base.

SceneActorEffectsContext (4 bytes) is not a view of the actor.

## The base ends at 0x3D4

* AnimationView is exactly 0x3D4 bytes and BehaviorView 0x3D0. No view agrees with any
  other past 0x3D4.
* The last base member, at 0x3D0, is `context`. Every init sets it to the actor itself:
  `SceneActorRegistry_SetObjectContext(actor, actor, 1, 1)`. AnimationView calls it `link`.
* From 0x3D4 the classes diverge into two families.

## Families

With the chase group (`struct SceneActorChaseGroup`, 0x250 bytes at 0x3D4..0x624; the init
calls `MonsterInit_InitializeGroup*`): Skeleton, Mimic, Ghoul, Spider (sub_080A4AFC), Vorn
(sub_080B43BC), Golem (sub_080CFC14), Mummy (sub_080DCC98), Dog (sub_080E5744), Octopus
(sub_080EC3F8), Ivy (sub_080F0C20), Axe (sub_080FE68C).

Without it (class fields start right at 0x3D4): Slime, Crow, Bee, Ghost, Lun (sub_0809DD78),
Tentacle (sub_080ABDCC), Bat (sub_08084F44).

ResetView is base + chase group + 0x624..0x6EC. Its fields past 0x624 are not shared ones.
They are the union of what a few chase families keep there: nearly every reader is a Vorn
unit (sub_080B0C80..sub_080B5E54), plus Golem (field682, field688, savedDirection690), Mummy
(field6E8) and Dog (animationFlags65C). These fields belong in those families' structs.

## Disagreements in 0..0x3D4

Every disagreement is a name, a signedness or a pointer type. No view places a member at a
different offset or with a different width, except where noted. Pad members
(`unknownXX`) are not listed. The monster structs agree with ResetView everywhere in the
base, so only Skeleton is shown.

| off | ResetView | RegistryActor | AnimationView | BehaviorView | PlacementView | TerrainCollisionView | Skeleton | disagreement |
|---|---|---|---|---|---|---|---|---|
| 0x059 | u8 body.direction |  | u8 direction | u8 heading | u8 direction |  | u8 body.direction | name |
| 0x09f | u8 field9F |  |  | u8 defense9F |  |  | u8 field9F | name |
| 0x0a0 | u8 fieldA0 |  |  | u8 reactionA0 |  |  | u8 fieldA0 | name |
| 0x0dc | struct SceneActorChaseTarget * tracking.target | struct SceneActorPositionSource * distanceTarget |  | void * gate.object |  |  | struct SceneActorChaseTarget * tracking.target | name |
| 0x0e9 | u8 tracking.heading |  | u8 facingAngle |  |  |  | u8 tracking.heading | name |
| 0x0f0 | u32 tracking.distance |  | u32 phaseTimer |  |  |  | u32 tracking.distance | name |
| 0x104 | u32 word104 | u32 field104 |  | const void * savedControl104 | u32 field104 |  | u32 word104 | name |
| 0x108 | u32 word108 |  |  | void * allocation108 |  |  | u32 word108 | name |
| 0x10c | u32 word10C |  |  | void * allocationArgument10C |  |  | u32 word10C | name |
| 0x110 | u32 word110 |  |  | u32 savedKind110 |  |  | u32 word110 | name |
| 0x178 | u16 colliderA.responseValue | u16 field178 |  | u16 primaryCollider.responseValue | u16 field178 | u16 collider.responseValue | u16 colliderA.responseValue | name |
| 0x1f0 | u32 field1F0 |  |  | u32 script1F0 |  |  | u32 field1F0 | name |
| 0x21c | void * target | const void * control |  | const void * control | const void * control |  | void * target | name |
| 0x220 | void * field220 |  |  |  | const void * resetControl220 |  | void * field220 | name |
| 0x22c | void * savedTarget | const void * defaultControl |  |  |  |  | void * savedTarget | name |
| 0x230 | void * reactionTarget230 |  |  |  | const void * savedControl230 |  | void * reactionTarget230 | name |
| 0x23c | void * field23C |  |  |  | void * callback23C |  | void * field23C | name |
| 0x258 | void * updateCallback | s32 (*)(void *) update |  |  |  |  | void * updateCallback | name |
| 0x25c | void * destroyCallback | s32 (*)(void *) destroy |  |  |  |  | void * destroyCallback | name |
| 0x264 | void * damageCallback | void * activeCallback264 |  |  |  |  | void * damageCallback | name |
| 0x274 | void * contextCallback | void * contextCallback274 |  |  |  |  | void * contextCallback | name |
| 0x2a8 | u8 state2A8 | u8 state2A8 | u8 phase |  | u8 state2A8 |  | u8 state2A8 | name |
| 0x2a9 | u8 phase2A9 | u8 phase2A9 | u8 subphase |  | u8 phase2A9 |  | u8 phase2A9 | name |
| 0x2aa | u8 behavior2AA | u8 behavior2AA | u8 nextPhase |  | u8 behavior2AA |  | u8 behavior2AA | name |
| 0x2ac | u32 counter2AC | u32 counter2AC | u32 phaseCounter |  | u32 counter2AC |  | u32 counter2AC | name |
| 0x2b0 | u8 field2B0 | u8 field2B0 |  | u8 substate | u8 field2B0 |  | u8 field2B0 | name |
| 0x2b1 | u8 field2B1 | u8 field2B1 | u8 phaseReady | u8 resetRequested | u8 field2B1 |  | u8 field2B1 | name |
| 0x2b2 | u8 field2B2 | u8 field2B2 | u8 directionalEntryFlag | u8 prepared | u8 field2B2 |  | u8 field2B2 | name |
| 0x2b3 | u8 field2B3 | u8 field2B3 | u8 restart |  | u8 restart |  | u8 field2B3 | name |
| 0x2b5 | u8 field2B5 |  |  | u8 kind | u8 field2B5 |  | u8 field2B5 | name |
| 0x2b6 | u8 field2B6 |  |  | u8 reactionDirection | u8 field2B6 |  | u8 field2B6 | name |
| 0x2b7 | u8 animationIndex |  | u8 selection |  | u8 selection |  | u8 animationIndex | name |
| 0x2be | u16 groundHeight2BE |  |  | u16 groundHeight |  |  | u16 groundHeight2BE | name |
| 0x2c4 | u32 profileMode | s32 livingCountByCategory | u32 profileMode |  | u32 profileMode |  | u32 profileMode | signedness, name |
| 0x2d8 | u32 field2D8 |  |  | s32 parameter2D8 | u32 flags2D8 |  | u32 field2D8 | signedness, name |
| 0x2dc | u32 field2DC |  |  | u32 reaction2DC | u32 flags2DC |  | u32 field2DC | name |
| 0x2e0 | u32 field2E0 |  |  | u32 payload2E0 |  |  | u32 field2E0 | name |
| 0x2e4 | u16 reactionSoundVariant |  |  | u16 result2E4 | u16 field2E4 | u16 field2E4 | u16 reactionSoundVariant | name |
| 0x2e6 | u16 field2E6 |  |  | u16 mode2E6 | u16 flags2E6 | u16 field2E6 | u16 field2E6 | name |
| 0x304 | u32 field304 |  |  | s32 counter304 |  |  | u32 field304 | signedness, name |
| 0x308 | u32 profileId | s32 profileIndex | u32 profileId |  |  |  | u32 profileId | signedness, name |
| 0x310 | s16 reactionStart.x |  |  | u16 biasX310 | s16 destination.x |  | s16 reactionStart.x | signedness, name |
| 0x314 | s16 reactionStart.z |  |  | u16 biasZ314 | s16 destination.z |  | s16 reactionStart.z | signedness, name |
| 0x320 | u8 [0xAC] shadow |  |  |  | u8 [0xB4] shadow320 |  | u8 [0xAC] shadow | name |
| 0x3cd |  |  | u8 nextDirection | u8 requestedDirection |  |  |  | name |
| 0x3d0 | void * context | void * context | struct SceneActorAnimationLink * link |  |  |  | void * context | name |

Two notes on that table. PlacementView's shadow at 0x320 is 0xB4 bytes against 0xAC
elsewhere; it runs into 0x3CD..0x3D0, which only the narrow views name. The 0xDC pointer is
the chase target in ResetView and a position source or gate object in the narrow views.
All three describe the same pointer: it names whatever the actor tracks.

## Design

* `struct RoomActor` (include/link/registry_1745/room_actor.h) is ResetView's 0..0x3D4. It keeps ResetView's
  names where the narrower views say nothing better, and takes the narrower names where
  their readers make the meaning clear (the rename map in the migration tool).
* agbcc accepts no anonymous struct members, so a class written as
  `struct Skeleton { struct RoomActor base; ... }` reads base fields as `actor->base.field`.
  Code that only touches the base takes a `struct RoomActor *` and keeps `actor->field`.
* The chase families add `struct SceneActorChaseGroup group;` at 0x3D4 after the base.
