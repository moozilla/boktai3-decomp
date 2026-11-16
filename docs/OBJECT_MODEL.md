# The object model: how the game builds its things

Tags: #engine #structs #cleanup #actors

What kinds of object the game has, how each is laid out in memory, and where our source
still hides that. Read from the matched C on 2026-11-15 and 2026-11-16; nothing here was
checked by running the game. The task
scheduler itself is in [ENGINE.md](ENGINE.md) section 3.

## The short version

The game is C, but it is object-oriented by hand. There are three families of object, and
they differ in who owns the memory and who calls the update function.

| Family | Memory | Updated by | Examples |
|---|---|---|---|
| **Tasks** | one `Task_Create(list, size)` block, starting with the 0x18-byte `struct Task` | the scheduler, every frame, through `task.update` | the player, every boss, every manager, menus, backgrounds |
| **Registry actors** | one `Heap_Alloc(size)` block starting with a shared base; linked into a manager | the scene-actor registry (itself a task), through function pointers stored in the base | Skeleton, Mummy, Golem, Slime, Bee, Ghost, Crow, Dog, Spider, Mimic, Vorn, Lun, Axe, Ghoul, Bat, Octopus, Tentacle, Ivy |
| **Pool slots** | a fixed array inside a task's block, each slot with an `active` byte | the owning task's update, looping over slots | projectiles, particles, afterimages, breakables, spark effects |

So a "monster" is not a task. A boss is. A bullet is neither: it is a slot in an array
that some task owns.

## Family 1: tasks

`struct Task` (`include/task.h`, included by `engine.h`) is the base class of everything the scheduler runs:
`prev`, `next`, `update`, `destroy`, `id`, `flags`, `list`. A subclass is any struct whose
first 0x18 bytes are that header; its size is the `size` argument to `Task_Create`, and its
"class" is the pair of callbacks given to `Task_SetCallbacks`.

* **The player** is a task. `struct Player` (`include/player.h`) begins with
  `struct Task task;` and is created by `Player_Create` (`Task_Create(6, 0xCAC)`).
* **Each boss** is its own task class with a very large body. `struct Nidhoggr`
  (`include/large_scene_animation_controller.h`) is `struct Task`, a `SceneEventReceiver`,
  then more than 0x66D8 bytes holding 26 parts with their own visuals and colliders. No
  Hresvelgr, Ratatosk or Vanargand unit touches the registry-actor base.
* **Managers** are tasks that own pools or lists (scene objects, triggers, the registry
  itself). ENGINE.md section 4 lists them.

## Family 2: registry actors

The room's enemies. Eighteen units link one into the scene-actor registry with
`SceneActorRegistry_LinkActor`, and they all follow one recipe; `Skeleton_Create` and
`Skeleton_Init` in `src/skeleton.c` are a readable example.

### The class

```c
struct RoomActor { ... };                       /* include/room_actor.h, 0x3D4 bytes */

struct ChaseActor {                              /* include/chase_actor.h, 0x624 bytes */
    struct RoomActor base;
    struct SceneActorChaseGroup group;           /* route, target placement, pursuit control */
};

struct Skeleton { struct ChaseActor chase; ... };  /* a chase class */
struct Bee      { struct RoomActor base;   ... };  /* a class without the chase layer */
```

* **`struct RoomActor`** is the base: body and position, drawable, the two colliders
  (`colliderA`, `colliderB`), palette, health, the saved control, the methods, the state,
  phase and behavior bytes the shared code steps through, the shadow, and `context` (the
  pointer the methods are called with; `SceneActorRegistry_SetObjectContext(actor, actor, ...)`
  makes it the actor itself in every class read so far).
* **`struct ChaseActor`** adds the chase group: the home route, the target placement, the
  saved pursuit control and the route and turn callbacks the shared chase code drives
  (`include/scene_actor_group.h`). The shared monster code (`MonsterInit_*`,
  `SceneActorReaction_*`, `SceneActorMovement_*`, ...) takes a `struct ChaseActor *`.
* **Classes.** Each class embeds its parent first, so a class pointer is also a parent
  pointer and code passes `&skeleton->chase` where the shared code wants one. agbcc has
  no anonymous members, so a class writes `skeleton->chase.base.health`.

