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

The Bronze Armor reward in scene 2803 reused storage after `Armor_Destroy`
unregistered the body. `TerrainOverlay_Update` still had that body in its
actor table. At 08003D00, the ROM kept the stale body in r4 and read
800010E0 into r1 as its sprite pointer. On wasm the same access trapped
at frame 69,282. The pending 08003C20 patch validates each overlay entry
against the live body registry. The guarded copy passed the stock 1,660-byte
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

The movies, function coverage, mGBA checkpoints and demo live under
`build/autopilot/` in the worker checkout. To reproduce the run before
integration, build the port with a generated copy containing the pending
terrain overlay patch, then run the planner from the root.

```sh
mkdir -p build/autopilot/story-overlays
cp tools/autopilot/pending_src_fixes/08003C20-terrain-overlay-stale-body.patch build/autopilot/story-overlays/
build/venv/bin/python port/build_game.py -j2 --source-patches build/autopilot/story-overlays
build/venv/bin/python port/agent/gametypes.py --jobs 2
make -f tools/emu/Makefile build/emu/harness
node tools/autopilot/autopilot.mjs story --until-story 42 --verify --coverage
```

The default room budget is 96. `--scenes 8` reproduces the earlier smoke
budget. A larger budget gives the explorer more room after story 42.
