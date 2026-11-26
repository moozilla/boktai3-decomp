# Source layout: decisions

Tags: #tooling #process

Companion to [the source layout](SOURCE_LAYOUT.md). The directory and header rules are in `symbols/source_dirs.csv` and
`symbols/header_dirs.csv`. The Player lists are recorded per file in
`symbols/source_files.csv`.

## Evidence used

* `gcl/names/chara.csv` and `data/actor_registry.csv`: the GCL key of each
  script-creatable object and, for 54 keys, its original Japanese name. This
  identified the coffin (棺桶 → TetherActor), the Warp Circle (ワープ魔方陣 →
  StackedActor), the Solar Stand (太陽スタンド → ResourceStation), the Pile Driver
  (パイルドライバー → ScenePanel), the Weapon Shop (武器屋 → SceneInventory), the
  Demo Manager (デモマネージャ → SceneMessages), the Vampire, the Bat Curtain, and
  that the link "registries" F246 and 33FF are LinkPanel and LinkLobbyPanel.
* The ROM text's monster list (docs/ASSETS.md): the five bosses are フレスベルグ,
  サバタ, ニーズホッグ, ラタトスク, ヴァナルガンド.
* ROM contiguity (`symbols/progress_subsystems.csv`) and a call graph over the
  tree (who calls whom across unit directories): a helper unit that sits
  between two blocks of one object and is called only by it belongs to it.
* Function prefixes and first functions of each translation unit.
* Running-game validation tied Vanargand to actor 7C56 in the bike chase,
  Nidhoggr to the fight replayed in scene 6425, the Pile Driver to the task
  family created by scenes 6182 and 6183, Armor and Statue to Bestiary entries
  29 and 28, and the slot-2 boss to Sabata. The corrected source rules carry
  the script and text references.

## Top level (14, was 13)

| New | From | Why |
|---|---|---|
| `player` | player | kept; 379 loose files split into ten parts |
| `monsters` | monsters | kept: the game itself says monster (魔物図鑑, the monster list); "enemies" would be our word, not theirs |
| `bosses` | bosses + parts of actors, effects, field | each boss gathers its helpers, including the Vampire's bat swarm |
| `npc` | actors | townspeople and story characters were mixed with the actor framework; a player looks for them as NPCs |
| `objects` | field + the Pile Driver from menus | dungeon and town objects, including the purification apparatus. Terrain machinery moved to `scene/terrain` |
| `effects` | effects | kept, now grouped: 92 directories became 48, most of them group members |
| `ui` | menus | the category id was already `ui`; HUD, dialogue and damage numbers are not menus. Menus are one group among `hud`, `dialogue`, `pause`, `town`, `books`, `title`, `results`, `widgets` |
| `minigames` | minigames | kept |
| `link` | link + link menus from menus | the 0810xxxx link menu block (record menu/panel, message panel) was filed under menus |
| `system` | system + solar calibration, clock events, challenge records, inventory data | the cartridge (sensor, clock, save chip users) and what is saved |
| `scene` | actors, engine, field | the world simulation every moving thing shares: the scene-actor framework, collision, terrain, paths, camera, scene lifecycle. It was split across three categories |
| `script` | engine | the GCL VM and runtime were eight `engine/script*` directories; one place now |
| `engine` | engine | what is left is runtime and hardware: boot, tasks, heap, video, math, game-side sound |
| `lib` | lib | upstream libraries, unchanged (tools/source_layout.py `is_upstream` relies on `src/lib/<x>/<file>`) |

`actors` and `other` are gone. "Actors" was the implementation word for
everything with a task; its contents went to the object they belong to (a boss,
a monster, an NPC) or to `scene/actor/` when they are the shared framework.

`engine` and `system` stay separate: `system` is what a player calls the
cartridge (sun sensor, clock, saving); `engine` is what a programmer calls the
runtime. Merging them would hide the solar sensor among DMA helpers.

## Grouping levels

* Object directory at three files or more; one file `category/object.c`; two
  files sit directly in the parent. Exception: a two-file object gets a
  directory when its file names would not say what they belong to: `objects/coffin/`
  (tether_actor.c), `objects/bat_curtain/` (breakable_barrier.c), `ui/save/`
  (scene_tilemap.c), `ui/game_over/` (particle_pool.c).
* A group level only for families a player or contributor would recognise; no
  group holds fewer than three members. `scene/actor/` is split into the
  framework's parts (registry, behavior, control, reaction, attachment,
  placement, animation) because together they are 420 files.
* Player is split by what the function does, from its name
  (`player_files_<group>.txt`): core 67, movement 75 (includes the camera
  offsets), attack 41, gun 25, damage 44 (plus player_contact), stats 43,
  interaction 47 (coffin chain, pushing, stations), cues 35 (plus player_cue),
  script 1 (plus ScriptActor), appearance (player_3.c renamed
  `player_visuals.c`, plus the character model and keyframe actor). An unnamed
  function (59 of them) goes with its ROM predecessor.

