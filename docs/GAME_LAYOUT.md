# Boktai 3 game layout: from ROM to gameplay

The grand map of the game: where each part lives in the ROM, which C unit or
header describes it, and what it does in a sentence or two. Addresses are ROM
addresses (`08xxxxxx`) of the Japanese U33J build. The C location links to
the matching translation units or their directories. File counts describe
the C files that intersect the range. For a ROM-ordered list, run
`build/venv/bin/python tools/unit_files.py START END`. Subsystem labels come
from the progress report, while [the source layout](SOURCE_LAYOUT.md)
describes the on-disk grouping. The mechanics are explained in
[ENGINE.md](ENGINE.md). This page is the map.

## Contents

1. [How the game is put together](#1-how-the-game-is-put-together)
2. [ROM regions](#2-rom-regions)
3. [Boot and entry](#3-boot-and-entry)
4. [Engine core](#4-engine-core)
5. [Field and scene objects](#5-field-and-scene-objects)
6. [Enemies, actors and hazards](#6-enemies-actors-and-hazards)
7. [Menus, dialogue and UI](#7-menus-dialogue-and-ui)
8. [Presentation and effects](#8-presentation-and-effects)
9. [Minigames](#9-minigames)
10. [Link and wireless](#10-link-and-wireless)
11. [Hardware and time](#11-hardware-and-time)
12. [Libraries](#12-libraries)
13. [Data areas](#13-data-areas)
14. [Unknown and not yet mapped](#14-unknown-and-not-yet-mapped)

Every subsystem row in `symbols/progress_subsystems.csv` appears in one of the
sections 3 to 11. There are 95 rows. Sections 12 and 13 cover the rest of
the ROM.

---

## 1. How the game is put together

```
 reset (08000000)  ->  AgbMain (0800145C)
                          |  hardware, RTC, sound, heap, registry, script VM
                          v
                    TaskSystem_RunFrame  (forever)
                          |  lists 0..13, each task's update() once per frame
                          v
   root task (list 0): input, message flip                  [section 4]
   |-- scene and field objects, created by event scripts     [section 5]
   |-- actors, enemies, hazards                              [section 6]
   |-- menus, dialogue, status screen                        [section 7]
   |-- backgrounds, palettes, fades, particles, weather      [section 8]
   |-- minigames, link menus                                 [sections 9, 10]
   `-- sound (MP2K), RTC, solar sensor, EEPROM               [sections 11, 12]
```

Nearly everything is a task (see [ENGINE.md](ENGINE.md), section 3). The event
scripts (section 4, script VM) start most objects, so the game logic lives partly in
the bytecode at `08DD6B95`–`08E8755B` and partly in the C.

---

## 2. ROM regions

The complete table with confidence levels is [ROM_MAP.md](ROM_MAP.md). This is
the summary by kind. Bytes that are not listed belong to the neighbouring
region.

| Range | Contents | Notes |
|---|---|---|
| `08000000`–`0800145B` | GBA header, crt0, ARM helpers linked at IWRAM `03000000` | [ROM_MAP.md](ROM_MAP.md) |
| `0800145C`–`0824DAF9` | Thumb code, about 7,900 functions, including the SDK libraries in section 12 | the matched C is in `src/` |
| `0822F248`–`0822F647` | MP2K mixer `SoundMainRAM`, linked at IWRAM `030035F0` | section 12 |
| `0824DAFC`–`08250673` | constant tables after the code | |
| `08250674`–`082638FF` | 81 MP2K voicegroups | section 12 |
| `0826456C`–`08267423` | MP2K song table, 1,483 entries | section 12 |
| `082641EC`–`0857D8D7` | 415 PCM samples (sound) | section 12 |
| `0857D8D8`–`08602C8B` | MP2K track streams | section 12 |
| `086056EC`–`08613E18` | pointer tables, including Thumb function-pointer tables (actor and state dispatch, medium confidence) | [ROM_MAP.md](ROM_MAP.md) |
| `0865xxxx`–`0875xxxx` | background tiles and 0x200-byte tilemaps, some LZ77 | section 13 |
| `0870A230`–`0870C217` | pause-menu and HUD background tiles | [ROM_MAP.md](ROM_MAP.md) |
| `0870C238`… | one pause-menu tilemap per screen, every `0x200` bytes | [screens/pause_menu.txt](screens/pause_menu.txt) |
| `089F8F00`–`089FAEFF` | 8×16 font | section 13 |
| `089FAF00`–`08A2AEFF` | 16×16 kana and kanji font | section 13 |
| `08BE2F04`–`08CDxxxx` | OBJ (sprite) graphics: button labels, menu titles, bike and course menus | section 13 |
| `08D5xxxx`–`08D6B1F7` | proc tables and palettes read by `Script_ReadProcTable` | section 4 |
| `08D6B1F8`–`08D74DDB` | script text pointer table (9,977 entries) | [FINDINGS.md](FINDINGS.md) |
| `08D74DDC`–`08DD6B94` | script text bank | section 13 |
| `08DD6B95`–`08E8755B` | event bytecode (keyword VM), with EUC-JP developer labels | [ENGINE.md](ENGINE.md), section 4 |
| `08E8755C`–`08E88144` | pointer table into the bytecode | |
| `08E88145`–`08FFFFFF` | free (zero padding, 1.47 MiB) | [ROM_MAP.md](ROM_MAP.md) |

The ROM must stay at or below 16 MiB, because EEPROM mapping in emulators and
on hardware depends on it ([FINDINGS.md](FINDINGS.md)).

---

## 3. Boot and entry

Reset, the ARM crt0, the IWRAM helpers, `AgbMain` and the first frame. The
`AgbMain` order is in [ENGINE.md](ENGINE.md), section 2.

| Range | Subsystem (from progress_subsystems.csv) | C location | What it does |
|---|---|---|---|
| `08000000`–`08000350` | GBA header and crt0 | - | Cartridge header, SDK crt0 (stacks, IntrMain install). Handwritten assembly; reported as intentional assembly. |
| `08000350`–`0800145C` | IWRAM ARM code | [`src/engine/iwram_code.c`](../src/engine/iwram_code.c) | Six ARM C functions copied to IWRAM 0x03000000 by AgbMain (palette bank cache, sprite/blit helpers). |
| `0800145C`–`080015AC` | Game entry point | [`src/engine/AgbMain.c`](../src/engine/AgbMain.c), [`src/engine/sub_08001590.c`](../src/engine/sub_08001590.c) | `AgbMain`: hardware and subsystem startup, then the endless `TaskSystem_RunFrame` loop. See ENTRY_AND_MENUS.md. |

[ENTRY_AND_MENUS.md](ENTRY_AND_MENUS.md) describes the entry point and the
start of the menus. The solar and RTC initialization are in section 11.

---

## 4. Engine core

Shared services that every other section depends on: the input and root task,
the heap, the scheduler, the resource registry, the message queue, and the script
VM. [ENGINE.md](ENGINE.md) explains each of them.

| Range | Subsystem (from progress_subsystems.csv) | C location | What it does |
|---|---|---|---|
| `08219A94`–`08219C20` | Runtime input and utilities | [`src/engine/runtime_input.c`](../src/engine/runtime_input.c) | Root task update: input, the message buffer flip and related helpers. Its initializer is at `08219AAC`. |
| `08219C20`–`08219F50` | Heap and memory operations | [`src/engine/heap_memory.c`](../src/engine/heap_memory.c) | Heap: init, first-fit allocation, coalescing free, memory clear and copy, DMA tile copy. |
| `08219F50`–`0821A184` | Task scheduler | [`src/engine/task_scheduler.c`](../src/engine/task_scheduler.c) | Task scheduler: fourteen priority lists, creation, registration, update, destruction, id lookup. |
| `0821A184`–`0821A2DC` | Resource registry | [`src/engine/resource_registry.c`](../src/engine/resource_registry.c) | Resource registry: 32 entries of id, flags and data, with find, insert and cleanup. |
| `0821A2DC`–`0821A454` | Message queue | [`src/engine/message_queue.c`](../src/engine/message_queue.c) | Message queue: double-buffered post and receive. |
| `0821A454`–`0821BAFC` | Script virtual machine | [`src/script/script_virtual_machine.c`](../src/script/script_virtual_machine.c) | Script virtual machine: procedure table, blocks, operands, variables, expression stack and keywords (ENGINE.md section 5). |

Script VM details: the procedure table, blocks, operands and keywords are in
[ENGINE.md](ENGINE.md), section 5, and the comparison with MGS GCL is in
[MGS_GCL_MATCHING.md](MGS_GCL_MATCHING.md) and [PROCESS_REVIEW.md](PROCESS_REVIEW.md).

### Engine services outside the table

These functions have names in `symbols/functions.csv` but no progress row, so
they are not in the table above. They are the services that the subsystem
sections call.

| Range | Functions | What they do |
|---|---|---|
| `08213D10`, `082144A4`–`08215400` | `InterruptsAndVideo_Initialize`, `Sprite_Init`, `Sprite_Remove`, `AnimData_FindEntry`, `AnimData_Load`, `AnimData_SelectPaletteBank`, `ObjGfx_LoadTiles` | interrupt and video setup, sprite and animation resource loading (ENGINE.md section 10) |
| `08215698`–`08216584` | `Video_GetBackgroundMap`, `Bg_ClearTilemap`, `BgGfx_LoadTilemapSimple`, `BgGfx_LoadTilemap` | background tilemap helpers |
| `08217EAC`, `082195E0`, `0821980C` | `Effect_Remove`, `Anim_Remove`, `Anim_LoadKeyframe` | pool removal and keyframe loading |
| `08218D1C`, `08218E5C` | `Text_DrawGlyph`, `TextWindow_BlankRect` | glyph drawing and window clearing (section 7) |
| `082196C4` | `ResourceHeader_Relocate` | copies an animation resource header and rebases its pointers |
| `0821FE40`–`0821FF84` | `Collider_Refresh`, `Collider_Remove`, `Collider_Init`, `Collider_SetPosition`, `Collider_SetCallback` | colliders (ENGINE.md section 9) |
| `08220C8C`–`08220F70` | `Anim_SetSequence`, `Anim_SetKeyframes`, `Anim_Step` | animation playback |
| `082215E4` | `Math_Atan2` | arctangent in 1/256 turns |

---

## 5. Field and scene objects

The objects of the world: blocks that move on the terrain, props that break,
items that drop and containers that open, interaction prompts, and scene
indicators. The scene-object and trigger families are the best examples to read
first ([ENGINE.md](ENGINE.md), sections 4 and 9).

| Range | Subsystem (from progress_subsystems.csv) | C location | What it does |
|---|---|---|---|
| `08003B44`–`080041CC` | Terrain overlays (tentative) | [`src/scene/terrain/terrain_overlay.c`](../src/scene/terrain/terrain_overlay.c), [`src/scene/terrain/sub_080041C0.c`](../src/scene/terrain/sub_080041C0.c) | Overlays that track actors and draw terrain edges, with queued commands. |
| `080041CC`–`08005E10` | Scene objects (tentative) | [`src/objects/scene_objects/scene_object.c`](../src/objects/scene_objects/scene_object.c) | Scene blocks: push, step, fall, flash, disappear. Created from script with keywords (`0x74` manager, `0x1A8` objects). |
| `08005E10`–`08006374` | Terrain modifiers (tentative) | [`src/scene/terrain/terrain_modifier.c`](../src/scene/terrain/terrain_modifier.c), [`src/scene/terrain/sub_08006368.c`](../src/scene/terrain/sub_08006368.c) | Registers and changes terrain cells at runtime, including the script change commands. |
| `08006F00`–`08008A70` | Items and containers (tentative) | [`src/objects/items/item_container.c`](../src/objects/items/item_container.c) | Dropped items (fall, land, bob, pick up) and treasure containers (open, locked). Singleton `TriggerManager`. |
| `08008A70`–`08009500` | Pooled interactables (tentative) | [`src/objects/breakable.c`](../src/objects/breakable.c) | Fixed-pool breakable props with a five-state machine, placed and activated from script (`0xEC`-byte slots). |
| `08009500`–`08009E30` | Directional interaction controllers (tentative) | [`src/objects/directional_controller.c`](../src/objects/directional_controller.c) | Interaction prompts gated by actor distance, facing and input. |
| `08009E30`–`0800A800` | Collision controllers (tentative) | [`src/objects/collision_controller.c`](../src/objects/collision_controller.c) | Pool of 16 hit-detecting props with idle and hit animation and an effect burst. |
| `0800CBA0`–`0800D874` | Falling objects (tentative) | [`src/objects/hazards/falling_objects.c`](../src/objects/hazards/falling_objects.c) | Objects that fall onto the terrain, with allocation, update and cleanup. |
| `0800F9C4`–`080111EC` | Group objects (tentative) | [`src/objects/board_pieces.c`](../src/objects/board_pieces.c) | Manager for sixteen objects with animation and movement; several movement callbacks are unmatched. |
| `08011884`–`08012040` | Scene indicators (tentative) | [`src/ui/hud/scene_indicator.c`](../src/ui/hud/scene_indicator.c) | Scene indicators: lifecycle, timing, visibility, terrain placement, script pause and resume. |
| `08012D08`–`080133CC` | Breakable barriers | [`src/objects/bat_curtain/breakable_barrier.c`](../src/objects/bat_curtain/breakable_barrier.c), [`src/objects/bat_curtain/sub_080133C0.c`](../src/objects/bat_curtain/sub_080133C0.c) | Breakable barriers: manager and callbacks (tentative grouping). |
| `0801E000`–`08020964` | Scene object controller (tentative) | [`src/objects/scene_objects/`](../src/objects/scene_objects/) (43 files) | Scene object controller: shared object slots (140 bytes), script handlers and callbacks. |
| `08020964`–`08020F6C` | Scene message controller | [`src/script/demo_manager.c`](../src/script/demo_manager.c) | Scene message controller: receiver registration, double-buffered dispatch and the scene-script lifecycle. |
| `08020F6C`–`080213EC` | Scene presentation commands | [`src/effects/screen/scene_presentation_commands.c`](../src/effects/screen/scene_presentation_commands.c) | Scene presentation commands: effect, palette-fade and blend script command handlers. |

---

## 6. Enemies, actors and hazards

Actors (characters and enemies) and the hazards that hurt them. The shared actor
code is large and only partly named. The "tentative" labels in the table are
descriptive, not confirmed roles.

| Range | Subsystem (from progress_subsystems.csv) | C location | What it does |
|---|---|---|---|
| `08012040`–`08012AD0` | Animated hazards (tentative) | [`src/objects/hazards/animated_hazard.c`](../src/objects/hazards/animated_hazard.c) | Animated hazards: sprite and collision hazards with warning, attack, hold and retraction states. |
| `080213EC`–`08021EA8` | Common scene actor controller | [`src/scene/actor/scene_actors.c`](../src/scene/actor/scene_actors.c) | Common scene actor controller: registration, health, damage, animation setup, event scripts and effect dispatch. |
| `08021EA8`–`08024F2C` | Actor graphics and basic states (tentative) | [`src/bosses/vampire/actor_graphics_and_basic_states.c`](../src/bosses/vampire/actor_graphics_and_basic_states.c) | Actor graphics and basic states: drawable setup, targeting, resource costs and animation state handlers. |
| `08024F2C`–`08026AA4` | Actor animation and action controllers (tentative) | [`src/bosses/vampire/`](../src/bosses/vampire/) (18 files) | Actor animation and action controllers: variant transitions, motion, recoil, frame holds and timed effects. |
| `08026AA4`–`080284FC` | Actor projectile and reaction controllers (tentative) | [`src/bosses/vampire/`](../src/bosses/vampire/) (22 files) | Actor projectile and reaction controllers: projectiles, turning, anchored reactions, destruction effects. |
| `080284FC`–`080297E8` | Actor damage and lifecycle (tentative) | [`src/bosses/vampire/`](../src/bosses/vampire/) (12 files) | Actor damage visuals and lifecycle callbacks. |
| `080297E8`–`0802C678` | Grid attack actor (tentative) | [`src/bosses/nidhoggr/grid_attack_actor.c`](../src/bosses/nidhoggr/grid_attack_actor.c) | Grid attack actor: attack animation states, damage reactions and command dispatch. |
| `0802C678`–`0802C8A0` | Pooled actor manager | [`src/scene/actor/actor_pool/actor_pool.c`](../src/scene/actor/actor_pool/actor_pool.c) | Pooled actor manager: intrusive actor list, update dispatch, party target cache and allocation. |
| `0802C8A0`–`0802CD04` | Pooled actor shared helpers (tentative) | [`src/scene/actor/actor_pool/pooled_actor_helpers.c`](../src/scene/actor/actor_pool/pooled_actor_helpers.c) | Shared helpers for pooled actors: facing, health, hit capture, target selection and turning. |
| `0802CD04`–`0802D5B4` | Pooled actor controllers (tentative) | [`src/scene/actor/actor_pool/pooled_actor_controller.c`](../src/scene/actor/actor_pool/pooled_actor_controller.c) | Pooled actor controllers: script initialization, drawable, collision, timers, state and action dispatch. |
| `0802D5B4`–`0802D9F8` | Scene actor resources and lifecycle (tentative) | [`src/npc/manager/scene_actor_lifecycle.c`](../src/npc/manager/scene_actor_lifecycle.c) | Scene actor resources and lifecycle: resource lookup, registry, allocation, motion registration and destruction. |
| `0802D9F8`–`08030710` | Scene actor animation and scripted actions (tentative) | [`src/npc/manager/scene_actor_animation.c`](../src/npc/manager/scene_actor_animation.c) | Scene actor animation and scripted actions: terrain height, path nodes, status effects, directional animation, proximity and phase actions. |
| `08030710`–`080308BC` | Scene actor manager | [`src/npc/manager/`](../src/npc/manager/) (5 files) | Scene actor manager: update, destruction, initialization and task creation. |
| `0803CDF0`–`0803EE58` | Scene pooled motion and actor controls (tentative) | [`src/link/wireless/battle/`](../src/link/wireless/battle/) (37 files) | Four actor slots and configurable pooled state entries; target x/z metrics, speed/lateral controls and a twelve-state callback table (ten callbacks exact). Incomplete; gameplay identity unconfirmed. |
| `0818D754`–`0818DC50` | Spinning approach motion (tentative) | [`src/monsters/armor/spinning_approach.c`](../src/monsters/armor/spinning_approach.c) | Two-stage spinning approach movement (four callbacks); see MOTION_0818.md. Not tied to a visible object yet. |
| `081E34D0`–`081EAB64` | Actor animation controller (tentative) | [`src/npc/cutscene_actor/actor_animation_controller.c`](../src/npc/cutscene_actor/actor_animation_controller.c) | Actor animation controller: shared actor layout, child effects, state transitions, palette fades and interaction selection (ANIMATION_081E.md). |

Two families of pooled actors are listed here: `Pooled actor manager` and the
helpers and controllers after it. The "Scene actor" rows are the scene's own
actor records. The actor animation controller at `081E34D0` is documented in
[ANIMATION_081E.md](ANIMATION_081E.md). The spinning approach motion at
`0818D754` is documented in [MOTION_0818.md](MOTION_0818.md); it is not yet tied
to a visible object in the game.

---

## 7. Menus, dialogue and UI

Choices, message boxes, dialogue, text windows, the status screen, prompts,
selection grids, the password screen and the sound test.

| Range | Subsystem (from progress_subsystems.csv) | C location | What it does |
|---|---|---|---|
| `08006374`–`08006F00` | Choice menu | [`src/ui/dialogue/choice_menu.c`](../src/ui/dialogue/choice_menu.c), [`src/ui/dialogue/sub_08006EF4.c`](../src/ui/dialogue/sub_08006EF4.c) | Script-driven choice list: input, text markup, layout and dispatch of the chosen callback. |
| `0800E3E0`–`0800F4C4` | Status screen (tentative) | [`src/ui/status_screen/`](../src/ui/status_screen/) (3 files) | Status screen: numeric display, palette fades and the scanline transition chain. Split in three files for compiler flags. |
| `0800F4C4`–`0800F850` | Numeric overlay (tentative) | [`src/ui/status_screen/numeric_overlay.c`](../src/ui/status_screen/numeric_overlay.c) | Counter display, a sibling of the status screen with the same background and palette load but no fades. |
| `0800F850`–`0800F9C4` | Result prompt | [`src/ui/results/result_prompt.c`](../src/ui/results/result_prompt.c) | Input prompt with callbacks and lifecycle. |
| `080111EC`–`0801150C` | Message menu | [`src/ui/dialogue/message_menu.c`](../src/ui/dialogue/message_menu.c) | Message box: background text windows, transition, update and cleanup. |
| `0801150C`–`08011884` | Password decoding | [`src/ui/title/password.c`](../src/ui/title/password.c) | Password text filtering, decoding and validation. |
| `08019E58`–`0801B49C` | Selection menu (tentative) | [`src/ui/widgets/selection_menu.c`](../src/ui/widgets/selection_menu.c) | Selection menu for actors and items: display, cursors, availability, confirmation and script initialization. |
| `080308BC`–`0803198C` | Dialogue controls and animation (tentative) | [`src/ui/dialogue/message_window/message_window.c`](../src/ui/dialogue/message_window/message_window.c) | Dialogue controls: text lookup, choice parsing, window rectangles, palette, scrolling, timing, fast text, graphics cleanup. |
| `0803198C`–`08031E54` | Dialogue task setup (tentative) | [`src/ui/dialogue/message_window/`](../src/ui/dialogue/message_window/) (8 files) | Dialogue task setup: allocator, callbacks, singleton and rectangle and VM accessors. |
| `08031E54`–`08032EDC` | Dialogue text interpreter | [`src/ui/dialogue/text_interpreter/`](../src/ui/dialogue/text_interpreter/) (34 files) | Dialogue text interpreter: inline commands, speaker labels, glyph escapes, number formatting, nested styles. |
| `08032EDC`–`080335F4` | Scrolling dialogue controller | [`src/ui/dialogue/scrolling_dialogue/`](../src/ui/dialogue/scrolling_dialogue/) (3 files) | Scrolling dialogue controller: text reveal timing, row clearing, show and hide, scheduler callbacks. |
| `080335F4`–`08033C58` | Text window manager | [`src/ui/dialogue/text_window/`](../src/ui/dialogue/text_window/) (29 files) | Text window manager: independent windows that each embed the interpreter; singleton list at `gUnk_020000E4`. |
| `08033C58`–`08034084` | Scripted dialogue transitions | [`src/ui/dialogue/dialogue_transition.c`](../src/ui/dialogue/dialogue_transition.c) | Scripted dialogue transitions: placement, timed fades, script arrays, display-register restore. |
| `081E1AF4`–`081E270C` | Sound-test menu | [`src/ui/title/sound_test.c`](../src/ui/title/sound_test.c) | Sound test: category and track browsing, playback indicators. |

### Documented menu code without a table row

These are known from [MENU_CONTEXT.md](MENU_CONTEXT.md) and the matched code,
but they have no row in `progress_subsystems.csv`.

| Address | Function | Evidence and status |
|---|---|---|
| `08175D34`, `08176020`, `08176224` | pause-menu config, sleep and save confirm handlers | medium: names from the menu labels, and static code; the confirm branches are not yet observed in a replay |
| `08163EB8` | shared pause-menu callback used by the three handlers above | MENU_CONTEXT.md |
| `081DC02C`–`081DCE34` | `Menu_EraseRect`, `Menu_DrawText`, the region menu (`Time_ShowRegionMenu`, `Time_OnRegionListSelect`, `Time_HandleRegionKeypad`, `Time_DestroyRegionMenu`, `Time_LoadRegionList`) and `Time_OnLoadSave` | region selection and the load-save hook (medium confidence) |

---

## 8. Presentation and effects

Backgrounds, palettes, fades, particles, shadows, scanline effects, weather,
sprite and tile animation, and the timed presentations that run them.

| Range | Subsystem (from progress_subsystems.csv) | C location | What it does |
|---|---|---|---|
| `080015AC`–`08002AA8` | Background controllers | [`src/effects/screen/background_controller.c`](../src/effects/screen/background_controller.c) | Background layers: static and scrolling families with map loading, camera follow, blend and an eight-command queue. |
| `08002AA8`–`080036F8` | Palette controllers | [`src/effects/palette/palette_controller.c`](../src/effects/palette/palette_controller.c) | Palette blending, brightness scaling, oscillation, script-driven transitions and hardware palette upload. |
| `080036F8`–`08003B44` | Color and fade controller | [`src/effects/palette/color_fade_controller.c`](../src/effects/palette/color_fade_controller.c) | Two-channel colour-blend fade with an eight-entry command queue (fade in, fade out, hold). |
| `0800A800`–`0800C49C` | Homing and orbiting effects (tentative) | [`src/effects/orbits/homing_effect.c`](../src/effects/orbits/homing_effect.c) | Six particle effects (four sprites each) that drift, orbit and home in on the player; `EffectManager`. |
| `0800C49C`–`0800C7F4` | Effect emitter (tentative) | [`src/effects/particles/effect_emitter.c`](../src/effects/particles/effect_emitter.c) | Emitter task that spawns effects; allocation, initialization, update and cleanup. |
| `0800C7F4`–`0800CBA0` | Damage controller | [`src/objects/hazards/damage_controller.c`](../src/objects/hazards/damage_controller.c) | Hit and death handling for an actor; runs its hit and death scripts with `Script_Call`. |
| `0800D874`–`0800E0CC` | Scanline presentation (tentative) | [`src/effects/screen/scanline_presentation.c`](../src/effects/screen/scanline_presentation.c) | Per-scanline animation layers (six display layers) and their lifecycle. |
| `0800E0CC`–`0800E3E0` | Palette sets | [`src/effects/palette/palette_set.c`](../src/effects/palette/palette_set.c), [`src/effects/palette/sub_0800E3D4.c`](../src/effects/palette/sub_0800E3D4.c) | Palette sets: allocation, initialization, blending and upload. |
| `08012AD0`–`08012D08` | Debris particles | [`src/effects/particles/debris_particles.c`](../src/effects/particles/debris_particles.c) | Debris particles: manager callbacks (tentative grouping). |
| `080133CC`–`080136B4` | Terrain shadows | [`src/effects/shadows/terrain_shadow.c`](../src/effects/shadows/terrain_shadow.c) | Terrain shadows: shadows that follow the terrain height. |
| `080136B4`–`08013C14` | Scaled shadows | [`src/effects/shadows/scaled_shadow.c`](../src/effects/shadows/scaled_shadow.c) | Scaled shadows: shadows scaled to the object's height. |
| `08013C14`–`08013F98` | Shadow registry | [`src/effects/shadows/shadow_registry.c`](../src/effects/shadows/shadow_registry.c) | Shadow registry: the list of active shadows. |
| `08013F98`–`08014F64` | Particle groups | [`src/effects/particles/groups/`](../src/effects/particles/groups/) (12 files) | Particle groups: manager lifecycle and particle creation, update and emission. |
| `08014F64`–`08015160` | Fragment groups | [`src/effects/particles/groups/fragment_group.c`](../src/effects/particles/groups/fragment_group.c) | Fragment groups: manager and particle emission (provisional names). |
| `08015160`–`08015F60` | Animated particles | [`src/effects/particles/groups/`](../src/effects/particles/groups/) (16 files) | Animated particles: manager, ring, burst and directional emitters, script creation. |
| `08015F60`–`080164B8` | Fading particle groups | [`src/effects/particles/groups/`](../src/effects/particles/groups/) (7 files) | Fading particle groups: group lifetime and fading animation. |
| `080164B8`–`0801689C` | Palette level transitions | [`src/effects/palette/palette_level_transitions.c`](../src/effects/palette/palette_level_transitions.c) | Palette level transitions: increase, decrease, configure and reset per channel. |
| `0801689C`–`08017270` | Floating item effects (tentative) | [`src/effects/floating_item_effect.c`](../src/effects/floating_item_effect.c) | Floating item effects: bobbing projection, departure and collection animation. |
| `08017270`–`08017428` | Scanline wave distortion | [`src/effects/screen/scanline_wave.c`](../src/effects/screen/scanline_wave.c) | Scanline wave distortion: update, initialization, creation and cleanup. |
| `08017428`–`08018438` | Weather and storm controller (tentative) | [`src/effects/weather/`](../src/effects/weather/) (25 files) | Weather: storm wait, onset, active and end states, weather particles, script commands. |
| `08018438`–`080187BC` | Palette cycling | [`src/effects/palette/palette_cycle.c`](../src/effects/palette/palette_cycle.c) | Palette cycling: interpolated palette cycles with script initialization. |
| `080187BC`–`08018D94` | Orbiting particles | [`src/effects/orbits/orbiting_particles.c`](../src/effects/orbits/orbiting_particles.c) | Orbiting particles: orbit movement, terrain placement and script creation. |
| `08018D94`–`08018F38` | Hardware blend transitions | [`src/effects/screen/blend_transition.c`](../src/effects/screen/blend_transition.c) | Hardware blend transitions: animated blend coefficients. |
| `08018F38`–`080192D0` | Circular scanline window | [`src/effects/screen/circle_window/`](../src/effects/screen/circle_window/) (6 files) | Circular scanline window: scanline generation, expansion, contraction and lifecycle. |
| `080192D0`–`08019518` | Clock events | [`src/system/clock/clock_events.c`](../src/system/clock/clock_events.c) | Clock events: script events on real-time clock edges. |
| `08019518`–`080195DC` | Palette frame cycling | [`src/effects/palette/palette_frames.c`](../src/effects/palette/palette_frames.c) | Palette frame cycling: seven-frame palette animation. |
| `080195DC`–`08019A14` | Tile animation | [`src/engine/video/tile_animation.c`](../src/engine/video/tile_animation.c) | Tile animation: eight channels with timing, DMA upload and script controls. |
| `08019A14`–`08019E58` | Sprite pool | [`src/engine/video/sprite_pool.c`](../src/engine/video/sprite_pool.c) | Sprite pool: sixteen slots with allocation, animation playback and removal. |
| `081E270C`–`081E2898` | Timed presentation | [`src/effects/screen/presentation/timed_presentation.c`](../src/effects/screen/presentation/timed_presentation.c) | Timed presentation: update, cleanup and initialization (ENTRY_AND_MENUS.md). |
| `081E2898`–`081E34D0` | Scripted presentation | [`src/effects/screen/presentation/`](../src/effects/screen/presentation/) (13 files) | Scripted presentation: allocation through global callback cleanup. |

Shared controllers and their layouts are in
[`include/effects/screen/background_controller.h`](../include/effects/screen/background_controller.h),
[`include/effects/palette/palette_controller.h`](../include/effects/palette/palette_controller.h) and
[`include/effects/palette/color_fade_controller.h`](../include/effects/palette/color_fade_controller.h).
[BACKGROUND_CONTROLLERS.md](BACKGROUND_CONTROLLERS.md) covers the background
families. The engine's palette and tile services are in
[ENGINE.md](ENGINE.md), section 10.

---

## 9. Minigames

Two rows are minigame code. The sound test is in section 7. Both rows are
tentative: the purpose of the transformation sequence and the result scoring
still needs a playthrough to confirm.

| Range | Subsystem (from progress_subsystems.csv) | C location | What it does |
|---|---|---|---|
| `0801B49C`–`0801D104` | Transformation and result game (tentative) | [`src/minigames/result_game/`](../src/minigames/result_game/) (42 files) | Result and transformation game: shared state, scoring, graphics and a transformation sequence (tentative). |
| `0801D104`–`0801E000` | Result game tutorial (tentative) | [`src/minigames/result_game/result_tutorial.c`](../src/minigames/result_game/result_tutorial.c) | Tutorial for the result game: prompts, movement and press demonstrations, setup and teardown. |

---

## 10. Link and wireless

The link feature uses Nintendo's RFU (wireless adapter) SDK, version 1024, plus
the game's own controller and menus. The SDK is in section 12. [RFU.md](RFU.md)
describes it. The notes for the link menus are in
`notes/link-menus_2026-10-12.md`.

| Range | Subsystem (from progress_subsystems.csv) | C location | What it does |
|---|---|---|---|
| `08034084`–`08036CA4` | Wireless connection controller | [`src/link/wireless/connection/`](../src/link/wireless/connection/) (104 files) | Wireless connection controller: link status, peer packet builder, send and acknowledgement, event dispatch. |
| `08036CA4`–`08036E08` | Wireless text encoding | [`src/link/wireless/wireless_text.c`](../src/link/wireless/wireless_text.c) | Wireless menu text encoding and shared offsets (notes/link-menus_2026-10-12.md). |
| `08036E08`–`08037228` | Wireless status menu | [`src/link/wireless/status_menu.c`](../src/link/wireless/status_menu.c) | Wireless status menu callbacks. |
| `08037228`–`08037B20` | Wireless peer panel | [`src/link/wireless/peer_panel.c`](../src/link/wireless/peer_panel.c) | Wireless peer panel callbacks. |
| `08037B20`–`080380C4` | Wireless packet exchange | [`src/link/wireless/packet_exchange/`](../src/link/wireless/packet_exchange/) (8 files) | Wireless packet exchange in the menus. |
| `080380C4`–`080394D4` | Wireless trade and selection menu (tentative) | [`src/link/wireless/trade_menu/`](../src/link/wireless/trade_menu/) (26 files) | Wireless trade and selection menu. |
| `080394D4`–`080395D4` | Wireless exit task | [`src/link/wireless/wireless_exit.c`](../src/link/wireless/wireless_exit.c) | Wireless exit task. |
| `080395D4`–`0803AC9C` | Wireless game items and display (tentative) | [`src/link/wireless/battle/wireless_game_items_and_display.c`](../src/link/wireless/battle/wireless_game_items_and_display.c) | Wireless game items and display for linked play. |
| `0803AC9C`–`0803BAA0` | Wireless result panel (tentative) | [`src/link/wireless/result_panel/`](../src/link/wireless/result_panel/) (18 files) | Wireless result panel. |
| `0803BAA0`–`0803CDF0` | Wireless scene objects (tentative) | [`src/link/wireless/battle/`](../src/link/wireless/battle/) (16 files) | Four wireless scene slots with actor resource lifetime, position and proximity callbacks. |

---

## 11. Hardware and time

The solar sensor is a cartridge device read through GPIO, and the RTC is a
serial device on the same cartridge pins. Both are documented in their own
files. The EEPROM save is in section 12.

| Range | Subsystem (from progress_subsystems.csv) | C location | What it does |
|---|---|---|---|
| `08243C10`–`08243EFC` | Cartridge solar sensor | [`src/system/solar/`](../src/system/solar/) (11 files) | Cartridge solar sensor: GPIO and timer 3 driver with init, shutdown, interrupt and measurement (SOLAR_SENSOR.md). |

| Range | Subsystem | C location | What it does |
|---|---|---|---|
| `08248970`–`08249238` | SIIRTC (RTC) | [`src/lib/rtc/`](../src/lib/rtc/) (16 files) | cartridge clock: unprotect and protect, status, time read and write. The alias `Rtc_Initialize` in `symbols/functions.csv` names the same address as `SiiRtcUnprotect` (see ENGINE.md, section 17) |
| `082284F4`, `082290F4` | `Clock_ClearState`, `Clock_CheckRtcDate` | [`src/system/clock/sub_082284F4.c`](../src/system/clock/sub_082284F4.c), [`src/system/clock/sub_082290F4.c`](../src/system/clock/sub_082290F4.c) | game clock state and the date check at boot |
| `0822AE00`, `0822A7A8` | `Time_CalculateSunriseSunset`, `Time_CalculateSunriseSunsetCore` | [`src/system/clock/Time_CalculateSunriseSunset.c`](../src/system/clock/Time_CalculateSunriseSunset.c), [`src/system/clock/Time_CalculateSunriseSunsetCore.c`](../src/system/clock/Time_CalculateSunriseSunsetCore.c) | sunrise and sunset from the boot-time arguments |
| `080192D0`–`08019518` | Clock events (section 8) | [`src/system/clock/clock_events.c`](../src/system/clock/clock_events.c) | script events on clock edges |

---

## 12. Libraries

The SDK and compiler-runtime code that the game links. Most of it is matched
against open source, as the table says. Source names and evidence are in the
linked notes.

| Range | Library | C location | Notes |
|---|---|---|---|
| `0822F1B4`–`08231448` | MP2K (`m4a`) sound driver | [`src/lib/m4a/`](../src/lib/m4a/) (4 files) | [THIRD_PARTY.md](../THIRD_PARTY.md); `symbols/proposed/m4a.csv`; the mixer `SoundMainRAM` at `0822F248` runs from IWRAM |
| `08243EFC`–`08248590` | RFU SDK 1024 (STWI transport, serial interrupt, link API, identification) | [`src/lib/rfu/`](../src/lib/rfu/) (4 files) | [RFU.md](RFU.md) |
| `08248590`–`082485EC` | `libagbsyscall` BIOS wrappers | `symbols/functions.csv` | standard syscalls such as `CpuSet` and `VBlankIntrWait` |
| `082485EC`–`08248970` | EEPROM SDK V124 | [`src/lib/eeprom/`](../src/lib/eeprom/) (6 files) | [EEPROM.md](EEPROM.md) |
| `08248970`–`08249238` | SIIRTC | [`src/lib/rtc/`](../src/lib/rtc/) (16 files) | [RTC.md](RTC.md) |
| `08249238`–`0824ACFC` | libgcc runtime (`_call_via_rX`, division, floating point) | [`src/lib/libgcc/`](../src/lib/libgcc/) (6 files) | [LIBGCC.md](LIBGCC.md) |
| `0824ACFC`–`0824DA3C` | libm (Sun fdlibm) | [`src/lib/libm/`](../src/lib/libm/) (22 files) | [LIBM.md](LIBM.md) |
| `0824DA3C`–`0824DAFA` | libc (`memcpy`, `memset`) | [`src/lib/libc/`](../src/lib/libc/) (3 files) | [LIBM.md](LIBM.md) |

The MP2K rows in this table are the library. The song table and the sound data
that the game plays through it are in section 13.

---

## 13. Data areas

These are not code. Each area has a full description in the linked document.

| Area | Where | What | Document |
|---|---|---|---|
| Script text | `08D6B1F8`–`08D74DDB` (table), `08D74DDC`–`08DD6B95` (bank) | 9,977 entries, bit 31 marks text, two-byte kana and kanji codes, inline `<TAG=…>` markup | [FINDINGS.md](FINDINGS.md), [ENGINE.md](ENGINE.md) section 11 |
| Event bytecode | `08DD6B95`–`08E8755B` | keyword VM bytecode, with developer labels in EUC-JP for demos and debug messages | [ENGINE.md](ENGINE.md) section 5; [FINDINGS.md](FINDINGS.md), *Debug leftovers* |
| Sound | `0826456C`–`08602C8B` | song table, voicegroups, 415 PCM samples, track streams | [ROM_MAP.md](ROM_MAP.md); `tools/m4a.py` |
| Background tiles and maps | `0865xxxx`–`0875xxxx` | 4bpp tiles, tilemaps, some LZ77 | [ROM_MAP.md](ROM_MAP.md) |
| Pause and HUD | `0870A230`–`0870DF78` | background tiles and per-screen tilemaps | [screens/pause_menu.txt](screens/pause_menu.txt), [ROM_MAP.md](ROM_MAP.md) |
| Fonts | `089F8F00`–`08A2AEFF` | 8×16 Latin and accented glyphs, 16×16 kana and kanji | [FINDINGS.md](FINDINGS.md) |
| Sprite graphics | `08BE2F04`–`08CDxxxx` | button labels, menu titles | [ROM_MAP.md](ROM_MAP.md) |
| Portraits and graphics | `08A2xxxx`–`08B1xxxx` | medium confidence | [ROM_MAP.md](ROM_MAP.md) |

The solar sensor, the RTC and the EEPROM have their own data tables in their
documents. Use `tools/vram_map.py` to find which ROM bytes a given screen's tiles
came from.

---

## 14. Unknown and not yet mapped

The leading 0803CDF0–0803EE58 stretch now has a tentative pooled motion/actor
controller group and shared types in `include/link/wireless/scene_pool.h`; ten additional
functions match. Its gameplay identity and remaining state callbacks are still
unresolved. See [scene-pool notes](../notes/scene-pool_2026-10-13.md).

What this layout does not yet explain, roughly by size.

### Code without a subsystem row

Four ranges have no entry in `symbols/progress_subsystems.csv`. Together they
hold most of the remaining Thumb code.

| Range | Size | What is known |
|---|---|---|
| `0803CDF0`–`0818D754` | 1.38 MB | about 2,070 function files at the time of the first mapping pass. This is the largest unmapped area and most of the field, menu and actor logic that is not yet grouped. The pause-menu config handlers (`08175D34`…) are in it |
| `0818DC50`–`081E1AF4` | 344 KB | about 820 function files. The region menu and `Menu_*` helpers are named (section 7). The rest has no names |
| `081EAB64`–`08219A94` | 192 KB | about 430 function files. Video, sprite, animation, background and text services (section 4, *Engine services*). The row before it is `081E34D0`, the actor animation controller |
| `0821BAFC`–`08243C10` | 164 KB | about 560 function files. Colliders, animation, the MP2K driver body (`0822F1B4`–`08231448`) and the RTC and time code (`082284F4`–`0822AE00`), which are named in section 4 and sections 11 and 12 |

### Unmatched gaps inside mapped subsystems

The progress notes of several rows say that some functions still do not match.
These gaps are in `progress_subsystems.csv` evidence and the subsystem notes:

* Items and containers: one pickup routine (`080070D0`) unmatched.
* Group objects: seven functions not yet attempted (`0800FC28` and `0800FE98` are twins).
* Selection menu: four graphic initializers and the grid navigation.
* Actor controllers: projectile and target-transfer gaps, and the death controller.
* Wireless menus: unmatched spans in each menu row.
* Some "debris", "barrier" and "shadow" rows have unmatched reset tails.

### Not yet understood

* The **frame synchronization** point (the scheduler loop does not wait for VBlank
  in `AgbMain`) (ENGINE.md section 17, question 1).
* The **save contents** and the caller of the EEPROM writer
  `sub_0822EE68` (ENGINE.md section 14 and 17, question 4).
* The **meaning of the 14 priority lists** and the scene-change kill rule
  (ENGINE.md section 17, question 2).
* The **script procedure ids** used by the `0x7` opcode, and the keys of the
  `ScriptProc_Start` remap table (ENGINE.md section 17, question 5).
* Which **pause-menu tabs** other than config, sleep and save exist and how they are
  reached.
* The **calibration** of the solar reading to light levels.
* The **packet format** of the link and the state it carries.
* The **story** side: which scripts own which chapters. The labels in the bytecode
  help ([FINDINGS.md](FINDINGS.md), *Debug leftovers*).

Use `tools/asmat.py ADDR 40` for the assembly of any unnamed function and
`tools/function_context.py` to see which functions a scene runs
([RUNTIME_CONTEXT.md](RUNTIME_CONTEXT.md)).
