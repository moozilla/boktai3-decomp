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
  is drawn in English at the bottom of the screen. `subs("both")` adds the
  Japanese, `subs("jp")` / `subs("off")`, `subpos("top")` moves the box above
  bottom-of-screen UI, `subid(true)` shows string numbers.
* **HUD** (top left): recording timer, current map and tileset, god mode, and
  whether the studio is connected. `hud(false)` hides it.

## Console commands (Tools › Scripting)

| Command | Does |
|---|---|
| `help()` | list commands |
| `bm("name")`, `go("name")`, `bms()` | save / load / list bookmarks (savestate + screenshot): the warp list |
| `capture("tag")` | dump palettes, VRAM, OAM, I/O, RAM and a screenshot for the studio |
| `god(true)` | keep life at maximum |
| `rec_start("name")`, `rec_stop()` | record inputs (from a reset) as a harness script in `build/centaur/` |
| `replay("tests/x.txt")` | play a harness input script inside mGBA |
| `where()` | frame, map, tileset, palette, life, last text |
| `callproc(id, {args})` | run a script procedure (also on the Live page) |
| `warp(scene)`, `scene()` | experimental scene load (see below); current scene |
| `peek*/poke*`, `watch(a)` | memory helpers |

## The Live page (http://localhost:8642/live)

* **Now on screen**: the current line in English and Japanese, linked to the
  text editor; subtitle mode buttons.
* **Game**: map, tileset, palette, life; capture, god mode, recording.
* **Remote pad**: press buttons from the browser (arrows, Z/X = B/A, A/S = L/R,
  Enter = Start; Shift for a long press).
* **Text the game fetched** / **Resources loaded**: live logs; click a string to
  edit it, a resource to open it in the Graphics page.
* **Bookmarks**: thumbnails; click to jump there.
* **Captures**: the screenshot plus analysis: the palette resource that the BG
  palette RAM came from (remembered per stage in `data/stage_palettes.csv`, then
  used by the Maps view), and each on-screen sprite matched to its resource
  with its real 16 colours (kept in `build/centaur/sprite_palettes.json`, then
  used by the Sprites and Large sprites views).
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

`warp(N)` requests scene `N` the way the script `load` command does (see
ENGINE.md, Scenes): the stage loads with its real palette, but the player is
not spawned, so it is for looking, not playing. The Live page lists the scenes
(`tools/mgba/scenes.py`) with a click-to-warp list.

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

* Real warps. `warp(N)` now replays what "continue" does (`sub_081E132C` at
  `081E14E0`): `0x0300546C = 0`, `Script_Call(*0x020003A8)` (the continue
  procedure, 1436 in the test save), `0x0300523C = 0`, request `0x11`, next
  scene `N`. The stage loads; the player object (`0x02000580`) is still not
  created, also with `GameState+0x30` (the saved position, s16 x/y/z) seeded
  from one of the scene's entrances. Tried and ruled out: request word 1 vs
  `0x11`, calling an entrance procedure (`916`) after the load. Next: trace a
  real door (walk through one in a recording, `emu:setWatchpoint` on
  `0x02000580`) and diff what it sets.
* Event flags (`GameState +0x720`–`+0x730`) as a named, editable list.
* Sprite palettes per enemy family from captures, to settle which enemy is which.
