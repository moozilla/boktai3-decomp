# The object model: how the game builds its things

Tags: #engine #structs #cleanup #actors

What kinds of object the game has, how each is laid out in memory, and where our source
still hides that. Read from the matched C on 2026-11-15; nothing here was checked by
running the game, and names marked *(proposed)* are not in the source yet. The task
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

Nineteen units build one of these, and they all follow the same recipe. `Skeleton_Create`
and `Skeleton_Init` in `src/skeleton.c` are a readable example.

1. `X_Create` allocates the block: `Heap_Alloc(0x74C)` for Skeleton. The size is the
   subclass (Slime 0x40C, Crow and Bat 0x43C, Lun 0x44C, Bee 0x490, Ghost 0x62C,
   Octopus 0x63C, Ivy 0x640, Spider 0x65C, Mimic 0x664, Golem 0x6B0, Dog 0x6E0,
   Vorn and Mummy 0x720, Skeleton 0x74C, Tentacle 0x934, Axe 0xF24; read from each
   `X_Create(void)`). Ghoul links into the registry too, through a creator of a
   different shape whose size is not listed here.
2. `X_Init` fills the **base**: body and position, drawable, two colliders, palette,
   health, saved control state, script hooks.
3. It then installs the **virtual methods**, function pointers stored in the base:
   update, destroy, on-script-message, standing, hidden, reaction, defeat, default
   reaction, contact predicate, damage, contact, plus tables indexed by behavior and
   phase. `MonsterInit_InstallSequenceCallbacks` (`include/scene_actor_init_dispatch.h`)
   writes most of them in one call.
4. The monster's own fields live after the base (`struct Skeleton` has `actionFlags`,
   `routeProbe`, `destination`, `healthThreshold`).
5. `SceneActorRegistry_LinkActor` adds the block to the manager's list; from then on the
   registry task calls the installed pointers every frame.

A script parameter (keyword `k`) selects a **variant** inside one class: Skeleton branches
on category 7, 8, 0x11 and 0x17.

Nineteen source files call `SceneActorRegistry_LinkActor`. Whether props such as the
wooden crate and floor switch are members has not been checked; Ivy is, which suggests the
base means "a thing in the room with a body that reacts to being hit" more than "enemy".

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
5. **Name the registry-actor base.** *(proposed)* Today it is eleven overlapping views:
   `SceneActorRegistryResetView` (used about 2,700 times), `SceneActorRegistryActor`
   (about 320), and nine narrower ones. An unrelated struct is already called
   `struct SceneActor` (`include/scene_actor_lifecycle.h`; the one in `scene_indicator.h`
   was a `gPlayers` view and is gone), so that name is taken. Proposal: `struct RoomActor` for the base, with each subclass
   embedding it first:

   ```c
   struct Skeleton {
       struct RoomActor base;
       u32 actionFlags;
       ...
   };
   ```

   and the callback slots typed and named as methods (`update`, `destroy`, `onMessage`,
   `onStanding`, `onHidden`, `onReaction`, `onDefeat`, ...). Merge the views field by
   field (`tools/layoutmerge.py` does this for globals with several struct types), one
   field at a time, keeping every
   dependent unit byte-identical. This is a rename across more than 500 files; do it as
   a scripted change that can be re-run, not by hand.
6. **Studio.** Once 1 to 5 land, the actor browser can show the three families and, for
   registry actors, the subclass size, variant list and installed methods.

## Open questions

* Whether Hresvelgr, Ratatosk and Vanargand share a boss base beyond `struct Task` and
  the event receiver, or are fully separate like Nidhoggr appears to be.
* What the task header's byte at +0x16 is (ENGINE.md guesses a scope or priority).
* Whether the 174 task-creating commands fall into a few sub-families by list index
  (8 = scene objects, 11 = menus and fades, 9 = backgrounds and triggers) cleanly enough
  to label in Studio.