## Naming rules

Lowercase snake_case nouns; the game's name where known (from chara.csv or the
monster list), else the project's function prefix when it is a real noun
(`named_object`, `modifier_menu`), else the most honest description of the
behavior, marked `guess` in the table's note. No digits, hex, slot numbers,
registry keys or "tentative" anywhere below `src/` and `include/` (except the
upstream file names in `lib/`, and real format names such as `fixed8_` in a
header). Single files are renamed only when the old name was a placeholder or
misleading.

## Placeholders renamed (evidence, one line each)

| Was | Now | Evidence |
|---|---|---|
| `actors/registry_a8_cb` | `npc/sabata_rider/` | GCL 6316 creates A8CB for Sabata's purple bike alongside Django's E881 bike |
| `actors/task_4_e01` | `effects/projectiles/projectile/` | trail sprites, impacts, terrain turns; calls into Projectile_* and TimedEffect_* in the same ROM block 081D75E8-081DAF50 |
| `actors/singleton_slot_7` | `bosses/vanargand/` | GCL 6325 calls 6314, which creates Vanargand (actor 7C56) during Django's bike chase |
| `actors/scene_actor_14` | `effects/sprites/sprite_effect/` | "shared animation data for category 14 actors": sprite bursts, groups and particles used by Nidhoggr, Octopus and effect schedules |
| `actors/state_actor_kind_086121_c4` | `npc/story_character/` | 96-state animation table driven by SceneCharacter_CommandFromScript (its only caller) |
| `actors/scripted_message_actor_08612344` | `npc/story_character/` | state table that waits on scene messages; called only by SceneCharacter |
| `bosses/boss_slot_2` | `bosses/sabata/` | running-game verification confirmed Sabata |
| `monsters/enemy_d459` | `monsters/armor/` | palette 0227 with sprite package 7E82, Bestiary index 29 (text 9867) |
| `monsters/enemy_f426` | `monsters/statue/` | palette 0225 with sprite package 7E82, Bestiary index 28 (text 9866) |
| `link/registry_1745` | `link/session/` | session state, handshake requests, transfer counters: the link session controller |
| `link/registry_f246` | `link/link_panel/` | data/actor_registry.csv: key F246 is LinkPanel_Create |
| `link/registry_33_ff` | `link/lobby/` | key 33FF is LinkLobbyPanel_Create |
| `menus/registry_3_f74` | `ui/town/town_map.c` | GCL 2554 creates 3F74, which displays San Miguel and has eleven town service callbacks |
| `effects/registry_6_a37` | `effects/screen/magic_cannon_departure.c` | GCL 4928 creates 6A37 for the scrolling sky before the space flight |
| `engine/registry_e7_bc` | `scene/collision/collision_manager.c` | the file starts with Bounds_TestOverlap, Collision_DispatchOverlaps, Collider_CreateManager; the E7BC pool and result counters follow |
| `other/registry_selection` | `ui/world_map.c` | GCL 2570 creates 2388 for the nine world destinations |

Other names that changed meaning, not only spelling:

| Was | Now | Evidence |
|---|---|---|
| `field/tether_actor` | `objects/coffin/` | 棺桶 (key 0B50) creates TetherActor; seven chain segments, player tether mode |
| `field/stacked_actor` | `objects/warp_circle/` | ワープ魔方陣 creates StackedActor |
| `field/stored_resource_station` | `objects/solar_stand/` | 太陽スタンド creates ResourceStation |
| `menus/scene_panel_*` (4) | `objects/pile_driver/` | GCL 6182 and 6183 create CBB0 for the outdoor and arena Pile Driver |
| `actors/actor_damage_and_lifecycle` | `bosses/vampire/` | Vampire_Create, key D972 ヴァンパイア |
| `menus/inventory_*` (3) | `ui/town/shop/` | 武器屋 creates SceneInventory_CreatePresentation0 |
| `engine/scene_message_controller` | `script/demo_manager.c` | デモマネージャ creates SceneMessages |
| `field/trap` | `scene/terrain/path/` | 18 of 22 functions are Path_*; "Trap" was five functions |
| `effects/following_sprite_effect` | `player/core/player_core.c` | 0813ADC4 is a Player block (Player_SetAnimation, UpdateFacingSide) |
| `effects/large_scene_animation_controller` | `bosses/nidhoggr/` | 114 of 159 functions are Nidhoggr_* |
| `effects/palette_transition` | `ui/books/monster_book/` | BestiaryDisplay_Create; sits just before the Bestiary; callers are the Monster Book |
| `effects/pal_fade` | `system/game_state/` | GameState challenge best ranks and records |
| `field/group_objects` | `effects/scene/bat_flock.c` | the registered E534 constructor and launch handler display a purple bat flock using sprite package DA6D |
| `menus/scene_hud` | `ui/pause/pause_display/` | SceneHud ranges are interleaved with the pause menu and pause inventory |
| Phrase names (`actor_animation_and_action_controllers`, `modifier_selection_and_stat_previews`, ...) | their object | see the table: each joined its owner |

