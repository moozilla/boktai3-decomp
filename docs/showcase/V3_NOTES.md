# Showcase v3: what was built, what was cut, what was learned

Tags: #showcase #scripts #tooling

The third showcase video, "The Stranger", follows [V3_PLAN.md](V3_PLAN.md).
The output is `~/Movies/boktai3-showcase/boktai3-showcase-v3.mp4` (about 8
minutes; the second draft, after the user's review of the first) with a
chapter list beside it. Everything ROM-derived stays under
`build/showcase/v3/`. The tools are in `tools/showcase/v3/`:

| File | What it does |
|---|---|
| `show.py` | scenes, cutscene timelines (actions at frame offsets compiled into director steps), dialogue, keyframe sprite commands |
| `romx.py` | new archive entries (sprite packages), palette pools appended to the pool table, packages from indexed images, package aliases in another pool |
| `art.py`, `art/` | the stranger, the Claudesona and her portraits as 1x frames; [art/PROMPTS.md](../../tools/showcase/v3/art/PROMPTS.md) has the Codex prompts |
| `faces.py` | redraws the Claudesona's face with the same features in every frame |
| `pixelate.py`, `compare.py` | an upscaled Codex sheet to true-resolution indexed sprites; a 1x/4x style check next to the game's sprites |
| `proofs.py` | the throwaway scenes that proved each mechanism |
| `part1.py` | Part 1: one ROM with every set piece as its own scene; `record PIECE` records one |
| `record.py`, `run.py` | warping in (mGBA headless), then recording in the harness with adaptive dialogue timing |
| `tour.py`, `tour_stops.py` | Part 2: the Studio at 1280x800 in a 1920x1080 frame with a caption panel, plus the live edit |
| `live_demo.lua` | the game side of the live edit (Centaur, linked to the Studio) |
| `edit.py` | the cut, the chapters and the loudness |

Rebuild: `part1.py build`, then `record PIECE` for each piece (`cold doors
night marionettes count ride reveal coffins vride sunback title music`, and
`credits 3 3300`), `tour.py` (with the Studio running, in three runs:
`--only 0..15`, `--only 16`, `--only 17,18` to `rec/studio_pages`,
`studio_live`, `studio_cards`), then `edit.py`.

## Kept and cut

The order: the cold open (a line of dialogue, the ghoul wobbles, the stranger
floats in and snaps, the fan), the door chase (they run for each exit; a
line each; a closet), night on cue (the townsfolk hurry home and out again),
the marionettes (the cast rise and orbit the stranger, facing it), the Count
(his lunge reversed, his colors cycled, a coffin), a rider on Vanargand's
head, the overload (a patch of strange light, the gauge past full, the cloak
burns away), the encore (a ring of the game's coffins, purified into ones
and zeros; Claude rides Vanargand), "Want to see?" and the sun handed over,
the title in the game's own text crawl, the Studio, and the decomp's credits
in the end roll's crawl over a parade of Claudes.

Cut:

* **The switch light show.** Floor switches call our procedures fine
  (`place_floor_switch`, proc 1084, takes on_press/on_release), but a cascade
  needs many switch and gate objects in one room and a way to set their state
  from a script; the game's own cascade (`proc_1104` activating objects
  `6A40`... one by one) was found too late to build on.
* **The unshipped room.** Every stage a script loads is reachable from some
  scene; the 61 maps no script loads are menu screens, title cards and
  backdrops, not rooms. Area 2 ("unused room", scene 2824) is the ending's
  sky stage.
* **The ROM as a stage.** Not attempted.
* **A Mode 7 wobble for the marionettes.** Not attempted; they bob and the
  ring breathes instead.

## Findings

* **The SUN gauge** is GameState+0x876 (0-10), the sunlight from the sensor:
  the calibration word at `*0x030053F8 + 4` minus the reading (it is -2 in
  the test save, so no light registers until it is set, e.g. 0xE8). The
  drawer (in `sub_08164864`) writes a cap, `level` lit cells, empty cells to
  10 and a cap, with no clamp: hold the sensor task's state byte
  (`*0x03002604 + 0x19`) at 1 and the level at N > 10 and the bar runs past
  its frame and wraps onto the next row.
* **Time of day**: the stage palette follows the sun clock's light phase,
  `0x0300543E`: values 0-5 select its time-of-day variants (in the test
  save's field 0 and 5 looked alike, as did 2 and 3; 6 and 7 blank it). The
  game reads the cartridge clock only about every ten seconds, so the night
  piece holds the phase instead.
* **OBJ palettes**: a scene has 16 OBJ palette slots and typically uses
  three, so at most about 13 extra palettes show at once (the fan uses ten
  hues twice). The pool table (`9B1B C5E9`) starts with a u16 count, so new
  pools can be appended after the game's 955.
* **Message windows** open at the top of the screen while story progress is
  190-191 (`proc_1019`); setting it in a scene keeps the HUD in view.
  Message speed is GameState+0x12 (0 fastest). `Text_LookupString` fires
  when a page starts typing, which is how the recorder times each page.
* **Live edits**: mGBA's `cart0` domain writes through `GBAPatch`, so
  Centaur can patch strings and palette pools in the running game. Objects
  keep their own copies of their palettes (the player's, for damage tints),
  so a pool edit also rewrites those copies in work RAM.
* **Lua key presses** (`emu:setKeys`) persist until changed: release them
  explicitly or the game sees one long press.
* **Motion while a message is up**: the director waits on the message, so
  the marionettes' orbit and the light's color cycle run on the game's
  repeating timer (`chara 430D`, its `e` block returning the next delay;
  `show.track`).
* **The coffin** Django drags is large sprite `2499` (every angle and the
  lid opening), drawn in pool 643 + its kind; the shop's coffins are the
  pools after it.
* **Vanargand** (scene actor 7C56): 51 rise, 52 and 59 bring in the left and
  right claws, 57 lowers the head, 56 raises it high; a rider follows the
  head by a curve measured from a fixed-camera recording (`proofs.vseq`).
* **NPC facing**: the town NPC packages' frames 0-4 run up, up-diagonal,
  side, down-diagonal, down, with the side frames facing left
  (`proofs.facing` draws a ring to check).
* **Screen fades** tint sprites drawn in new palette pools with the BG
  group, not only the stage.
* **The text crawl** (`chara 3DF7`, the prologue's and the end roll's) takes
  its pages from `s` and its timing from a binary record `t` (per page:
  hold frames, then 2, 4, 30, 8, 60, 32; -1 and six zeros end it); lines are
  not centered. The end roll's scene mode has no map camera, so the credits
  run in a scene of our own on the dream stage (black without Vanargand).
* **Cutscene limits**: at most four messages per receiver per step
  (`SceneMessages_Enqueue`) and 16 scripts per cutscene, so `show.py` folds
  all script actions at one instant into one case of a single procedure.
