# Source layout pass 3: decisions

Tags: #tooling #process

These are the reviewed pass 3 design decisions. Active source and header
placements live in `symbols/source_dirs.csv`, `symbols/source_files.csv`,
and `symbols/header_dirs.csv`. The Player range rules and exact file
placements preserve the existing translation units.

## 1. Player regrouped by ROM range

A directory has to become one unit file per contiguous range, so the Player
was cut along its address order, not by verb. Cuts are at the points where
the code changes job. The identity worker's traces placed the weapon paths
(melee contact path for the five swords; projectile path for the Gun Del Sol
frames, Tempest and Rockbuster), the special motion effects, paralysis and the
departure cue. Each run is named for what most of it does:

| Range | Directory | What |
|---|---|---|
| 0811C5F8–0811D4B0, 0813C798–0813FC38 | `player/appearance` | character model and stat config; visual palette and sprite resources |
| 0813ADC4–0813C040, 0813FC38–08140410, 0815E034–0815F5B4 | `player/core` | facing and animation; action-mode init and permission tests; contact responses, `Player_Update`, `Player_Create`/`Destroy` |
| 08140410–08142194 | `player/movement` | terrain clamps, turning, forces, camera offsets, speeds |
| 08142194–08144310 | `player/interaction` | pushing, held interactions, coffin-chain targets and movement |
| 08144310–0814A4B0 | `player/motions` | the motion callbacks: one update/exit pair per motion ID (Spiral Burst cast, paralysis, departure, death, form changes, recoils, poses) |
| 0814A4B0–0814DE0C | `player/attack` | melee path: attack power and elements, target facing, grab, action variants, contact colliders, combos |
| 0814DE0C–0814F804 | `player/gun` | Gun Del Sol path: frame selection, projectiles, charge tiers, Tempest, Rockbuster |
| 0814F804–08156BBC, 0815A400–0815E034 | `player/cutscene` | cue states, cue commands and dispatch, script commands that drive Django |
| 08156BBC–08158FC0 | `player/control` | input direction and history, action sequences, interaction-mode movement |
| 08158FC0–08159978 | `player/stats` | equipment values, life, energy, resistance, modifiers |
| 08159978–0815A400 | `player/state` | scripted position requests, interpolated moves, forced facing, stun, flags |
| 0815F5B4–0815F7F4 | `player/target_registry` | lock-on targets |
| unchanged | `player/effects/*`, `player/clone.c` | already one range each |
| 08237EC8–082389D8, 08239130–08243C10 | `link/player` | the link-play Django (below) |

Result: 165 runs over 393 files become 22 ranges in 18 Player directories, plus
2 in `link/player`. Every directory is one range except core (3), appearance,
cutscene and link/player (2 each).

**The tail block is a second Django, and it goes to `link/player`.** Functions
08237EC8–08243C10 surround the existing `link/link_player` unit
(08239130–0823AE5C, `LinkPlayer_*` stats, abilities and session data). They
have their own copies of movement, terrain recovery and camera clamps
(`Player_ClampCameraOffsets_0823B3C4`, `Player_BeginTerrainRecovery_0823BA78`).
Their constructor `sub_08243B0C` creates a 0xCAC task with
`Player_UpdateLinkCore`, and the identity worker traced five of them to the
Link Player dispatch tables (08E87A10, 08E87A90). The only gap in the block is
`effects/directional_effect` (082389D8–08239130), which stays its own object.
This supersedes the corrections' `player/special_actions`, `player/link_actions`,
`player/cues`, `player/damage` and `player/script` groups. Their files are in
the range directories above, and their identities are kept in the notes of
`player_moves.csv`.

## 2. Mechanism names

The identity tables settle none of the listed directories yet, so most stay
as they are and are recorded in `needs_evidence.csv` with the script evidence
found here (creating procedures, scenes, debug prints). Changed this pass:

* `objects/orbit_motion.c` + `objects/scene_script_object.c` →
  `objects/dungeon_prop/`. They are one ROM unit (081B817C) whose constructor is
  `DungeonProp_CreateFromScript` (key 3D91, proc 1092, called by 94 dungeon
  procedures). It becomes a two-file directory, an exception like the coffin's.
* `effects/scene/effect_part` → `effects/scene/actor_effects` (EffectPart and
  EffectGroup parts spawned on actors through `SceneActorEffects_*`).
* `effects/scene/bat_flock.c` → `effects/bat_flock.c`: a specific effect, out
  of the generic group.
