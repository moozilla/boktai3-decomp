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
| 2 | player characters (`F5EB` the player, `2B06` actor_name_of 102) | sub_0815D688 | sub_0815D6B0 (52 cases) | 29 | 2 stand(dir), 3 face(dir), 4 idle, 7 walk_to(x, y, z), 15 move_to, 32 pose, 47–50 overlay on/off |
| 3 | registry actors | SceneActorRegistry_RegisterCategoryReceiver, Armor_Create | | 6 | |
| 4 | scene actors (Vanargand, Ratatosk...) | SceneActors_Initialize | sub_08022F88 (67 cases, inline) | 43 | |
| 5 | NPCs (`place_character`) | SceneNpc_CreateFromScript, TaggedSceneNpc_Create | sub_081D01C8 (68 cases) | 62 | the player's shapes: 1 stand, 3 face, 4 walk_to, 25 pose, 26 pose_end |
| 6 | camera `FF40` | sub_08227478 | sub_08226930 | 4 | 0 reset, 1 snap_to(x, y, z), 2 pan_to(x, y, z, frames), 5 follow_player(frames) |
| 7 | screen `730A` | sub_08021394 | sub_0802123C | 8 | 0 particles_at, 1 particles_stop, 2 particles_on, 3 fade(out, layers, r, g, b, ...), 4 fade_palette, 5 blend_palette, 6 quake(strength, frames), 7 quake_stop, 8 flash |
| 8 | audio `D3A9` | SceneAudioEvent_Register | SceneAudioEvent_DispatchPending | 9 | all (named handlers): 0 play_sound, 2 start_bgm, 4 fade_out_bgm, 6/9 channel 12 |
| 9 | scene panel | ScenePanel_RegisterReceiver | ScenePanel_DispatchCommands | | |
| 10 | group objects, staged effects, stacked actors | GroupObjects_Initialize and others | | 7 | |
| 11, 12 | | sub_08118890, sub_081202CC | | 7, 4 | |

Character instance names come from `actor_name_of` (proc 1154), a table from
a character number to a name hash; `cutscenes.py` prints `character N` for
those not identified on screen yet.

## Decompiling it

`python3 tools/assets/cutscenes.py --targets` lists each kind's registration,
dispatcher and handlers with their status. About 15.9 KB, 40% matched. The
largest are the dispatchers: player `sub_0815D6B0` (1,432 bytes), scene actors
`sub_08022F88` (1,644, its commands inline), NPCs `sub_081D01C8` (1,552). The
player's 51 handlers are small (100–400 bytes) and half are matched already.
Matching a dispatcher names its commands at once, and every cutscene in the
game reads better for it.

## Tools

* `tools/assets/cutscenes.py`: the decoder (`N`, `N --json`, `--commands`, `--targets`).
* Studio: Browse › Cutscenes, `/api/cutscene/N`.
* `tests/prologue.txt`: a harness run of the whole prologue. Probing
  `SceneMessages_Enqueue` (`probe enq 080209D0`) logs the frame each message
  is delivered (`r1` is the message), which is how the timings above were
  checked.
