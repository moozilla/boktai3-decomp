# Flight recorder

Record a session of the source port, keep every write to game memory, and answer
"why does X have value V at frame F" with a dynamic backward slice printed as the C
statements, with their runtime values, that produced it. `--check` validates the slice
by perturbing its causes in a replay.

```sh
python3 port/build_game.py --record          # build/port/game-rec/game.wasm (instrumented)
python3 port/agent/gametypes.py              # build/port/agent/types.json (typed names)
P=build/venv/bin/python
$P -m tools.recorder record field --save tests/saves/ShinBok2.sav --script my.js
$P -m tools.recorder verify field                          # write log complete?
$P -m tools.recorder history field 'gPlayers[0]->position.x' --changes
$P -m tools.recorder who-wrote field gSolarMeasurement 470
$P -m tools.recorder why t1 gSolarMeasurement 470 --check
$P -m tools.recorder serve field                           # the Recorder page, :8765
```

Scripts are the agent's JavaScript dialect (`port/agent/README.md`: `yield N`,
`yield g.press('down', 30)`, `yield* continueSave()`). Places are addresses
(`0x0300425C`, `0x0203C9A4:2`), globals with field paths (`gGameState->unk5A4`,
`gTaskLists[3].head`) or object labels at that frame (`Player#77.position.x`).

## How it works

**Instrumented build.** `port/build_game.py --record` compiles every unit with `-g`,
runs `opt -passes=sroa` (locals become SSA values, so the remaining loads and stores
are real memory traffic) and `instrument.py`, which calls `rec_rt.c` at every store,
load, memcpy/memset, bus access (MMIO, keys, the solar sensor GPIO), basic block and
function entry/return. A site id is kind (3 bits) | unit (13) | local (16); each unit's
`build/port/game-rec/rec/UNIT.facts.json` has every instruction with its C line and
column, block labels and site ids (`facts.py` maps the prepared source's lines back
to the original file).

**Modes** (`rec_rt.c`). Record logs stores to GBA RAM always, and to IO and the
module's static data only from game-side units: the emulated hardware's own writes
(HBlank DMA, DISPSTAT, audio, timing) follow from logged RAM and would be 95% of the log.
Trace logs every event of the game-side units (and the hardware's RAM writes).

**Sessions** (`runner.mjs`, `build/recorder/sessions/NAME/`): per frame keys, solar
level and RAM hash; the write log (16-byte rows: frame, site, address | width, value);
snapshots every 60 frames with the wasm call stack they were taken at; screens every
30 frames; memory the script edited. About 2 MB per 10 s on the title screen, 55 MB in
the field. `verify` applies the log to each snapshot and compares with the next.

**Replay** (`runner.mjs serve`, `emu.py`). A snapshot restores linear memory, not the
wasm call stack (locals, return addresses), so it is restored only where the live
stack (the wasm frames of a JS stack trace in the frame import) matches the one it was
taken at; otherwise the replay runs forward (from power-on if need be). Every replayed
frame's RAM hash must equal the recording's, unless the replay was perturbed
(`solar`, `keys`, `poke`), in which case the first differing frame is reported.

**Lift** (`lift.py`). Static RAM by global name and field path (types.json from the
prepared headers' debug info); heap objects by what points at them: tasks in
`gTaskLists` typed by their update function's parameter (`Player#77`), and typed
pointers followed from globals a few levels deep (`gGameState->unk5A4`).

**Why** (`slicer.py`). Re-traces the frames involved (cached under
`build/recorder/tmp/traces/`) and walks back from the store that last wrote the
target: SSA operands to their definitions, phis through the predecessor block that
ran, parameters into the caller's call, call results into the callee's `ret`, loads to
the last store to their bytes (in the frame, or earlier through the write log), copies
narrowed to the bytes that matter, and control dependences (postdominators) to the
deciding branch and its condition. Pointer null checks are not followed. A statement
is expanded once per address: a run of the same store to the same address (a counter,
a position) is counted through the write log and the slice continues at the store
that started it; other repeats are reported as earlier cycles. Inputs: the host's
solar level (via the sensor GPIO), buttons, ROM data, memory from the save, arguments of
code the port entered (interrupts).

`--check` replays with one cause changed per run: the host solar level from the first
sensor read, a bit of each earlier-frame store in the slice, and the value of each
collapsed run at the frame boundary before the target. Links inside the target frame
cannot be perturbed (replays change state only at frame boundaries).

## Limits

* Byte-precise, not bit-precise: when `opt` packs two fields into one integer (a
  `{x, y}` pair stored as an i32), asking about one field also pulls in the other.
* The trace starts at a frame boundary: values held in wasm locals of functions
  already running (the main loop) and port stack slots written before it are inputs.
* A snapshot's call stack must match the live one; long sessions whose main loop
  changes call depth fall back to running forward.
* Rebuilding the wasm changes the stack signatures: re-record sessions after a rebuild.
