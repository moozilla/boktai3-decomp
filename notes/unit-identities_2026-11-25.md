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

### Moving platforms, passage barriers, and the two Armor families

Tags: objects, moving platform, A4FC, passage barrier, 71BA, Armor, Black Knight, E043, D459

Warp 0042 to scene 4808, entrance 8, story 131. In
`replays/review-named-platform`, proc 4804 calls 4807, which creates A4FC
ID 085F with kind 1 and its route. The constructor loads B019. The fixed
stage point in the saved slot was (2432, 512, 2432). Load `arrived.ss` and
stage the Player at (2480, 512, 2500) as in
`replays/review-platform-ride/commands.txt`. The camera frames the blue and
orange platforms while the route and entry trigger update. The switch's
proc 4805 and the route callback 4806 release 085F, and text #7181 names the
moving platform. The capture establishes the platform's update and sprite.

Warp 0042 to 4755, entrance 7, story 131. Proc 4752 calls 1071 with a line of
71BA objects at z=1280. Use `replays/review-cell-gate/arrived.ss` and
`replays/review-passage/commands.txt` to stage (1664, 512, 1460) and approach
the Wind Cannon row. At frame 1878 the passage test, removal of the terrain
modifier, and Player terrain interaction run together. The screenshot
locates the edge. The barrier operates through the terrain modifier and has
no independent graphics.

D459 and E043 warrant separate destinations. In the 0009 replay
`axe-armor-attack`, wait for `EnemyD459_FireProjectile` beside the axe
Armor at 2993. `wave.png` catches the shockwave described by text #2538.
For E043, warp 0010 to 6472, entrance 47, story 43, or 0070 to 6265,
entrance 47. Use the `iron-armor-rematch` and `black-knight-dialogue`
command files. GCL 1629 and 1639 give E043 type 1 and type 11 respectively.
The Monster Book indexes resolve to #9891 (Iron Armor) and #9902 (Black
Knight). `Armor_Create` and 0818A820 run for both.

### White Forest loot deposits, quest barriers, ice boulders, and puzzle blocks

Tags: objects, loot, quest barriers, White Forest, Trinity, F1F9, 3DB0, 377D, E02A

Warp 0029 to 3905, entrance 7, story 95. Proc 3904 calls 1553, then 10801,
creating F1F9 ID 5639 at (1856, 256, 832) with graphics 9385. From
`review-jewels/arrived.ss`, use `review-quarries/commands.txt` to place
Django nearby and repeat sword inputs. The attack path calls
`CallbackPool_ApplyDamage` for slot 0200FC30, then
`ActorGroup_CallMemberScript` for the same slot. Proc 1133 rolls its item
drop and conditionally invokes 1138. Compare `intact.png`, `hit03.png` and
`hit11.png`. Use a descriptive loot deposit name while the exact label
remains unknown.

Warp 0029 to 3931, entrance 7, story 95. Proc 3935 calls 1091, placing
3DB0 ID E912 with graphics 474D. Ordinary sword contact ran the native
contact callback. The break requires contact flag 0x200. In
`review-trinity-release`, load the room state, stage Django near the prop,
and write 1 to the watched task's `destroyRequested` byte. This stages
the post-contact condition. The original game runs its shatter effect,
dispatches proc 3845, and shows Trinity's acceptance in text #6834.
`frozen.png`, `trinity.png`, and `dialogue.png` show the sequence.
Other 3DB0 calls carry graphics 77CB and 77B3 and different quest callbacks.

Warp 0029 to 3955, entrance 7, story 95. Proc 3952 creates the 377D
ice boulder via 1176 and two E02A obstacles via 1059 and 1060. The
`review-proximity` trace shows the 377D constructor load graphics 863A.
The large blue boulder is visible at (896, cell height, 896).
The obstacle constructor takes IDs 0EEC and 0EED with graphics 9D41.
Proc 3953 centers on those obstacles and dispatches both removals when
the switch runs. `review-blocks-close` reframes the room. The 6D69
controller has a separate task, script key and slot representation.

### Sliding doors, dungeon scenery and light puzzles

Tags: objects, doors, scenery, light puzzles, 6D69, D4E3, 3D91, 2E1C

