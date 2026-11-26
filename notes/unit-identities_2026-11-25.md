# Unit identities from checkpoint replays

Tags: #actors #scene #assets #tooling

The proposed layout mixed encounter names, sprite guesses and code families.
The runtime records are in [symbols/identities.csv](../symbols/identities.csv).
The layout changes are in `build/layout/corrections.csv`. Paths under
`build/identify/` refer to local replay outputs. The original ROM and the
translation's checkpoint bank are read-only inputs.

## Reproduce the capture

Run at the decomp root. The translation is two levels up in a nested worker
worktree. Substitute `../boktai3trans/saves` from the main checkout.

```sh
build/venv/bin/python tools/whoruns.py --scan-bank ../../boktai3trans/saves
build/venv/bin/python tools/whoruns.py BossSlot2_UpdateFrame
build/venv/bin/python tools/whoruns.py 081EC380-081F2CD4 --functions
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
`src/ui/pause/menu_sequences`. The latter also has account/retry animations
at 0816AE38–0816B390, and the range divisions are recorded below.

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

Neutral A, quick-selected Sol Trance followed by A/B, the five standard
swords, and the gun frames sampled above did not enter the burst or spiral
callbacks at 08143604–08149DF0. The ROM stores those callbacks in player
motion tables at 08610B94 and following. Their placement in the gun list
still needs specific gameplay triggers. The six measured projectile and
charge files are supported by these traces.
