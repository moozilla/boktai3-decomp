# How the Boktai 3 engine works

A reader's guide for someone who knows C and wants to mod, port or re-implement
the game. It describes the engine as the matching decompilation shows it. Names
are provisional (see `include/engine.h`), and every claim that is not directly
visible in code is marked *(guess)*, with the evidence pointed at.

Companion documents:

- [GAME_LAYOUT.md](GAME_LAYOUT.md): the whole game from ROM to gameplay, subsystem by subsystem.
- [ROM_MAP.md](ROM_MAP.md): what lives at which ROM address.
- [FINDINGS.md](FINDINGS.md): first mapping pass (text, sound, graphics, hardware).
- [ENTRY_AND_MENUS.md](ENTRY_AND_MENUS.md), [BACKGROUND_CONTROLLERS.md](BACKGROUND_CONTROLLERS.md), [ANIMATION_081E.md](ANIMATION_081E.md), [MOTION_0818.md](MOTION_0818.md), [MENU_CONTEXT.md](MENU_CONTEXT.md): subsystem notes.
- [SCRIPT_TRACING.md](SCRIPT_TRACING.md), [MGS_GCL_MATCHING.md](MGS_GCL_MATCHING.md), [PROCESS_REVIEW.md](PROCESS_REVIEW.md): the script system and its comparison with MGS GCL.
- [SOLAR_SENSOR.md](SOLAR_SENSOR.md), [RTC.md](RTC.md), [EEPROM.md](EEPROM.md), [RFU.md](RFU.md), [LIBGCC.md](LIBGCC.md), [LIBM.md](LIBM.md): hardware and SDK pieces.

## Contents