Warp 0054 to 5297, entrance 7, story 156. The
`review-door-pool` trace runs 6D69's constructor, which loads the
7ACD and 7AD3 door panels, then appends D4E3 slots and updates them.
The camera walks past a raised panel in `after.png`. Keep this
sliding-door pool separate from the E02A puzzle blocks.

From the 0029 warp to 3905, load `review-jewels/arrived.ss` and run
`review-prop-snow/commands.txt`. Proc 3908 calls 1093 for several
3D91 props, including AAEE and AAEF. Their constructor loads B07C
and runs `SceneScriptTask_Update`. At frames 1924 and 1926 a sword
strike runs the flash and burst paths. Compare `near.png` and
`after.png`. B07C includes several other prop shapes. Both source
files belong with dungeon scenery.

Warp 0038 to 4479, entrance 7, story 125. Proc 4477 calls 1174 for
B8CC, creating a 2E1C slot and changing the light-region controls.
`review-prop-s0` follows the active callbacks in the room.
`after.png` captures the changed doorway view. Proc 1171's comments
refer to light spots and candlesticks. The title given to the
shared 2E1C pool describes the script role. The exact graphic for
the B8CC slot still needs an isolated sprite capture.

### Otenko and the field story cast

Tags: Otenko, Trinity, tutorial, NPC, BB34, 8EA3, StateActor, 8AAB

Load `bank/0000/checkpoint.ss` and use
`review-state-params/commands.txt`. Stock scene 2201 shows Otenko in
the garage beside the talkable character. In a separate 0006 warp to
2544, entrance 57, story 30, proc 2489 calls 1676 and creates BB34
ID 9CFE. `Actor_InitDrawable` receives 97D3 from 081D0CD5 and
`SceneNpc_OnModeUpdate` runs. The stock screenshot shows the field
companion. The 2544 warp supplies the constructor evidence before the
World Map takes over.

The staged `review-native-tutorial` run invokes original GCL proc
1482 from an active room. The 8AAB task draws the tutorial Otenko
over the active room. The real caller is chapter dispatcher 1474.
Procs 1479 and 1480 select it from title attract demos and Options.
The controlled call uses the original ROM data and renderer.
The adjacent four Params3D0 files have a separate display registration
through proc 1432 and 4A25.

After the White Forest barrier shatters in `review-trinity-release`,
Trinity shows his portrait and dialogue and the state unit runs
`StateActor_UpdateDashAnimation13` and
`StateActor_UpdateAnimation20State`. The 4915 Nidhoggr intro separately
runs `StateActor_StartKind28Animation4` for another cast member.
Use `StoryCharacter_` for the 51 scoped StateActor functions.

### Pause Status, Inventory, and room lifecycle

Tags: pause, status, inventory, Warp Leaf, scene lifecycle, game state, Pile Driver

Load 0032 at 4217 and follow `pause-tabs/commands.txt`. Gun, Swords,
and consumable pages use the PauseInventory cursors and item-use
callbacks. Slots and unlock descriptors come from the separate game
state unit. Status shows the level, Life, Ene, and allocation
values. Entering Status calls `SceneCatalog_SetDescription` and
`SceneCatalog_GetLevelScaledValue` in `to_05`. `page05.png` shows
the tab. The prefix proposal scopes the status functions and leaves
the settings helpers for their own names.

Use the Warp Leaf in the same checkpoint as recorded in
`warp-leaf-departure/commands.txt`. Selecting it calls the GCL load
handler, old-room release, and new-room initialization. Compare
`departure.png` and `landing.png`. This unit owns room lifetime.
Pause ownership reads and gGameState updates are separate paths.
In stock 0016's Pile Driver tutorial, proc 6192 calls registry command
8954. Its 0806F218 callback increments the four persistent counters
directly. Record the per-file move to game state so the mixed former
scene state directory does not move it with the spatial queries.

### Effects with source and on-screen witnesses

Tags: particles, Bat Curtain, falling objects, Wind Cannon, action effects, Trinity

