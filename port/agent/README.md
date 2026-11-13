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

While stopped at a breakpoint every command works (read the arguments, call
functions, run snippets) and `run`/`continue` resume from the breakpoint.

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
