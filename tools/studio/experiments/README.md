# Studio Experiments

Run the same Boktai 3 state with different inputs, then link a measured change
to the instruction that wrote it. Experiments run the stock ROM locally in
headless mGBA. Results stay under `build/studio/experiments/runs/`.

## Run and inspect

```sh
build/venv/bin/python -m tools.studio.experiments proof
build/venv/bin/python tools/studio/serve.py --no-preload
```

The proof tests pressing Up from one state in the field and in the pause menu,
then forks directly from the captured paused frame and tests reopening the
field. It also compares the exact frame at which a tap and a held attack
initialize contact. The command prints paths to the contact sheets,
transcripts and JSON results. Each trial directory has numbered PNGs.

Open `http://127.0.0.1:8642/experiments` for the notebook. Run the example,
scrub the synchronized playbacks and select a field value. The sidebar shows the
last observed store that changed it, the actual written values, any active
probed functions, and a C excerpt when scope reproduces the ROM bytes. **Fork
frame** starts an editable experiment from the selected trial and frame. Code's
**Probe this function in an experiment** preloads the selected C entry point.

## Author a recipe

Copy [pause.json](pause.json) or download a run's recipe. The same schema is
accepted by Studio and the CLI.

```sh
build/venv/bin/python -m tools.studio.experiments checkpoints
build/venv/bin/python -m tools.studio.experiments fields 'gPlayers[0]->position.x' 'gGameState->life'
build/venv/bin/python -m tools.studio.experiments run ./my-experiment.json
build/venv/bin/python -m tools.studio.experiments show RUN_ID
build/venv/bin/python -m tools.studio.experiments snapshot RUN_ID menu 60 'gGameState->life'
```

- `checkpoint` selects power-on, the reference save, or an available story-bank
  save. Captured mGBA bookmarks from Centaur are also available when present.
  Use `--trans PATH` on CLI runs for a translation checkout outside the usual
  locations. The `continue` option tries the reference title-screen
  sequence. A checkpoint may need extra `prepare` input, which runs once before
  the branches split. The first image shows where the recipe actually starts.
- `duration` and `sample_every` count emulated VBlanks. All trials share the
  same sample axis. Inputs are `wait N`, `hold KEYS N`, `tap KEYS` (hold for two
  frames, then release for eight), and `lux N` (raw cartridge threshold
  0–255, lower for brighter). Join keys with `+`. A shorter trial waits for the
  remainder of its duration.
- `observe` takes named 8-, 16-, and 32-bit integer fields. The path uses C
  field operators and constant array indices. Stock agbcc measures offsets,
  index bounds, widths and signedness. A pointer chain can become unbound
  during play. Include more than one field for a synchronized comparison.
- `probe` takes function names. Each entry records its VBlank and registers.
  On stores, the observer reports active entries from the configured probes,
  using observed return addresses and stack depth. Calls between those entries
  can be absent from the path.
- Optional `checks` turn a claim into a comparison. Select a trial, a pair of
  sample frames using `from` and `to`, and either a watched `field` or probed
  `function`. The comparison uses the field delta or the number of entries
  in the half-open frame interval. Operators are `eq`, `ne`, `lt`, `le`, `gt`
  and `ge`. A changed pointer binding leaves the field delta unresolved.
- A `fork` has the form `{"run": "RUN_ID", "trial": "TRIAL_ID", "frame": 60}`.
  Use a capture from the same ROM that mGBA can restore. The playback preserves the recorded sensor threshold when starting a new fork.

## Bring in a play session

Use **Record from here** in Studio Live with Centaur connected. Stop the
recording, then select it from **Example questions and recordings** in
Experiments. The saved recording and a no-input control replay from the
same captured state. You can edit the trials or select other named fields
before running. The importer uses up to 1,800 controller frames and accepts
continuous recordings. Each trial uses the recipe's RTC and cartridge settings.

```sh
build/venv/bin/python -m tools.studio.experiments import-live recording-NAME > build/recording-recipe.json
build/venv/bin/python -m tools.studio.experiments run build/recording-recipe.json
```

## Tool API

All requests use the local Studio server. Supply JSON for POST requests.

| Method and path | Use |
| --- | --- |
| GET `/api/experiments/catalog` | Find checkpoint IDs, RAM symbols and recipe limits. |
| GET `/api/experiments/example` | Get a complete runnable recipe to adapt. |
| GET `/api/experiments/examples` | List built-in example IDs. Append an ID to the example route. |
| POST `/api/experiments/fields` | Resolve a JSON array of named paths to their target ABI layouts. |
| GET `/api/codemap/fn/NAME` | Read a function's C, disassembly and cross-references. |
| POST `/api/experiments/runs` | Submit a recipe. The response identifies the persisted run. |
| GET `/api/experiments/runs/ID` | Poll job state. |
| GET `/api/experiments/runs/ID/result` | Read frame values, environment fingerprints, checked claims and attributed stores. |
| POST `/api/experiments/runs/ID/snapshot/TRIAL/FRAME` | Read a JSON array of named fields from the restored frame. |
| GET `/api/experiments/runs/ID/artifact/TRIAL/writes.json` | Read the value-changing CPU stores for one trial. |
| GET `/api/experiments/runs/ID/artifact/TRIAL/probes.tsv` | Read the raw function entry and register trace. |

The observer measures selected CPU-written fields. DMA, direct emulator writes
and fields outside the recipe can affect state without an observed CPU store.
Values that lack matching evidence carry an explicit attribution state. Writer
PCs resolve to known symbol extents. C line and compiler-pass evidence is
optional and attached only to an exact scope comparison. Savestates and the
ROM are local inputs and are excluded from the artifact routes.
