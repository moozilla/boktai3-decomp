# Unit identities from checkpoint replays

Tags: #actors #scene #assets #tooling

The proposed layout mixed encounter names, sprite guesses and code families.
The runtime records are in [symbols/identities.csv](../symbols/identities.csv).
The layout changes are in `build/layout/corrections.csv`. Paths under
`build/identify/` refer to local replay outputs. The original ROM and the
translation's checkpoint bank are read-only inputs.

The coordinator's batch inputs are `build/layout/corrections.csv` and
`build/layout/prefix_renames.csv`. Their checked-in copies live under
`symbols/`. Apply a prefix only within its listed source units. Several
legacy prefixes also occur in unrelated parts of the ROM.

## Reproduce the capture

Run at the decomp root. The translation is two levels up in a nested worker
worktree. Substitute `../boktai3trans/saves` from the main checkout.

```sh
build/venv/bin/python tools/whoruns.py --scan-bank ../../boktai3trans/saves
build/venv/bin/python tools/whoruns.py BossSlot2_UpdateFrame
build/venv/bin/python tools/whoruns.py 081EC380-081F2CD4 --functions
build/venv/bin/python tools/whoruns.py cannon_battery growth_hazard cannon_emitter tentacle_actor
```

The first command converts the VBA checkpoints to batteries, boots each one
through Continue, saves an mGBA state, and indexes 90 idle frames. The scene
column is the value the game stored at `GameState+0x5A4`. This boot path can
restart a fight even when the original VBA file was taken after its defeat.
Coverage counts function entry PCs from `symbols/progress_functions.csv`.
Probes give a frame, argument registers and LR. Idle coverage can find a
running task but can miss its attack, defeat or interaction callbacks.

Use the saved mGBA states for A/B tests of inputs. In the harness script,
`trace off` before the boot sequence and `trace on` before the first `mark`
keep boot out of the comparison. Use the same marks for the screenshots and
the calls they describe. A marked window should stay in one scene if used
for scene attribution. Use a `--segment-scene` override when several
single-scene marks belong to one run.

```sh
mkdir -p build/identify/recheck
cat > build/identify/recheck/commands.txt <<'SCRIPT'
loadstate ../bank/0033/checkpoint.ss
probe boss 0817F734
probe player 0815F0D0
mark approach
hold UP+RIGHT 70
hold UP 35
wait 85
shot fight
SCRIPT
tools/emu/harness baserom.gba build/identify/recheck/commands.txt build/identify/recheck
build/venv/bin/python tools/whoruns.py --record build/identify/recheck \
  --save 0033 --scene 4233 --label 'White Forest boss replay'
```

To stage a specific scripted entrance, make a fresh battery from the same
checkpoint. The helper below uses the stock Continue input prefix.

```sh
build/venv/bin/python - <<'PY'
import sys
from tools.assets import saves
saves.BANK = '../../boktai3trans/saves'
sys.argv = ['saves.py', 'bank', '--make']
saves.main()
PY
build/venv/bin/python tools/assets/saves.py set build/saves/0032.sav \
  build/identify/recheck/start.sav scene=4227 entrance=57 story=111
{
  printf 'trace off\nprobe actor_key 0822556A\nprobe boss 08180380\nprobe text 0821ADD4\n'
  sed '/^save ingame/,$d' tests/save_menus_field.txt
  printf 'trace on\nmark opening\nshot dialogue\n'
  for i in {1..54}; do printf 'tap A\nwait 40\n'; done
  printf 'mark fight\nhold UP+RIGHT 70\nhold UP 35\nwait 100\nshot attack\n'
} > build/identify/recheck/warp.txt
BOKTAI3_SAV=build/identify/recheck/start.sav \
  tools/emu/harness baserom.gba build/identify/recheck/warp.txt build/identify/recheck
```

For each target, record `0822556A` before the spawn or `0821ADD4` when the name
appears. `tools/assets/gclview.py PROC...` prints the creation script, and
`build/text/english.tsv` maps the observed text ID to the translation. The
replays below use the original Japanese ROM.

## Sabata and the seven proposed folders