1. [Mental model](#1-mental-model)
2. [Boot and the frame loop](#2-boot-and-the-frame-loop)
3. [The task system](#3-the-task-system)
4. [Object composition and lifecycles](#4-object-composition-and-lifecycles)
5. [The script system](#5-the-script-system)
6. [Memory: heap, resource registry, game state](#6-memory-heap-resource-registry-game-state)
7. [Global state: GameState and Player](#7-global-state-gamestate-and-player)
8. [Input, random numbers, tables](#8-input-random-numbers-tables)
9. [Terrain, colliders and actors](#9-terrain-colliders-and-actors)
10. [Graphics](#10-graphics)
11. [Text and dialogue](#11-text-and-dialogue)
12. [Sound](#12-sound)
13. [Clock, sunlight and the solar sensor](#13-clock-sunlight-and-the-solar-sensor)
14. [Save data (EEPROM)](#14-save-data-eeprom)
15. [Wireless (RFU)](#15-wireless-rfu)
16. [Recurring patterns](#16-recurring-patterns)
17. [Where to start, and what is unsettled](#17-where-to-start-and-what-is-unsettled)

---

## 1. Mental model

* The game is a single **cooperative scheduler** over linked-list **tasks**. There
  are no OS threads. `AgbMain` initializes hardware and subsystems, then loops
  forever calling `TaskSystem_RunFrame`.
* Every game object is a **task**: a C struct whose first 0x18 bytes are a common
  header (update and destroy callbacks, id, flags, priority list). Objects are
  allocated from a **heap** in EWRAM, zeroed, linked into one of **14 priority
  lists**, and destroyed one frame after they are killed.
* Objects are mostly created by **event scripts** (a bytecode VM whose instructions
  carry keyword-tagged parameters). A typical object has a `*_CreateFromScript`
  constructor, an `*_Initialize`, an update function that runs every frame, often
  a small state machine, and a destroy callback.
* Managers are **singletons** or **fixed pools** owned by one task. A pool slot is
  a struct with an `active` byte, and the manager's update loops over the slots.
* Coordinates are 16-bit integers (`struct SceneVec`, `include/scene_vec.h`). The
  units are not established in code *(guess: sub-tile fixed point)*; angles use
  1/256 of a turn (`Math_Atan2`, `include/engine.h`).

The codebase follows the same shape: `src/*.c` units hold matched C for
contiguous ROM ranges, `src/fn/sub_XXXXXXXX.c` hold one function each, and
`include/*.h` hold the structs that the units share. The header comments are
the most readable summary of each subsystem.

---

## 2. Boot and the frame loop

Entry point: `AgbMain` in [`src/fn/AgbMain.c`](../src/fn/AgbMain.c) (ROM
`0800145C`, [ENTRY_AND_MENUS.md](ENTRY_AND_MENUS.md)). The ARM crt0 at `080000C0`
jumps to it; six ARM C functions (agbcc ARM mode, `src/iwram_code.c`) run from IWRAM
at `03000000` (ROM `08000350`).

`AgbMain` in order:

1. Sets `WAITCNT` (`04000204`) and preserves two reserved EWRAM words across the
   `RegisterRamReset` call, then clears IWRAM from `03000000` by DMA.
2. `Rtc_Initialize`, `Clock_ClearState`, `Clock_CheckRtcDate` (see section 13).
3. `m4aSoundInit` (MP2K, section 12) and `InterruptsAndVideo_Initialize`.
4. Copies the IWRAM code block (`CpuSet`) and calls `sub_082485EC`.
5. `GameState_SelectRamBase`, which chooses where the game state lives (section 7).
6. `TaskSystem_Initialize`, then the subsystem initializers: `sub_08213DCC`
   (graphics setup, *guess*), `sub_0821BAFC` (script VM and the map resource), `sub_08215118`
   (starts a script proc and sets up the tile bank at `gUnk_030042E4`),
   `sub_082151C0` (animation resource header).
7. Starts two script procedures with `ScriptProc_Start(0xCEAA, 0x3002)` and
   `ScriptProc_Start(0x98F5, 0xFF54)` (section 5).
8. Enables the VBlank interrupt (`IE |= 4`) and sets `IME = 1`.
9. Calls `Time_CalculateSunriseSunset(0x23A666, 0x8BC000, 0)` *(guess: latitude and
   longitude as fixed-point)*, then `sub_0821B048` (script VM tables and loader) and
   `sub_08225328` (registry, command tables and callbacks).
10. `for (;;) TaskSystem_RunFrame();`

The loop does not visibly wait for VBlank. Where frame synchronization happens
is not established (open question 1 in section 17).

---

## 3. The task system

Source: `TaskSystem_Initialize` ([`src/fn/sub_08219AAC.c`](../src/fn/sub_08219AAC.c)),
`Task_Create` ([`sub_08219FBC.c`](../src/fn/sub_08219FBC.c)),
`Task_SetCallbacks` ([`sub_0821A04C.c`](../src/fn/sub_0821A04C.c)),
`Task_Kill` ([`sub_0821A0C0.c`](../src/fn/sub_0821A0C0.c)),
`TaskSystem_RunFrame` ([`sub_0821A054.c`](../src/fn/sub_0821A054.c)),
and the list primitives [`sub_08219F74.c`](../src/fn/sub_08219F74.c) (insert at
head) and [`sub_08219F94.c`](../src/fn/sub_08219F94.c) (unlink). Declared in
[`include/engine.h`](../include/engine.h).

### The 0x18-byte task header

Every object struct begins with this header (C declarations in
`include/engine.h` as `struct Task`; object headers embed it as `u8 task[0x18]`,
see `include/background_controller.h`, `include/palette_controller.h`):

| Offset | Size | Field | Meaning |
|---|---|---|---|
| `+0x00` | 4 | `prev` | previous task in the list (or list head pointer for the first) |
| `+0x04` | 4 | `next` | next task in the list |
| `+0x08` | 4 | `update` | `void (*)(struct Task *)`, called every frame while alive |
| `+0x0C` | 4 | `destroy` | `void (*)(struct Task *)`, called once after the kill |
| `+0x10` | 2 | `id` | unique, nonzero; from the counter `gUnk_03005240` |
| `+0x12` | 2 | `flags` | bit 0 = killed (set by `Task_Kill`) |
| `+0x14` | 1 | `list` | index 0..13 into `gUnk_03005280` |
| `+0x16` | 1 | (unnamed) | priority or scope *(guess)*: `sub_0821A0F8(prio)` kills every task with a smaller value; the root task stores 1 here |
| `+0x17` | 1 | | not named |

The header is 0x18 bytes, so the object's own fields start at `+0x18`.

### Lists

`gUnk_03005280[14]` (`struct TaskList { head; pauseMask; }`, eight bytes each).
The lists run in index order. A list whose `pauseMask` overlaps
`gUnk_0300523C` (the engine suspend flags) is skipped for that frame. Bits 1
and 2 of `gUnk_0300523C` also stop `Sound_PlaySong` from remembering songs, so
the flags mean at least "paused" and "sound off" *(guess)*.

Which priority means what is only partly known. Counting `Task_Create` call
sites in `src/`: list 8 (99; scene objects, damage controllers, emitters,
terrain modifiers), 11 (65; choice menu, group objects, colour fades), 10 (55;
collision props and pooled interactables), 9 (46; backgrounds, item triggers,
numeric overlays). The root task is list 0. Lower lists run first, so list 0 input and message work comes
before anything else.

### Lifecycle

* **Create**: `Task_Create(list, size)` allocates `size` bytes from the heap
  (`Heap_Alloc`, section 6), zeroes them, sets `list`, assigns the next id
  (the counter starts at 1), and inserts the task at the **head** of its list.
  It returns NULL when the heap is full. Constructors check for NULL, then call
  `Task_SetCallbacks(p, update, destroy)`.
* **Run**: `TaskSystem_RunFrame` walks each enabled list. It saves `next` before
  calling `update`, so an update may kill its own task. Because new tasks are
  inserted at the head, a task created during a frame is first updated on the
  next frame *(derived from the code: the walk is already past the head)*.
* **Kill**: `Task_Kill(p)` sets bit 0 of `flags` and returns 0. It does nothing
  else, so the object keeps running until the next walk.
* **Destroy**: on the next walk the scheduler calls `destroy`, unlinks the task
  (`sub_08219F94`), and frees the block (`Heap_Free`). The destroy callback
  therefore still sees valid memory and may kill or free child objects.
* **Lookup**: `sub_0821A130(id)` finds a task by id across all lists.
* **Scope kill**: `sub_0821A0F8(prio)` kills tasks whose `+0x16` byte is below
  `prio` *(guess: called when a scene or map is left; callers not yet traced)*.

### The root task

`TaskSystem_Initialize` creates a task at the static address `gUnk_03001668`,
on list 0, with update `sub_08219A94` and no destroy. That function runs
once per frame and does the work that every other system relies on:
`sub_0810BB0C` (not yet traced), `sub_080369BC` (runs
`sub_080367B4` when `gUnk_02000484` is set), `sub_08219B24` (button polling)
and `sub_0821A308` (flips the message double buffer while not paused, section 5).
It also sets the random seed (`gUnk_03005278 = 123456`) and the task-id counter.

---

## 4. Object composition and lifecycles

### Embedding

Objects embed their parts as sub-structs at fixed offsets. The sizes are the
constructor's `size` argument:

| Object | Header | Size | Embeds | Source |
|---|---|---|---|---|
| Scene object (block) | `struct SceneObject`, `SceneObjectManager` pool | `0x74` manager, `0x1A8` per object | `SceneBody` (position, velocity), `SceneAnimation`, `SceneDraw`, two `SceneCollider`s, `SceneCellRef` | [`include/scene_object.h`](../include/scene_object.h), [`src/scene_object.c`](../src/scene_object.c) |
| Pooled interactable | `PooledInteractableManager` | `0x80` manager, `0xEC` per slot | `PooledRoute`, `PooledDrawable`, collider | [`include/pooled_interactable.h`](../include/pooled_interactable.h) |
| Collision prop | `CollisionManager` (`Task_Create(10, 0xA2C)`) | 16 slots | `CollisionSprite`, `CollisionAnim`, `SceneCollider` | [`src/collision_controller.c`](../src/collision_controller.c) |
| Homing or orbiting effect | `EffectManager` | six particle effects of four sprites | `EffectSprite`, packed vectors | [`src/homing_effect.c`](../src/homing_effect.c) |
| Trigger (drop or container) | `TriggerManager` (`Task_Create(9, 0x5C)`) | linked list of `0xBC`-byte records | `TriggerAnim`, `TriggerGraphics`, shadow | [`include/item_container.h`](../include/item_container.h) |
| Background / scrolling background | `BackgroundController` (`Task_Create(9, 0x140)`, scrolling `0xF8`) | command queue of 8 entries | `BackgroundCommandQueue` | [`include/background_controller.h`](../include/background_controller.h) |
| Palette controller | `PaletteController` | `PaletteWave`, `PaletteSlide`, `PaletteTransition` (256-colour output) | | [`include/palette_controller.h`](../include/palette_controller.h) |
| Color fade | `ColorFadeController` | two `ColorFadeChannel`s, 8-entry command queue | | [`include/color_fade_controller.h`](../include/color_fade_controller.h) |

Each object is a single allocation, so destroying the task frees everything it
owns, but only if the destroy callback frees or kills its children.

### Managers

A **singleton** manager is stored in a global pointer and created on first
use. `sub_08005778` (scene objects, global `gUnk_02000024`) and
`sub_08008270` (triggers, `gUnk_02000030`) return the existing task or create it
and call `Task_Kill` on failure. The text window list uses the same idea at
`gUnk_020000E4` (section 11).

A **pool** is an array of slots with an `active` (or `state`) byte. Allocation
is a linear scan for a free slot, for example `sub_08008A70` in
`pooled_interactable.c`. The manager's update loops over all slots. The
16-slot sprite pool (`SpritePool_*`, `0x08019A14`) and the six-slot homing
effect pool follow the same pattern.

An **intrusive list** links objects by pointer. The pooled actor manager
(`0802C678`) keeps its actors in such a list and caches a party target.

### The typical lifecycle

Taking the trigger manager and scene objects as examples:

1. **Script creates it.** The event bytecode calls a native command, for example
   `SceneObjects_CreateFromScript` ([`src/scene_object.c`](../src/scene_object.c),
   around line 1536). It reads its parameters with keyword lookups:

   ```c
   id    = Script_GetKeywordValue('n', 0);   /* object id */
   type  = Script_GetKeywordValue('t', 0);   /* kind, selects a default resource */
   flags = Script_GetKeywordValue('f', 0);
   if (Script_SeekToKeyword('p')) { position.x = Script_GetValue(); ... }
   ```

   Keyword letters are the one-character tags of the script operands (section 5).
2. **Constructor.** `Task_Create` → `Task_SetCallbacks(p, update, destroy)` →
   `*_Initialize`, which fills the record and loads its resource. A failing
   initialize calls `Task_Kill` and returns 0.
3. **Update.** Each frame, the update function reads the object's state and runs
   the matching handler. Handlers return when the state ends or call a helper that
   advances an animation.
4. **Interactions.** Colliders and other objects reach the object through
   callbacks stored in the collider (`Collider_SetCallback`), and through
   script commands that look the object up by its id (`sub_0821A130` for any
   task, `sub_080057FC` for scene objects).
5. **Script callbacks.** When something notable happens, the object runs an
   event script by calling `Script_Call(p->script, &args)` (scene objects,
   damage controllers, choice menus, pooled interactables).
6. **Destroy.** The object is killed when its work is done, or the script
   removes it. The destroy callback releases child tasks and resources.

### State machines

State machines are a `state` (or `kind`) field plus a `counter` or `timer`, and
an `entered` flag. The first frame of a new state sets `entered`, and the state
handler clears it on its first run. Examples: `sub_08008A98` and `sub_08008AA4`
in [`src/pooled_interactable.c`](../src/pooled_interactable.c), and the
comment on `struct SceneObject.entered` in `include/scene_object.h`.

---

## 5. The script system

The event script system is a keyword-based bytecode VM. The matching C is
mostly in `src/fn/sub_0821A4xx`–`sub_0821BAxx`, and the bytecode is in the ROM
at `08DD6B96`–`08E8755B` ([ROM_MAP.md](ROM_MAP.md)). Its structure and command names come from
MGS's GCL (see Command names below), though the encoding differs. See [MGS_GCL_MATCHING.md](MGS_GCL_MATCHING.md) (the matched
functions and the comparison) and [PROCESS_REVIEW.md](PROCESS_REVIEW.md) (what
the comparison does and does not show).

### The VM state

`gUnk_02000610` holds the interpreter state (`pc` and a stack of block bases).
Its declared type differs between units, which is a known problem (section 17).
`gUnk_02000438` holds the **procedure table**: a pointer to `u32` entries, each
giving a block offset (low 24 bits, relative to a base pointer) and a flag in its
top byte.
`Script_ReadProcTable` (`0821ACB4`) reads it. `sub_0821ACDC(id, &flag)` looks up
procedure `id` (1-based).

### Blocks and dispatch

`Script_Call(id, args)` (`0821AD08`) looks up the procedure, skips its block
header with `sub_0821A66C`, and passes the payload to `sub_0821AFD8` with the
argument pointer (which presumably reaches the block dispatcher). The dispatcher
`sub_0821AF2C` reads one instruction at a time, using the high nibble of the
first byte:

| High nibble | Meaning | Handler |
|---|---|---|
| `0x0` | end of block | returns |
| `0x3` | expression, value stored in `gUnk_02000610[1]` | `sub_0821BA5C` (expression stack) |
| `0x6` | **native command**: 16-bit little-endian id, then arguments | `sub_0821AC6C` looks up a native handler |
| `0x7` | **procedure call**: inline procedure id and arguments | `sub_0821AD2C` collects them |
| other | not block-level instructions; keywords appear inside argument lists (see below) | the dispatcher's `switch` has no case for them |

The payload length comes from the low nibble. Values 0 to 12 are literal lengths.
13, 14 and 15 mean that a one-, two- or three-byte length follows
(`sub_0821A66C`). Callers advance by the decoded length.

Native commands are registered as linked tables of `{id, function}` entries
(`sub_0821ABE8` registers, `sub_0821AC30` searches). The engine commands are
the functions that these tables point at. Examples by name are
`SceneObjects_CreateFromScript`, `Trigger_CreateDrop` and
`ChoiceMenu_CreateFromScript` *(guess: the table entries themselves are not yet
listed)*. Their names carry the `_FromScript` suffix
when they parse keywords.

### Keywords

Parameters are keyword-tagged operands. A keyword operand has the tag nibble
`0x5` in bits 4–7 of the decoded operand, and its value shifted right 16 bits is
an ASCII letter (for example `'n'`, `'p'`, `'M'`, `'a'`), as `Script_SeekToKeyword`
checks it. The value follows it as one operand. The keyword helpers, in
[`src/fn/sub_0821A96C.c`](../src/fn/sub_0821A96C.c) and neighbours:

| Function | Address | What it does |
|---|---|---|
| `Script_SeekToKeyword(k)` | `0821A96C` | scans the current argument block from its start for keyword `k`; on success sets `pc` after it and returns the position |
| `Script_GetValue()` | `0821AB40` | evaluates the operand at `pc` and advances |
| `Script_GetPc()` | `0821AB18` | current position |
| `Script_GetKeywordValue(k, default)` | `0821ABA8` | seek, then read the value after the keyword; `default` if absent |
| `Script_ParseStringRef()` | `0821AABC` | reads a 16-bit string id (for `Text_LookupString`) |
| `Script_PushCtrlNextKeyword` / `Pop` | `0821A940`, `0821A950` | save and restore the keyword search state for nested reads |

Because `Script_SeekToKeyword` scans from the start every time, a constructor
that reads twenty keywords scans the block twenty times. Constructors read their
keywords in the order that the script writes them, and a missing keyword simply
returns the default. This makes the script format forgiving to extend.

### Operands

`Script_DecodeOperand` (`0821A6F0`, `src/fn/Script_DecodeOperand.c`) decodes
one operand and returns its type and value. The first byte selects the type:

* `0xC0`–`0xFF`: an immediate value `(op & 0x3F) - 1`, range -1 to 62.
* Scalars of several widths (signed and unsigned 8-bit and 16-bit little-endian;
  the widths are in the MGS comparison notes).
* String references go through `Text_LookupString` (`0821ADD4`), which takes a
  string id, looks up `0x08D6B1F8` (a table of `u32` offsets, bit 31 marks text),
  and adds the base `0x08D74DDC`. Section 11 has the details.
* **Variables** use a 4-byte descriptor (`sub_0821B2B0`). The type is in bits
  24–27. Bits 20–23 choose the base: `0x8xxxxx` is the game state
  (`gUnk_02000710`), `0x1xxxxx` is the region at `gUnk_02000700`, and anything
  else uses `gUnk_02000708`. The low 16 bits are the offset from that base. When
  the first byte's high nibble is `2`, an index operand follows. The
  matched accessors are `sub_0821B34C`, `sub_0821B4C8`, `sub_0821B540` and
  `sub_0821B5BC`. Types (`sub_0821B34C`): 1 and 6 are 16-bit, 2 and 3 a byte,
  4 a single bit (bits 16–19 give its number, the index adds to it), 8 is
  24-bit and 9 a word. `gcx.py` prints them as `region[offset.bit]:type`
  (`r708[0x6e.2]:t4`). At the `start` bookmark the regions are `gs` =
  `0x0203C400`, `r708` = `0x0203DA00` (the script work area: story progress
  `r708[0x3c]`, the arrival entrance `r708[0x70]`, most scene flags) and
  `r700` = `0x0203E200` (scratch).

### Command names

Native command ids are 16-bit hashes of the command names, as in MGS's GCL.
The hashes match MGS's names exactly, which shows that the script system
derives from GCL. Recovered names of commands and registry keys are in
`gcl/names/` (see [gcl/README.md](../gcl/README.md)):

| Table | Entries (hash = name) |
|---|---|
| `0861418C` basic | `0D86` if, `4A6F` switch, `64C0` eval, `121F` call, `CD3A` return, `B96E` print |
| `08E8791C` engine | `22FF` mesg (`082255BC`), `9906` chara (`08225560`), `D4CB` trap (`08225624`), `C8BB` load (`08225508`), `C091` map (`08225620`), `E43C` restart (`082257A0`), `B745` command (`08225590`: calls the registry routine of its key and keeps the result, like MGS4's `command`), `0BB3` unknown |
| `080F38C8` | three entries `0CF4`, `0CC8`, `0CD8` (two-letter hashes `dt`, `ch`, `cx`; low confidence) |

**Scenes.** The `load` command (`08225508`) changes scene: it stores the scene
number in `0x030039F0` (`sub_0821B03C`) and in `GameState+0x5A4` (the scene
saved with the game), and sets the request word `0x030053E8` to 1, or `0x11`
unless keyword `n` is given as 0. A scene number is a script procedure: the
late-game test save stands in scene 3243, whose entry runs `proc_861 1;
proc_864 0`, then the stage setup (`proc_884 0xcf70 0xf8ea 0x7239`) and the
area's objects. `tools/mgba/scenes.py` lists 496 scene entries (456 with a
stage); an entry is `proc_861 FLAGS; proc_864 MODE` with any flags (the
prologue uses 56, boss and story scenes 9, 25, 33, 41). Cutscenes inside a
scene are tables played by SceneMessages: [CUTSCENES](CUTSCENES.md). Writing the three words from Lua loads the scene's stage but does not
spawn the player; ordinary doors pass an entrance as well: the door's event
sets the entrance number in `r708[0x70]` (`proc_953`) and the destination's
entrance procedures (`proc_909`, `proc_916`, ...) test it to place the player
(ASSETS.md, "Stage scripts"; `tools/assets/stages.py`). Note that
`tools/assets/gcx.py` prints procedure headers one lower than the numbers
calls use.

The 722-entry table at `08603300` (`data/actor_registry.csv`) has the same
layout. It is the `chara` table: every object a script can create by name,
including controllers as well as actors. Its names are not recoverable by
guessing: about 9,400 candidate spellings of character, enemy and object names
matched 99 keys, which is the 1% chance rate. A few common words do match
plausible entries (`delay` 082284B4, `varsave` 08225410, `end` 0822548C,
`select` 080422DC, `radio` 08197890) but are unconfirmed.

### Resource keys (`ScriptProc_Start` is a resource lookup)

`ScriptProc_Start(type, name)` (`0821A520`) is misnamed: it finds a
**resource**, not a script procedure. Both arguments are 16-bit string hashes
(`sub_08219BD8`: rotate-left-5, add the byte, to NUL; MGS's `GV_StrCode`
scheme). `type` is the hash of a file extension (`0x922E` = `anm`, `0xC091` =
`map`, `0xA635` = `fnt`, `0xCEEF` = `ptn`, `0xA8D9` = `gcx`); a `switch` maps it
to a key of the archive directory at `08614D6C`, and `name` is searched in that
archive. The result is the resource's data (sprite packages are then rebased by
`ResourceHeader_Relocate`). This is the role of MGS's `GV_GetCache`. Archives
and formats are in [ASSETS.md](ASSETS.md). The engine's startup loads the
single-entry resources `0xCEAA`, `0x98F5` and `0x9B1B` (`sub_08215118`).
A rename (for example `Resource_Find`) is pending, to avoid churn in units that
other sessions are editing.

The `gcx` resource (`08D5A85C`) starts with `0x42A05217`, which read as a Unix
time is 2005-06-03, a plausible build date (MGS GCX files also begin with a
timestamp), followed by offsets that are not yet decoded.

### Messages

A double-buffered message queue (`0821A2DC`–`0821A454`, 0x188-byte buffers)
carries engine messages between systems. `sub_0821A340` posts a message and
`sub_0821A3E8` reads consecutive messages. The root task flips the buffer each
frame while the game is not paused (`sub_0821A308`). The function name
`GV_SendMessage` is the MGS counterpart in the comparison notes, and the
mapping is a role comparison only.

### Tracing a script

The emulator harness can record every native call and actor lookup during a
replay: [SCRIPT_TRACING.md](SCRIPT_TRACING.md) lists the probes (`native_enter`
`0821AC6C`, `native_call` `0821AC9E`, `actor_key` `0822556A`). This is the
practical way to find out what a given script command does at runtime.

---

## 6. Memory: heap, resource registry, game state

### The heap

Source: `sub_08219C20` (init), `sub_08219C40` (`Heap_Alloc`),
`sub_08219CB4` (second allocator), `sub_08219D38` (`Heap_Free`), in
[`src/fn/`](../src/fn/). Declared in `include/engine.h`.

* The heap is the region from `gUnk_02000714` to `0x0201F400`, about 123 KiB of
  EWRAM. Init makes one free block covering it.
* Every block has a 16-byte header: `prev`, `next`, `size` (low 20 bits include
  the header; bit 31 = free), and an unused word. Requests are rounded up to a
  multiple of 16, plus the header.
* `Heap_Alloc(size)` is a **first fit** from the front, and splits the block
  when the remainder is larger than a header. `Heap_Free` marks the block free
  and coalesces it with free neighbours.
* `sub_08219CB4` walks the list from the back and splits off the top of the free
  block. `sub_0821A004` (a second task constructor) uses it. Why there are two
  allocators is not established *(guess: keeping long-lived allocations apart)*
  (open question 3).
* `Memory_Clear`, `Memory_Copy` (`sub_08219DD8`, `sub_08219EBC`) and
  `BgGfx_LoadTiles` (`sub_08219F04`, DMA3 in 512-byte chunks) are the bulk
  helpers.

The heap is the only general allocator. Fixed-size objects use pools, and
resources use the registry below.

### The resource registry

Source: `src/fn/sub_0821A184.c` to `sub_0821A2C4.c`. A fixed array of up
to 32 eight-byte entries at `gUnk_0203B000`:

| Field | Size | Meaning |
|---|---|---|
| `id` | u16 | resource id |
| `flags` | u8 | `0x80` = in use; `0x02` = `data` is heap memory that cleanup must free; `0x01` = kept by a cleanup that does not set bit 0 of its `keep` argument (`sub_0821A218` keeps up to the last such entry) |
| `data` | pointer | the resource |

Operations: `sub_0821A284(id, data, flags)` inserts (`sub_0821A184` finds a free
entry), `sub_0821A1C8(id)` finds an entry by id, `sub_0821A218(keep)` frees the
entries that are not kept, and `gUnk_030052F0` is the high-water count. The
script VM registers its map resource this way (`sub_0821BAFC` registers id
`0x56C2`, 0xE24 bytes, *(guess: current map or collision resource)*).

### Game-state storage

`GameState_SelectRamBase` ([`src/fn/sub_0821B084.c`](../src/fn/sub_0821B084.c)) picks the base of the game-state block at boot:
`base = gUnk_0203C400 + (TM3CNT_L & 127) * 8`. The block sits at a small,
boot-dependent offset, so no code may assume a fixed address for it, and pointers
must be read from the globals below. The derived regions are:

| Global | Offset from base | Use |
|---|---|---|
| `gUnk_02000710` | 0x0000 | `struct GameState`, see section 7 |
| `gUnk_0200070C` | 0x0B00 | region (not yet named) |
| `gUnk_02000708` | 0x1600 | default variable base for other descriptor bases (section 5) |
| `gUnk_02000704` | 0x1A00 | region |
| `gUnk_02000700` | 0x1E00 | variable base for descriptor base `0x1` |

*(guess: the timer jitter moves the block within EWRAM on each boot.)*

---

## 7. Global state: GameState and Player

### `struct GameState`

[`include/game_state.h`](../include/game_state.h). Pointer `gUnk_02000710`.
The header is built from offsets seen across `src/` and marks each
field with a confidence. Confirmed or strongly suggested areas:

* `+0x28` `life`, `+0x2A` `lifeMax` (`s16`, *guess*), used by `sub_0813E208` as a ratio.
* `+0x30` position (`positionX/Y/Z`, *guess*). `Memory_Clear(+0x30, 0x74A)`
  resets the save-dependent part, so a new game clears from `+0x30` to `+0x77A`.
* `+0x4C` `unlockMask` (`u32`, *guess*, bit per index).
* `+0x52` `selected`, `+0x58` `party[4]`, `+0x68` `inventory[16]`
  (`-1` = empty), `+0x88` `slots88[12]`, `+0xA0` and `+0x100` 48-entry tables,
  `+0x160` 432-entry table (`-1` = empty), `+0x4C0` 16-entry table indexing
  `gUnk_08E60F48`.
* `+0x4F0` `date`, `+0x4F4` `hour`, `+0x4F8` `minute`, `+0x4FC` `second`: the RTC time at the last check
  *(guess; section 13)*.
* `+0x538`, `+0x562`, `+0x566`, `+0x56A`, `+0x56C`, `+0x574`, `+0x576`,
  `+0x57C`: counters capped at 9,999 *(guess)*.
* `+0x596` `flags596` (bits 0–9), `+0x5A0` `region` (*guess*: current
  region or scene id, compared by the interactable controllers).
* `+0x720`–`+0x730` event flag words (`flags720` … `flags730`, bits 0–31 in
  each). These are the story and event flags the scripts test.
* `+0x868` `flags868` (bit 1: a feature enabled), `+0x86C` (also read as `u32`).

Only fields with a known access are named, so the array sizes are the next
known offset and not proven. Use the header as the authoritative list.

### `struct Player`

[`include/player.h`](../include/player.h). `gUnk_02000580[4]` holds four
pointers to player-like objects. Slot 0 is the local player. The other slots
are used by the multiplayer code *(guess)*. Known fields: `statusFlags`
(`+0x20`, bit `0x100000` = cannot interact, *guess*), `position` (`+0x30`,
`struct SceneVec`), `facing` (`+0x3A4`), `actionMode` (`+0x418`; 6 = alternate
effect mode, *guess*), `actionFlags` (`+0x448`, bit 4 = items always visible,
*guess*), `sceneMode` (`+0x534`).

Several units cast `gUnk_02000580` to their own partial views of the player
(`EffectActor`, `CollisionActor`, `TriggerActor`). Retyping shared globals this
way is the main remaining risk for the matched C (section 17).

---

## 8. Input, random numbers, tables

### Input

`sub_08219B24` polls the buttons each frame from the root task. The results
are in `gUnk_03005260`, a `u16` array (`[1]` = newly pressed keys, per the
comments in `include/result_prompt.h` and `include/status_screen.h`). Menus and
dialogue read it through their own `*_HandleInput` functions. The
`ChoiceKeys` struct in `include/choice_menu.h` is another view of it.

### Random numbers

Two sources:

* A linear congruential generator in `gUnk_03005278`:
  `x = x * 0x5D588B65 + 1`, returning `x & 0x7FFF` (`sub_08219C00`). The seed is 123456.
* A table of 1024 `u16` values at `gUnk_0203B400` (EWRAM), walked with the index
  `gUnk_03005308` (`& 1023`). Scene code takes one sample per step and advances
  the index as it goes. The table is written by `sub_082210B8`. Its values come from
  the generator *(guess)*.

### Sine and angle tables

* `gUnk_086149C4`: a sine table in ROM (`s16`), used by the scrolling background's orbit mode
  (`include/background_controller.h`) *(guess: full cycle indexed by an angle)*.
* `Math_Atan2(x, y)` returns an angle in 1/256 of a turn from a table
  (0 = +x, 0x40 = +y; `(0,0)` returns 0).

---

## 9. Terrain, colliders and actors

### The terrain map

The current map's collision grid is `gUnk_030052F4` (an `EngineMap`, with
`gUnk_030052F8`/`gUnk_030052FC` as width and height in cells). Each cell in
`struct SceneCell` is four bytes: `height` (low nibble = level 0–15; high nibble =
kind, 0 = solid floor), `tile`, and `flags` (bit 0 = occupied by an object,
bit 3 = blocks entry; `include/scene_object.h`).

Scene objects move one cell at a time. `Scene_CanEnterNeighborCell` and
`Scene_CheckNeighborHeight` in `src/scene_object.c` test the neighbour;
`Scene_TryStepDown` and `Scene_DropObjectsAbove` handle falls; terrain
modifiers (`TerrainModifiers_Change`, `src/terrain_modifier.c`) rewrite cells at
runtime. The overlays (`src/terrain_overlay.c`) draw terrain-edge sprites for
tracked actors.

### Colliders

A `SceneCollider` (in `include/scene_object.h`) carries a state word (bit 2 =
collision disabled) and contact flags describing the colliding thing, such as
the attack kind. The engine-wide API is in `include/engine.h`:

* `Collider_Init(collider, id, flags, ...)`, `Collider_SetPosition`,
  `Collider_SetCallback(collider, callback, owner)`, `Collider_Refresh`,
  `Collider_Remove` (unlink from the active list).

Callbacks are the hook: a pooled interactable's collider callback
(`sub_08008ABC`) moves its slot to a new state when it is hit by a matching
attack kind.

### Actors

Actors are the characters and enemies. Their shared code is in the actor
families listed in [GAME_LAYOUT.md](GAME_LAYOUT.md) (`0802C678` to `0802D9F8`,
`080213EC` to `08024F2C`, `081E34D0`). Actors use `Actor_PlayAnimationEntry`
(`include/engine.h`) to start an animation entry by kind, and
`ActorShadow_Enable`/`ActorShadow_Disable` (`08013424`, `08013440`) to add or
remove a shadow that follows the terrain (the shadow's update is installed at
`+0x34`). `Actor_CanInteractWithController` (`080096A0`) and
`DirectionalController_CheckInput` (`080097E0`) gate interactions by distance,
facing and input.

---

## 10. Graphics

The engine uses the GBA's tiled backgrounds, OBJ sprites, and palette hardware.
Most logic is in the controllers listed in section 4.

* **Backgrounds.** `Bg_ClearTilemap(bg)` zeroes a 0x800-byte tilemap.
  `BgGfx_LoadTiles(src, dst, bytes)` copies tile data by DMA3 in 512-byte chunks.
  `BgGfx_LoadTilemap` (`08216584`) and `BgGfx_LoadTilemapSimple` load tilemaps.
  Background controllers (section 4) load maps by id (`Background_FindMapId`,
  `08001EB4`), set layer modes, scroll with the camera and blend layers
  (`include/background_controller.h`; [BACKGROUND_CONTROLLERS.md](BACKGROUND_CONTROLLERS.md)).
* **Palettes.** `Palette_*` (`08002AA8`–`080036F8`) blend colour ranges, scale
  brightness, oscillate, run transitions, replace resources and upload to
  palette RAM (`include/palette_controller.h`). `PaletteSet_*` (`0800E0CC`)
  allocate and blend palette sets. The fade work buffer is at `02036FDC` (ROM_MAP).
  `ColorFade_*` (`080038B4`, `08003960`) run two colour-blend channels that
  interpolate to a target over `1 << shift` frames.
* **Palette level and cycling effects.** `PaletteLevels_*` (`080164B8`), palette
  cycling (`08018438`) and frame cycling (`08019518`).
* **Sprites.** `Sprite_Init(sprite, animData, flags)` (`082144A4`) sets up an
  idle sprite, and `Sprite_Remove` unlinks it from the pool at `gUnk_030042C0`
  *(guess)*. The sprite pool of 16 slots (`08019A14`) is a separate manager.
  `ObjGfx_LoadTiles` (`08215400`) loads OBJ tiles.
* **Animation.** `Anim_SetSequence`, `Anim_SetKeyframes`, `Anim_Step` (advance
  one tick, nonzero when finished) and `Anim_LoadKeyframe` play frames from an
  animation resource (`include/engine.h`). Animation resources live in the
  registry-like header at `gUnk_03004300`, with sub-table bases at
  `gUnk_030042F8`/`gUnk_030042FC` (`sub_082151C0`). `AnimData_FindEntry(id)` and
  `AnimData_Load(anim, id)` find an entry by id. `AnimData_SelectPaletteBank(anim, index)`
  selects one 32-byte palette bank at `gUnk_030042E4`. The display-track and hit-switch
  updaters read its sixteen RGB15 colors and blend each 5-bit channel independently.
* **Command queues.** The background and colour-fade controllers accept up to
  eight queued commands, filled by script commands and drained by the frame update.
* **Scanline effects.** Per-scanline (HBlank) tables drive wave distortion,
  circular windows and the status-screen wipes (`0800D874`, `08017270`,
  `08018F38`). The solar sensor uses the timer interrupt at a different slot
  ([SOLAR_SENSOR.md](SOLAR_SENSOR.md)).
* **Hardware blend.** `BlendTransition_Update` (`08018D94`) animates the alpha
  blend coefficients.
* **Tile animation.** `TileAnimation_*` (`080195DC`) runs eight animated tile
  channels with timing and DMA upload.
* **Graphics data.** Tiles are mostly uncompressed 4bpp; some background data
  is LZ77 (BIOS `LZ77UnCompWram`). The ROM regions are in [ROM_MAP.md](ROM_MAP.md).
  `tools/vram_map.py` maps tiles on screen back to their ROM source.

---

## 11. Text and dialogue

* **String table.** `Text_LookupString(id)` (`0821ADD4`, high confidence in
  `symbols/functions.csv`) finds a script string: a table of 9,977 `u32`
  offsets at `0x08D6B1F8`, bit 31 marks text, the base is `0x08D74DDC`, and the
  mask is `0x7FFFFFFF`. Some entries are binary records, not text.
* **Markup.** Strings contain inline ASCII tags parsed at runtime: `<PROC=n>`,
  `<LABEL=NAME>`…`</LABEL>`, `<END>`, `<WEIGHT>`, `<NAME>`, `<ALTER>` (choices),
  `<EXTEND=n>`, `<VAR=n>`, `<LOCK=n>`, `<MOJISE=…>` (text sound), `<BIKE>`
  ([FINDINGS.md](FINDINGS.md), section *Text*).
* **Glyphs.** `Text_DrawGlyph` (`08218D1C`) draws 8×16 tiles for ASCII and
  16×16 tiles for two-byte codes (lead bytes `0x80`–`0x85`, index = code − `0x8000`).
  Glyph width is fixed. `Text_FindChar` (`0803223C`) searches a string.
* **Text windows.** A `TextWindow` is an independent window with its own
  interpreter embedded at offset `+0x60` (`TextState_Reset`, `TextState_SetBounds`,
  `TextWindow_Find(id)`, `TextWindow_SetScript`, `TextWindow_Refresh(id, index)`,
  `TextWindow_BlankRect`). Windows are kept in a singleton list at `gUnk_020000E4`.
* **Dialogue.** The dialogue stack is layered: `Dialogue controls and animation`
  (`080308BC`; choice parsing, window rectangles, palette, scroll and timing),
  `Dialogue text interpreter` (`08031E54`; inline commands, speaker labels,
  number formatting, nested styles), `Scrolling dialogue controller` (`08032EDC`;
  reveal timing, row clearing, show/hide) and `Scripted dialogue transitions`
  (`08033C58`). Message boxes are `MessageMenu_*` (`080111EC`).
* **Choices.** `ChoiceMenu_*` (`08006374`–`08006F00`) handles input, markup
  layout and dispatch of the selected choice into a script
  (`Script_Call(script, &args)`, `src/choice_menu.c`).
* **Password.** Text filtering and decoding for the password screen is in
  `src/password.c` (`include/password.h`).

---

## 12. Sound

The sound driver is **MP2K** (Sappy), matched against pret's `m4a`:
`src/lib/m4a.c` has 57 of 58 functions in C ([../src/lib/m4a.c](../src/lib/m4a.c);
[THIRD_PARTY.md](../THIRD_PARTY.md)). Its mixer `SoundMainRAM` is copied to IWRAM
at `030035F0`. The data:

* `gSongTable` at `0826456C`: 1,483 entries `{song pointer, ms, me}` (BGM and SFX),
  entry 0 a dummy.
* 81 voicegroups at `08250674`, 415 PCM samples at `082641EC`, and track streams
  from `0857D8D8` (5,611 unaligned GOTO/PATT/REPT pointers; walked by `tools/m4a.py`).

The game's wrappers:

* `Sound_PlaySong(n)` (`0822B2F8`): if the suspend bits (`gUnk_0300523C & 6`) are
  clear, it records `n` in `gUnk_03005470[ms]` so that the song can be resumed,
  then calls `m4aSongNumStart(n)`.
* `Sound_PlaySongChecked` (`0822B3BC`): plays the song only when the flags in
  `gUnk_030053F0`/`F4` allow it *(guess)*.
* `Sound_StopSong` (`0822B358`).

Scripts play sounds with song numbers, for example `Sound_PlaySong(402)` on an
item pickup (`src/item_container.c`). The sound test (`SoundTest_*`,
`081E1AF4`–`081E270C`) browses the table by category.

---

## 13. Clock, sunlight and the solar sensor

* **Cartridge RTC.** The real-time clock is a serial device behind cartridge
  GPIO registers `080000C4`, `080000C6` and `080000C8`. The SDK-level routines
  match pret's `siirtc` family, compiled at `-O0` ([RTC.md](RTC.md)). Boot calls
  `Rtc_Initialize`, `Clock_ClearState` (`082284F4`) and `Clock_CheckRtcDate`
  (`082290F4`). `symbols/functions.csv` names `08248970` as `Rtc_Initialize`, but
  [RTC.md](RTC.md) calls the same address *Unprotect* (open question 3).
* **Game clock.** `GameState` keeps `date`, `hour`, `minute` and `second` from the
  RTC at the last check *(guess)* (section 7). Clock-driven script events are
  `ClockEvents_*` (`080192D0`–`08019518`), which fire when a clock condition
  changes edge.
* **Sunrise and sunset.** `Time_CalculateSunriseSunset` (see
  [FINDINGS.md](FINDINGS.md), the raphaelr notes). The region menu is
  `Time_*` (`081DC6D0`–`081DCE34`).
* **Solar sensor.** A cartridge light sensor read through GPIO, with timer 3
  and its interrupt. `Solar_InitTimer` (`08243C10`), `Solar_StartMeasurement`
  (`08243E4C`) and `Solar_GetMeasurement` (`08243EB8`) are the public calls, and
  the result is a signed count, -1 when there is no reading. The game code that
  consumes the result is around `0822C198`. See [SOLAR_SENSOR.md](SOLAR_SENSOR.md)
  for the protocol and the RAM map (`03006A48` result, `030035B0`–`030035C0`
  interrupt state). The raw count is not calibrated to lux, and its calibration
  constants are unknown.

---

## 14. Save data (EEPROM)

The cartridge has 8 KB of EEPROM ([EEPROM.md](EEPROM.md), [FINDINGS.md](FINDINGS.md)).
The SDK routines `EEPROMRead` (`082486B4`), `EEPROMWrite` (`08248778`),
`EEPROMCompare` (`082488D8`), `EEPROMWrite1_check` (`08248930`) and DMA3 transfer
(`08248634`) are matched EEPROM_V124 code.

The game's save writer is `sub_0822EE68` (a non-matching draft, in
`src/fn/sub_0822EE68.c` under `#ifdef NONMATCHING`). Its layout, from the code:

* The data is split into **8-byte blocks**, starting at a block index. All blocks are
  written with `sub_08248764`, then all are compared with `sub_082488D8`. A
  failure aborts.
* A **footer block** follows: XOR of all halfwords, sum of all halfwords,
  the data length, and `0xFFFF` as an end marker. It is written and verified the
  same way.
* The function returns `length + 8` on success and -1 on failure.

What it holds (traced 2026-10-17 by probing `sub_0822EF44`, the reader, as a
file loads): two save files, A and B, each three checksummed regions, plus a
0x28-byte header at block 0 read at boot.

| Region | File A block | File B block | Bytes | Loaded to |
|---|---|---|---|---|
| script variables | 0x11 | 0x92 | 0x400 | `0203DA00` (`gUnk_02000708`, the `r708` variables: story progress at `+0x3C`, entrance at `+0x70`) |
| game state | 0x113 | 0x254 | 0x778 | `0203C400` (`gUnk_02000710`, `struct GameState`) |
| game state, continued | 0x395 | 0x3B6 | 0xE0 | `0203CB78` |

`sub_0822BBD8` saves and `sub_0822BC6C` loads. Saving first copies the word at
`0x0200060C`, the script image's build timestamp `0x42A05217`, into the game
state's first word; loading rejects a file whose first word differs (a game
that has never saved has 0 there). mGBA and VBA store each 64-bit EEPROM
block byte-reversed. `tools/assets/saves.py` reads and writes saves, reads
VBA-M savestates the same way, and builds a battery save from any savestate.

---

## 15. Wireless (RFU)

The link is Nintendo's RFU SDK, version 1024: 145 functions matched as C in
`src/lib/librfu_*.c` ([RFU.md](RFU.md)). The game's layer sits on top:

* `Wireless connection controller` (`08034084`–`08036CA4`): link status, a
  packet builder for the peer, send and acknowledgement, event dispatch and
  singleton accessors.
* The menus (`08036CA4`–`080395D4`, `0803AC9C`–`0803BAA0`): status menu, peer
  panel, packet exchange, trade and selection, result panel, and the linked game
  items and display. Notes: `notes/link-menus_2026-10-12.md`.
* `Wireless scene objects` (`0803BAA0`–`0803CDF0`): four scene slots with
  actor resource lifetime, position and proximity callbacks.

The packet format and the event dispatch are the parts that a port would need.
[RFU.md](RFU.md) describes the SDK side.

---

## 16. Recurring patterns

These show up across units. Learn them once and the code reads quickly.

* **Create, initialize, update, destroy.** Section 4. Constructors return NULL
  on failure, and the caller kills the task.
* **Entered flag.** A state machine clears its `entered` flag on the first run
  of a new state (section 4).
* **Command queue.** Background, palette and fade controllers keep up to eight
  queued commands, each with up to eight `s16` arguments, filled by scripts and
  drained once per frame.
* **Pools with an active byte.** Fixed slot arrays, a linear scan to allocate
  (section 4).
* **Intrusive lists.** Object lists linked by pointer, with a cached target.
* **Keyword-driven constructors.** `_FromScript` functions read keywords with
  defaults, so scripts can omit most parameters (section 5).
* **Double buffers.** The message queue alternates between two 0x188-byte
  buffers, flipped once per frame.
* **Random table and sine table.** Section 8.
* **Fixed-point positions.** `s16` coordinates in `struct SceneVec`, angles in
  1/256 turns, and fractional blend values (0..64 in the palette slides).
* **Hardware through fixed addresses.** DMA through `0x040000D4`, timers and
  interrupts through the usual I/O registers. Some functions also carry
  *register-pinning* hints (`asm("reg")` locals and empty `asm("")`
  barriers) in matched code. These are compiler-matching concessions, not part
  of the engine's design. Ignore them when reading the logic
  (`CLAUDE.md`).

---

## 17. Where to start, and what is unsettled

### Reading order for a mod

1. [GAME_LAYOUT.md](GAME_LAYOUT.md) for the map of subsystems.
2. `include/engine.h` for the shared API, then `include/game_state.h` and `include/player.h`.
3. One example object end to end: `src/item_container.c` (triggers) or
   `src/scene_object.c` (blocks). Start at its `*_CreateFromScript`.
4. [SCRIPT_TRACING.md](SCRIPT_TRACING.md) to see what a script command does at runtime.

### Open questions in the engine

1. **Frame synchronization.** `AgbMain` calls `TaskSystem_RunFrame` in a loop with
   no visible VBlank wait. The wait may sit in `sub_08219A94`'s callees
   (`sub_0810BB0C` is not yet traced) or in the interrupt handler.
2. **Priority lists and the `+0x16` byte.** The meaning of each of the 14 lists and of
   the `+0x16` byte is not confirmed. `sub_0821A0F8` kills tasks below a priority,
   and its callers have not been traced, so the scene-change rule is unknown.
3. **Two allocators and one name.** `sub_08219C40` (forward first fit) and
   `sub_08219CB4` (backward) both work on the same heap, and `sub_0821A004` uses
   the second. `symbols/functions.csv` names `08248970` `Rtc_Initialize`, but
   [RTC.md](RTC.md) calls it *Unprotect*.
4. **Save contents.** Answered in section 14: two files of three regions
   (variables, game state, continuation); `tools/assets/saves.py`. The
   global `030053F8` is a save-data pointer in [ROM_MAP.md](ROM_MAP.md) and an
   object pointer in [SOLAR_SENSOR.md](SOLAR_SENSOR.md). Both cannot be right
   without more evidence.
5. **Script VM typing.** `gUnk_02000610` is declared as `u32[]`, `u8 *` and
   `struct VM` in different units, and the procedure table (`gUnk_02000438`) is
   read the same way. The keys of the ten-entry `ScriptProc_Start` remap table are
   not explained. The mapping from procedure-call ids (`0x7` opcode) to actual
   procedures is not written down.

### Also unsettled

* The units of positions and the coordinate frame of the terrain map.
* Which pool each `Sprite_*`, `Anim_*` and `Effect_*` helper uses at runtime
  (`gUnk_030042C0`, `gUnk_030042C8`, `gUnk_030042D0` are guesses in `include/engine.h`).
* The meaning of `gUnk_03005470` and the song-resume behaviour across scenes.
* The calibration of the solar reading.
* Which network packets carry which game state.

Everything in this document that is not directly evidenced should carry a
*(guess)* tag. If a guess is confirmed, remove the tag and cite the function
address that proves it.