## Merges (units that are one object)

| Directory (files) | Units |
|---|---|
| `bosses/vampire/` (86) | the Vampire and its controllers, contact minions and bat swarm (actors/actor_graphics_and_basic_states, actor_animation_and_action_controllers, actor_projectile_and_reaction_controllers, actor_damage_and_lifecycle, contact_actor_pool_*, contact_swarm_states_and_spawning) |
| `bosses/sabata/` (54) | boss_slot_2, scene_event_actor, actor_impact, actor_position, actor_state, actor_decision, field/scene_volume (one ROM block, 08179B9C-08180488) |
| `bosses/vanargand/` (287) | vanargand, vanargand_projectile, paired_scene_actor, actor_status, effects/positioned_effect, singleton_slot_7, actor_display, effects/sprite_trail |
| `bosses/nidhoggr/` (53) | nidhoggr, effects/large_scene_animation_controller, grid_attack_actor, sequence_contact_actor_pool, orbit_contact_actor, contact_burst_effects |
| `objects/pile_driver/` (107) | the four scene_panel units |
| `monsters/armor/` (111) | armor, actor_controller, actor_marker, actor_palette, scene_combat_actor, spinning_approach_motion, enemy_d459 |
| `monsters/ivy/` (18) | ivy, phase_object, child_group |
| `monsters/axe/` (3) | axe, actor_phase |
| `minigames/coffin_bike/` | glove, actor_recoil |
| `monsters/centipede/` | tentacle_actor, tentacle_body |
| `monsters/cannons/wind/` | fan_cannon, fan_cannon_manager |
| `npc/story_character/` (109) | scene_character, state_actor_kind_086121_c4, scripted_message_actor_08612344 |
| `npc/manager/` (7) | scene_actor_manager, scene_actor_resources_and_lifecycle, scene_actor_animation_and_scripted_actions |
| `objects/scene_objects/` (44) | field/scene_objects, actors/scene_object_controller (both SceneObjects_*) |
| `objects/actor_group/` (20) | field/actor_group, other/callback_pool |
| `scene/actor/{registry,behavior,control,reaction,attachment,placement,animation,actor_pool}/` | 27 scene_actor_* and pooled actor units, by part |
| `effects/particles/groups/` (36) | particle_groups, fragment_groups, fading_particle_groups, animated_particles (contiguous managers 08013F98-080164B8) |
| `effects/projectiles/projectile/` (42) | task_4_e01, projectile, timed_effect |
| `effects/healing/` (35) | health_restore_effect, health_burst_effect |
| `effects/shadows/` (3) | scaled_shadows, shadow_registry, field/terrain_shadows |
| `effects/screen/presentation/` (14) | scripted_presentation, timed_presentation |
| `effects/particles/native_effect/` (3) | field/native_effect, field/scene_debris |
| `engine/sound/` (41) | music_player_slots, scene_audio_events |
| `system/solar/` (39) | solar, cartridge_solar_sensor, other/solar_calibration |
| `system/clock/` (53) | clock_and_sun_schedule, time (region menu), time_palette_transitions, engine/clock_events |
| `system/save/` (92) | save, save_data |
| `system/game_state/` (48) | game_state, effects/pal_fade |
| `ui/town/shop/` (120) | the three inventory_* SceneInventory units |
| `ui/books/monster_book/` (41) | monster_book, bestiary, effects/palette_transition |
| `ui/books/collection/` (9) | collection, collection_menu |
| `ui/modifier_menu/` (39) | the three modifier menu units |
| `ui/pause/map/` (33) | map_screen, map_menu |
| `ui/dialogue/message_window/` (9) | dialogue_controls_and_animation, dialogue_task_setup |
| `ui/widgets/input_grid/` (25) | input_grid_and_navigation, input_marker_widgets |
| `ui/status_screen/` (4) | status_screen, numeric_overlay |
| `link/session/` (62), `link/link_panel/` (21), `link/lobby/` (22), `link/records/` (13) | link_session + registry_1745; link_panel + registry_f246; registry_33_ff + record_menu; link_record_panel + record_panel |
| `link/wireless/battle/` (54) | wireless_game_items_and_display, wireless_scene_objects, actors/scene_pooled_motion_and_actor_controls |
| `minigames/result_game/` (43) | transformation_and_result_game, result_game_tutorial |
| `player/damage/`, `player/cues/`, `player/script/` | loose Player files + player_contact; + player_cue; + actors/script_actor |

