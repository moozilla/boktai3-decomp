# Monsters source recovery

## Golem

65 → 1 warning, 0 errors. All three translation units match the ROM.

- `src/golem_main.c`: 36 → 0. Shared chase control, route, and gate operations.
  Contact uses one native position. Revival restores the typed actor snapshot.
  Charge recovery, recoil, and slide use the shared body and charge fields.
- `src/golem_2.c`: 12 → 0. 080C9C04 uses an ordinary route block. The shared
  gate, fallback, and control operations restore the PRE stack-slot order.
  The context countdown and camera shake fields replace the private record.
- `src/golem_main_2.c`: 17 → 1. Shared charge, player, and phase operations.
  080CFC14 has the only `concession-ok`. Both 8-byte CpuSet calls use control
  0x05000002 and the same source word at sp+0x10.
- Open, 080CE188 `Golem_ResumeRouteAfterControl`: the 0x200 local supplies one
  mask to the test and the later BICS. Direct literals split its lifetime and
  change allocation and reload rotation. Clearing the flags observed by the
  query preserves most of the function but leaves the extra narrow extension
  and separate complement. The exact source stays in place with the warning.
- Header checks passed for all four Golem record dependents and all eight
  defeat-effects dependents. No tool changes.

## RANDOM TABLE

080C9848 `Golem_UpdateDefeat` uses `gRandomTable` directly. Six expansions of
`SCENE_ACTOR_SPAWN_DEFEAT_SPARK_RING` in `scene_actor_part_burst.h` use
`ParticleRandom_ReadSample`. The sampler's capture macro preserves the draw
and particle controls together. A direct read and the broader draw macro
missed. Replacing the older burst helper for its other callers changed ROM
bytes, so the ring operation is available independently. The search is in
`build/cx/work/defeat-search`. Mummy's 080D3878 defeat now uses the same
six expansions and direct table, with the full 1,256-byte function exact.

## Mummy

87 warnings at intake, with two more exposed by linter coverage fixes.
26 warnings remain and all three translation units match, with zero errors.

- `src/mummy.c` 4 → 0. Uses the shared display sequence and typed player
  and Mummy contact fields.
- `src/fn/sub_080D1784.c` 2 → 0 under the improved linter. At 080D1784 the
  shared route check and terrain snap restore PRE stack ordering after the
  discarded arithmetic and signed offset mask are removed.
- `src/mummy_entry.c` 83 → 26 under the improved linter. Named range,
  drawable, route, snapshot and interaction fields. Shared control and
  phase cover idle and pursuit. 080D211C is named `Mummy_UpdateDeath`.
  080DA5C8 matches with native fields and an ordinary tracking flag
  clear. 080D3878 uses the same six-ring effect as Golem.
- Two `concession-ok` entries cover fixed fill sources. 080DCC98 has
  two `CpuSet` calls with control 0x05000002 and source sp+0x10.
  080D812C uses that control with source sp+8 and buffer sp+0xC. Replacing
  either with `Memory_Clear` changes the call and surrounding bytes.
- Open. Revival's sole number-named local controls whether the health
  value or address is prepared first. The shared drawable operation also
  cleared the hidden recovery mask when it became available independently.
  Displacement and pursuit gate masks alter allocation when used directly,
  even with typed route and sampler alternatives. The remaining status,
  reaction and context masks change load order or narrow operations.
  Burn palette and flash stores also change their allocation. Exact
  sources retain these locals. See `build/cx/work/mummy-final-warn.log`.
- `include/mummy.h` now names the 0x6EA countdown `interactionFrames`.
  Both dependent units still match.
- Tools. 0042 flags arithmetic in a block whose only updated scalar
  is discarded. 0052 detects number-named locals inside grouped scalar
  declarations. Both changes have regression coverage. 38 linter tests pass.


## PARAMETER HELPERS

Counts are call sites, with distinct calling functions where they differ.
Earlier exact changes remain available for coordinator review.

