# Cutscenes: the SceneMessages event tables

Tags: #scripts #scene #actors

Every scripted cutscene in the game is a data table played by one small
director, `SceneMessages` (`src/fn/sub_08020*.c`, all matched C). The scene
script starts it, names the scripts it may call, and gets control back when
it ends; between those, the table moves the camera, the characters, the
screen and the sound. `python3 tools/assets/cutscenes.py N` prints one as a
timeline. The Studio's Browse has every cutscene under **Cutscenes**, by
place, with its timeline, speakers and portraits. A worked example, the whole
prologue: [gcl/examples/prologue.md](../gcl/examples/prologue.md).

## Starting one

```
SceneMessages_ScriptStart(scene=2, step=0, ids=[0xd04c, 0x9786, 0x4b06, 0xe962, 0xa6ce],
                          scripts=[proc_6542, proc_1675, proc_1687, proc_1753, proc_1742], onfinish=proc_6538)
```

Command 7348 (`SceneMessages_ScriptStart`, `08020DF0`) reads keyword `d` (the
table index, 0–239), `c` (the step to start at), `p` (up to 16 ids), `P` (the
scripts, same order) and `e` (the procedure to run at the end). The director
is the receiver `0x4E69`, registered when a scene starts.

## The table

`gEventBytecodePtrs` (`08E8755C`) holds 240 pointers, one per cutscene; each
points to an array of steps (null-terminated), each step to a null-terminated
array of messages. `sub_08224EBC(n, step, i)` returns message `i` of a step.
All 240 slots are used: 3,651 steps and 7,790 messages. 220 have a
script start in the scripts (`cutscenes.py` lists them); the other 20 are
started with a computed index or not at all.

A message is `8 + 2 * nargs` bytes:

| Offset | Size | Field |
|---|---|---|
| 0 | u16 | receiver: an instance name (the NAME of `chara TYPE NAME`) or a manager's fixed name |
| 2 | u8 | receiver kind (below) |
| 3 | u8 | 1: the step ends when this message is finished |
| 4 | u16 | 0 |
| 6 | u8 | command (per kind) |
| 7 | u8 | nargs |
| 8 | s16[nargs] | arguments |

## The step loop

`SceneMessages_Update` (`08020B8C`) runs each frame. When the advance flag
(`+0x1E`) is set it moves to the next step and hands that step's messages to
their receivers (`SceneMessages_Enqueue`, double-buffered by
`SceneMessages_SwapBuffers`). A step with no messages ends the cutscene and
runs `onfinish`. Every receiver takes its messages in its own task and
switches on the command byte. A message with byte 3 set leaves the receiver
"pending" (`sub_08020D54`); when the receiver finishes that command it calls
`Trigger_ConsumePending(receiver, 1)`, which sets the director's advance flag
(`sub_08020D28`). So each step names the one message whose end moves the
cutscene on: usually a wait, sometimes a camera pan or an animation.

The director's own commands (kind 1, `SceneMessages_Dispatch`):

