# Worked example: the prologue, from New Game to the Sealed Prison

What happens between choosing New Game and taking control of Django in his
cell, decoded from the scripts and the cutscene tables and checked against a
headless run (`tests/prologue.txt`, about 25,000 frames, A pressed every 50).
Script excerpts are the readable view (`python3 tools/assets/gclview.py PROC`),
trimmed; cutscene excerpts come from `python3 tools/assets/cutscenes.py N`
([docs/CUTSCENES.md](../../docs/CUTSCENES.md) explains the tables). Names in
`proc_N` form have no recovered name yet.

## The story

A text crawl tells of the world sinking into darkness. Sabata, the Dark Boy,
wakes in a dream and finds Vanargand, the Beast of Destruction, rising in
front of him. He fights it and cannot hurt it. The Boy in White, Ratatosk the
Puppeteer, appears and says Vanargand is an Eternal that the Dark Boy cannot
kill, that he needs Sabata's cooperation and does not need his permission;
Sabata screams as the screen turns red. In the dark, Sabata faces Django (the
Solar Boy, who defeated the Count) and Otenko and says he will go on
regardless. Then the title card,
*Boktai 3: Sabata's Counterattack*, and Django wakes in the Sealed Prison.

## Scenes and timing

| Frame | Scene (`GameState+0x5A4`) | What it is |
|---|---|---|
| 90 | 1430 | title screen |
| 1363 | 1470 | file select and name entry |
| 3036 | **6551** | the text crawl |
| 6778 | **6554**, entrance 58 | Sabata's dream: cutscene 0 |
| 9770 | 6554, entrance 47 | the fight with Vanargand, then cutscene 1 |
| 12061 | 6554, entrance 59 | cutscene 2: the Puppeteer |
| 15589 | 6554, entrance 60 | cutscene 3: Sabata, Django and Otenko |
| 20908 | **6559** | the title card |
| 21328 | 2698, entrance 1 | the Sealed Prison: cutscene 5, the tutorial |

The prologue is three scenes, 6551, 6554 and 6559 (the Studio's Places ›
Prologue). Scene 6554 is entered four times; each entrance runs its own setup
and its own cutscene. Until this example the scene list missed all three
(their `scene_start` flags are 56, not 1; `tools/mgba/scenes.py` now accepts
any flags, which brought the list from 354 to 496 scenes).

## 6551: the text crawl

```
def proc_6551():                       # the scene entry
    scene_start(flags=56)
    ...
    proc_6552()                        # proc_927(57): the entrance-57 setup
    play_bgm(id=56)
    spawn sub_08034030(name=0xc334, index=0, s=text(2157), value=data(9966), flags=1, procedure=proc_6553)

def proc_6553():                       # the crawl's last page has been shown
    sub_0822B0B0(f=12)
    spawn sub_082284B4(name=0x5f25, delay=130):   # a 130-frame timer
        e:
            sub_08227D1C(0)
            door_transition_951(scene=proc_6554, entrance=58, 570)
```

`sub_08034030` is the crawl object: it pages through the strings from 2157
(an age of darkness, the dark clan, the warrior who fell to a comrade's
betrayal, the Beast of Destruction waking) and calls `procedure` at the end. The title card in 6559
is the same object with string 2171.

## 6554: one scene, four entrances

```
def proc_6555():                       # the entrances: position, then (on arrival, start)
    entrance_at_916(x=1571, z=1571, facing=4, 0, 128, 0, 58, proc_6544, proc_6545)
    entrance_at_914(x=1280, z=1280, facing=0, 47)
    entrance_at_916(x=1280, z=1280, facing=0, 0, 64, 0, 59, proc_6547, proc_6548)
    entrance_at_916(x=1088, z=1472, facing=1, 3, 128, 0, 60, proc_6549, proc_6550)
    set_continue_proc(proc=proc_6556)

def proc_6556():                       # where each part continues to
    match appearance_point:
        case 58: door_transition_943(scene=proc_6554, entrance=47, 570, 0, 0)
        case 47: door_transition_943(scene=proc_6554, entrance=59, 570, 0, 0)
        case 59: door_transition_943(scene=proc_6554, entrance=60, 570, 0, 0)
        case 60: door_transition_943(scene=proc_6559, entrance=61, 4, 0, 0)

def proc_6557():                       # Vanargand, by entrance
    match appearance_point:
        case 47:
            Hud_SetMode(hud_mode=1)
            spawn Vanargand(name=0x7c56, D=0, m=0, L=(30000, 30000, 30000), a=(1, 2, 4, 1, 1, 1))
            spawn sub_082284B4(name=0x43a6, delay=1800, script=proc_6546)
        case 58: spawn Vanargand(name=0x7c56, D=1, m=0)
        case 59: spawn Vanargand(name=0x7c56, D=2, m=0)
```