| Helper | Call sites | Assessment |
| --- | ---: | --- |
| `SceneActorRoute_SetContextFlags` | 1 | Needed for the match. A file-local spelling of a word flag set. |
| `Golem_SetStatusFlags` | 3 | Needed for the match. The actor type is right, but the operation duplicates generic word flag arithmetic. |
| `Golem_MarkContactPlacement` | 1 | Needed for the match. It preserves mask-first evaluation on a typed placement field. |
| `Golem_BeginRevival` | 1 | Needed for the match as written. The health and timer belong to revival, but the health parameter carries the literal. Mummy has the same two writes. |
| `Golem_HasRouteFlag` | 2 in 1 function | Needed for the match. A file-local wrapper around the existing flag test. |
| `Golem_InvokeRouteFallback` | 4 in 1 function | Real operation. Dispatches the actor's registered route fallback with its owner. |
| `Golem_RevivalAnimationFinished` | 1 | Needed for the match. Reads the existing restart byte through a predicate. |
| `SCENE_ACTOR_SPAWN_DEFEAT_SPARK_RING` | 12 in 2 functions | Real operation. Each ring draws a particle count and spawns that ring in the shared Golem and Mummy effect. |

`Mummy_LoadContactLatch` has one caller. Its mask parameter predates this
run. This run changed its storage to the canonical Mummy field.

## Dog

78 warnings at intake and one more from grouped-local coverage. 40 remain,
with zero errors and the full translation unit exact.

- Shared route restoration, pursuit setup, ballistics control and defeat
  drawable operations replace duplicated state writes. The ordinary cell
  coordinate operation replaces both statement expressions. Three address
  indirections are gone. The typed trail vector uses a direct zero store.
- `include/dog.h` names the 0x696 action cooldown beside the near and far
  attack timers. Its dependent unit matches.
- `concession-ok` at 080E5744. Both `CpuSet` calls pass sp+0x10 and
  0x05000002. The `Memory_Clear` candidate changes the call and block.
- Open. At 080DF2E0 a direct tracking flag update moves the byte load
  ahead of the mask. At 080E52EC using the actor pointer directly lets
  the compiler fold the default-duration test and reuse the offset.
  The typed and explicit callback forms have the same issue.
  080DF908 still needs the existing relative table form for the third
  sample. Named direct sampling changes allocation across the velocity
  stores. The other masks and control locals move loads or change the
  register lifetimes when used directly. See `build/cx/work/dog-ready-warn.log`.

## Mimic

25 → 11 warnings, zero errors, full translation unit exact.

- 080E7EC8 initializes both colliders with ordinary `ContactPosition`
  values in place of the paired union views. The nearby-player search
  uses a named Z displacement and the player target field. Recovery,
  pursuit, defeat and hit controls use the existing shared operations.
- `concession-ok` at 080E87C4. Both `CpuSet` calls pass sp+0x10 as the
  fixed source and 0x05000002 as the control. `Memory_Clear` changes
  the call and surrounding setup.
- Open. The remaining eleven literal locals are mostly tests on motion,
  tracking, player and slot state. Inline literals change where the flag
  or mask is loaded, often disturbing the route or proximity registers
  that follow. The 0x80 expiry test still misses with the shared motion
  field query. See `build/cx/work/mimic-ready-warn.log`.

## Octopus

46 → 17 warnings, zero errors, full translation unit exact.

- 080E9F88 and the hold, throw, release, and return paths use the
  common control and state operations. Waiting for an adjacent target
  no longer threads a zero through raw state offsets. Route, animation
  and defeat script reads use the owning records.
- `concession-ok` at 080EC3F8. Both `CpuSet` calls pass sp+0x10 with
  control 0x05000002. Replacing the widget's fill with `Memory_Clear`
  changes its call and later setup.
- Open. The remaining zero, one and flash count locals affect the
  prepared-state and collider store order. The shared drawable
  operations cleared the mask across callers. The placement,
  motion and player-status locals still shift mask and
  field loads with the literal in place. See
  `build/cx/work/octopus-ready-warn.log`.


## Ivy

39 → 18 warnings, zero errors, both translation units exact.