The controlled effect runs load the 0029 room snapshot from 3955.
`review-native-*/commands.txt` contains the original routine called,
argument memory writes and screenshot timing. The local harness at
`build/identify/witness_harness.c` adds a `native` command to the stock
mGBA harness. It interrupts `Player_Update`, passes the original
Thumb function its arguments, and resumes the interrupted registers.
The room and display remain active. The ROM bytes are unchanged.
The before/created/after shots give a control against environmental
effects already visible in the room.

| Run | Witness |
| --- | --- |
| `review-native-fading` | Native 08015F60 produces orange flame motes. The FallingObject burst calls this routine. |
| `review-native-fragments` | Native 08014CA4 loads 84EE for a type 0 radial spread. White-blue fragments are visible in `after.png`. |
| `review-native-bouncing` | Native 08014634 creates a dark fragment group with original physics. 6165 also runs the same manager. |
| `review-native-burst-sparks` | Proc 1043 calls registry 1488. The initial spark explosion uses graphics 8639 and ends with smoke. |
| `review-native-trinity-sparkles` | Proc 3845 calls 3D64, loads CC88 and draws green dots. The same constructor and update run in the 3931 Trinity release. |
| `review-native-debris` | Native 08012B34 loads fragment DA6D. The Bat Curtain unit contains the production callers. |
| `review-native-hit-orb` | Proc 1179 selects 95A8, which loads sprite 066A and draws a bobbing green crystal. The Pirate Island room at 4859 also runs the constructor. |

For the action flash, load the 0016 Pile Driver field control and
stage motion 9 as recorded in `player-motion/commands.txt`.
`action9.png` shows the effect around Django. The Vanargand attack
pairing at 6165 uses the separate actor attachment group.
Centipede's strike at 4076 uses the shared hit-burst pool, which also
runs during the Black Knight battle.

Load 0042 at 4755 to trace the Wind Cannon. The
`review-wind-particle` probe records return address 082099F1 for
`EnemyParticle_Emit`. This locates the call in the Wind Cannon update.
`wind-cannon-west/gale.png` shows the room activity. The same emitter
pool has live coverage in the Black Castle Screamer room and a
separate Poison Hand caller.

### Kuro, interaction prompts, and floor traps

Tags: Kuro, NPC, D774, floor traps, 2FA5, interaction prompts, 883A, Sabata

The 0006 house warp at 2537 runs D774 and loads E00E from
`TaggedSceneNpc_Create`; the World Map interrupts some entrances.
For an isolated view, `review-native-tagged-live` calls proc 1675 to
create the actor manager and then proc 1790 with a nearby position
and the original tag. It uses the same D774 ID 51E2 and E00E model.
Kuro's scenes use that actor ID in proc 2266. Proc 2235 calls 2266
and plays Kuro's named speaker line #3515. Text #2603 calls him
Sumire's black kitten. The original E00E sprite sheet is under
`build/assets/sprites/`.

Use 0013 at 3161 for the floor traps. Proc 3163 places four 2FA5
slots and the pool loads A945. `review-stage-pressure` dumps the
initialized first slot at (1152, 512, 896). The
`review-native-pressure-exact` replay loads `idle.ss` and sets that
slot to state 2, the state assigned after contact. The native
countdown, particle, press, and contact callbacks run and the slot
calls the room actor and Player queries. This also witnesses the
4384 sprite puffs from the adjacent trap and spell effect unit.

For the prompt layer, warp 0030 to 3992, entrance 8, story 101.
`review-directional` captures 883A construction and approach.
The separate 3955 instance from proc 1030 targets 3954, the callback
that shows the fireball instruction. The shared layer applies to
other prompts and interactions.

For the Sabata frost cloud, load 0033 in 4233 and use the native
`review-native-frost-cloud` command file with the `battle.ss`
snapshot. The effect task at 0200E864 is the live task from the EC71
actor at 0200D824. Its production owner passes actor+41C in
`BossSlot2_UpdatePresentation`. The experiment starts that task
and seeds five particles at a staged source, then lets the original
task animate them. Compare `before.png` and `after.png`.

### Shared effect pools

Tags: particles, spark effects, short-range shot, actor trails, Ratatosk

