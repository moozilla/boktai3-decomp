# Source naming debt before the filename pass

Tags: layout naming ownership

The inventory uses canonical function symbols and their C definition paths. It
compares each prefix with the directory owner, accounting for shared Player and
pause families, qualified directory names, and the Bestiary name for Monster Book.
For each prefix it counts functions with a reference from a different source
directory. It skips comments, literals, and ordinary C prototypes. C identifiers
used as callbacks count as references. Script dispatch, assembly, and indirect
calls still need review before choosing an owner. A cross-directory reference can
also be a normal call into the owner’s public interface.

Regenerate the detailed inventory with:

```sh
build/venv/bin/python tools/naming_debt.py
build/venv/bin/python tools/naming_debt.py --directory bosses/hresvelgr
```

## Reviewed directories

The review column records the original counts. The current scan counts every
family flagged in each directory, including smaller prefixes. The last column
gives the next action for the reviewed families and highlights shared code.

| Directory | Review | Current | Referenced elsewhere | Next action |
| --- | ---: | ---: | ---: | --- |
| `bosses/hresvelgr` | 70 | 70 | 5 | Rename the private OffsetActor routines after tracing the attached actor. Move the cross-owner vector length, direction burst, damage display, and debug helpers once their consumers and interfaces are confirmed. Check the other Math routines for shared primitives before giving them boss names. |
| `bosses/vanargand` | 60 | 77 | 0 | Rename the PairedSceneActor routines for their Vanargand roles. Their C references stay in the fight. The HresvelgrChild label needs a separate identity check before it can be named confidently. |
| `bosses/ratatosk` | 57 | 56 | 3 | Rename the local SceneActor callbacks after separating animation, proximity, and state roles. Move the shared distance, particle, and slot-motion helpers once their orbit, projectile, and Player consumers are confirmed. |
| `bosses/count` | 52 | 136 | 0 | Rename the local SceneActors and SceneActorAnimation callbacks for their Count roles after checking the phase tables. The measured C references for these groups stay with Count. |
| `link/wireless/battle` | 48 | 102 | 13 | Rename the ScenePool actor-slot and spawn routines for wireless battle. They have local C references. Keep the exported battle endpoints with their owner and verify which WirelessGameItems operations support packet exchange or trade independently before moving them. |
| `npc/story_character` | 47 | 138 | 0 | Rename the SceneCharacter lifecycle and animation routines for story characters. Check their script entry points and the MessageActor group separately before choosing one prefix for the whole directory. |
| `system/save` | 41 | 41 | 16 | Move shared Region, Script, Clock, Palette, and screen-transition services with the owning subsystems after checking their data dependencies. Keep save lifecycle and serialization with Save. Check private state and setup routines before assigning their owners. |
| `ui/books/monster_book` | 36 | 26 | 0 | Rename the local PaletteTransition routines for the Monster Book display after confirming their animation variants. Their C references stay within the book. |
| `objects/warp_circle` | 34 | 0 | 0 | Complete. The 34 routines now use WarpCircle and the current scan finds zero mismatched prefixes here. |
| `ui/pause/display` | 34 | 48 | 12 | Rename the SceneHud routines for pause display. Outside C references come from other pause-menu directories, which use this display interface. The measured references support keeping it with pause display. |

## Counts by directory

The worker snapshot has 5,270 flagged functions across 190 source directories.
Of those, 1,089 have C references from another directory.
Directories missing from this table had no flagged prefixes.

