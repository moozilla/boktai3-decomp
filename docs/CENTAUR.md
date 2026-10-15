# Centaur: playing with the decomp

Centaur is the human-and-machine play setup: mGBA running
`tools/mgba/centaur.lua`, linked to the local studio. While you play, it shows
English subtitles over the Japanese game, logs what the game loads, keeps
savestate bookmarks and records inputs. The studio's Live page (port 8642)
receives the logs, sends commands back and analyses scene captures, so the
person playing and the agent working on the code see the same thing.

## Quick start

```
tools/mgba/build_mgba.sh          # once: mGBA 0.11-dev with overlays and breakpoints (~5 min)
make english                      # optional: the English ROM (needs ../boktai3trans)
tools/mgba/play.sh                # Japanese ROM + subtitles; or: tools/mgba/play.sh build/english.gba
```

`play.sh` generates `build/centaur/data.lua` (strings, translations),
starts the studio if needed, opens http://localhost:8642/live and starts mGBA
with Centaur. Keep the battery save next to the ROM (`baserom.sav`) as usual.

Playing the **Japanese** ROM with subtitles keeps recordings and bookmarks valid
for the matching build; the English ROM reads better but differs in timing.

## In the game window

* **Subtitles**: each string the game fetches (`Text_LookupString`, `0821ADD4`)
  is drawn in English in a small box just under the place where the game
  draws it, and stays only while its glyphs are on screen. Every string on
  screen gets its own box (menus show one per item). How: the text renderer
  puts glyphs into BG0's map with `sub_08218FB4` (16×16) and `sub_08218F2C`
  (8×16), tile x/y in r1/r2, as palette 15 entries for tiles 0x100–0x1FF (the
  glyph cache, `Text_DrawGlyph`). Centaur gives those cells to the last string
  fetched and drops a string when its cells no longer hold glyph entries in
  VRAM. A string that never draws glyphs (names, control strings, text in
  graphics) shows briefly at the bottom edge if it is longer than a name.
  `subs("both")` adds the Japanese, `subs("jp")` / `subs("off")`,
  `subpos("top")` moves that fallback box to the top, `subid(true)` shows
  string numbers. `substyle("shadow")` (outlined text, the default) or
  `substyle("box")`, `subsize(1.0)` (1 = 6 GBA pixels) and `suboffset(px)`
  set the look; the Live page has the same controls, and they are kept in
  `build/centaur/settings.json`. Text the game draws as sprites (the region
  select screen's button labels, for example) is not subtitled yet.
* **HUD** (top left): recording timer, current map and tileset, god mode, and
  whether the studio is connected. `hud(false)` hides it.

## Console commands (Tools › Scripting)

| Command | Does |
|---|---|
| `help()` | list commands |
| `bm("name")`, `go("name")`, `bms()` | save / load / list bookmarks (savestate + screenshot): the warp list |
| `capture("tag")` | dump palettes, VRAM, OAM, I/O, RAM and a screenshot for the studio |
| `god(true)` | keep life at maximum |
| `rec_start("name")`, `rec_stop()` | record inputs from here: `build/centaur/NAME.ss` + `NAME.txt` starting with `loadstate NAME.ss`; `go()` while recording adds a `loadstate` line |
| `rec_start("name", "reset")` | record from power-on instead |
| `replay("build/centaur/x.txt")` | play an input script inside mGBA (harness format plus `loadstate`) |

Recordings replay in the headless harness too:
`tools/emu/harness baserom.gba build/centaur/NAME.txt OUTDIR`. `loadstate PATH`
(relative to the script) loads an mGBA savestate; Centaur's states come from
mGBA 0.11, so `tools/emu/Makefile` builds the harness against the
`tools/mgba/build_mgba.sh` library when it exists (with that library's own
compile definitions: `ENABLE_DIRECTORIES` changes `struct mCore` and is not in
its `flags.h`), else against the system mGBA 0.10, which cannot read them.
`first_level_from_start.txt` replays to the same final frame (32208) and
screen in both.
| `where()` | frame, map, tileset, palette, life, last text |
| `callproc(id, {args})` | run a script procedure (also on the Live page) |
| `warp(scene[, entrance])`, `scene()` | go to a scene the way a door does, player placed (see below); current scene |
| `peek*/poke*`, `watch(a)` | memory helpers |

## The Live page (http://localhost:8642/live)

* **Now on screen**: every string whose glyphs are on screen, in English and
  Japanese with its tile rectangle, linked to the text editor; subtitle mode buttons.
* **Game**: map (with its label), scene, tileset, palette, life; capture (with
  an optional label), god mode, recording (from here, or from power-on).
* **Text the game fetched**: newest first, with a divider wherever the scene or
  map changed. **Resources loaded**: with a preview; click to open it in the
  side panel.
* **Screen** (the middle column): the game screen with the inspector below
  (see the Captures page). **Refresh** captures it as `live`; **follow**
  refreshes every 2 s.
* **Bookmarks**: thumbnails; click to jump there. **Recent captures** link to
  the Captures page.
* **Resources loaded** has a filter per type; types that are not graphics
  (`hp`, `anm`, ...) open in a data view (description, LZ77-decompressed hex).

Links to maps, sprites and palettes anywhere in the studio open in a side
panel (Esc closes it; ⌘-click opens a tab). Every page has the same menu.

* **Map around the player**: the Maps view of the current stage (chosen by
  the scene) with the player and the screen rectangle drawn over it, and the
  stage's doors, entrances, items and enemies (see below); it follows the
  player and switches stage when the scene changes. Centaur sends a `player`
  message every 6 frames: `GameState+0x30/+0x32/+0x34` and the BG2 camera
  (`0x03004CB0`/`CB2`, ASSETS.md "BG scroll in RAM"); the studio serves the
  latest at `/api/live/player`.

## Stage maps (http://localhost:8642/gfx#maps)

A stage (map, tileset, HP) is drawn with its scene data from
`tools/assets/stages.py` (`/api/stagemeta`, built on first use into
`build/assets/stages.json`, rebuilt when those tools change) and its terrain from
`tools/assets/hp.py` (`/api/hp/HASH`): doors (orange, labelled with the
destination scene; click to open that stage with the arrival entrance
circled), entrances (green, with their ids), items and chests (hover for the
item and its flag), enemies with their patrol paths, and optionally the
terrain cells (walls red, slopes yellow, floor green) and named boxes. Hover a
marker for the conditions it depends on. When several scenes use a stage,
pick one; its palette (`proc_888`) colors the map unless a capture or the
palette menu is chosen. `#maps/MAP-TILESET-HP/SCENE.ENTRANCE` links to a scene
and entrance.

Testing beside a running studio: `serve.py --port 8652 --link-port 8653` and
`CENTAUR_LINK_PORT=8653` for the mGBA it should talk to.

## The Captures page (http://localhost:8642/captures)

A capture list (label, group, filter; the stage survey is hidden unless
asked) and an inspector:

* the screenshot with outlines on every sprite and on the text the game drew:
  hover for the resource or string, click a sprite for its detail (drawn from
  the captured VRAM, the resource frame, a label/notes editor, "Has text…"),
  shift-click to combine several, click a string to edit it in the side panel;
* **Sprites** or **OAM entries**: by default the OAM entries of one sprite
  are grouped (same package, boxes touching or a tile apart in a row) and the
  group's frame is the one whose parts use most of its tiles, so a button
  icon and its label read as one sprite; each is drawn inline with its
  resource, frame, label, palette bank and pool entry;
* **Text**: every string on screen, English and Japanese;
* **BG layers**: each background map as it sits in VRAM (unscrolled: the
  capture cannot read the scroll registers);
* **Palettes**: BG and OBJ palette RAM, which resources match each bank, and
  the sprite palette pool entry (`9B1B`) the game loaded into each OBJ bank.
  The sprite list shows that entry per sprite.

The analysis remembers the stage palette per map (`data/stage_palettes.csv`)
and, per sprite resource, the whole OBJ palette RAM with the bank its palette
bank 0 used (`build/centaur/sprite_palettes.json`). Package parts carry their
own banks, so a single learned bank used to color the title subtitle wrongly.
The Maps view can color a map with the palette RAM of a capture that showed it.

Sprite packages no longer need a capture for their colors: each names its
palette in the `9B1B` pool (see ASSETS.md, Sprite palettes), which the Sprites
view uses by default ("in-game"), with the capture's colors or the palette
menu as alternatives. The pool itself is browsable at `/gfx#pool`.

## Actors, scripts and code (http://localhost:8642/actors, /scripts, /code)

Cross-referenced by `tools/assets/actors.py` (also a command line:
`actors.py actors`, `procs`, `code ADDR`), from `data/actor_registry.csv`,
the generated disassembly (`gen/code.s`), the script image and the text bank.

* **Actors**: the 722 entries of the script actor registry, which scripts
  create with `chara KEY NAME` or `cmd_B745 KEY` (every B745 operand is a
  registry key; enemy families are created that way). Each shows its callback
  and *loader* (the function the callback hands its newly allocated actor to),
  the sprites they name (16-bit literals in the callback, loader and their
  nearby callees that are sprite package or large sprite hashes, with a
  thumbnail in captured colors when the survey saw them), the procedures that
  create it, the scenes that reach those procedures, and a label/notes editor
  (kind `actor`). Filters: with a sprite, created by a script. Keys are hashes
  of the original names, which are not recovered.
* **Names from the game** (the first row of the Actors list): the monster list
  (strings 9838–9921) and the character list (9818–9833), with the registry
  entries linked to each. The links are candidates through the family's
  sprite, from the visual identifications in ASSETS.md; the code that adds
  9838 to an enemy kind is not found (see `actors.py`'s docstring for what was
  ruled out).
* **Scripts**: every procedure in the gcx image, numbered as calls use them
  (one above `gcx.py dis` headers). Each shows callers and callees, the scenes
  that reach it, the actors it creates, its strings (opening in the text
  editor) and debug prints, and the body with procedures, strings and actor
  keys linked; label/notes kind `script`. Enemy spawner procedures (flags 5
  and 6) have no script callers: the engine or stage data starts them, so no
  scene is listed for them.
* **Code** (`/code#ADDRESS`): any function's disassembly as `tools/asmat.py`
  prints it, its callers and callees, and its C from `src/` when it is
  decompiled (marked when it is a NONMATCHING draft). Function addresses
  anywhere in the studio open here in the side panel.

## Notes on anything

`data/notes.csv` (kind, key, label, notes) labels maps, sprite packages, large
sprites, palettes and resources; the label shows in lists and searches and is
edited in place in the Graphics page, the capture inspector and the side
panel. Capture labels, groups and notes live with the capture
(`build/centaur/captures/NAME/info.json`).
* **Graphics with text**: sprites and maps tagged as containing text (from a
  capture or the Graphics page), with their English and a status; stored in
  `data/gfx_text.csv`, the to-do list for graphics translation.

## For agents

* `tools/mgba/headless.sh SCRIPT.lua [ROM] [SAVE]` runs a Lua script in the
  patched headless mGBA (breakpoints on, fixed clock), unthrottled. A script
  can `dofile` Centaur, `replay` a route, `capture` and `os.exit(0)`.
* While the person plays, `POST /api/live/cmd` with `{"c": "eval", "lua": "..."}`
  runs Lua in their emulator (for example `capture("x")` or `where()`); the
  result arrives as an event in `GET /api/live`. `build/centaur/centaur.log`
  records what Centaur did.
* mGBA quirks found while building this: socket I/O inside a breakpoint
  callback, and adding callbacks from inside a callback (as `socket.connect`
  does), crash mGBA, so Centaur queues messages and connects through the raw
  socket. Headless mGBA crashes without a video buffer; the patch adds one.

## Scenes, warps and the stage survey

`warp(N, E)` goes to scene `N`, arriving at entrance `E`, the way a door
does: it runs `proc_945(N, E, 0)`, the game's own door transition without a
trigger box. That calls `proc_953`, which puts the entrance in `r708[0x70]`
(the number the scene's entrance procedures test to place the player), and
`proc_952`, which starts the transition actor that loads the scene. Without
`E` it uses the scene's default entrance from `build/centaur/scenes.lua`
(`tools/mgba/scenes.py`: the first unconditional entrance that
`tools/assets/stages.py` finds); a scene with none falls back to the older
"continue" warp, which loads the stage without a player. The Live page lists
the scenes with a click-to-warp list, and clicking an entrance on a map warps
there. Story flags stay as they are, so a scene can look different from a
real playthrough.

Verified headlessly: from the `start` bookmark (scene 2698), `warp(2705, 57)`
creates the player at (640, 256, 2688) and walks him in to (640, 378, 2438),
exactly as the door in `first_level` does; `warp(2712, 8)`, `warp(2712)`
(entrance 7) and `warp(3243, 126)` (the late field) place a playable player
too, and so does `warp(2712)` from the field of the late-game test save.

What was missing before: the older warp wrote the scene request words
directly, so `r708[0x70]` kept the entrance of the last real door; no entrance
procedure of the new scene matched it, nothing placed the player, and the
player object was never created.

`tools/mgba/headless.sh tools/mgba/survey.lua` warps through every scene from
the field of the test save and captures each one (about ten minutes). The
analysis finds the stage map decompressed in RAM and the palette resource of
its BG palette RAM, so every surveyed stage gets its real palette in
`data/stage_palettes.csv` and the Maps view, and the Live page's scene list gets
thumbnails. Some scenes move on to another scene by themselves (shown as
"→ N"). The survey of 2026-10-14 captured 165 stages and 98 sprite palettes,
including twelve enemy families (ASSETS.md). A stage's palette can differ
between runs in its first hex digit (`17EE`, `37EE`, `57EE` for the same
stage): probably time-of-day variants of one palette, so the recorded palette
is the one seen at that moment. Run `python3 tools/studio/live.py analyse-all` if the studio was not
running during a survey.

## Running script procedures

`callproc(id, {args})` diverts the CPU at the next entry of
`TaskSystem_RunFrame` (`0821A054`, which takes no arguments) into
`Script_Call(id, args)` (`0821AD08`; its first argument is the procedure
number, looked up by `sub_0821ACDC`), which returns to RunFrame's entry, where
the second breakpoint hit restores the real return address. Arguments are
built at `0x0203FF00`. Verified headlessly: procedure 916 ran exactly once and
the game continued. Procedure numbers are the ones calls use (one more than
`gcx.py dis` headers).

## Next

* Warps are solved (see "Scenes, warps and the stage survey"). For the record,
  a door does: exit trigger → `Script_Call(860)` (GameState+0x740 goes 7 → 8,
  the back entrance) → request `0x030053E8 = 0x11` with the next scene
  `0x030039F0` → the player object pointer `0x02000580` becomes 0 →
  `Script_Call(2705)` from `08225173` → the entrance procedure matching
  `r708[0x70]` places the player → a new player object 11 frames after the
  request. **GameState+0x30/+0x32/+0x34 is the live player position**.
* `first_level` was recorded from a reset with `go("start")` partway, before
  recordings could start from a bookmark; replay it as `loadstate
  bookmarks/start.ss` followed by its inputs after the first wait (it stays in
  sync: the door at about 1,950 frames in).
* Event flags (`GameState +0x720`–`+0x730`) as a named, editable list.
* Sprite palettes per enemy family from captures, to settle which enemy is which.
