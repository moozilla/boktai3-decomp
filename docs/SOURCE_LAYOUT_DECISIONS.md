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
(`LinkPlayer_ClampCameraOffsets`, `LinkPlayer_BeginTerrainRecovery`).
Their constructor `sub_08243B0C` creates a 0xCAC task with
`Player_UpdateLinkCore`, and the identity worker traced five of them to the
Link Player dispatch tables (08E87A10, 08E87A90). The only gap in the block is
`effects/directional_effect` (082389D8–08239130), which stays its own object.
This supersedes the corrections' `player/special_actions`, `player/link_actions`,
`player/cues`, `player/damage` and `player/script` groups. Their files are in
the range directories above, and their identities are kept in the notes of
`player_moves.csv`.

## 2. Mechanism names

Pass 3 kept provisional mechanism names where it had no runtime evidence.
Pass 4's runtime identifications below resolve those placements. Pass 3 moved
these units based on their known roles:

* `objects/orbit_motion.c` + `objects/scene_script_object.c` →
  `objects/dungeon_prop/`. They are one ROM unit (081B817C) whose constructor is
  `DungeonProp_CreateFromScript` (key 3D91, proc 1092, called by 94 dungeon
  procedures). It becomes a two-file directory, an exception like the coffin's.
* `effects/scene/effect_part` → `effects/scene/actor_effects` (EffectPart and
  EffectGroup parts spawned on actors through `SceneActorEffects_*`).
* `effects/scene/bat_flock.c` → `effects/bat_flock.c`: a specific effect, out
  of the generic group.
* Shared effects stay under `effects/`. Runtime evidence now distinguishes
  impact fragments, hit effects, projectile motes, spell and trap dust,
  tether rings, and enemy attack particles. Owner-specific effects follow
  Player, Vanargand, and falling hazards.

## 3. Redundant prefixes dropped

`link/link_menu` → `link/menu`, `link_panel` → `panel`, `link_selection` →
`selection`, `link_player` → `player`; `link/wireless/wireless_exit.c` →
`exit.c`, `wireless_text.c` → `character_table.c`; `ui/pause/pause_display` →
`display`, `pause_menu` → `menu`; `objects/markers/interaction_marker` →
`interaction`, `saved_position_marker` → `saved_position`. `ui/hud/hud_sequence`
no longer exists (corrections moved it to `ui/pause/menu_sequences`).

## 4. Overlaps

* **Former `scene/state`** splits by role. The Pile Driver increments persistent
  counters through `SceneState_IncrementCounters` (0806F218). The new-game script
  calls 0806F2CC to reset persistent challenge flags and records. Both files
  belong in `system/game_state`. The startup reset belongs in `scene/lifecycle`,
  room actor-range queries in `scene/room_queries`, and wrapped background cell
  addressing and fills in `engine/video`.
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
* `SceneInventory_` → `Shop_` (97); the 11 in Storage → `Storage_`. The five
  constructor variants are the Weapon, Item and Accessory shops, the
  traveling merchant and GariGari-kun's stall.
* `SingletonSlot7_` → `VanargandHead_` (40). The worker saw his head in the bike
  chase and the ending. `Vanargand_` stays for the arena body.
* `BossSlot2_` → `Sabata_` (19), and the six helper prefixes in his block
  (`ActorImpact`, `ActorPosition`, `ActorState`, `ActorDecision`, `SceneVolume`,
  `SceneEventActor`) → `Sabata*_`.
* **Armor.** The coordinator's note maps `EnemyD459_` to Armor, but `Armor_`
  already names the E043 アーマー family. Its defeat table 0824F004 unlocks Book
  entries 52–64: Bronze, Iron, Flame, Frost, Cloud, Earth and Dual Armor, and
  the Black Knight. D459 is Book #29 ARMOR (甲冑), a stationary sprite that
  fires an axe shockwave described by game text #2538 and observed in replay.
  D459 uses `monsters/axe_armor/` (08204748–08205944) and the prefix
  `AxeArmor_`. STATUE #28 uses the prefix `Statue_`.
* `StateActor_` → `StoryCharacter_` (51), after its directory. The runtime
  identifications below document the kinds seen in their original scenes.
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

