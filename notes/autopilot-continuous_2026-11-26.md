# Continuous Autopilot replay through Lifeless Town

Tags: #port #autopilot #verification #scene

Each story run started with a blank cartridge save. `--verify` replayed the
recorded input polls on the original ROM and compared story, scene and player
position. The run used the source port and the worktree-local mGBA harness.

| Run | Story | Scene | Port frames | ROM functions executed |
|---|---:|---:|---:|---:|
| Initial eight-room run | 9 | 2738 | 36,115 | 2,140 |
| Terrain overlay repaired | 24 | 2814 | 69,581 | 2,653 |
| Town and bike task views restored | 41 | 3028 | 200,772 | 3,604 |
| Permanent skeleton defeats recognized | 42 | 3073 | 176,907 | 3,631 |
| Default continuous route | 50 | 3154 | 214,322 | 3,913 |
| Live floor-trap routing | 51 | 3154 | 212,902 | 3,893 |

The Bronze Armor reward in scene 2803 reused storage after `Armor_Destroy`
unregistered the body. `TerrainOverlay_Update` still had that body in its
actor table. At 08003D00, the ROM kept the stale body in r4 and read
800010E0 into r1 as its sprite pointer. On wasm the same access trapped
at frame 69,282. The 08003C20 repair (now in `src/scene/terrain/terrain_overlay.c`
under `UBFIX`) validates each overlay entry against the live body registry. The guarded copy passed the stock 1,660-byte
unit check. A port rebuild with that overlay reached the seal chamber and
matched the original ROM.

The source layout pass renamed the `WorldMapMenu`, shop controller and
`CoffinBike` types. Using those recovered types restored the live cursor
and rider used by the existing planner. The run toured San Miguel at
story 30, healed from 1 to 41 life through the pause menu, and finished the
coffin bike course. It reached the skeleton fight in scene 3028, past the
previous endpoint in scene 3016.

The final ROM dump in 3028 had one of two defeats recorded by proc 1148.
Skeleton 118D had 1 HP, hidden control `0x39` and a revival delay of 20,000.
Skeleton 118C still had 133 HP. `Skeleton_UpdateDefeat` records a defeat
and disables revival at delays above 19,999, even though it leaves one HP.
The raw lock-on registry still listed the hidden actor, so matching its HP
alone left the planner targeting an actor the gate had already counted.
The target filter now checks the defeat state, control kind, action flag
and revival delay from the recovered skeleton type. It excludes the actor
from both registries. The continuous replay defeated both skeletons and
took the exit to scene 3035. The area trigger and dialogue there raised
story 42 before the player entered scene 3073.

The raw mGBA scene checkpoints are keyed to input polls. Saving through
the first poll that saw a scene captures its state after the transition.
All 102 scene checkpoint labels in the story-42 replay matched the actual
scene word in the saved EWRAM, covering 61 distinct scenes.
The execution-only trace mode retains all execution bitmaps and skips
80 MiB of read-map output per replay. A 706-tick comparison kept the same
execution maps, RAM dumps and screenshot. Full tracing took 1.086 seconds
and execution-only took 1.019 seconds in that check.

The default run proceeded through 3073, rode the return lift in 3078,
and collected key 44 in 3087. It unlocked the forward door in 3035,
defeated the armor in 3095, and reached story 50. The next key detour
led to scene 3161. Its CD23 approach crossed trap cell (4,3). GCL proc
3164 returned the player to scene 3154 at entrance 82, repeating the
same route until the room budget ran out. The story-50 endpoint matched
and all 133 checkpoint labels matched across 72 scenes.

The floor-trap planner reads the active `TileTriggerManager` slots and
excludes their horizontal contact squares. `FloorTrap_TickIdle` at 08107320
and `FloorTrap_ApplyContactEffects` at 08106E98 query the player within 128
units of the trap on each horizontal axis. The route around the four active
traps reached scene 3168 and collected the Yellow Key. The story event
returned the player to scene 3154 at story 51. All 120 saved entries match
their scene labels, covering 73 scenes. The 20 functions unique to the
story-50 run include the trap dispatcher, particles and cue animations.

The movies, function coverage, mGBA checkpoints and demo live under
`build/autopilot/` in the worker checkout. The terrain overlay repair is
in the source under `UBFIX`, so the port build needs no extra patch.

The port takes its code addresses from `build/boktai3.elf`. A worktree
without a built base clones the main checkout's ELF, which can predate the
function renames. Every renamed function then drops out of the port's code
table, and the first VBlank traps in `invoke` ("interrupt handler at unknown
address 0x8213fd9"). Build the ROM in the worktree before the port.

```sh
make disasm && python3 tools/build.py
build/venv/bin/python port/build_game.py -j2
build/venv/bin/python port/agent/gametypes.py --jobs 2
make -f tools/emu/Makefile build/emu/harness
node tools/autopilot/autopilot.mjs story --until-story 51 --verify --coverage
```

The default room budget is 96. `--scenes 8` reproduces the earlier smoke
budget. The story-51 run uses 91 rooms, so longer runs from a blank save
need `--scenes 400`.

## Past story 51 (2026-11-28, unverified)

Continuing from the story-51 movie, the planner reached story 54 in the
Count's arena (scene 3247). It entered with 37 of 100 life, and the Count
killed the player with 197 HP left. After Continue, the Count registers
only after its intro script runs. The room loop then saw no targets and no
doors, so it waited while the player took damage. Two general fixes
followed. Before a boss or curtain fight, the planner now heals to three
quarters of max life, using up to three items. When a boss is live in a
room where no door works, it fights again, up to three times per room. That run healed from 37 to 100 and beat the
Count with 29 life left. It reached story 57 in 3219 at frame 244,134 (port only, not yet
verified on the ROM). Story 58 is set in scene 3124 in the same dungeon. Exploration instead
left for the overworld and looped between the hubs 2569 and 2553.
A story-goal fallback (story.py: the scene that sets the next value) now
takes over after 24 rooms without a beat. It is still being tested.