`0033` loads White Forest scene 4233. Its label is “After Sabata,” but the
battery restore restarts the fight. The `0032` warp above shows the purple
sprite during the dialogue in 4227, looks up text #6580, and proceeds normally
into 4233. At frame 4204 key EC71 calls `BossSlot2_CreateFromScript` (08180380).
The same sprite is visible while `BossSlot2_UpdateFrame` (0817F734) and
`BossSlot2_UpdatePresentation` (0817F9C0) execute. The grounded screenshot is
`build/identify/replays/sabata-full/attack.png`. The dialogue name is visible
in `build/identify/replays/sabata-dialogue/message03.png`. Proc 4235 creates
EC71 for the fight. Proc 6415 creates it again for the rematch. To stage the
rematch, set `0033` to `scene=6415 entrance=47 story=112` and after Continue
hold `UP+RIGHT` for 45 frames, then `UP` for 30.

The two fights together enter all seven proposed folders across
08179B9C–08180488. The rematch adds three `ActorDecision_*` callbacks, which
idle coverage missed. For ownership, the source manifest supplied names and
aliases for the interval. A reference search across `src/` found no callers
outside those seven folders. EC71's constructor is the registry entry. The
`scene_volume` folder contains two ranges and includes the boss's volume
and palette callback. It stays with Sabata along with the other six folders.

## Pile Driver

From 0016, use `scene=6201 entrance=7 story=59`. Hold `DOWN+LEFT` for 80
frames after reaching the checkpoint. The outdoor apparatus moves into
view in `build/identify/replays/pile-generators/approach.png`. Proc 6208
calls 6182, which creates CBB0 in outdoor mode. Use
`scene=6195 entrance=102 story=59` for the Count's purification arena. Proc
6199 calls 6183, which creates the same key with four subjects and mode 1.
`ScenePanel_Create` (0805CB38) and `ScenePanel_CreateSubjects` (0805BDD0) both
run at frame 1470 in the arena. `ScenePanel_TickLifecycle`, `ScenePanel_TickRig`,
`ScenePanel_UpdateBackgroundEffect` and `ScenePanelSubject_Update` run while
the captive is on screen. Advancing the text with A gives runtime strings
#9293–9299, which describe generators delivering the Solar Pile, the
immortal resisting through ectoplasm, and pushing it back into the coffin.
A 0035 warp to `scene=6195 entrance=102 story=115` shows Sabata as the captive
under the same task. Compare `build/identify/targets/pile-purification/action.png`
and `build/identify/targets/pile-sabata/action.png`.

The four ranges at 08057960–0805DD18 belong together under
`src/objects/pile_driver`. The apparatus appears in ordinary exploration and
runs the post-defeat Battle Drive with several bosses as subjects.

## Layered actor and Nidhoggr's interior fight

Stock 0072, 0076 and 0084 all load scene 6323. Proc 6325 calls 6314, which
creates key 5856 with actor ID 7C56, the Vanargand name. The stock 0072
state followed by `hold R 240` gives
`build/identify/replays/layered-stock/chase.png`. The huge head at left
executes `SingletonSlot7_UpdatePresentation` (081F1274), its actor-display
helpers and the sprite-trail pool (`SpriteGroup_UpdateSlots`). The regular
Vanargand code at 081AB50C handles a different part of the encounter. Put
081EC380–081F2CD4 in `src/bosses/vanargand` with it. A warp to scene 6587 also
creates 5856 via that scene's script for the ending confrontation.

Stock 0046 loads Pirate Island scene 4915 and creates 917F at frame 1503.
A warp to `scene=4915 entrance=57 story=143` plays the named introduction.
After Continue, `tap A` and `wait 40` repeated 38 times reaches the name on
screen in `build/identify/replays/grid-ident/message37.png`. The probe recorded
text #7368 at frame 3630. `GridAttack_Update` (0802BF30) and the three support
pools run in the stock fight catch-up, `build/identify/replays/grid-story/nearby.png`.
`GridAttack_UpdateDeathState` unlocks Monster Book entry 80, whose name is
text #9918, Nidhoggr. Proc 4909 creates B06E for the preceding dragon
sequence. Proc 4918 creates 917F for the interior fight, and scene 6425
recreates it for the rematch. Group 917F and its support units under
`src/bosses/nidhoggr`.

## The two shooters