Pairs the review named: `collection`/`collection_menu` merged;
`record_menu`/`record_panel` both moved to link (lobby and records, two
objects); `countdown_controller`/`countdown_timer` stay two files side by side in
`ui/hud/` (two objects, one file each); `vanargand_projectile` merged into
Vanargand; the solar trio merged; the script interpreter is one directory. The
dialogue text interpreter stays in `ui/dialogue/`: it interprets the `<...>`
markup inside message text, not GCL, and sits with the message window that
embeds it.

## Headers

Rule: a header goes to `include/` + the first two levels of its owner's new
source directory + its own name. `global.h`, `debug.h` at the root. When a
single-file unit is renamed, its header follows (`group_objects.h` →
`bat_flock.h`). 394 headers follow their unit; 2 are root headers; the rest
(183, all from `include/engine/shared/` and the five loose `include/player/`
headers) were placed by owner or by who includes them:

* **Owned by an object or subsystem** (80): beside it even when other
  categories include it, e.g. `scene_pool.h` → `link/wireless/`, `state_actor.h`
  → `npc/story_character/`, `scene_panel_*.h` → `objects/pile_driver/`,
  `scene_inventory_*.h` → `ui/town/`, `scene_input*.h` → `ui/widgets/`,
  `area_effect_link.h` → `scene/area_effect/`, `directional_force.h` →
  `objects/hazards/`, `player_3.h` → `player/appearance/player_visuals.h`, the
  13 `display_*`/`palette_*`/`render`/`sprite_drawable_api` headers →
  `engine/video/`, vectors, interpolation and random helpers → `engine/math/`.
* **Included from one place** (17): with that directory (e.g. armor palette
  headers → `monsters/armor/`).
* **Shared within one category** (14): at the category root, e.g. the monster
  init and chase helpers in `include/monsters/`, `effect_group.h` in
  `include/effects/`, `state_actor_action_entry.h` in `include/bosses/`.
* **Shared across categories, no single owner** (72; the exceptions):
  `include/scene/actor/` (59: the scene-actor framework's types, contact
  actor/collider/position, chase, control, terrain reaction...),
  `include/engine/` (10: engine, engine_globals, engine_pause(_flags), heap,
  input, ram, runtime, task, task_manager; word_flags is owned there too),
  `include/player/` (player.h, player_slot_status.h: the Player struct, owned
  by the player), `include/scene/directional_script_args.h`.
  `include/scene/actor/` ends with 87 headers in all (the 59 plus the framework
  units' own). It is large but is one framework's API, not a catch-all;
  splitting it by part would need include rewrites inside the framework's own
  headers that cross parts.

Placeholder header names renamed: registry_actor(_collision).h →
script_puppet(_collision).h, slot7_*.h → layered_boss_*.h, task4e01_projectile.h →
projectile_task.h, registry_e7bc.h → collision_manager.h, registry_choice_menu.h →
town_map.h, registry_selection_menu.h → world_map.h,
motion_0818.h → spinning_approach_chain.h.

## Counts

| | Before | After |
|---|---|---|
| Top-level categories | 13 | 14 |
| Directories containing C files | 449 | 234 |
| Loose files in a category with no structure | 379 (player) | 0 |
| Largest directory | player (379 loose) | bosses/vanargand (287) |
| Deepest | 2 levels | 3 levels (95 directories) |
| `include/engine/shared` | 180 headers | gone |

Per category, directories before → after (files after):
actors 87 → 0; bosses 6 → 6 (886); effects 92 → 48 (730); engine 44 → 6 (185);
field 47 → 0; objects 0 → 26 (516); lib 7 → 7 (61); link 25 → 19 (551);
menus 80 → 0; ui 0 → 36 (871); minigames 2 → 2 (94); monsters 38 → 23 (430);
npc 0 → 6 (241); other 4 → 0; player 9 → 18 (476); scene 0 → 22 (653);
script 0 → 2 (32); system 8 → 5 (315).

Largest after: vanargand 287, hresvelgr 237, ratatosk 169, ui/town/shop 120,
monsters/armor 111, story_character 109, pile_driver 107,
link/wireless/connection 104, system/save 92.

## Least sure

1. Whether the seven units around **Sabata** (scene volumes, impact, position,
   state, decision) also contain shared boss helpers.
2. **Player gun vs. attack split**, based on function names (projectile, charge,
   burst → gun; slash, spin, combo → attack). Some "burst"/"spiral" actions may
   be melee specials.

The shipped entry point for Player link action dispatchers remains unobserved.
`system/inventory/` holds the item data (63 functions at 0822CCC8) and 14
pause-inventory helpers from 0816CD6C that share the unit directory; a later
pass could move those 14 to `ui/pause/inventory/`.
