# Agent harness: play and probe the running game

`port/agent/play.py` drives the source port (the decomp's C compiled to
WebAssembly) from the command line, one short command per tool call. The game
runs in a daemon that stops at frame boundaries or breakpoints; each command
runs there and returns. Runs are deterministic (fixed clock, solar level and
inputs), so checkpoints are replays.

```sh
P="python3 port/agent/play.py"
$P start --save tests/saves/ShinBok2.sav   # daemon + live view on http://127.0.0.1:8766/
$P continue-save --shot                    # title -> file select -> Continue (into the field)
$P press down 30 --shot                    # hold Down 30 frames; prints the screenshot path
$P status                                  # frame, scene, player position, life, why it stopped
```

Needs `build/port/game/game.wasm` (`python3 port/build_game.py`, or
`start --build`) and `baserom.gba`. State lives under `build/port/agent/`.

For anything longer than one step, write JavaScript: `js` runs a script in the
game with typed access to every global, game functions by name, hooks, and
`yield` to run frames. A loop that walks, waits for a text box and reads a
struct is one tool call:

```sh
$P js 'yield* walkTo(1664, 2600); yield g.press("up", 20); return where()'
$P js -f probe.js            # a file; `js -` reads stdin
```

## Commands

| Command | Does |
|---|---|
| `start [--save F] [--time ISO] [--solar N]` | (re)start the daemon stopped at power-on |
| `run N [KEYS]`, `press KEYS [N] [--after M]`, `seq 'a:6 _:20 down+left:30'` | advance frames holding keys (`a b select start right left up down r l`, joined with `+`) |
| `shot [FILE]` | PNG of the last frame (default `build/port/agent/shot.png`) |
| `read WHERE [LEN] [-t u8/s16/u32...] [--deref]`, `write WHERE VALUE [-s 1/2/4]` | memory by address or symbol |
| `call FUNC [ARGS]` | call any game function now; prints r0 |
| `c 'C statements'`, `c -f file.c`, `p 'EXPR' [--format]` | run C in the game (below) |
| `break FUNC... [--exit]`, `clear`, `continue [--frames N]` | stop at a function's entry/exit; continue runs until one hits |
| `trace start [--root FUNC]`, `trace stop [--depth N]` | call tree with arguments and results (`build/port/agent/trace.txt`) |
| `script start`, `script stop` | script procedures called and GCL commands run, with names (`build/port/agent/script.txt`) |
| `callproc ID [ARGS]`, `warp SCENE ENTRANCE` | run a script procedure; warp the way a door does (proc 945) |
| `goto X Z [Y]` | move the player (struct Player position) |
| `checkpoint NAME`, `restore NAME`, `restart [--save F]` | deterministic replay points |
| `play`, `pause` | free play at 60 fps; the viewer page takes the keyboard |
| `sym REGEX/ADDR` | look up functions and data |
| `js CODE`, `js -f FILE [--bg] [--max N]` | run JavaScript in the game (below) |
| `hooks [--off ID/all]` | JS hooks and background scripts still installed |

While stopped at a breakpoint every command works (read the arguments, call
functions, run snippets) and `run`/`continue` resume from the breakpoint.

## JavaScript

`js` runs inside the game's worker (`gameapi.mjs`), at a frame boundary.
Without `yield` it is a console: the last expression is the result. With
`yield` it is a script that runs the game (`--max` frames at most, default
72000; `--bg` returns at once and leaves it running).

| | |
|---|---|
| `$.gPlayers[0].position.x` | typed globals: structs, arrays and pointers follow the game's headers; assignments write |
| `g.at(addr, 'struct Task')`, `view.$cast('struct X')` | a typed view of any address |
| `view.$show(depth)`, `view.$get()`, `view.$addr` | layout dump, plain object, address |
| `g.fn.Script_Call(945, 0)`, `g.call(name or addr, ...)` | call a game function (r0 back) |
| `g.u8/u16/u32/s16(addr[, value])`, `g.hex(addr, n)`, `g.bytes` | raw memory |
| `g.name(addr)`, `g.addr(name)`, `g.find(regex)` | symbols |
| `g.on(fn, (args, info) => ..., {exit, once})`, `g.off(id)` | a callback at every entry (args r0..r3) or exit ([result]) of a game function; return `'stop'` to stop there |
| `yield N`, `yield g.press('down+b', N)`, `yield g.tap('a')` | run frames, holding keys |
| `yield g.until(() => cond, max)` | run until a condition (checked every frame); true if it happened |
| `yield g.untilCall('Fn', {exit})` | run until a function is entered; returns `{args, frame}` |
| `yield* skill(...)` | a generator from the library (below) |
| `g.c('C statements', ['player.h'])` | compile and run a C snippet from JS |
| `g.shot(file)`, `g.status()`, `g.callproc(id, ...args)`, `print(...)` | daemon helpers |