| Command | Meaning |
|---|---|
| 0 run | call the script of `ids[k]` (first argument = that id; the rest are the script's arguments), `SceneMessages_RunScript` |
| 1 wait | wait N frames (a step that waits N lasts N + 1 frames; checked against a headless run of the prologue) |
| 2 wait_signal | wait until a script calls `SceneMessages_ConsumeSignal` (command 2502) |

### Dialogue

The first script is usually the starter's own procedure, written as a
`match` on a counter (`r700[0x119]`). Starting the cutscene runs case 0; each
"run" of it from the table runs the next case, which either shows a line
(`sub_08030D9C(text=...)`, command 1C64, then `message_then`) or closes the
window (`close_message_window`) and sends the signal. The table pairs each
"run the dialogue script" with "wait for the signal", so a line stays until
the player presses A. `cutscenes.py` replays the counter to put each line in
its step.

## Receiver kinds and their commands

A kind is the class of the object that registered
(`SceneMessages_RegisterReceiver(id, kind)`, `08020CD4`). Names in `cutscenes.py`
are low confidence unless the handler is named; the rest print as
`cmdN<handler>`.

| Kind | Receiver | Registered by | Dispatcher | Commands used | Read so far |
|---|---|---|---|---|---|
| 1 | director `4E69` | SceneMessages_Initialize | SceneMessages_Dispatch | 3 | all |
| 2 | player characters (`F5EB` the player, `2B06` actor_name_of 102) | sub_0815D688 | PlayerCue_Dispatch (52 cases) | 29 | all handlers matched and named (table below) |
| 3 | registry actors | SceneActorRegistry_RegisterCategoryReceiver, Armor_Create | | 6 | |
| 4 | scene actors (Vanargand, Ratatosk...) | SceneActors_Initialize | SceneActors_DispatchCommands (67 cases, inline; NONMATCHING draft) | 43 | most set state 2 with a sub-state (0-8, 10-14, 18-24, 66); 2 place(x, y, z), 9 effect, 15/16 graphics variant, 25 flash; 26-65 do nothing here |
| 5 | NPCs (`place_character`) | SceneNpc_CreateFromScript, TaggedSceneNpc_Create | SceneNpc_DispatchCommands (68 cases; NONMATCHING draft) | 62 | the player's shapes: 1 stand, 3 face, 4 walk_to (speed 10), 5 run_to (speed 20), 25 pose, 26 pose_end, 64 place(x, y, z); 13-16 swap the palette; most others set state 0 with a sub-state |
| 6 | camera `FF40` | Camera_Initialize | Camera_DispatchCommands | 4 | all matched: 0 reset, 1 snap_to(x, y, z), 2 pan_to(x, y, z, frames), 3 `sub_08226474`(frames), 4 sway(a, b, mode) (guess), 5 follow_player(frames) |
| 7 | screen `730A` | sub_08021394 | SceneCommands_Dispatch | 8 | all matched: 0 particles_at, 1 particles_stop, 2 particles_on, 3 fade(out, speed, r, g, b, bg, obj, other, 0), 4 fade_palette, 5 blend_palette, 6 quake(strength, frames), 7 quake_stop, 8 flash, 9 done |
| 8 | audio `D3A9` | SceneAudioEvent_Register | SceneAudioEvent_DispatchPending | 9 | all (named handlers): 0 play_sound, 2 start_bgm, 4 fade_out_bgm, 6/9 channel 12 |
| 9 | scene panel | ScenePanel_RegisterReceiver | ScenePanel_DispatchCommands | | |
| 10 | group objects, staged effects, stacked actors | GroupObjects_Initialize and others | | 7 | |
| 11, 12 | | sub_08118890, sub_081202CC | | 7, 4 | |

Character instance names come from `actor_name_of` (proc 1154), a table from
a character number to a name hash; `cutscenes.py` prints `character N` for
those not identified on screen yet.

### Player commands (kind 2)

Handlers are `PlayerCue_*` (`0815AE94`–`0815C73C`); each sets a motion with
`sub_0813AFEC(p, motion, variant)` (`+0x457`/`+0x458`) and an update routine
in `+0x58C`. Directions are 0–7. Names with evidence come from screenshots of
the prologue (cutscenes 2, 3 and 5, `tests/prologue.txt`); `motionN` names
only say which motion plays (low confidence).

| Cmd | Name | Args | What it does |
|---|---|---|---|
| 0 | hide | | flag bit 0 of `+0x94`; off screen until placed again (cutscene 3) |
| 1 | place | x, y, z, dir | sets the position; dir unused |
| 2, 3, 4 | stand, face, idle | dir | motion 0 (variants 0, 0, 1) |
| 5, 6 | stand_v2, stand_v3 | dir | motion 0, variants 2 and 3 |
| 7 | walk_to | x, y, z | motion 1 to a point |
| 8, 9, 10 | motion7_front, motion7, motion7_end | | motion 7 facing 4 / 0, then variant 3 |
| 11–14 | kneel, kneel_down, kneel_gasp, get_up | dir | motion 0x15 variants 0–3 (Sabata wounded in 3, Django waking in 5); 13 plays sound 0xC6 |
| 15 | move_to | | motion 6 to a point |
| 16, 19 | motion29, motion29b | dir, n | motion 0x1D, diagonal facing |
| 17, 18 | motion30, motion30_end | dir, n, b | motion 0x1E; 18 moves variant 5 to 4 |
| 20, 21, 22 | motion7_turn, motion7_v7, motion7_v3 | dir | motion 7 |
| 23–26 | aim, shoot, aim_hold, aim_end | dir / dir, n | motion 3; 24 spawns effect 0x4B 150 ahead and 230 up with sound 0xCD (guess: the gun) |
| 27 | face_side | on | direction 5 (mirrored) or 3 |
| 28, 29 | motion11, motion10 | | motion 0xB / 0xA |
| 30 | form_motion | | motion 0xA or 0xB by `+0x418` |
| 31 | state_motion | | motion 0xD–0xF by state |
| 32 | pose | n (4, 5: n, a, b) | `sub_0815C73C` |
| 33, 34 | run_to, run | x, y, z / dir | motion 2 (guess: running) |
| 35–38 | windblown, _v2–_v4 | side / dir | motion 0 with update `sub_0814FAE8`: the scarf blowing in the wind |
| 39, 40 | stand_diagonal, stand_diagonal_b | dir | motion 0, diagonal facing |
| 41, 42 | motion17, motion17_v2 | | motion 0x11, facing 1 |
| 43 | shake_effect | | random offset, `sub_082315A8`, sound 0x191/0x133 |
| 44 | refresh | | `sub_0813D4E0` |
| 45, 46 | flag_5a1_on/off | | `+0x5A1` |
| 47–50 | overlay_on, overlay_set, overlay_off | n | `+0xAE4`/`+0xAD6` |
| 51 | motion38_to | x, y, z | motion 0x26 to a point, in state 2 only |

After a command the dispatcher calls `sub_0814F858` and `sub_0814F874`
(3, 5–11, 14, 16–26, 28, 33 and 35–42 only the first; 29–31 the second only when
the handler returns 0; 51 neither).

## Decompiling it

`python3 tools/assets/cutscenes.py --targets` lists each kind's registration,
dispatcher and handlers with their status. The player, camera and screen
receivers are matched. The scene-actor (`SceneActors_DispatchCommands`,
1,644 bytes) and NPC (`SceneNpc_DispatchCommands`, 1,552) dispatchers are
NONMATCHING drafts (81% and 71%): the scene actor's graphics-variant
branches and the NPC facing math still allocate registers differently. Open:
Vanargand (`7C56`) is sent scene-actor commands 51–62, which this dispatcher
ignores, so another receiver must handle them; the only other kind-4
registration is in `sub_08197890`.

## Tools

* `tools/assets/cutscenes.py`: the decoder (`N`, `N --json`, `--commands`, `--targets`).
* Studio: Browse › Cutscenes, `/api/cutscene/N`.
* `tests/prologue.txt`: a harness run of the whole prologue. Probing
  `SceneMessages_Enqueue` (`probe enq 080209D0`) logs the frame each message
  is delivered (`r1` is the message), which is how the timings above were
  checked.