Warp 0013 to `scene=3182 entrance=8 story=53` and 0009 to
`scene=2993 entrance=8 story=40`. After Continue, 30 idle frames then
25 `DOWN+LEFT` frames show the statues. The other run shows the armor near
the bottom right after 20 idle and 32 `DOWN+RIGHT` frames. The screenshots
are in `build/identify/replays/shooters-lifeless-{statue,armor}/near.png`.
Proc 3184 calls 1589 and then 1590 for F426. Proc 2996 calls 1591 and then
1593 for D459. Both keys also create actors on the same frame in a 0054
warp through Dark Castle scene 5297. `EnemyF426_SpawnProjectile` (08203B54)
and `EnemyD459_Update` (08204F40) were observed after creation. Both actors
load sprite package 7E82 with different palette banks. Their defeat code
unlocks Book entries 28 and 29. With Monster Book names starting at #9838,
these are Statue (石像, #9866) and Armor (甲冑, #9867). The destinations are
`src/monsters/statue` and `src/monsters/armor`.

The chapter order agrees with the fan walkthrough. [Casebier's Boktai 3 walkthrough](https://gamefaqs.gamespot.com/gba/926529-shin-bokura-no-taiyou-gyakushuu-no-sabata/faqs/49880)

## Town, map, and menus

The stock 0007 checkpoint runs 3F74 in scene 2553. It displays the San Miguel
map and hands off to one of 11 service procedures from proc 2554. The stock
0008 checkpoint runs 2388 in scene 2569, with the WORLD map and nine
destination callbacks from proc 2570. Starting from 0008 and pressing A on the
first entry switches to 2553. Compare
`build/identify/replays/world-map/world-map.png` and `opening.png` in the same
folder. The proposed paths are `src/ui/town/town_map.c` and `src/ui/world_map.c`.

Start in 0032/4217, press START, then press R between pause pages. The order
starts with Items, Valuables, Gun, Sword, Accessory, and Status. The marked
Status transition calls `Hud_InitStatusMenu` (0817565C),
`SceneCatalog_SetDescription` (08174F14), and `InventoryGrid_UpdateModifierPreview`
(08174F48). The Status shot is `build/identify/replays/pause-tabs/page05.png`.
The four equipment and valuables pages call the 0816B3C0–0816B66C navigation
routines, including `SceneInput_NavigateGridKeys`. Move the catalog to
`src/ui/pause/status_options.c` and the broader sequence unit to
`src/ui/pause/menu_sequences`. The Dark Card runs the first half of the
same unit at 0816AE38–0816B390. Grid navigation begins at 0816B3C0.

### Dark Card Replay and loan services

Stock 0032/4217 already has the Dark Card. Press START, R for Valuables, A
to enter the grid, Right to the card in slot 1, and A twice to select and
use it. `PauseMenu_ApplyItemSlot` dispatches item 39, which has action kind
9 in ROM table 08E61280. That kind calls `PauseMenu_RequestRecoveryRefresh`
(0816D3E8), followed by `Hud_BeginRetryPanel` (0816B390) and
`ScenePanel_UpdateAccountIntro` (0816AF5C). The original screen shows
Doomy with Replay (やり直し) and Dark Loans (暗黒ローン). Text #5163–5171
describes both services.

In the Replay branch, A opens the 400 Soll confirmation, and Left, A
accepts it. `SelectionMenu_ApplyConfirmedReward`, `ScenePanel_BeginAccountExit`
and `SceneSequence_CallCompletionScript` execute before the game returns
to the room. In the other branch, Down, A selects Dark Loans and calls
`DarkLoan_OpenDisplay`, which opens the loan screen. See `options.png` and
`loan-offer.png` in `build/identify/replays/dark-card-services/`, and
`confirm.png`, `thanks.png`, and `room.png` in
`build/identify/replays/dark-card-replay/`. Each directory has its original
`commands.txt` and named `probes.tsv`.

## Magic Cannon, Dark Generator, and Muspell Head

Warp 0051 to `scene=4924 entrance=57 story=149`. After Continue, repeat
`tap A` and `wait 52` to accept the launch. The script at 4928 creates key
6A37 at frame 2547. The task at 081C46C0 updates until frame 3083. The
scrolling horizon and Moon are visible in
`build/identify/replays/magic-cannon/launch15.png`. The flight and Dark Castle
arrival come later, so the unit is the Magic Cannon departure backdrop.

The Dark Generator is a Dark Loans overdue-debt mini-game. To reach it in
0032/4217 without advancing the real clock, make a battery copy with the
following fields changed and use the stock Continue prefix. The `vars` fields
are relative to the script save region, and `game2` starts at GameState+0778.

| Region | Offset | Size | Value |
|---|---:|---:|---:|
| vars | 0070 | 2 | 7 (door entrance) |
| vars | 006E | 1 | 2 |
| vars | 0099 | 1 | 0 |
| vars | 00BC | 2 | 4 |
| vars | 00BE | 2 | 1 |
| vars | 00C0 | 2 | 1 |
| vars | 00C8 | 1 | 4 |
| game | 050C | 4 | 35000 |
| game2 | 0004 | 4 | 0 |

Read and write the two checksummed copies using `tools/assets/saves.py`
(`current`, `load`, `write_sav`). Proc 871's overdue check dispatches proc
1001 and then proc 1003. `tap A` plus `wait 28` repeated 75 times progresses
through the Dark Loans dialogue and into work. B3D1 constructs the control
panel at frame 2209 (`ControlPanel_CreateFromScript`, 081C27EC).
`ControlPanel_BeginInteraction` (081C1E00) then runs. The treadmill, green
meter and worker are visible in `build/identify/replays/loans/work60.png`.
Proc 1003's dialogue calls it the Dark Generator. Move the unit to
`src/minigames/dark_generator.c`.

The creature spawned by 5153 is Muspell Head (ムスペルヘッド), despite the generic
`ScriptCombatActor` names. Warp 0030 to `scene=3992 entrance=8 story=101`.
The entry records two `ScriptCombatActor_CreateFromScript` hits at frames
1473–1474, and the pair at (1152, 1664) and (1664, 1664) run the projectile
update. Proc 3991 calls 1604 and then 1606 to spawn them, and defeat unlocks
Book entry 31 (#9869). The heads share package 7E7D with other monsters. Their
red shots can be seen in `build/identify/replays/muspell-close/advance.png`.
For a clearer sprite view, continue the same warp, wait 40 frames, and write
the player position at 0200B634–0200B638 to (1792, 256, 1856) using
16-bit `poke` commands. After 60 frames, `muspell-visible/pair.png` shows
the head beside Django while the same actor update continues.

## Two bat units

Warp 0014 to `scene=6401 entrance=47 story=55` for the Count rematch. After
Continue, hold `UP+RIGHT` for 35 frames then wait for `08044854` to execute
with `watchpc 08044854 0 2147483647` and `waitpc 1500`. Drawn bats appear as
`ContactSwarm_SpawnCharge` and `ContactSwarm_UpdateCharge` execute. Compare
`build/identify/replays/count-swarm/spawn.png` with `fight.png` 34 frames
later. D972 is the Count's Vampire task and it unlocks Book #9915. The
ContactSwarm code belongs in that boss's directory.

The independent 0800F9C4–080111EC unit also draws a sixteen-bat flock.
Package DA6D is loaded for each piece. No stock checkpoint exercised the
unit and the GCX procedure body has no literal E534. To identify the saved
asset, a scratch harness stopped at `Player_Update`, called the existing
constructor at 080111A8, then called the straight advance handler at
0800FC28 with a spawn centered at (1920, 512, 640). The stock ROM then
ran `GroupObjects_Update` and `BoardPiece_UpdateAnimation` while the purple
bats moved across the screen. The same state without the call is empty of
bats. See `build/identify/replays/flock/{before,flock,advance}.png` and the
`stageflock` test extension under `build/identify/`. This verifies the
unit's display and action. Its story placement remains unobserved.

The registry entry at 08604748 is the only 32-bit literal E534 in the ROM,
and the following word at 0860474C is the only pointer to the constructor.
The 558,244-byte GCX procedure body starts at 08DD6BA0 and has zero
little-endian E534 literals. `build/venv/bin/python tools/assets/cutscenes.py
--commands` gives the message routing, and `cutscenes.table()` enumerates
the shipped packet rows. The seven cutscenes with kind 10 packets address
01FD, 1ACF, A013, A3E7 and F3E4 with commands 6–10 and 12–13. The flock's
dispatcher implements only 0, 1, 2 and 11. This narrows the missing
placement to a dynamic route outside the built-in cutscene packets, if
any exists. The name records the observed effect and its registered key.

## Coffin Bike and Sabata’s scripted rider

Warp 0008 to `scene=6274 entrance=130 story=31` and continue. Proc 6275 calls
1277, which creates E881 via `Glove_CreateFromScript` (0811AB38). The field
checkpoint 0008 in 2569 has no code hits in 08116870–0811B048. The ride
runs `Glove_UpdateTask`, `Glove_UpdateStopping` and
`Terrain_ResolveActorMotion` while the player and bike HUD are visible.
`hold R 55` and `hold R+UP 35` give
`build/identify/targets/bike-lifeless/{ride,steer}.png`. The later 6323
chase also uses the E881 unit for Django. The collision in `bike-impact`
executes `ActorRecoil_ReceiveBodyContact` (081162D8) and
`ActorRecoil_ApplyDirectionalImpulse` (08116538). The idle control executes
neither, and the HP drop is visible in `bike-impact/obstacles.png`. Name the
ride and recoil units `src/minigames/coffin_bike`.

The same `Glove_UpdateTask` (0811A604) drives the riders in the two-player
Back Street Bike Battle. The linked mGBA replay described below records
948 calls per core over the sampled race window. Both players and their
separate HUD values are visible in `build/identify/link-peer/play-3-0.png`
and `play-3-1.png`.

In a copy of stock 0072, set GameState+063C to 100000 and +0638 to 1 to select
Sabata's ending branch. Continue from 6323, tap A and wait 28 frames repeatedly.
Script 6316 invokes A8CB at frame 2534 while Django's own bike is still
running. In `build/identify/replays/ending-sabata-puppet/line33.png`, Sabata's
purple rider is visible alongside Django and text #5585 names him. The
A8CB update, knockback and fall handlers are observed as the rider enters.
The destination `src/npc/sabata_rider` describes the reached role.

## Weapon comparisons

Use one stock checkpoint, 0032, for each variant. For the five sword types,
copy record ID 0, 1, 2, 4, or 7 from the 32-byte weapon table at 08E6092C.
Write kind into GameState+01C0, durability 1000 into +01C2, and the three
words from record +0C into +01C4. Set the selected equip row (+0050) and its
inventory slot (+0068) to zero. On Continue, confirm the descriptor via
`Player_BeginDescriptorAction` and the weapon icon. The five cases are
Gradius, Estoc, Kopis, Zweihander, and Kagerou. Key B uses the following
replay windows after 25 idle frames.

```text
mark single
hold B 7
shot single
wait 46
mark combo
hold B 6
wait 9
hold B 6
shot second
wait 9
hold B 6
shot third
wait 70
```

Holding B for 100 and releasing gives the sword pose and slash/thrust at
`build/identify/replays/weapon-*/release.png`. Identical inputs in the gun
variant pass through `Player_BeginIndexedAction` and produce the projectile
path. The sword cases all share `Player_InitializeActionContact` (0814D724),
`Player_SelectActionVariant` (0814D02C), and
`Player_UpdateAttackColliderMotion` (0814D264). The combo probes reach
`Player_HasWeaponComboEntry` on four of the five weapon samples.

For the gun, the active row at GameState+0050 is 11 in the test. Set the
one-based frame record at +04C8 for that row's existing inventory ID 4.
With story 58, scene 6201, entrance 7, walk 40 frames `DOWN+LEFT` and face
right for two frames. Press and release B. Frame 1 uses
`Player_UpdateSelectedAttackCharge` and `Player_SpawnSelectedAttackProjectile`
(`build/identify/replays/gun-1/mid.png`). Frame 8 adds
`Player_UpdateAttachedChargeTiers`, `Player_UpdateAttachedEntryPosition`,
and `Player_OnChargedAttackExit`. Hold B for 90 frames and release for the
charged shot in `build/identify/replays/gun-8/charge-flight.png`.

Frame 11 takes the dedicated 0814EEE0 callback while B is held. Its indexed
frame is Tempest, the rotating spread described by text #967. Move that
file from core to gun. The Rockbuster is weapon descriptor 48 and goes
through `Player_BeginDescriptorAction`, then fires via
`Player_UpdateAttachedAttackCharge` (0814F328). The seven original gun-list
files retained by the audit are 0814DE0C, 0814DFDC, 0814E3A0, 0814E3D8,
0814E978, 0814E990, and 0814F328.

### Motion and Link callbacks

Neutral A, Sol Trance with A/B, five swords and the sampled gun frames
leave the disputed burst and spiral PCs untouched. A controlled command in
`build/identify/state_harness.c` calls `ActorSequence_Begin` (0813AFEC)
with the live Player pointer and a chosen motion ID. The original task then
runs the normal motion dispatch. Use the 0016 outdoor checkpoint and record
one mark per motion. The command script is
`build/identify/replays/player-motion/commands.txt`. No ROM bytes changed.

| Motion | Observed result | Destination |
|---:|---|---|
| 9 | 08143604 draws the full-body gold energy effect | `player/special_actions` |
| 13, 14 | 08147CF0 and 08147FB8 play the red spiral and transition to a form cue | `player/cues` |
| 16, 17 | 08144408 and 08144624 run the special burst callbacks. The 16 image shows the blue effect | `player/special_actions` |
| 21 | 08144D64 runs the incapacitated pose after a Plasma Cannon shot, with 08144D60 as its exit callback | `player/damage` |
| 27 | 08144F40 fades the screen and removes the actor in the departure pose | `player/cues` |

0814692C is the alternate motion table's mode 9. 081481E4, 08148B64,
08148E4C and 08149DF0 are sprite and spiral states in additional motion
tables, so they go with the cues. Those assignments use dispatch and
source evidence. The ordinary gun and sword actions both take motion 3,
and none of the controlled motion captures establishes a regular input
sequence for the other modes.

### Spiral Burst startup

Entering mode 17 first exercises its animation loop. To witness the
initial cast, `build/identify/effect_harness.c` calls the original
08144624 routine while Django is still idle in the 0016 outdoor snapshot.
The harness clears the selected resource timer and seeds the sunlight
level immediately before entering the routine. Use `stageplayer 08144624 0`
and `stageplayer 08144624 8` from the same state. The first resource check
returns 0. The second returns 1, calls `SpiralBurst_Create` (082347C8) and
draws the green spiral around Django. Both weapon probes stay idle. See
`build/identify/replays/spiral-start/comparison.png`, `commands.txt` and
`probes.tsv`. The sunlight dependency and visual are verified in this
controlled run. The normal input trigger remains open.

### Plasma Cannon contact and the timed reaction

Load 0060 with `scene=5875 entrance=7 story=166`. The cannon's native update
reports the position (3456, 512, 2688) and faces increasing Z. The control
waits at the room entrance. Place only Django at (3456, 512, 2944) so he is
in its firing lane. Use 16-bit `poke` commands at 0200B3A4, 0200B3A6 and
0200B3A8 in the setup savestate. The command file and all register snapshots
are in `build/identify/replays/plasma-contact/`.

`OrangeSpine_FireProjectile` runs at frame 1880. The projectile hits at
1885 and `PlayerContact_ApplyResponse` calls `Player_RequestTimedReset`
with a duration of 70. Motion 21 starts at 1900, and `Player_UpdateChargeMotion`
begins at 1901. `status.png` shows stars circling Django. The contact segment
also runs the exit function `Player_OnChargeMotionExit`. Neither entry runs
in the control segment. Use `player/damage` for both files.

The ordinary Warp Leaf from stock 0032 triggers motion 7 before returning
to the world map. Its probe file in `build/identify/replays/warp-leaf-departure/`
separates that path from the motion 27 fade. The native tile trigger calls
`Player_BeginDirectionalCue`, which can start mode 27 with a separate
`PlayerCue_UpdateDeparture` callback. The exact 08144F40 update still has a
controlled capture only.

Five late files go in `player/link_actions`. `Player_AccumulateActionCharge`
(0823C02C) gets its charge from the link-session update paths. The other
four are indexed by 08E87A10 or 08E87A90 from `Player_UpdateLinkKind` and
`Player_UpdateHalfChargeLinkKind`. The `link_actions` path describes that
dispatch family. Its shipped entry point is still unconfirmed. The latter
dispatcher has no code or pointer cross-reference in the generated assembly
or the ROM literal search. The constructor at 08243B0C, which installs
`Player_UpdateLinkCore`, likewise has no direct caller or literal pointer.

### Connected Bike Battle control

The scratch host at `build/identify/link-peer/peer.c` connects two mGBA
0.11 cores with the library's `GBASIOLockstepCoordinator`. It runs them
one at a time at matched emulated timestamps and loads copies of stock
batteries 0032 and 0070. Connect at title **Link → Bike Battle**. Wait for
both players to show ENTRY, press START to open the primary player's
course selection, and confirm Back Street. Press START on each core's
customization screen. The start banner, independent movement and the
two active HUDs show the match advancing beyond setup.

```sh
cp build/saves/0032.sav build/identify/link-peer/player0.sav
cp build/saves/0070.sav build/identify/link-peer/player1.sav
build/identify/link-peer/peer baserom.gba \
  build/identify/link-peer/player0.sav build/identify/link-peer/player1.sav \
  build/identify/link-peer/bike-play.txt
```

Build the scratch host against the same mGBA include paths, build defines
and library as `tools/emu/Makefile`. `bike-start.log` records the serial
menu's choice exchange on both cores. `bike-play.log` records zero calls
to each of 0823C02C, 0823D080, 0823D3A0, 0823FE94 and 08240F9C during the
race. It also records zero calls to their three observed dispatch parents.
`Glove_UpdateTask` runs on both cores. The samples give a connected Bike
Battle control for the late Player files, whose actual gameplay route
remains open.

### Crossover Battle 2 control

Stock 0032 reaches **Link → Crossover Battle 2** through the title menu.
The original menu opens `Wireless_CreateController` and
`WirelessPeerPanel_Create`. In `build/identify/crossover-title/no-adapter.png`
the peer slot stays empty and the prompt calls for a wireless adapter.
The inputs and trace are in the same directory.

The round procedure is 1656. Warp the same 0032 battery to
`scene=1656 entrance=47 story=194` to isolate local arena behavior. The
stock ROM draws the duel and both health readouts in
`build/identify/replays/crossover-arena/start.png`. The Count's normal
`Vampire_Create` runs, and every observed Player task update is the ordinary
0815F0D0 `Player_Update`. `Player_UpdateLinkCore`, `Player_UpdateLinkKind`
and `Player_UpdateHalfChargeLinkKind` have no hits. This warp establishes
the local round's task ownership. Full wireless play still needs a peer.

The extra `src/player/sub_*.c` rows in `corrections.csv` override the
corresponding `player_files_*.txt` membership. Apply them by removing each
file from its original list and using the corrected destination directory.


## Book and story cross-checks

The Monster Book starts at text #9838. For each named defeat handler with
an immediate `Bestiary_UnlockEntry`, the index gives the in-game label
independently of the tentative sprite description. This also exposed four
names outside the original guess list.

| Actor | Warp from the save bank | Proof frame and Book name |
|---|---|---|
| Popbud, F639 | 0019 → 3400, entrance 7, story 66. In the close replay the player position is (2304, 512, 2432) | `popbud-close/exposed.png`, #9840 |
| Garmr Statue, 4CD8 | 0029 → 3885, entrance 7, story 95. Wait 30 frames after Continue | `garmr-statue/entry.png`, #9868 |
| Wind Cannon, with manager 9CC2 | 0042 → 4755, entrance 7, story 131. Wait 20 frames then hold `UP+RIGHT` for 50 | `wind-cannon-west/near.png`, #9872 |
| Plasma Cannon, 01A2 | 0060 → 5875, entrance 7, story 166. The close replay sets the player position to (3488, 512, 2400) | `plasma-close/exposed.png`, #9874 |

The paths in the last column are under `build/identify/replays/`. For the
two camera tests, wait 30 frames, set the live Player's world-position
halfwords at `Player+30`, `+32`, and `+34`, and wait another 30 frames.
The Player task pointer comes from the `Player_Update` probe or gPlayers
at 02000580. Spawners 1598, 1603, 1609 and 1619 give the actor keys, and the
replay captures the individual actor updates.

### Solar Roots in Hresvelgr's arena

Stock 0023 reloads scene 3588. Proc 3594 places four 1FBA plants, and the
rematch places the same four in proc 6397. Text #1817 describes using Earth
elemental attacks on the Solar Roots and #9052–9054 names the vegetation
and describes the shape it grows into. The root south of Django's entry
is at (2432, 512, 2432).

Use `build/identify/replays/solar-roots/commands.txt` with the setup state
from `solar-roots-setup/`. Both branches face `UP+LEFT`, set Django at
(2688, 512, 2432), and fire the equipped Gun Del Sol frame. For the Earth
branch, set the weapon configuration byte at `Player+0xC18` to 5. The two
branches run `GrowthHazard_ApplyAnimationEvent` when the shots hit. Only
Earth advances this root to `AnimatedMarker_BeginActive` at frame 2042.
Compare the flat root in `shot5.png` with the full-grown one in
`earth4.png`. Place the unit at `src/objects/solar_roots`.

### Pirate Island deck cannons

The `cannon_battery` unit places and updates banks of deck guns. Stock
0045/4906 renders the cannons while `CannonBattery_UpdateCannon` runs.
See `build/identify/replays/nidhoggr-ship/fire.png`. The 0051 warp to
4859 also records `CannonBattery_CreateCannon`, `CannonBattery_InitCannon`,
and `CannonBattery_UpdateCannon` with the guns visible in
`build/identify/targets/grid-magic-cannon/action.png`. Procs 4860 and 4905
create the 55E9 battery manager. Otenko calls these the cannons around the
helm in text #7463. Place the pooled cannon unit at `src/objects/deck_cannons`.

### Poisoner and Perplexer

The former `cannon_emitter` unit draws the pollen flowers. The Monster Book
names them Poisoner (#9838) and Perplexer (#9839), and #2509–2510 distinguish
their poisonous and disorienting pollen. Proc 3503 spawns two Poisoners
through key 98CB, and proc 5180 spawns a Perplexer through the same key.

Warp 0021 to `scene=3504 entrance=9 story=98` and 0054 to
`scene=5181 entrance=8 story=156`. The replay scripts position Django near
each plant and allow its original proximity logic to run. In
`pollen-identities/poisoner.png`, variant 1 fires and applies status kind 0
for 1800 frames. In `perplexer-live/pollen.png`, variant 0 fires and applies
status kind 1 for 900 frames. Both captures execute
`CannonEmitter_UpdateAnimation`, `ProximitySequence_EmitBurst` and
`SpriteEffectController_Create`. The first status runs the poison palette
path and the second randomizes the saved action selection. Place the unit
at `src/monsters/pollen_flowers`.

### Centipede

Warp 0030 to `scene=4076 entrance=7 story=101`. Proc 4075 calls 1574 and
then 1576, which spawns the moving actor through 0AC7. Hold DOWN+RIGHT for
40 frames and wait 60. The Centipede is visible beside Django in
`tentacle-dark-room/encounter.png`. `TentacleEnemy_Create` and
`TentacleActor_UpdateComponents` run, followed by the body unit's attack and
spread callbacks. The Health and position staging in the separate
`centipede-contact` comparison recorded sword contact setup without
reaching the defeat callback.

Read the running actor's category byte at `RoomActor+0x98` in the dump
made by `tentacle-dark-room/commands.txt`. It is 9. The defeat path passes
this category to `Bestiary_GetEntryForActor`, which maps it to entry 13,
text #9851, Centipede. Merge both units at `src/monsters/centipede`.
The similarly named `Centipede_Create` registry entry 0AD8 queues a terrain
overlay command. The visible combatant uses the 0AC7 constructor.

The story order is Count → Hresvelgr → Sabata → Nidhoggr → Ratatosk →
Vanargand after the opening dream. The table records each arena and the
reached functions. Stock 0023 reloads Hresvelgr in 3588. Move `UP+RIGHT`
48 frames for `hresvelgr/nearby.png`. Stock 0065 reloads Ratatosk in 5965.
Move `UP+RIGHT` 45 frames, `UP+LEFT` 22 frames, then wait 120 for
`ratatosk-close/attack.png`. The Vanargand arena is 6165 in stock 0070.
Nidhoggr's ship fight in 4906 and named interior form in 4915 use separate
controllers with the same identity. The 6323 chase is also distinct from
the Vanargand arena. These distinctions account for the extra boss groups.

The Pile Driver's live timer is visible in `pile-active-hp/ready.png`.
That 0016/6195 comparison used the same entrance with the tutorial flags
set and a staged subject gauge of 750/1000. The approach and named captive
captures are the story evidence. A separate controlled run with the saved
subject gauge at zero reaches the PURIFICATION COMPLETED screen.

## Scope of the evidence

The compact index has all 83 stock checkpoint captures, with focused
branches under `build/identify/replays/` and `targets/`. Function-entry
coverage for a mark is a lower bound on the callbacks the scene can reach.
The isolated E534 test establishes the bat assets and the launch behavior.
The flock's natural story placement and the usual controls for the forced
Player motions remain open. Their records distinguish those limitations
from the witnessed scenes.

## Reviewer follow-up

### Bouncing bombs and shop ranges

Load 0046 in scene 4915 and wait for 080692C8. The Nidhoggr grid actor
spawns a dark sphere at frame 1921. The next shots capture it bouncing
across the active tiles while `BouncingObject_TickActive` runs. The removal
callback fires from the same pool. Use `src/objects/bouncing_bombs` for
this family of projectiles. `review-bouncing-bombs/commands.txt` has the
trigger and timing.

Warp 0007 to `scene=1929 entrance=7 story=30`. GCL proc 1926 opens key
744F. The screen says Weapon Shop and shows Sheyenne and the swords for
sale. A opens the purchase prompt. The comparison, action selection and
purchase confirmation callbacks run in the three planned shop units.
The capture is `review-shop/menu.png`. The scoped `SceneInventory_`
prefix batch covers 97 symbols in these three units.