Hooks and background scripts stay installed across commands until `g.off()`
or `hooks --off all`. Every `js` command is logged, so checkpoints replay
scripts exactly.

### Library

`port/agent/lib/*.js` are skills every script sees by name; a file is a plain
script that adds functions to `exports` (it is reread when it changes, so an
agent can add a skill and use it in the next command). `core.js` has:

| | |
|---|---|
| `where()` | frame, scene, life, player x/y/z |
| `tasks()`, `taskOf(addr)` | every scheduler task with its update function; the task whose memory holds an address |
| `drawables()` | projected sprites: screen box, world position, owner task |
| `project(x, y, z)`, `unproject(sx, sy[, y])` | world ↔ screen (`SpriteOam_ProjectPosition`) |
| `yield* walkTo(x, z)` | walk there with the d-pad (learns directions, sidesteps, reports `blocked` with no map) |
| `message()`, `yield* advanceText()` | message window state (open, string id); press A until it closes |
| `yield* continueSave()` | power-on → title → file select → Continue |
| `yield* sprites()` | one frame of `Sprite_QueueFramePieces` calls with their OAM boxes |

Types come from `gametypes.py`: each prepared header compiled with debug info,
DWARF folded into `build/port/agent/types.json` (about 1400 types and 560
typed globals; `start` rebuilds it when headers change).

## In the browser

`python3 port/serve.py` runs the same module and API in the page's game
worker. Below the game, the Studio panel has a Console (the same JavaScript,
at 60 fps; scripts that yield run alongside play), Watch (expressions updated
every 10 frames), Tasks, and Inspect (outlines of projected sprites named by
their owner task and of every OAM entry; click for the struct, Shift+click to
move the player to the ground under the cursor). Subtitles show the English
line of the string the message window shows, beside the game's own text box,
using the fan translation beside the checkout (`../boktai3trans`, as the
studio does); the worker hooks `Text_LookupString` and posts the id when
`MessageWindow_GetStringAtOffset` returns, so menus don't trigger them; menu
labels come from `Menu_DrawText` and stay while the BG0 cells they drew do.
Their style, the keybinds and the display options live in `port/web/ui.js`.
Inspect names each sprite's task by hooking the render pass's OAM emitters
(`sub_08000430`, `Sprite_QueueFramePieces`) and the actor panel types the task's
object with `types.json` `params` (each function's first struct parameter);
`port/web/actor.mjs` is the worker side, `port/web/actor-panel.js` the page and
`port/web/actor-overview.js` its Overview (fields sorted by name and type into widgets). From DevTools or a browser-driving agent: `await game.js('where()')`.

## C snippets

`c` compiles C against the game's own headers (`include/`, the prepared copy
under `build/port/game/include`) and runs it inside the live game: globals at
their GBA addresses, struct layouts as agbcc lays them out, and any game
function called by name, declared or not (an undeclared one takes and returns
`int`). `print()` (printf subset), `dump(ptr, len)` and `RESULT(x)` come from
`port/agent/agent.h`; `-i player.h` adds a header.

```sh
$P c -i player.h 'struct Player *p = gPlayers[0]; print("%d %d %d\n", p->position.x, p->position.y, p->position.z);'
$P p 'gScriptVm.result'
$P c --trace 'Script_Call(945, 0);'     # and the call tree of what it ran
```

How: `snippet.py` compiles the snippet like a game unit (`bind_ir`,
`instrument_mmio`), turns every game function reference into its GBA address
and every call through an address into `port_icall`, the path ROM function
tables take, and links a side module that imports the game's memory, with its
own data and stack at 0x02800000 (free GBA address space).

## Hooks

`port/build_game.py` puts `port_hook_enter(address, r0..r3)` at the start of
every game function with a GBA address and `port_hook_exit(address, r0)`
before each return (`port/game/hooks.c`). They go in before optimization, so
inlined copies keep them. Idle they cost nothing measurable (the browser page
uses the same module). Breakpoints are a byte per ROM halfword; trace events
go to a ring at 0x02100000 (about 300,000 events).

## Studio

When the studio runs (`python3 tools/studio/serve.py`), the daemon connects to
its Centaur port (8643, or `CENTAUR_LINK_PORT`) and the Live page shows the
port like mGBA: player position on the stage map, story state and variables,
text the game fetched; warp, callproc, keys, poke, setvar and bookmarks
(checkpoints) come back as commands. `play.py play` keeps the game running for
it.