* The `effects/particles/` systems that serve many unrelated callers
  (`groups`, `spark_particles`, `scene_particle_pool`, `scripted_particle`,
  `native_effect`, `burst_emitter`) keep mechanism names: they are particle
  systems, and that is what a contributor looks for. The ones with a single
  likely owner (`particle_burst`, `particle_effect`, `sprite_particle`,
  `proximity_emitter`) and `monsters/enemy_particle` are needs-evidence
  candidates for folding.

## 3. Redundant prefixes dropped

`link/link_menu` → `link/menu`, `link_panel` → `panel`, `link_selection` →
`selection`, `link_player` → `player`; `link/wireless/wireless_exit.c` →
`exit.c`, `wireless_text.c` → `character_table.c`; `ui/pause/pause_display` →
`display`, `pause_menu` → `menu`; `objects/markers/interaction_marker` →
`interaction`, `saved_position_marker` → `saved_position`. `ui/hud/hud_sequence`
no longer exists (corrections moved it to `ui/pause/menu_sequences`).

## 4. Overlaps

* **`scene/state` + `scene/lifecycle` vs `system/game_state`**: `system/game_state`
  is what is saved (story flags, records, Solar Fruit). `scene/state` holds
  per-scene runtime counters and spatial filters that reset with the scene, so
  it merges into `scene/lifecycle` (30 files, two ranges). Both stay out of `system`.
* **`effects/scene` vs `scene/area_effect`**: `scene/area_effect` is rectangular
  gameplay areas that change terrain values and resource gain, not a visual,
  so it becomes `scene/areas` (function prefix `AreaEffect_` stays). `effects/scene`
  is the world-placed effect machinery (schedules, queues, repeated effects,
  collider effects, actor effect parts).
* **`ui/pause/catalog.c`**: the corrections made it `status_options.c`. It joins
  the Status page in `ui/pause/status/` (was `status_panel`).
* **`include/scene/actor` (87 headers)**: the two-directory include limit rules
  out `include/scene/actor/<part>/`, so the framework's headers are grouped by
  topic one level up. `include/scene/actor/` keeps 51 (registry, state,
  components, init, scripts, attachment). `include/scene/collision/` gains 15
  (contacts, collision responses, stimulus, damage visibility), 21 in all.
  `include/scene/motion/` is new with 21 (chase, routes, turning, motion,
  displacement, terrain movement, steering and route-sprite actors). The 51
  are one framework's core API.

## 5. Names that will become file names

`prefix_renames.csv` has 88 rules (scoped by directory where a prefix spans
objects) giving 1,091 renames. None collides with an untouched function, and
no `Registry[hex]`, `Task4E01`, `Pool[hex]`, `SingletonSlot`, `BossSlot`,
`Enemy[hex]`, `StateActor`, `ScenePanel` or `SceneInventory` prefix remains.
The main decisions:

* `ScenePanel_` is not all Pile Driver. 99 functions in `objects/pile_driver` →
  `PileDriver_` (+ `PileDriverRig_` 33, `PileDriverSubject_` 17). 14 inside
  Vanargand's block → `VanargandPair_`. 7 in the pause menu → `StatusPanel_`,
  `MenuPrompt_`, `MenuSequence_`, `PauseDisplay_`. One is the ship wheel's
  `ShipWheel_SlideOpen`. The six `ScenePanelEffect_` in Nidhoggr's block →
  `NidhoggrEffect_`.
* `SceneInventory_` → `Shop_` (97). It is the shop screen in general: five
  presentations, of which 0 is the Weapon Shop (武器屋). The other four shops
  are needs-evidence for their names, not for the prefix. The 11 in Storage →
  `Storage_`.
* `SingletonSlot7_` → `VanargandHead_` (40). The worker saw his head in the bike
  chase and the ending. `Vanargand_` stays for the arena body.
* `BossSlot2_` → `Sabata_` (19), and the six helper prefixes in his block
  (`ActorImpact`, `ActorPosition`, `ActorState`, `ActorDecision`, `SceneVolume`,
  `SceneEventActor`) → `Sabata*_`.
* **Armor.** The coordinator's note maps `EnemyD459_` to Armor, but `Armor_`
  already names the E043 アーマー family. Its defeat table 0824F004 unlocks Book
  entries 52–64: Bronze, Iron, Flame, Frost, Cloud, Earth and Dual Armor, and
  the Black Knight. D459 is Book #29 ARMOR (甲冑), a stationary sprite that
  fires, beside STATUE #28. They are two objects, so D459 →
  `monsters/suit_of_armor/` (its own range 08204748–08205944) with prefix
  `SuitOfArmor_`. `EnemyF426_` → `Statue_`.