- `src/ivy.c` 36 → 15. Shared state changes recover the hold, release and
  collision controls. The hide paths use the same drawable operations
  as Golem. The animation transition and defeat script use typed fields.
- `src/ivy_child.c` 3 → 3. The dispatch callback reads the existing child
  phase record. Direct flag tests and the standard random sampler still
  change the burst prologue and neutral-phase writes.
- `concession-ok` at 080EE1B0 and 080F0C20. Defeat passes sp+0x2C as the
  fixed input to the 8-byte launch-vector fill. Both initializer fills
  use sp+0x10 and the same 0x05000002 control. `Memory_Clear` missed.
- Open. At 080EE1B0 the ROM holds the table address in sb across the
  six rings. Named-table forms and both shared samplers reload the
  address between rings and change the particle argument lifetimes.
  The best union-free trial is 65 ROM and 69 draft instructions apart.
  The remaining literal and zero locals affect flag-load and state-store
  order. See `build/cx/work/ivy-ready-warn.log`.
- The existing drawable flag family moved into
  `include/scene_actor_drawable_flags.h`. All eight existing dependents
  passed and the same operation removed one more warning in Mummy and
  two in Octopus on revisiting those units.

## Sword

20 → 11 warnings, zero errors, all ten translation units exact.

- Follow-segment completion now uses the existing Sword wave component
  layout and action transition. Rest-orbit callback reads use the
  existing field. Attack initialization clears its fields at the stores.
- Open. Direct action-entry resets in 080F60B8–080F61C8 shift the zero
  load past the flag store. The shared typed candidate also moves the
  bit-test load. Rest-orbit and flashing exit retain their entry masks.
  The exit state and task mask still change store and branch order.
- 080F645C narrows the defeat flags' scope and drops the offset local.
  Plain nested blocks exchange the preserved flags and record-address
  registers, even with the typed game-state counters. The zero still
  carries across damage display. See `build/cx/work/sword-damage.log`.

## Axe

78 → 49 warnings, zero errors. Both translation units match. STOP arrived
during the phase routines, so the Axe review is partial.

- Plain `ContactPosition` replaces the unions in defeat, the heading
  burst, trail sparks and both collider updates. The waypoint selector
  stores its index as a scalar. Shared chase control and motion queries
  cover the turn, pause, pursuit and several attack phases. The existing
  drawable operation clears damage-spin visibility flags.
- `include/axe.h` names the lunge entry latch and spark cycle at ED1 and
  ED2. Both dependents passed. Defeat and trail sparks use the same
  named cycle. Reused the existing fixed-source position clear.
- Six `concession-ok` entries record ROM fixed-source fills with
  control 0x05000002. 080F9BE4 and 080FC1F8 use sp+0x10.
  080FAC30 uses sp for all three fills. 080FC998 and 080FCB64 use sp
  for the collider offset. `Axe_Init` (080FE68C) uses sp+0x10 for the widget
  position and pursuit origin. `Memory_Clear` trials changed the calls
  and setup.
- Open. 080FA670 still needs the exact counter address computation.
  Stock direct flag literals change the bit-load order. The native
  indexed palette loop and trail-slot accesses change pointer reuse.
  Several late state-entry routines still need recovery around the
  shared transition, including both route followers. `axe_contact.c`
  was verified at its starting source. See `build/cx/work/axe-final-warn.log`.

## STOP checkpoint

Stopped on the coordinator's 01:15:56 marker. Seven units were reviewed
through Sword and Axe was partially reviewed. Across those eight units
the intake count was 438, with three further warnings exposed by the
linter fixes. The checkpoint count is 173 and zero errors. All source and
header changes through 0068 have runner acknowledgments. Resume Axe at
`SceneActorControl_UpdateControl2APhase`, then `src/axe_contact.c`. The
fifteen units from Glove through Orange spine remain unreviewed.

Final verification passed for all 23 translation units in the reviewed
range. The linter regression suite passed all 38 tests. Commit 0069
records the short batch note and indexed history entry.
