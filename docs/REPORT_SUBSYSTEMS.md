# Subsystem report organization

The objdiff v2 report groups documented contiguous code families in
`symbols/progress_subsystems.csv`. These are presentation ranges, with functions
nested beneath named `game/` units. They describe observed ownership and APIs;
they do not assert recovered original object-file boundaries or specific in-game
identities. Tentative names remain marked as such.

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