* `StateActor_` → `StoryCharacter_` (51), after its directory. The kinds are
  needs-evidence.
* Identified objects lose their mechanism prefixes:
  * Coffin Bike `Glove_`, `ActorRecoil_`
  * Centipede `TentacleActor_`, `TentacleEnemy_`, `TentacleBody_`. The terrain
    overlay has its own constructor `CentipedeOverlay_Create` (key 0AD8).
  * Popbud, Plasma Cannon, Garmr Statue, Wind Cannon (and manager), Pollen
    Flower, Deck Cannon, Solar Root, Muspell Head
  * Count `Vampire_`; his bats `ContactSwarm_` → `CountBats_`
  * Nidhoggr's duel `GridAttack*_` → `NidhoggrDuel*_`
  * Dark Generator `ControlPanel_`; Bat Flock `GroupObjects_`/`BoardPiece*_`
  * Town Map, World Map, Magic Cannon Departure, Sabata's rider, Status Options,
    Menu Sequence
  * link `Registry*_` → `LinkSession_`, `LinkPanelSlots_` (`LinkPanel_Close`
    exists), `LinkLobbyPanel_`, `RecordPanel_`, `SceneMessageSprite_` (key 1909's
    own name)
* Unidentified hex prefixes get descriptive names; the object behind each is
  needs-evidence: `RegistryC281_` → `MoveSequence_`, `RegistryF3E4_` →
  `PopInSprite_`, `RegistryE7BC_` → `ScriptObjectPool_`, `Registry3C8D_` →
  `SavedVariables_`, `Pool*_` → `<owner>Pool_`, `Params3D0_` →
  `CutsceneDisplay_`, `SceneActor14_` → `SpriteEffectSet_`.
* Directory follow-up: `bosses/vampire` → `bosses/count`.

Headers follow (`headers_delta.csv`):
* `scene_panel_*.h` → `pile_driver_*.h`
* `layered_boss_*.h` → `vanargand_head_*.h`
* `scene_inventory_*.h` → `shop_*.h`
* the Nidhoggr duel headers → `nidhoggr_duel*.h`
* `scene_input_widget.h` → `include/engine/video/anim_sprite.h`

**Type**: `struct AnimSprite` is the engine's animated sprite. It holds
sequence and keyframe playback, position, scale, rotation,
palette bank and tiles, with an animation-list link. `Anim_*` and
`AnimData_*` operate on it; it appears in 219 files and twice inside
`struct Player`. `anim_sprite.h` also defines `AnimSpriteResource`,
`AnimSpritePlayback`, and the `AnimSprite_Hide` and `AnimSprite_Show` helpers.

## needs-evidence (left alone this pass)

`needs_evidence.csv` lists 20 paths:
* `objects/{scene_objects,actor_group,named_object,cell_gate,bouncing_object,breakable_prop}`
* loose `objects/{region_actor,collision_controller,directional_controller,tile_trigger,breakable}.c`
* all four NPC directories
* four single-owner particle systems and `monsters/enemy_particle`

The strongest lead is `objects/scene_objects`. Its creator runs in 64 dungeon
procedures whose prints place chests and moving floors, which points to
`gimmicks`, but a running scene should confirm it first.

## Counts

| | Baseline (pass 2 + corrections) | Pass 3 |
|---|---|---|
| Directories with `.c` files | 234 | 234 |
| Player directories / ROM runs | 13 / 165 | 18 / 22 (+ `link/player` 2) |
| Directories made of one ROM range | — | 174 of 233 that hold functions, counted over the whole ROM; the multi-range ones are mostly group directories of separate small objects (`effects/particles` 9, `effects/palette` 7) |
| Largest directory | `bosses/vanargand` 287 | same; then `bosses/hresvelgr` 237, `link/player` 187 |
| Placeholder function prefixes (hex, slot, registry, StateActor, ScenePanel, SceneInventory) | 37 | 0 |
| `include/scene/actor` | 87 | 51 (+ `collision` 21, `motion` 21) |

## Least sure

1. `link/player` for the 0823B264 block: the LinkPlayer names, the separate
   0xCAC task and five traced dispatch entries agree, but nothing was seen
   running it (`sub_08243B0C` has no ROM reference).
2. `player/motions` versus finer splits: one range, but it mixes damage
   reactions, form cues and special casts.
3. `SuitOfArmor_` for D459 instead of the coordinator's "Armor".
4. `StoryCharacter_`, `MoveSequence_`, `PopInSprite_`, `ScriptObjectPool_`,
   `SavedVariables_`: descriptive, pending identities.
5. Folding `scene/state` into `scene/lifecycle`.
