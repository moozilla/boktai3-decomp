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
initialize contact, and tests the pause bit by changing one typed field at a
time. The command prints paths to the contact sheets,
transcripts, JSON results, and an annotated pixel-to-sprite frame. Each trial directory has numbered PNGs.

Open `http://127.0.0.1:8642/experiments` for the notebook. Run the example,
scrub the synchronized playbacks and select a field value. The sidebar shows the
last observed store that changed it, the actual written values, any active
probed functions, and a C excerpt when scope reproduces the ROM bytes. **Fork
frame** starts an editable experiment from the selected trial and frame. Code's
**Probe this function in an experiment** preloads the selected C entry point. On the pause example, select frame 60 and
click Django's coat in the field trial or enter coordinates 117, 88. The sprite
evidence links the hardware slot to its native renderer, drawable address,
typed owner, watched fields, and the art archive containing the tile source.
**Replay without this sprite** runs the same trial twice. The control must
reproduce the stored screenshot and emulator state. The second replay hides
that hardware slot for one rendered frame and must reach the same serialized
state. Studio saves the two images and reports whether the pixel changed.

## Author a recipe

Copy [pause.json](pause.json) or download a run's recipe. The same schema is
accepted by Studio and the CLI.

```sh
build/venv/bin/python -m tools.studio.experiments checkpoints
build/venv/bin/python -m tools.studio.experiments fields 'gPlayers[0]->position.x' 'gGameState->life'
build/venv/bin/python -m tools.studio.experiments run ./my-experiment.json
build/venv/bin/python -m tools.studio.experiments show RUN_ID
build/venv/bin/python -m tools.studio.experiments pixel RUN_ID field 60 117 88
build/venv/bin/python -m tools.studio.experiments pixel-check RUN_ID field 60 117 88 7
build/venv/bin/python -m tools.studio.experiments snapshot RUN_ID menu 60 'gGameState->life'
build/venv/bin/python -m tools.studio.experiments sweep tools/studio/experiments/pause.json \
  --checkpoint bank-0004 --checkpoint bank-0064
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
- Each trial can add `interventions`, a list of objects with `frame`, an
  observed `field` and an integer `value`. An intervention runs after
  sampling that frame and before the next emulated frame. The compiler
  checks its field width and signedness. The harness resolves the named
  field against live RAM and labels the observed change as authored input.
  Open **Change one pause field** for paired trials that set the engine
  pause bit by itself or let Django's task run under the ordinary menu.
- `probe` takes function names. Each entry records its VBlank and registers.
  On stores, the observer reports active entries from the configured probes,
  using observed return addresses and stack depth. Calls between those entries
  can be absent from the path.
- Set `sprite_trace` to true to capture OBJ video state, native OAM submission
  and DMA upload evidence at the sampled frames. It is optional because it
  adds capture work. It works with arbitrary trial input. Sprite fragments are
  matched using position, shape, affine transforms, palette and tile data,
  with a check against the screenshot color. The tool preserves multiple
  matching fragments or queue entries. Exact typed instance binding is
  currently available for the player drawable members.
- Optional `checks` turn a claim into a comparison. Select a trial, a pair of
  sample frames using `from` and `to`, and either a watched `field` or probed
  `function`. The comparison uses the field delta or the number of entries
  in the half-open frame interval. Operators are `eq`, `ne`, `lt`, `le`, `gt`
  and `ge`. A changed pointer binding leaves the field delta unresolved.
  Add `when` with one to four conditions using the same comparison syntax
  and any trial. For example, confirm a pause claim only if the menu input
  function ran before the measurement. A failed or unbound prerequisite
  leaves the verdict unresolved and links to the failed condition's evidence.
- A `fork` has the form `{"run": "RUN_ID", "trial": "TRIAL_ID", "frame": 60}`.
  Use a capture from the same ROM that mGBA can restore. The playback preserves the recorded sensor threshold when starting a new fork.

Use **Run across saves** to try the current recipe at several checkpoints.
The ledger shows the actual starting screenshot for each trial and links each
verdict to the exact trial, frame, and measurement. It preserves contradicted
and unresolved checks alongside confirmed ones. Open it again from **Saved
sweeps**, or download its TSV and the starting-frame contact sheet. Pass `--all-story` to use the available story bank.

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
| POST `/api/experiments/sweeps` | Submit `{"recipe": {...}, "checkpoints": ["bank-0004", "bank-0064"]}` or use `"all_story": true`. |
| GET `/api/experiments/sweeps/ID` | Poll progress and read linked checkpoint results. Append `/answers.tsv` for the ledger or `/checkpoints.png` for starting frames. |
| POST `/api/experiments/fields` | Resolve a JSON array of named paths to their target ABI layouts. |
| GET `/api/codemap/fn/NAME` | Read a function's C, disassembly and cross-references. |
| GET `/api/experiments/by-function/ADDRESS` | Find recent recorded stores and entry probes by function address. |
| POST `/api/experiments/runs` | Submit a recipe. The response identifies the persisted run. |
| GET `/api/experiments/runs/ID` | Poll job state. |
| GET `/api/experiments/runs/ID/result` | Read frame values, environment fingerprints, checked claims and attributed stores. |
| POST `/api/experiments/runs/ID/snapshot/TRIAL/FRAME` | Read a JSON array of named fields from the restored frame. |
| GET `/api/experiments/runs/ID/pixel/TRIAL/FRAME/X/Y` | Read matching OBJ fragments and their native queue, source, actor, and art evidence. |
| POST `/api/experiments/runs/ID/pixel-check/TRIAL/FRAME/X/Y/SLOT` | Send an empty JSON object to replay with one matching slot hidden and return a saved comparison. |
| GET `/api/experiments/runs/ID/artifact/TRIAL/writes.json` | Read the value-changing CPU stores and optional authored interventions for one trial. |
| GET `/api/experiments/runs/ID/artifact/TRIAL/probes.tsv` | Read the raw function entry and register trace. |

The observer measures selected CPU-written fields. DMA, direct emulator writes
and fields outside the recipe can affect state without an observed CPU store.
Values that lack matching evidence carry an explicit attribution state. Writer
PCs resolve to known symbol extents. C line and compiler-pass evidence is
optional and attached only to an exact scope comparison. Savestates and the
ROM are local inputs and are excluded from the artifact routes.


Sprite attribution requires an exact match between captured OAM and the
observed game upload. The start frame was restored before submissions were
observed. Background tiles, blended or mosaic output, and unresolved actor
types remain outside the current lookup. A matching unblended OBJ color is
evidence of a candidate fragment. Scanline composition can still affect
visibility. Running the optional render comparison measures a selected
hardware slot's contribution independently of the candidate color match.
