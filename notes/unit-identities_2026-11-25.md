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

The chapter order can also be cross-checked against the fan walkthrough.
[Casebier's Boktai 3 walkthrough](https://gamefaqs.gamespot.com/gba/926529-shin-bokura-no-taiyou-gyakushuu-no-sabata/faqs/49880)