The Trinity barrier's shatter activates AnimatedParticles, which also
runs in the Vanargand fight. Ratatosk uses the independent moving
spark pool. Player motion 13 exercises the action trail pool at 6201.
The Count area also executes the nearby moving glyph file for a
different subsystem. In the 6323 Vanargand chase, the following
sprite manager updates character appearance slots. This is a
different use from the separate scatter-effect implementation
in the same source directory.

The controlled `review-native-moving-contact` and
`review-native-moving-impact` runs start two separate small-sprite
motion pools. Their moving dots have low visibility against the
pedestal. The `review-native-spread-shot` type 2 sweep has clear
orange flashes and a separate moving collider. Its production
trigger remains unknown.

### Strike traps, tether rings, and flying sprite effects

Tags: strike traps, 0866, tether rings, F156, Vanargand, projectile, debris, title

Load the 0029 state at 3955 and use the `review-native-collision-blast`
command file. Original proc 1180 creates 0866 slot E811. Deliver the
registered hit callback to the live slot and wait through its delay.
The resulting marker and sparks appear in `created.png` and `after.png`.
`EffectManager_SpawnParticles` runs at frame 1932.

The same room provides a controlled backdrop for smaller effects.
`review-native-tether-marker` invokes proc 852, creating F156 and
drawing a ring with 0BA8 and the original sparkles. Checkpoint 0024
also runs the ring updater at 3472. `review-native-directional-debris`
calls the regular 4147 fragment drawer with the 29–31 frames selected
by ContactBurst. The producer is the contact burst linked to Nidhoggr.
`review-native-departure-motes` loads 2E90, palette 1F5 and EBF6
animation 5, as set up in Vanargand's departure callback.

`review-native-ballistic` invokes 081394B4 and steps the launch,
flight, and scatter phases. Native trail motes use 1C1A and sequence
D1B8. The separate projectile profile run invokes 08114928 and
exercises its type 0 409C linear flight. The
`review-native-scatter-ability` slot instead creates a falling visual
and an independent scatter effect. Keep all three paths distinct
in the layout. For the shared debris drawer, replay 0008's bike
impact at 6274 or 0072's ending at 6323. Both execute the scene
debris callbacks.

### Title logo and scene-state file placement

Tags: title, title emblem, Params3D0, room queries, layout

Boot stock 0000 with `review-title-display/commands.txt`. At frame
711 the original 1432 script sets Params3D0 and creates 4A25 FBD8.
`late.png` shows the animated flaming emblem while the display
callback runs. The 8AAB tutorial sprite and the adjacent title
display are separate features.

The layout corrections for mixed source groups include explicit
file overrides. Apply unit destinations first and then file
overrides. The former scene-state group keeps the three actor
queries together. Its subsystem reset goes to scene lifecycle,
its Pile Driver counters and new-game field initialization to game
state, and its wrapped tilemap helpers to background video code.
The separation follows the floor trap, Pile Driver, title boot
and room-load witnesses and their direct callers.

### Renderer verification of Kuro and the floor trap

Tags: renderer, sprite attribution, Kuro, floor trap

For a direct sprite check, replay the two staged scripts through the
mGBA Studio sprite observer. The local supplement
`build/identify/renderer-check.py` enables the observer just before
the invocation and keeps the original `sprite-sources.tsv`.
Build the local `witness_draw_harness` from `witness_harness.c`,
`tools/studio/experiments/observe.c` and
`tools/studio/experiments/draw_trace.c`, with
`-DSTUDIO_EXPERIMENTS` and the normal harness compiler flags.
The drawtrace declaration can be derived with
`studio.experiments.graphics.trace_command()`.

In `review-render-tagged-live` at frame 1867, the drawable inside
the E00E actor submits two pieces at (205, 40) and (221, 45) using
ROM tiles 08CF44E8. The native initializer earlier records that
actor at 0200ED24 and the drawable is at 0200EE24.
In `review-render-pressure-exact` at frame 1860, the 2FA5 slot's
drawable 0200E064 submits four pieces along the right-hand side
of the trap room. This links the visible trap to the same slot
whose press and contact handlers ran in the trace. The independent
particle and collision draws in the other two renderer samples did
not yield comparable slot-local addresses.