Each cutscene ends in a small procedure (`proc_6536`–`6539`: restore the HUD,
then `run_continue_proc`) that moves to the next entrance. Entrance 59's end,
`proc_6538`, also calls `advance_story(value=2)`. Vanargand is spawned in a
different mode (`D`) for each part. In the fight (entrance 47) it has 30,000
life and the HUD is on, and a 1,800-frame timer starts cutscene 1. The fight
cannot be won: it ends after 30 seconds whatever the player does. In the run,
cutscene 1 starts 1,816 frames after the scene loads.

## Cutscene 0: Sabata wakes (entrance 58)

Started by `proc_6545` → `proc_6540` (`SceneMessages_ScriptStart(scene=0, ...)`).
36 steps; the receivers are Sabata (the player instance `2B06`), Vanargand,
the camera, the screen and the sound.

```
    0  proc_1675(0); audio.start_bgm(78, 0); wait 1f
    1  Player 2B06.idle(4, 0); wait 120f
    3  say "Boy in Black: "Where..."; until the player presses A
    4  Player 2B06.face(5, 0); wait 30f          # looks left, right, left, right
    ...
   13  camera.pan_to(1144, 406, 1144, 200); wait 120f
   14  Player 2B06.walk_to(1264, 256, 1264, 0); wait 120f
   15  say "Boy in Black: "Is this... a dream?"; until the player presses A
   23  audio.start_ch12(106, 0); audio.play_sound(1425, 0); wait 0f
   24  Vanargand.cmd51(); screen.quake(90, 10); wait 60f     # it rises
   26  audio.fade_out_ch12(8, 0); screen.quake(90, 20); Vanargand.cmd52() and wait
   29  say "Boy in Black: Is this what called me here?"; until the player presses A
   34  audio.fade_out_bgm(5, 0); audio.play_sound(641, 0); screen.fade(1, 6, 31, 31, 31, ...); wait 180f
```

The last step fades to white over 180 frames. The run's message log matches
step for step: every `wait N` lasts N + 1 frames, and the dialogue steps last
until the A press.

## Cutscene 1: the fight ends (entrance 47)

The timer's script `proc_6546` starts cutscene 1 at step 1: Sabata takes a pose, the
camera pans to Vanargand, the screen shakes (`quake(50, 50)`), Vanargand runs
two attack commands (53, 54, each ending its step when its animation ends),
and the screen fades to red.

## Cutscene 2: the Puppeteer (entrance 59)

`proc_6542` brings in four more scripts: `proc_1687` spawns Ratatosk (actor_name_of
104) at a position, and `proc_1742` and `proc_1753` call `sub_0822AEF8` with
742 and 1366. Lines from string 5481 on: Sabata recognizes the beast; the Boy in
White says Vanargand is an Eternal that the Dark Boy cannot kill and that he,
the Puppeteer, needs Sabata's cooperation; Sabata screams as the screen fades to
red (`screen.fade(1, 7, 31, 0, 0, ...)`).

## Cutscene 3: in the dark (entrance 60)

`proc_6549` places Sabata (`2B06`) as a second player character, and
`proc_6550` starts cutscene 3, the longest (63 steps, strings 5522–5575). It is
built from repeated scene cuts: fade to black, move everyone
(`Player.cmd1(x, y, z, dir)` sets a position and facing), snap the camera,
fade in, one or two lines. The receivers are the player (`F5EB`), Sabata
(`2B06`) and Otenko (actor_name_of 1, a scene NPC placed with
`proc_1676`; identified in the Sealed Prison, where he is the only other
character). `proc_1683`–`1685` add ScriptKeyframe sprites (actor_name_of
116 and others) for the last scenes; Otenko speaks the last line. At the end the continue procedure goes to scene 6559.

## 6559 and on

The title card is the crawl object again (string 2171, "BOKTAI 3 SABATA'S
COUNTERATTACK"); its end runs `proc_542` and `door_transition_951(scene=proc_2698,
entrance=1, 7)`. Scene 2698 opens with cutscene 5 ("???: "..."", Otenko
waking Django) and the tutorial.

## What this showed

* Cutscenes are data, not script: a 240-entry table of steps and messages
  that a small matched director plays (docs/CUTSCENES.md). The scripts only
  start one, supply the dialogue and spawn the cast.
* Scene detection missed every scene whose `scene_start` flags are not 1:
  the prologue, boss rooms and story scenes (142 in all).
* Still unknown: Vanargand's commands (scene actor dispatcher `sub_08022F88`,
  inline, unmatched), most player commands past the simple ones, and the NPC
  commands.