## Pass 4 runtime identifications

| Earlier name | Observed owner and placement |
|---|---|
| `scene_objects`, scene object controller | D9AE creates solid puzzle blocks in scene 3955 (`objects/puzzle_blocks.c`). The separate 6D69 controller creates sliding doors in scene 5297 (`objects/sliding_doors/`). |
| `actor_group`, `named_object`, `cell_gate` | F1F9 creates breakable loot deposits in scene 3905. A4FC moves the scene 4808 route platforms. 71BA opens the passage at the Wind Cannon row. |
| `bouncing_object`, `breakable_prop` | The same bomb pool runs in the Nidhoggr duel and in Mummy and Ratatosk attacks. 3DB0 shatters quest barriers and invokes the Trinity release script. |
| `region_actor`, `collision_controller`, `directional_controller` | 377D creates ice boulders. 0866 creates strike-triggered traps. 883A dispatches directional interaction prompts. |
| `tile_trigger`, `breakable` | 2FA5 reacts to floor-trap contact. 2E1C candlesticks, braziers and light fixtures use weapon reactions and scripts. |
| Scripted NPC, tagged NPC, title actor | BB34 drives Otenko's field dialogue. D774 loads Kuro's cat model. The Options tutorial and title replay use the same Otenko controller. |
| Story-character kinds | The shared body runs Trinity (kinds 27 and 28), Sabata's projection (29), Smith (15), Violet (19), the Coffin Shopkeeper (22), Coach (24), Elefan (43), Carmilla (9), the fleeing resident (17), the Girl with Braids (20), and her father (18). |
| Particle systems | Player motion 13 runs the spiral trail. Original recoil callbacks emit the rising puffs. The special-action cue emits action flashes. The Vanargand chase uses its own sprite particle manager. The F156 rings respond to tether proximity. Enemy attack particles are shared by Wind Cannon, Poison Hand and Screamer. |

The Player range boundaries remain the Pass 3 boundaries. Two Pile Trap
actions inside the motions range use `PileTrap_` symbols. The scenery
files use their confirmed names under `objects/dungeon_prop/`, and the
Status page stays in `ui/pause/status/`.

## Pass 5 identity corrections

The title emblem runs five units at ROM addresses 081EAB64 through
081EB7DC, including its constructor. They and
`text_entry_widget.h` belong to `ui/title/`.
The Bat Curtain owns its debris, and the Strike Crystal has its own file
under `objects/`. Sabata’s White Forest encounter drives the frost emitter.
The actor palette fade runs during Repair and belongs with palette effects.

Identity correction row 27 traces the flying sprite to `PlayerCue_Shoot` in
a cutscene. Its unit at 082373C8 stays in
`src/player/effects/spiral_trail/moving_glyph.c`. Moving it into the cutscene
directory would introduce a separate ROM range there. The unit stays intact
within the reviewed Player range map.

The function prefixes follow each defining unit and its callers. Loot
deposit, moving platform, sliding door, Vanargand, and Hresvelgr helpers
owned by their units adopt those names. Shared SceneActors services keep
their names. Vanargand also calls the two shared Debug position helpers.
The static Coffin Bike helper initializes its braking animation and names
that action independently of the public stopping callback.

The filename plan records twenty explicit basenames. Four names beyond the
Sabata, rider, Pile Driver, Nidhoggr effect-pool, and animated sprite units
describe the main Nidhoggr duel, its minions, its orbs, and its bursts.
The tilemap units keep their two reviewed descriptive names.

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

1. `link/player` is a medium-confidence ownership name for 08238238–082389D8
   and 08239130–08243B54. A staged call to 08243B0C created the alternate blue
   Django and ran the separate 0xCAC task. The constructor has no observed
   caller. Story-character calls identify the earlier timed sprites, and
   `DarkLoan_DrawPreview` calls `StatGauge_Draw` at 08243B54.
2. `player/motions` versus finer splits: one range, but it mixes damage
   reactions, form cues and special casts.
3. `StoryCharacter_`, `MoveSequence_`, `PopInSprite_`, `ScriptObjectPool_`,
   `SavedVariables_`: descriptive, pending identities.