### Candlestick and Fireball hint follow-up

Tags: candlestick, light fixtures, Fireball, interaction prompts, 2E1C, 883A

Load `review-prop-s0/arrived.ss` from the 0038 warp to 4479.
The slot dump in `resume-light-inspect` locates B8CC at
(1408, 512, 1408) and its F099 drawable at 02010C78. Reposition
the Player and strike as in `resume-light-actions`. The sword
reaches `Breakable_OnHit`. The `resume-candlestick` replay then
supplies reaction bit 4 to that callback for the live slot.
`Breakable_React` and `Breakable_RunScript` call proc 4478 at
frame 1860. Compare `unlit.png` with `lit.png`. The flame appears
on the candlestick and the lit portion of the floor expands.
The renderer trace ties the candle and flame pieces to the slot's
drawable. The pool's other sprite resources are B991 and 999B.

Load `review-proximity/arrived.ss` from 0029 at 3955. In
`resume-hint-actions`, stand near E6CC and align the Player before
pressing A. At frame 1980 `DirectionalController_SelectTarget`
selects 0200E174 from the normal input handler. The same update
runs proc 3954, and `prompt-1-1.png` shows the Fireball heading.

For both position preparations, read the current Player pointer from
`gPlayers` at 02000580. Confirm it against the `Player_Update`
probe and add the position field offset 0x30. The actor address
varies by room and checkpoint.

### Floor trap contact and Bat Curtain debris follow-up

Tags: floor traps, Bat Curtain, Count, sprite attribution, contact

For the unforced trap transition, load the 0013 room state
`review-stage-pressure/idle.ss`. The `resume-floor-inspect` dump
locates Django and the first 2FA5 slot. Run
`resume-floor-walk/commands.txt`. This writes position (1152, 512,
896) to the Player and leaves the trap slot untouched. The original
idle handler enters the countdown at frame 1841. The slot starts
the press animation at frame 1852 and runs contact effects at 1858.
`trap.png` shows the active tile. The earlier renderer trace provides
the matching A945 screen pieces.

Warp 0014 to the Count rematch, scene 6401, entrance 47, story 55.
`resume-count-curtain` logs native creation of the 5734 Bat Curtain
and saves the running arena. In `resume-count-debris-fourth`,
position Django beside the fourth curtain and call its registered
hit handler at 08012D78 with slot 0200DD88. Its own update calls
08012B34 and loads DA6D at frame 1849. `Debris_Update` then runs
for the flying pieces. Compare `before.png`, `debris.png` and
`scatter.png`. At frame 1851, the renderer ties the curtain pieces
to 0200DD88 and the debris pieces to 02011C78, 02011D28 and 02011DD8.
The staged hit proves the ownership in its retail arena.

### Linear and gravity-driven mote isolation

Tags: particles, 1C1E, hardware OBJ, renderer, gravity

Load the 0029 snapshot at 3955 for each run. The
`resume-pixel-{control,contact,impact}/commands.txt` files use the
same initial state, camera and frame timing. In the two spawn runs,
invoke 0812D7A8 or 08125EC4 with the same position, velocity,
lifetime and frame kind. In the control, advance the same frames.
Both spawners use graphics 1C1E, so the same artwork cannot establish
which pool drew the screen pixels.

At frame 1869, compare the OAM attribute triples as multisets.
Both spawn runs add a single OBJ at index 3. Match its nontransparent
texture pixels against the screenshot with
`studio.experiments.obj_pixels.matching_objects` using the OAM,
OBJ VRAM, palette and DISPCNT dumps. The linear spawn changes 30
pixels in the rectangle bounded by (135, 87) and (141, 94),
with maximum coordinates excluded. The gravity-driven spawn
changes the same shape three pixels lower. Original probes show
the two separate motion callbacks on those frames.

Repeat each spawn run with `hideobj 3 1869` before running the
target frame. The observer hides the slot for scanline rendering
and restores the original hardware state. The resulting
`resume-pixel-*-hide/frame2.png` matches the no-spawn control exactly.
The controlled render establishes the effect identity. A shipped
trigger for the linear mote constructor remains open.

### Coffin Bike charge sprites