| Directory | Functions | Referenced elsewhere |
| --- | ---: | ---: |
| `ui/title` | 199 | 0 |
| `engine/video` | 167 | 93 |
| `effects/sprites` | 149 | 22 |
| `npc/story_character` | 138 | 0 |
| `bosses/count` | 136 | 0 |
| `objects/coffin` | 135 | 25 |
| `link/wireless/battle` | 102 | 13 |
| `link/wireless/connection` | 100 | 18 |
| `ui/hud/display` | 97 | 51 |
| `effects/particles` | 92 | 10 |
| `monsters/armor` | 84 | 0 |
| `script` | 84 | 37 |
| `ui/pause` | 80 | 40 |
| `monsters` | 79 | 1 |
| `bosses/vanargand` | 77 | 0 |
| `npc/manager` | 77 | 23 |
| `player/cutscene` | 77 | 0 |
| `effects/screen` | 72 | 0 |
| `bosses/hresvelgr` | 70 | 5 |
| `ui/widgets` | 70 | 5 |
| `scene/actor/control` | 69 | 21 |
| `scene/actor/behavior` | 67 | 22 |
| `scene/actor/actor_pool` | 64 | 0 |
| `scene/actor/reaction` | 63 | 24 |
| `scene/collision` | 63 | 25 |
| `scene/actor/registry` | 62 | 21 |
| `objects/moving_platforms` | 57 | 2 |
| `ui/hud` | 57 | 10 |
| `ui/results` | 57 | 1 |
| `bosses/ratatosk` | 56 | 3 |
| `lib/rfu` | 55 | 0 |
| `minigames/result_game` | 53 | 0 |
| `effects/shadows` | 51 | 13 |
| `player/appearance` | 51 | 38 |
| `ui/dialogue` | 50 | 0 |
| `ui/pause/display` | 48 | 12 |
| `objects/hazards` | 46 | 3 |
| `system/clock` | 46 | 11 |
| `scene/actor/placement` | 44 | 39 |
| `monsters/ivy` | 43 | 6 |
| `system/save` | 41 | 16 |
| `bosses/nidhoggr` | 40 | 0 |
| `objects` | 40 | 0 |
| `monsters/skeleton` | 38 | 2 |
| `ui/town` | 38 | 3 |
| `scene/areas` | 37 | 6 |
| `effects` | 36 | 8 |
| `ui/dialogue/text_interpreter` | 34 | 17 |
| `effects/orbits` | 33 | 9 |
| `effects/particles/trap_and_spell_effects` | 33 | 7 |
| `effects/projectiles` | 33 | 5 |
| `bosses/sabata` | 32 | 0 |
| `link/records` | 32 | 5 |
| `monsters/axe` | 32 | 0 |
| `effects/bursts` | 30 | 5 |
| `objects/items` | 30 | 2 |
| `engine/math` | 29 | 4 |
| `effects/healing` | 28 | 2 |
| `ui/pause/menu` | 28 | 17 |
| `effects/particles/hit_effects` | 26 | 8 |
| `monsters/vorn` | 26 | 3 |
| `objects/deck_cannons` | 26 | 0 |
| `ui/books/monster_book` | 26 | 0 |
| `ui/pause/menu_sequences` | 26 | 12 |
| `ui/widgets/selection_panel` | 26 | 2 |
| `objects/loot_deposits` | 25 | 1 |
| `ui/widgets/input_grid` | 25 | 22 |
| `scene/actor` | 24 | 13 |
| `scene/collision/body` | 24 | 13 |
| `effects/palette` | 23 | 4 |
| `effects/scene` | 23 | 4 |
| `objects/solar_roots` | 23 | 2 |
| `scene/terrain/path` | 23 | 17 |
| `scene/actor/attachment` | 22 | 10 |
| `ui/town/shop` | 22 | 2 |
| `npc` | 21 | 0 |
| `scene/actor/animation` | 21 | 11 |
| `scene/actor/steering_actor` | 21 | 4 |
| `link/wireless/trade_menu` | 20 | 0 |
| `minigames/coffin_bike` | 20 | 9 |
| `ui/modifier_menu` | 20 | 0 |
| `effects/particles/impact_fragments` | 19 | 4 |
| `script/runtime` | 19 | 2 |
| `effects/projectiles/projectile` | 18 | 2 |
| `effects/projectiles/projectile_pool` | 18 | 10 |
| `scene/terrain` | 18 | 9 |
| `ui/hud/damage_numbers` | 18 | 1 |
| `ui/save` | 18 | 4 |
| `effects/particles/enemy_attacks` | 17 | 7 |
| `engine/video/palette` | 17 | 13 |
| `ui/results/stage_results` | 17 | 5 |
| `effects/orbits/screen_orbit_effect` | 16 | 5 |
| `engine/sound` | 16 | 1 |
| `engine/video/anim` | 16 | 12 |
| `scene/collision/stimulus` | 16 | 6 |
| `ui/books/forging_book` | 16 | 0 |
| `effects/particles/impact_bursts` | 15 | 6 |
| `effects/particles/spark_explosions` | 15 | 2 |
| `effects/sun_discs` | 15 | 2 |
| `lib/m4a` | 15 | 1 |
| `link/player` | 15 | 0 |
| `objects/bat_curtain` | 15 | 0 |
| `objects/bouncing_bombs` | 15 | 3 |
| `objects/dungeon_prop` | 15 | 7 |
| `link/session` | 14 | 6 |
| `objects/passage_barriers` | 14 | 0 |
| `player/effects/spiral_trail` | 14 | 2 |
| `system/inventory` | 14 | 6 |
| `ui/pause/inventory` | 14 | 4 |
| `link/wireless/packet_exchange` | 13 | 11 |
| `npc/sabata_rider` | 13 | 0 |
| `objects/solar_stand` | 13 | 0 |
| `ui/game_over` | 13 | 4 |
| `effects/bursts/linked_effect` | 12 | 2 |
| `effects/feathers` | 12 | 2 |
| `effects/scene/actor_effects` | 12 | 4 |
| `engine/video/sprite_component` | 12 | 5 |
| `monsters/centipede` | 12 | 0 |
| `monsters/mummy` | 12 | 0 |
| `system/game_state` | 12 | 6 |
| `ui/pause/map` | 12 | 0 |
| `effects/particles/tether_rings` | 11 | 0 |
| `effects/sprites/sprite_effect` | 10 | 1 |
| `link/encoded_records` | 9 | 4 |
| `link/lobby` | 9 | 0 |
| `link/selection` | 9 | 0 |
| `link/serial` | 9 | 6 |
| `monsters/golem` | 9 | 0 |
| `player/effects/burst` | 9 | 5 |
| `scene/actor/route_sprite_actor` | 9 | 1 |
| `ui/pause/status` | 9 | 3 |
| `effects/projectiles/timed_sprite` | 8 | 2 |
| `objects/quest_barriers` | 8 | 0 |
| `player/core` | 8 | 6 |
| `ui/status_screen` | 8 | 0 |
| `effects/particles/spark_particles` | 7 | 2 |
| `effects/sprites/timed_character_effects` | 7 | 1 |
| `engine` | 7 | 3 |
| `link/handshake` | 7 | 1 |
| `link/panel` | 7 | 4 |
| `monsters/sword` | 7 | 5 |
| `objects/hazards/falling_object_effects` | 7 | 1 |
| `player/effects/recoil_puffs` | 7 | 1 |
| `system/solar` | 7 | 7 |
| `link/menu` | 6 | 2 |
| `link/wireless/rfu_manager` | 6 | 5 |
| `objects/hazards/timed_hazard` | 6 | 0 |
| `scene/terrain/grid` | 6 | 0 |
| `effects/bursts/radial_burst` | 5 | 2 |
| `effects/screen/presentation` | 5 | 0 |
| `link/message_panel` | 5 | 5 |
| `link/wireless/result_panel` | 5 | 5 |
| `monsters/pollen_flowers` | 5 | 0 |
| `monsters/popbud` | 5 | 0 |
| `objects/markers` | 5 | 0 |
| `objects/sun_orb` | 5 | 5 |
| `scene/lifecycle` | 5 | 5 |
| `ui/town/solar_bank` | 5 | 0 |
| `ui/town/storage` | 5 | 0 |
| `effects/directional_effect` | 4 | 2 |
| `effects/scene/collider_effect` | 4 | 0 |
| `objects/items/collectible` | 4 | 0 |
| `objects/items/sprite_drop` | 4 | 0 |
| `effects/bursts/fragment_burst` | 3 | 0 |
| `effects/bursts/fragment_effect` | 3 | 0 |
| `effects/projectiles/sprite_projectile` | 3 | 0 |
| `link/score_panel` | 3 | 2 |
| `scene/camera` | 3 | 2 |
| `ui/pause/menu_transition` | 3 | 0 |
| `effects/orbits/orbit_effect` | 2 | 2 |
| `monsters/cannons/wind` | 2 | 0 |
| `player/effects/action_flash` | 2 | 0 |
| `player/effects/particle_cloud` | 2 | 0 |
| `player/motions` | 2 | 0 |
| `scene/room_queries` | 2 | 1 |
| `ui/dialogue/scrolling_dialogue` | 2 | 0 |
| `effects/bursts/charged_burst` | 1 | 0 |
| `monsters/ooze` | 1 | 0 |
| `monsters/screamer` | 1 | 0 |
| `npc/otenko` | 1 | 0 |
| `objects/floor_switch` | 1 | 0 |
| `objects/solar_bomb` | 1 | 0 |
| `objects/wooden_crate` | 1 | 0 |
| `player/attack` | 1 | 1 |
| `player/control` | 1 | 1 |
| `player/movement` | 1 | 1 |
| `player/pile_trap` | 1 | 0 |
| `player/target_registry` | 1 | 1 |
| `ui` | 1 | 1 |
| `ui/dialogue/text_window` | 1 | 0 |
