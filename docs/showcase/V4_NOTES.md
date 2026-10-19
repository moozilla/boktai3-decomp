# Showcase v4: Claude's Quiz Show (playable)

v3 was a video. v4 is a hack you play: pick "Explore the Dungeon" on the
Sealed Prison's dungeon menu and Django lands on the Pile Driver arena,
recolored in Claude's palette, where Claude (the v3 Claudesona sprite) hosts a
six-question quiz about what the decompilation found. Right answers play the
game's jingle and drop a treasure chest; wrong ones summon Claude-colored
ghouls, and the show goes on when they are all down. "Not now" sends Django to
the real prison. Code: `tools/showcase/v4/` (quiz.py builds it, play.py has the
reusable parts, record.py records a scripted run, bps.py makes the patch).

## Things learned (engine facts the hack depends on)

* **Growing tables instead of borrowing ids.** The string table (gcx section 1)
  and `gEventBytecodePtrs` (one literal at 08224EE0) have no bounds checks, so
  copying them to free space with extra entries gives new string ids (9977+)
  and cutscene slots without touching anything the game uses.
* **Runtime dialogue** is `proc_1016(0,1)`, `r700[0xb.1]=1`,
  `command 1C64 r: text`, `proc_1019(next)`; a choice menu is
  `proc_1191(name, handler, 0, 0, 0)` + `command 71E7 r: text` with `<ALTER>`
  options, the choice in the handler's arg1. The last `proc_1191` argument is
  the cursor's starting option. About 26 characters per line.
* **Menu scenes can't host dialogue**: a message from the world map (2570/2571)
  leaves a black screen; the dungeon menu's Explore (call 2587, switch on
  gs[0x5AC]) is a clean place to branch into a new stage.
* **Ghouls** (command 05A5): `D:` is the on-death procedure (actor+0x1F8, run
  from SceneActorReaction_BeginDefeatBurst); `H=0` makes them stay down
  (`H=1` ghouls get back up); `C` is palette pool minus 402.
* **Chests**: `proc_1119(0, item, x, z, 2, 1, 0, 0, 0, 0)`; flags 0x2000001
  suppress the drop when no mission is active.
* **Sounds**: `command 1870 i: 662` right-answer jingle, `i: 402` wrong.

## Scripted play and recording

`tools/mgba/scenario.lua` gained ops for a bot player: `until` (wait for a
script variable, tapping A), `goto` (isometric steering), `swing`, `fight`
(walk to each point and swing until a variable reaches 0) and `keylog` (write
every frame's keys as harness `hold`/`wait` lines). The hack sets r700[0x346]
to 1 while a choice menu is open and 2 when the show ends, so the bot never
answers by timing. The harness, built with `-DUSE_FFMPEG`, replays the key log
from the bot's savestate and records H.264/AAC frame-exactly.

A `fight` has to stop the frame its variable hits 0: B pages through dialogue,
so a swing that outlives the last ghoul answers the next question blind.