Tags: Coffin Bike, charge, sprite attribution, 409C, 1C1E

Load `targets/bike-lifeless/baseline.ss` from 0008 at 6274.
The unmodified B-hold replay runs `SpritePair_UpdateAnimation`
but the bike's primary kind is zero. The chase control at 6323
uses kind four. In `resume-bike-charge-render`, write kind two
to Glove motion at 0200EE01 and hold the original B input. The
existing charge value exceeds the threshold. The original
`ChargeMarker_TickHold` invokes the 1C1E mote spawner every
12 frames and the motes approach the rider.

After 60 held frames, `ActorMarker_ShowSlot` reveals slot zero.
`sprite-sources.tsv` attributes the bright 409C charge marker
near (90, 76) to 0200F4E8 from the first frame of the reveal.
Compare `before.png`, `gathering.png`, and `charged.png`.
The compact display slot was reserved during Glove initialization.
The file also contains a following ability effect, called from
`ActorAction_TrySpawn`, which uses its own 409C pool. The shipped
configuration selecting primary kind two remains unobserved.

### Nidhoggr burst release and Coffin Bike palette recovery

Tags: Nidhoggr, Coffin Bike, palette, 4147, contact burst

Load `bank/0046/checkpoint.ss` at 4915. The
`resume-grid-contact-inspect` snapshot has a live GridAttack actor at
0200DE24 and ContactBurst at 0200F3C4. In the matched release run
`resume-grid-scatter-release`, write 255 to its contact count limit
at 0200DFCA for three frames. The original grid actor calls
`ContactBurst_SetEnabled` with zero at frame 1930. The attached
task then calls `DirectionalParticle_Spawn` eight times with the
4147 sprite. The renderer submissions for the resulting slots appear
at frame 1933 in the same screenshot. The comparison run
`resume-grid-scatter-control` keeps the original threshold and
shows the intact contact burst.

Load 0008's 6274 Coffin Bike snapshot and compare the
`resume-bike-fade-{control,recovery}` runs. In recovery, enable the
secondary path, set action kind one and stage a one-frame recovery
timer. `Glove_UpdateStopping` invokes `PalFade_Start` on its live
motion at frame 1898 with palette rows 0378 and 0379. Compare
`initial.png` and `fading.png` for the gradual restoration.
The matched screenshots differ only around the bike and rider.
The early and middle OBJ palette dumps differ in the banks used
by those draws. At `finished.png` both the palette and the
screenshot match the no-fade run exactly.

### White Forest deposit item drop

Tags: loot deposits, White Forest, F1F9, item drop

Repeat the 0029 strike tape at 3905 as in
`resume-quarry-matched-drop/commands.txt`. Immediately after
`hit05.png` at frame 2051, set script counter `r708[0xE1]`
to 17. The deposit still takes the original sword damage.
The counter guarantees the drop branch for this configured
loot. At frame 2056, the F1F9 slot calls proc 1133.
At frame 2057, the original 0955 command reaches
`Trigger_CreateDropFromScript`. The deposit disappears and
a red drop remains in `hit07.png`. The earlier control used the
same attack tape and lacked the drop from this slot.
This verifies the loot-deposit role. The path describes that
role while its exact in-game label remains unknown.

## Pass 3 Player range

Tags: Link Player, Crossover Battle, Coffin Bike, timed sprite, Dark Loans

Search both half-open ranges before moving them. The existing checkpoint
and replay index has zero hits in the alternate Player scheduler loop
before the controlled actor test. The local Crossover round (0032 to
1656) runs `Player_Update` 0815F0D0. The Back Street cable match uses the
Coffin Bike scheduler on both units. The adapter-connected Crossover
session remains unobserved. The direct Thumb call and little-endian ROM
literal scan in `build/identify/pass3/constructor_refs.py` finds no
references to the alternate constructor 08243B0C. The sole pointer to
`Player_UpdateLinkCore` is 08243B44, inside that constructor.

```sh
build/venv/bin/python tools/whoruns.py 08237EC8-082389D8 08239130-08243C10 --functions
build/venv/bin/python build/identify/pass3/constructor_refs.py
build/venv/bin/python build/identify/pass3/replay_link_actor.py
```