| Class | Parent | Header | Size (`X_Create`) |
|---|---|---|---|
| Skeleton | ChaseActor | `skeleton.h` | 0x74C |
| Vorn | ChaseActor | `vorn.h` | 0x720 |
| Mummy | ChaseActor | `mummy.h` | 0x720 |
| Dog | ChaseActor | `dog.h` | 0x6E0 |
| Golem | ChaseActor | `golem.h` | 0x6B0 |
| Mimic | ChaseActor | `mimic.h` | 0x664 |
| Ghoul | ChaseActor | `ghoul.h` | (other creator) |
| Spider, Octopus, Ivy, Axe | ChaseActor | `spider.h`, `octopus.h`, `ivy.h`, `axe.h` | 0x65C, 0x63C, 0x640, 0xF24 |
| Ghost | RoomActor | `ghost.h` | 0x62C |
| Bee | RoomActor | `bee.h` | 0x490 |
| Crow | RoomActor | `crow.h` | 0x43C |
| Slime | RoomActor | `slime.h` | 0x40C |
| Tentacle | RoomActor | `tentacle_body.h` (its segments start at 0x3D4; the init unit's `TentacleActor` still views it as a ChaseActor) | 0x934 |
| Lun, Bat | RoomActor | `lun.h`, `bat.h` | 0x44C, 0x43C |

Vorn and Mummy share their layout past the chase group (flags at 0x6D4, the route probe
at 0x6F4); shared reaction units read both through one view.

### The methods

`X_Init` fills the base and then installs the methods, function pointers stored in the
base. `MonsterInit_InstallSequenceCallbacks` (`include/scene_actor_init_dispatch.h`)
writes most of them in one call. Named from the code that calls them:

| Member | Called by | Example (Vorn) |
|---|---|---|
| `update` | `SceneActorRegistry_UpdateActors`, every frame | `Vorn_OnRender` (it runs `SceneActor_UpdateBehaviorCallbacks`) |
| `destroy` | `SceneActorRegistry_UpdateActors` / `_DestroyActors`, before the block is freed | `Vorn_OnDestroy` |
| `onMessage` | `SceneActorRegistry_SendToCategory` and the other senders | `SceneActorScript_OnCommand` |
| `onUpdate` | `SceneActor_UpdateActiveCallbacks`, with `context` | `Vorn_OnUpdate` |
| `onDamage` | `SceneActor_UpdateActiveCallbacks`, before `onUpdate` | `SceneActorReaction_ResolvePlacementDamage` |
| `onContact` | the shared contact tests | `SceneActorPlacement_ResetStateThree` |
| `targetQuery` | the shared pursuit code | `SceneActorTarget_ResolvePendingObject` |
| `onStanding`, `onHidden`, `onEntry`, `onAlternateEntry`, `onReaction`, `onDefeat` | the state handlers `MonsterInit_SetControl` switches to | `Vorn_BeginIdle`, `Vorn_UpdateHiddenRecovery`, ... |
| `defaultReaction`, `contactQuery`, `animationQuery` | the shared reaction and animation code | |
| `behaviorCallbacks`, `alternateBehaviorCallbacks`, `phaseCallbacks` | tables indexed by `behavior2AA` and `phase2A9` | |

The slots are typed with the typedefs in `include/scene_actor_components.h`: `update` and
`destroy` are `SceneActorResultCallback` (`s32 (*)(void *actor)`), the six state handlers
`SceneActorAction`, and `onMessage` `SceneActorCommandHandler` (the actor and the script
command, as `SceneActorScript_OnCommand` takes them). A class whose handler takes its own
pointer type (`u32 Bee_Destroy(struct ChaseActor *)`) installs it with a cast to the slot's
type, since the registry calls every class the same way. Still `void *`: `onUpdate`,
`onDamage`, `onContact` and `targetQuery` (typing those four changes which constant
`Axe_Init` keeps on the stack; its installer passes them in as `void *`), and
`defaultReaction`, `routeReset`, `sinkCallback`, `reactionGate`, `field240` and the behavior and phase tables.

### Not members

The props are not registry actors. The wooden crate is one of twelve slots in a
`WoodenCrateManager` (`WoodenCrate_AllocateSlot`), and the floor switch is one of eight
slots on the scene-object list (`FloorSwitch_AllocateSlot`, `SceneBody_GetRegistry`). Both
are family 3. Of the eighteen classes that link into the registry, none is a prop.

## Family 3: pool slots

A pool is `struct Slot slots[N]` inside a task, with a linear scan for a free slot
(`Breakable_FindFreeSlot`, `SpritePool_*`). Vanargand owns three such pool tasks:
`VanargandProjectilePool`, `VanargandAfterimagePool` and `VanargandAttackPair`, each made
with `Task_Create(9, ...)`. Slots have no header in common across pools.

## What scripts can create

`data/actor_registry.csv` is named as if it listed actors. It is the table of every native
command a GCL script can call, 722 entries keyed by hash. Reading each entry's function:

* 174 create a task directly (`Task_Create`), mostly on lists 8, 11 and 9;
* about 137 only read script arguments and set state;
* the rest are commands on existing systems: `GameState_*`, `Sound_*`, `Inventory_*`,
  `Camera_*`, `MessageWindow_*`, `SaveData_*`, `Weather_*`.

Its `name` column follows `symbols/functions.csv` (regenerated 2026-11-15; 24 callbacks are
still `sub_XXXXXXXX`), and `tools/promote_names.py` refreshes it after each naming batch.

## Where the source hides this (cleanup list)

Each item is a phase-4 cleanup and changes no bytes. Do them in this order; the first
three are small and independent, the last is large and should wait until the phase-3
workers are idle, because it touches most gameplay files.

1. *(done 2026-11-15)* **`data/actor_registry.csv` names.** Regenerate the `name` column from
   `symbols/functions.csv` by callback address, and say in the file's consumers and docs
   that it is the script command table. Keep the file name unless every consumer
   (`tools/`, Studio, `gcl/`) is updated in the same change.
2. *(done 2026-11-15, 94 of 116)* **Task headers spelled as byte arrays.** 116 structs
   began with an anonymous 0x18-byte array (`unk0`, `unknown0`, `pad0`, or `u8 task[0x18]`).
   94 were checked against their creator and now begin with `struct Task task;`. Two are
   not task headers (`PasswordEntry` is a heap block; `ScriptKeyframeEntry` is an element of
   the array inside a task) and 20 have no creator that ties the block to their type yet;
   `notes/object-cleanup_2026-11-15.md` lists them.
3. *(declarations done 2026-11-15)* **`gPlayers` had nineteen declarations**: eight in
   headers (`struct Player *[4]`, the views `AreaEffectPlayer`, `CellGatePlayer` (as a plain
   pointer), `InteractionMarkerPlayer`, `OrbitBackgroundPlayer`, `LevelAdvancePlayer`,
   `ChasePlayer`, and `struct SceneActor *`) and eleven in units (`u8 *[]` four times,
   `void *` once, and six `struct Player *[4]` whose units each defined their own small
   `struct Player`). It is one array of four `struct Player *`, declared once in
   `player.h`; every unit matches with that form. Still open: about 200 sites read it
   through a cast (`(*(u8 **)gPlayers)[0x418]`, `(struct SceneActorChasePlayer *)gPlayers[0]`
   and 21 other view types).
4. *(done 2026-11-15)* **Duplicate boss structs.** `struct Nidhoggr` was defined in its
   header and again in `src/fn/sub_08191508.c`, and `src/fn/sub_08190C98.c` had a third
   view (`struct BossParts`). One definition, in the header.
5. *(done 2026-11-16)* **The registry-actor base.** It was eleven overlapping views
   (`SceneActorRegistryResetView`, about 2,700 uses; `SceneActorRegistryActor`, about 320;
   the animation, behavior, placement, terrain-collision and gate views; private copies in
   units). Now it is `struct RoomActor` and `struct ChaseActor` with the classes embedding
   them (family 2 above), and the method slots are named (most are typed). Every step was
   byte-identical for every dependent unit. Two scripts redo it on another branch:
   `tools/retype_fields.py --steps tools/retype/room_actor.txt` rewrites the accesses that
   no longer compile, and `tools/fold_views.py HEADER CLASS UNIT:VIEW` folds a unit's private
   view into its class. Still open:
   * Every class has a header now, but some views remain. Axe has six: AxeColliderBlock
     declares the colliders as arrays of two, and the trail views need types that only their
     units define. Ivy has two, and Bat and Crow share two in `scene_actor_stored_motion.c`.
   * Views that do not fold as they are: `VornRetreatContext` and `MummyCheckpointContext`
     (they read a flags word through a bitfield), `GolemSlideContext` (it reads 0x680
     unsigned, `GolemBody` signed), `GolemCombatResetContext` (byte arrays over two
     positions), `DogApproachContext` (reads inside the bite collider), and nine Skeleton
     views. Five of those are the parameter types of functions that `skeleton.h` declares
     with `struct ChaseActor`; three read 0x6E0..0x6EC as words.
   * Code that takes a `struct ChaseActor *` but only reads the base, and the non-chase
     classes' code (Bee, Crow, ...), should take `struct RoomActor *`.
   * About 300 casts to `struct ChaseActor *` and 21 to `struct RoomActor *` remain.
6. **Studio.** Once 1 to 5 land, the actor browser can show the three families and, for
   registry actors, the subclass size, variant list and installed methods.

## Open questions

* Whether Hresvelgr, Ratatosk and Vanargand share a boss base beyond `struct Task` and
  the event receiver, or are fully separate like Nidhoggr appears to be.
* What the task header's byte at +0x16 is (ENGINE.md guesses a scope or priority).
* Whether the 174 task-creating commands fall into a few sub-families by list index
  (8 = scene objects, 11 = menus and fades, 9 = backgrounds and triggers) cleanly enough
  to label in Studio.
