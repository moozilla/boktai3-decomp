# Subsystem report organization

The progress report (`tools/progress.py --json`, published to
[decomp.dev](https://decomp.dev/moozilla/boktai3-decomp)) is a three-level
hierarchy:

1. **Category**: a top-level subsystem such as *Monsters*, *Bosses*, *Player*
   or *Menus, HUD and dialogue* (`symbols/progress_categories.csv`). decomp.dev
   lists the categories in its category menu; choosing one shows only its units.
2. **Unit**: one object or subsystem inside a category, named
   `Category/Unit`, for example `Monsters/Ghoul` or `Bosses/Vanargand 2`
   (`symbols/progress_subsystems.csv`). Each tile of decomp.dev's treemap is a
   unit; clicking it opens the next level.
3. **Function**: the unit's functions, each a tile of the unit's treemap.

decomp.dev draws one flat treemap per view and does not nest tiles, so the
category level is the category menu, not a border inside the treemap. Units are
listed category by category, so each category occupies one area of the full
treemap. Source file locations (`src/fn/`, `src/*.c`) do not affect the report.

Grouping never changes credit: every function keeps its address, size and
exact or fuzzy status, and the totals are unchanged (`tests/tools/test_progress.py`).
Ranges describe observed ownership and APIs; they do not assert recovered
original object-file boundaries or in-game identities. Names are provisional.

## How rows are made

* **Reviewed rows** of `progress_subsystems.csv` (evidence not starting with
  `auto:`) are written by people or workers from source and notes. Their
  `category` column may name a category directly.
* **Generated rows** (evidence `auto: ...`) cover all the code between reviewed
  rows. `python3 tools/subsystems.py --regroup` (after `make disasm`) cuts that
  code into contiguous objects by function-name prefix (`Ghoul_*`), smoothing
  over stray prefixes, and pulls unnamed functions into the object they call,
  are called by, or share a pointer table or data label with. Objects under
  1 KB fold into their closest neighbour. Repeated prefixes are numbered in ROM
  order (`Player 1`, `Player 2`).
* A unit's category is its row's `category` if set, else the first
  `progress_categories.csv` rule whose name regex matches (within the rule's
  range if it has one), else the first address-only rule containing it.
* `python3 tools/subsystems.py` prints the whole tree; `--tree` with
  `--regroup` prints it after regrouping.

## Keeping the structure tidy

* **Name functions `Prefix_Action`**, with one prefix per object (`Ghoul_UpdateChase`).
  The prefix is what puts a function in the right unit. A generic prefix
  (`SceneActor_`, `Actor_`) in the middle of an enemy is fine; it is outvoted.
* **After a naming batch**, run `make disasm && python3 tools/subsystems.py --regroup`
  and commit the CSV with the names. Only generated rows change.
* **When you identify an object or subsystem**, add a reviewed row with real
  evidence and, if no rule places it right, a `category`. Reviewed rows are
  never rewritten by `--regroup`; they absorb adjacent unnamed code only.
* **New enemy, boss or menu family**: add its prefix to the matching rule in
  `progress_categories.csv` if the address ranges there do not already cover it.
* **Never leave code unclassified**: `test_every_game_function_has_a_unit_and_category`
  fails if a unit falls through to *Not yet classified*, is a lone function, or
  if units of one category are not listed together.
* **Source layout** is independent: put a consolidated subsystem in
  `src/<subsystem>.c` named after its row (docs/UNIT_WORKER.md, `tools/unitize.py`).

## Current categories

| Category | What it holds |
|---|---|
| Engine core | Boot, AgbMain, task scheduler, heap, script VM, video/sprite/BG/text services, terrain navigation, camera |
| Minigames | Transformation and result game, its tutorial |
| Link and wireless | Wireless controller and menus, link lobby and session panels, serial and RFU manager |
| Hardware, time and save | Clock and sun schedule, solar sensor, save data, game state flags |
| Bosses | Nidhoggr, Hresvelgr, Vanargand, Ratatosk, the slot-2 boss |
| Field objects and hazards | Scene objects, containers, switches, crates, traps, hazards, terrain effects |
| Monsters | One unit per enemy: the `08084F44–0810033C` enemy block (Bat, Ghoul, Slime, Bee, Ghost, …, Axe) and later enemy families (Armor, Glove, cannons, spines, …) |
| Player | Player control, cues, attacks, barrier and clone |
| Menus, HUD and dialogue | Dialogue, text windows, pause menu, HUD, inventory, shops, books, name entry, password |
| Scene actors and NPCs | The shared scene-actor framework, pooled and contact actors, NPCs |
| Effects and presentation | Particles, palette and scanline effects, projectiles, presentation controllers |
| Libraries | MP2K, RFU, syscalls, EEPROM, SIIRTC, libgcc, libm, libc |

## Recent contiguous work

The October 15 cleanup adds 73 groups across `08047BB4–08084F44`. Each range is
bounded by reviewed progress entries, includes unfinished functions in that
family, and cites its source layout or subsystem notes. The main divisions are:

| Range | Families and evidence |
|---|---|
| `08047BB4–0804D620` | Temporary animations, directional particles, contact bursts, wireless indicator, and the tether actor. See the October 14 animation/contact-burst and tether notes. |
| `0804D620–08052130` | Countdown, staged effects, rectangular areas, resource station, repeated effects, traversal gates, orbit background, directional force, breakable props, and proximity particles. Each has distinct lifecycle or singleton ownership. |
| `08052130–08057960` | Stacked actor, interaction markers, hit switches, display tracks, keyframe pool, icons, script references, and timers. The existing narrow headers establish the separate owners. |
| `08057960–0805DD18` | Four related ScenePanel groups: presentation effects, the fifteen-sprite rig, parent control/lifecycle, and subject states. See the ScenePanel documents and rig notes. |
| `0805DD18–080638BC` | HUD map/clock rendering, input widgets/grid, inventory display, action transitions, catalog/transaction completion, and audio events. Inventory phase views share storage; grouping does not turn those overlays into independent allocations. |
| `080638BC–0806E34C` | Roaming objects, attachment manager, actor slots, particle/bouncing/emitter pools, sprite-effect controllers, steering actors, and route sprites. Different manager globals and record strides delimit the pools. |
| `0806E34C–0807462C` | Grid/spatial queries, effect scheduling, range watcher, pair records, registry lifecycle/queries/manager, palettes/scripts, contact candidates, and profiles. |
| `0807462C–08084F44` | Registered-actor collision setup, animation selection, terrain transitions, placement/sound feedback, behavior dispatch, motion/update, attachments, target/effect control, reaction callbacks, control phases, and the adjoining visibility/stored-motion callbacks. See the corresponding `SCENE_ACTOR_*.md` documents. |

The ordinary and register-pinned members retain their native report categories.
Incomplete bodies retain their assembly status or recorded `NONMATCHING` score.
Grouping changes the number of displayed units, never code-byte or function
credit. The report comparison checks every function's virtual address, size and
score, overall totals, and both register categories.

## Middle-ROM families

These seven groups cover identified portions of previously fragmented code:

- `0813C040–0813C26C`: directional sprite-effect consumers sharing sprite `7FC`,
  sequence `840` and associated direction/phase fields. The label is tentative.
- `0813C2EC–0813C798`: a distinct four-effect controller with graphics at `9A8`,
  four effect records, and shared center coordinates at `A80/A84`.
- `081915E0–0819792C`: the reviewed portion of a large scene-animation owner.
  Helpers address sixteen keyframes at `543C` and state bytes beginning `71DC`;
  later callbacks use state `724A` and nine-entry flag/health arrays. Creators
  `19783C/197890` both allocate `72FC` bytes. This is a descriptive controller
  name, without a claimed game-screen identity.
- `0819792C–08197DD0`: a separate animation/graphics-buffer task. Its initializer
  distinguishes sprite and animation-backed slots and their tile pointers; its
  task storage is allocated from a count using `count * 32 + 0x28`.
- `08197DD0–08197EC4`: three-source palette selection and transitions. `197E0C`
  observes `08228DB8`, interpolates selected values through `08221110` over
  64 ticks, and is installed by `197E74`.
- `08197EC4–0819804C`: the scripted key-trigger task, with message channel,
  key-mask, script references and lifecycle functions.
- `08221774–0822223C`: scalar approach, wrapped-angle/clamping and related
  vector helpers. Independently reviewed raw-tail boundaries are accounted
  separately from their preceding C function.

A compiled-object extent audit of the recent contiguous work and these middle
runs found one retained-tail accounting error: `08221800` compiled to 52 bytes
but its old progress span extended 2,536 bytes to `082221E8`. The following
2,484 bytes contain 31 independent routines. Their prologues, control-flow
returns, literal pools and padding were reviewed, and explicit build/progress
boundaries now separate them. The ROM-byte denominator stays unchanged.
Unrecovered entries receive no exact credit; new C is accepted only after a
full reviewed-extent comparison. This correction is independent of grouping.

## Validation

The cleanup adds80 subsystem ranges in total (117→197). With identical current
function inventory, sizes, exact/fuzzy scores and register categories, the local
objdiff report changes from8,907 units to7,574. The accompanying local treemap
preview uses deterministic binary packing; the live site's rectangle placement
may differ. Grouping preserves credit exactly.


`tools/progress.py` validates sorted, nonoverlapping ranges and known endpoints.
`tests/tools/test_progress.py` checks byte/function accounting, names, category
preservation and inventory validation. A report before and after grouping can
be compared without the ROM. New matching code still requires the normal fast
function checks and a combined byte-identical ROM build before integration.

Display-only names for the phase-palette controller and scalar approach helpers
are recorded in `symbols/proposed/report-subsystems.csv`. They make those nested
functions readable without changing their build symbol identities.