The controlled `pass3-staged-isolated-django` replay starts from stock
0029 warped to White Forest room 3955. Its harness calls 08243B0C with
an emulated session record and native script keywords for slot one and a
position near ordinary Django. The constructor uses DE23 for the body and
loads D495 for the auxiliary sprite. It returns task 0200ED14. Restore the
original script and session globals and the published slot and count
before continuing. Arm the new actor's action initializer by setting its
byte at +464. The original scheduler runs `Player_UpdateLinkCore` then
`Player_UpdateLinkKind` on the new task starting at frame 1856. Compare
`before.png`, `second.png`, and `move.png`. The normal red Django and the
blue alternate body are both visible. This proves the actor and dispatch
family. The capture stages the constructor and initial state, so its normal
mode and shipped entry point remain open. Use `link/player` as an ownership
name for the alternate actor and its local effects and controls.

Pass 3's first and last boundaries also cover two independent units. The
seven files 08237EC8 through 082381C8 are a standalone timed sprite task.
`review-native-timed-visual` calls its original constructor in the 3955
room, then its sprite draws, ticks and expires. The original callers at
081CB3F4 and 081CBE30 are story-character state callbacks. Route those seven
files to `effects/sprites/timed_character_effects`. Start the alternate
Player block at 08238238. At the far end, 08243B54 is the stat gauge. In the
0032 Dark Card replay, `DarkLoan_DrawPreview` calls it during the loan offer.
Use `ui/pause/menu_sequences` for that file. End the alternate player block
at 08243B54, exclusive. The gap 082389D8–08239130 remains directional effects.

For the cable control, `build/identify/pass3/peer_range.txt` starts
Back Street from 0032 and 0070. The second trace (`peer_range-corrected.log`)
samples both whole ranges. At frames 3224–3313 there are zero executed
instructions from those ranges on either core. `Glove_UpdateTask` runs
on both. The first range sample used the wrong between-step PC adjustment
and logged one apparent hit in the gauge's literal pool. The corrected
host drains due events and uses the same next-instruction PC as the
headless harness. The connected race screenshots are
`build/identify/pass3/pass3-connected-race-{0,1}.png`.

## Pass 3 object paths, rows 1–11

Tags: scene objects, D9AE, F1F9, moving platform, passages, ice, traps, candlestick

Rerun 0029 to 3955 (`build/identify/pass3/replay_blocks.py`). Proc 3952
uses D9AE with name 4B5D and count two at frame 1473. The manager at
0200E4E4 runs `SceneObjects_InitPool`. Next frame E02A builds IDs EEC
and EED with 9D41 at slots 0200E574 and 0200E71C. The stride is 0x1A8.
In `pass3-d9ae-camera/commands.txt`, keep Django at the entrance and pan
the camera to the stones. At frame 1853 their draw records at 0200E604
and 0200E7AC each submit three parts. `pair.png` catches the two solid
stone blocks across the passage. After the staged removal of the first
slot, only the second drawable submits and `after.png` shows one rock.
GCL 3953 pans to the same pair and removes each ID at its fourth phase.
Use `objects/puzzle_blocks.c` for this live collision and graphics pool.

The remaining object leads cross-reference the following replay records
by their pass-three order. All paths in the table are under
`build/identify/replays/`.

| Pass 3 path | Evidence and destination |
|---|---|
| `objects/actor_group` | 0029→3905, `resume-quarry-matched-drop/hit07.png`. GCL 3904→1553→10801 reaches F1F9 at ID 5639. The normal sword contact breaks the deposit and the staged drop counter makes proc 1133 create the visible drop. Use `objects/loot_deposits`. |
| `objects/named_object` | 0042→4808, `review-platform-ride/ride.png`. A4FC ID 085F uses route state and B019. The puzzle text #7181 calls it a moving platform. Use `objects/moving_platforms`. |
| `objects/cell_gate` | 0042→4755, `review-passage/hit.png`. Proc 4752→1071 registers four 71BA edge slots next to the Wind Cannons. The controlled approach reaches the original passage predicate and clears the blocking cell. Use `objects/passage_barriers`. |
| `objects/bouncing_object` | 0046→4915, `review-bouncing-bombs/bounce.png`. The grid attack runs Spawn, TickActive and removal on the moving dark bombs. Mummy and Ratatosk projectile code also calls the pool. Use `objects/bouncing_bombs`. |
| `objects/breakable_prop` | 0029→3931, `review-trinity-release/dialogue.png`. 3DB0 comes from proc 1091. White Forest's sprite 474D and ID E912 run the shatter and unlock Trinity. Proc 1090 in the lead configures a separate hit switch. Use `objects/quest_barriers` for the multi-sprite barrier unit. |
| `objects/region_actor.c` | 0029→3955, `review-proximity/close.png`. Proc 1176 creates 377D and loads the large blue 863A ice boulder. Use `objects/ice_boulders.c`. |
| `objects/collision_controller.c` | 0029→3955, `review-native-collision-blast/created.png`. Proc 1180 creates 0866 E811. The staged registered collision callback emits its bright marker and delayed sparks. Proc 1179 creates the separate floating 95A8 crystal. Use `objects/strike_traps.c`. |
| `objects/directional_controller.c` | 0029→3955, `resume-hint-actions/prompt-1-1.png`. A selects the 883A sign position and opens the Fireball hint. Use `objects/interaction_prompts.c`. |
| `objects/tile_trigger.c` | 0013→3161, `resume-floor-walk/trap.png`. Original 2FA5 runs its press and contact from Django stepping into the slot after repositioning. The renderer binds A945 to the displacement. Use `objects/floor_traps.c`. |
| `objects/breakable.c` | 0038→4479, `resume-candlestick/lit.png`. ID B8CC uses F099 and its hit callback invokes the lighting script and brightens the doorway. Use `objects/light_fixtures.c`. |

## Pass 3 NPC paths, rows 12–15

Tags: Otenko, Kuro, Trinity, Sabata projection, BB34, D774, 8EA3, 5B01

1. `npc/scripted_npc` is Otenko's field actor. BB34 always loads 97D3 in
   its constructor. It appears in stock 0000's 2201 garage and 0085's 6612
   ending. The latter checkpoint runs `SceneNpc_OnModeUpdate` while Otenko
   is the named speaker in `bank/0085/checkpoint.png`. Proc 6317 uses BB34
   in a separate finale branch and reuses the same constructor.
2. `npc/tagged_npc` is Kuro. D774 is the constructor in proc 1790. The
   preceding 1789 resets related script variables. The 2537 speaker and
   the in-room render replay tie E00E and ID 51E2 to Kuro, as above.
3. `npc/story_character` is the shared cast dispatcher. Kind 27 is
   Trinity in the 3931 White Forest shatter cutscene. For kind 28, stock
   0046 invokes proc 4914 and 1677, allocates 59B0 at frame 1470, and
   loads 35E3 one frame later. `StateActor_StartKind28Animation4` runs
   on the same actor during the Nidhoggr room setup. At entrance 57 the
   story sequence plays Trinity's dialogue #7360 and #7362. See
   `grid-ident/message27.png`. For kind 29, warp 0001 to 6587 entrance
   62, story 1. 8EA3 allocates 177C and loads DE23 at frame 1473.
   `pass3-cast-kind29/line06.png` shows the purple Sabata projection.
   Text #8232 and #8233 name Sabata and refer to the phantom disappearing.
   These are witnessed kind assignments and the engine's other kind
   numbers still need a cast inventory. Keep the shared StoryCharacter
   destination and use the scoped prefix.
4. `npc/cutscene_actor` includes the animated Otenko tutorial controller.
   Original proc 1481 dispatches the boot tutorial through 1474. Proc
   1482 spawns 8AAB and sends 5B01 (`Actor_ConfigureFromScript`, 081EA1C0).
   The `review-native-tutorial` replay calls 1482 and draws Otenko in
   `created.png`. Keep the separate Params3D0 title display in `ui/title`.

Use `build/identify/pass3/npc_gcl.py` and `npc_calls.py` to list keyed
placements and callers. `replay_cast.py` holds the kind 28 and 29 tapes.
