# Scene actors and NPCs, first half

The wind-down marker arrived at 01:15 PST. Reviewed 15 of 42 units in queue order.
The reviewed units went from 323 to 121 warnings under the final detector,
a reduction of 202. All changed units and header dependents matched with stock
agbcc. The runner confirmed all 84 commit requests at stop.


## Scene object controller (tentative)

23 → 0 warnings, zero errors. The changed functions and header dependents match the ROM.

- Activation, deactivation, force-active, and promotion share the typed slot iterator and
  state mapping. The mapping places the callback load independently of the pose inputs.
  Force-inactive uses the explicit callback and pose setter, preserving their setup order.
- Raise and lower share the script-position to idle-variant conversion. The converted
  byte hoists before the terrain mask, while the callback load stays in the loop.
- Placement writes the cached height through the slot field, then registers it. Both
  placement functions use the typed animation banks and common pose reset.
- The active callback shares its idle transition. Both lift directions use the moving
  variant for their destination. Lowering uses a packed jitter vector and direct height
  calculation. The lowered callback uses the signed shake timer seen in the raised sibling.
- Initialization uses the typed pool and the reverse halfword fill. Scratch checks
  reproduced the same fill in TaskSystem_Initialize and SpriteOam_ResetQueue.
- The frame update and construction/destruction use the shared pool. Slots embed the
  existing sprite layout and point to the shared terrain override. Named five entry points.

## Common scene actor controller

11 → 0 warnings, zero errors. All 27 functions match the ROM.

- Constructors and eight-direction animation use direct slot and animation field stores.
  Removing address captures restores the original lifetime of the zero defaults.
- Action and damage flags use inline setters and predicates. These materialize the mask
  before reading the field, as in the ROM.
- Both event runners share an indexed argument copy and bitfield count assignment. The
  indexed loop forms the packet address before entering the copy. The guard reads the
  named primary player status.
- Parameterized attachment modes use the same direct assignments as Actor_SetAttachmentEffect.
- Ambient particles use a halfword cursor and the existing hit-spark sampler. Splitting
  the first increment from its mask preserves the literal placement.

## Actor graphics and basic states

89 → 0 warnings, zero errors. All 84 functions still match the ROM.

- Damage contacts now use the typed collider cost and the actor's pending damage field.
  Exact sibling flag predicates establish the order of evaluation.
- Disappearance and damage reactions share the action initializer. The reaction hit
  angle uses the existing graphics field.
- Phase selection uses typed graphics fields and the common phase entry. A signed
  health ratio getter preserves the threshold branch.
- Twenty-two animation handlers share the action layout, phase setup, and animation
  setter. The particle flag uses a separate typed graphics view.
- Death and recovery handlers share the typed action setup, animation setter and timer.
  Separate state and graphics views retain the actor alias used by recovery.
- The forward terrain probe multiplies the angle and distance inside one helper and
  checks the observed height interval. This restores the distance evaluation order.
- The animation callbacks reuse the controller's facing setter. All 27 graphics
  dependents and all 28 facing-setter dependents matched.

## Actor animation and action controllers

32 → 17 warnings, zero errors. All 18 source files in the range match.

- The shared graphics view embeds the existing keyframe resource and playback layouts at
  0x220 and 0x240. The reset callback returns a byte, which the charge handlers check.
  All 50 header dependents match.
- Typed playback arguments remove the staged drawable offsets. The shared charge projection
  writes the velocity vector for both lunges, recoil and the charge slide. The charge slide
  also uses its duration inline.
- The player-grab target table is the initializer's 26-word array. Each target's
  position address follows its player pointer in the table. The two readers use the
  same byte-displacement order.
- Open: 15 literal masks in 13 handlers (080251C4, 0802533C, 08025714, 080257D0,
  0802592C, 08025FFC, 08026248, 0802634C, 08026398, 08026488, 0802667C,
  08026780 and 08026908). The literal form moves the mask after the field or global
  load. In the flash and charge handlers this also changes the callback address setup.
- Open: two angles in 0802592C. Direct assignments form the field address before the
  angle. The shared facing setter recomputes both facings and changes the branch and
  tail structure. The existing three warnings in this function remain visible.

## Actor projectile and reaction controllers

54 → 22 warnings by the queue's original detector, zero errors. The new duplicate-arm
check finds one preexisting site, so the current detector measures 55 → 23. All 19
source files match the ROM. All 57 dependents of the recovered header fields matched.

- Route, travel and knockback handlers use the existing charge projection on typed
  vectors. The swarm attack passes ordinary zero arguments after adding its offset.
- Death, sway and both damage entry paths share the phase and timer transition.
  The death function writes its motion through the vector fields.
- The pose and shake families use the shared playback and callback layouts.
  The turn handlers keep their existing facing calculation because the shared
  setter changes the low register assignment in the turn branch.
- Open: 19 action and blink masks. Direct masks shift the literal after the load.
  Combining the blink branches also changes its control flow. The sway entry value
  and the two death effect values are open for the same evaluation-order reason.
- Open: identical flag-assignment arms in 08027AD4. Folding the arms keeps the same
  instruction shapes and changes 11 register operands. The written branch stays visible.

## Actor damage and lifecycle

3 → 2 warnings, zero errors. The changed Vampire functions and the presentation
callback's caller match.

- The blend control is one four-byte bitfield record. Its first byte is read as
  a signed interpolation amount where required. The redundant byte union and unused
  flag view are gone.
- Open: the Vampire damage mask moves after the actor flag load when written inline.
  The blend floor's two stores consume a zero established before the palette branches.
  Two direct zeros and a paired state assignment change that allocation.

## Grid attack actor

55 → 46 warnings, zero errors. The full unit and both packet header dependents match.

- Grid positions use the shared vector. Linked-actor messages use the actual
  SceneEventCommand packet. The stagger handler writes its damage defaults directly.
- Six requests share the complete idle and duration transition. The action-start
  duplicate was removed using the existing action mode initializer.
- Open: 28 masks. Direct forms move the mask after the load or change callback address
  reuse. The pending-damage low flag writes directly.
- Open: 14 requested-state literals. Direct and paired request-and-entry forms move
  the ID behind the field address. Returning the entry flag also adds a branch.
- Open: the staged action zero and the blend endpoint zero. Their constant lifetimes
  span state assignment and the palette branches respectively.
- Open: both cell-jitter constants and the existing volatile pointer parameter in
  `GridAttack_OffsetCoordinate`. An ordinary pointer removes the copied mask and
  swaps the allocation of the coordinate, grid mask and random index mask. The
  signed and unsigned pointer versions behave alike.

## Pooled actor manager and shared helpers

Manager 2 → 2 and helpers 3 → 3. Nothing changed in either unit.

- The removal check hoists a bit mask across traversal. The direct test emits a
  shift in the loop. The removal request and both collider flags also need early
  masks, with the direct forms moving one instruction each.
- The descending target-array initialization stages null before its induction
  variable. Direct array, cursor and decrementing-loop versions choose a different
  order or loop shape.

## Pooled actor controllers and scene actor lifecycle

Pooled controller 6 → 4 warnings, whole unit exact. The script initializer passes
zero animation arguments and writes its state defaults directly. Its playback
and collider masks still need an early constant. Lifecycle 1 → 1. The direct
motion-body flag moves below the field load by one instruction.

## Scene actor animation and scripted actions

39 → 22 warnings across the unit, with 11,544 bytes exact in 0078–0079.

- Timer, flashing and turn defaults write through their fields. Reading the route
  point kind directly removes the single-pass loop. Navigation reuses the existing
  facing equations and two walkers use the shared horizontal projection.
- The character state entry sets the next state, clears ticks and marks the command
  as entered. Six handlers share the target interruption that saves the prior state.
- Open: 15 flag masks. Direct sites disturb load order or address reuse. The three
  navigation thresholds move the constant-pool load below a saved position copy.
- Open: the two script-release constants and two route-scheduling zeros. The direct
  release switch changes the fallthrough and address setup. Direct route scheduling
  changes the surviving branch allocation and common tail.

## Scene actor manager

1 → 0 warnings. The shared manager and actor layouts expose the minute edge, actor
list and queue buffers. An explicit minute-change branch and direct buffer clear
match all 200 bytes (0080).

## Scene pooled motion and actor controls

2 → 1 warnings. Both translation units match, as do all 67 scene pool header
dependents. The direction transition resets the state frame and workspace. The
same operation also matches in the direction-repeat and position-limit siblings
in scratch. The departure controls use direct zeros. A direct clear of the last
byte moves below the flags store, leaving the one constant warning (0081–0083).

## Contact actor pool setup and display

1 → 0 warnings. Each contact-direction branch writes its typed destination
directly. A single write after both branches swaps the owner and collider homes.
The full display unit matches (0084).

## Contact actor pool states and lifecycle

Stopped in this unit at 0804373C. All 17 original warnings are still visible.
The tracked priority function was rechecked at stop and matches all 788 bytes.

- Match-tested scratch drafts in `build/cx/work/contact-state-*-enter.c`
  share the state/frame entry across six handlers. The palette interpolation
  draft uses the actual 32-byte bank stride and pool tile buffer. These drafts
  were still in scratch when the stop arrived.
- Separate variables in the pool initializer swap the slot and index registers.
  Direct scale 256 in roaming emits a shift in place of the signed multiply.
  Direct palette selection in the hit reaction moves the palette ID after the
  graphics address. The shared graphics layout itself matches in scratch.
- Units 17–42 are pending.

## PARAMETER HELPERS

Counts are distinct callers on this branch at the checkpoint. These matching changes
are retained for review.

| Helper | Callers | Assessment |
| --- | ---: | --- |
| `SceneObjSlot_ResetPose` | 5 | Real operation. Set the pose and reset its frame together. |
| `SceneObjSlot_EnterIdle` | 2 | Real operation. Map an idle state to its action callback. |
| `SceneObjSlot_ApplyActivation` | 4 | Real operation. Map all four states to their callbacks and initial poses. |
| `SceneObjSlot_LiftVariant` | 3 | Real operation. Convert the lift script position to its idle variant. |
| `SceneObjSlot_LiftMotionVariant` | 2 | Real operation. Select the in-motion variant for either lift destination. |
| `SceneObjSlot_SetAnimation` | 1 | Needed for the match at forced deactivation. It stages the last-pose constant. |
| `Memory_Fill16Reverse` | 1 | Real operation. Same reverse memory fill independently matched in two other units' scratch copies. |
| `SceneActors_SetActionFlags` | 1 | Needed for the match. Both writes belong to the same effect updater. |
| `SceneActors_TestActionFlags` | 1 | Needed for the match. This mask must precede the field read. |
| `SceneActors_SetDamageEffectFlags` | 1 | Needed for the match. Both branches belong to the same effect updater. |
| `SceneActors_TestPlayerStatus` | 1 | Needed for the match. It stages the primary-player status mask before the load. |
| `SceneActors_RunEventScript` | 2 | Real operation. Both event slots build the same packet and dispatch it. |
| `TestColliderFlags` | 1 | Needed for the match. This file clones the existing actor-impact flag predicate. |
| `SceneActorState_TestFlags` | 4 | Needed for the match. This is another action flag predicate besides the common controller's. |
| `SceneActorState_EnterPhase` | 9 | Real operation. Set the phase, clear its timer and request entry. |
| `SceneActorState_StartAction` | 8 | Real operation. Set the mode, index and entry phase together. |
| `SceneActorState_GetHealthRatio` | 1 | Needed for the match. The signed getter preserves the signed threshold branch. |
| `SceneActorState_SetFlags` | 5 | Needed for the match. The moved shared setter has the same mask-staging behavior as the common controller's private setter. |
| `SceneActorState_SetAnimation` | 41 | Needed for the match. It stages the animation ID before formation of the field address. |
| `SceneActorState_ClearFlags` | 1 | Needed for the match. Single flag clear at the phase-6 entry. |
| `SceneActorState_TestPlayerFlags` | 1 | Needed for the match. Single grab eligibility predicate. |
| `ProbeHeightOutOfRange` | 1 | Needed for the match. Reconstructs the unsigned interval comparison and its constant order. |
| `SceneActorGraphics_IsForwardGroundOutOfRange` | 1 | Needed for the match. Extracts the terrain probe and stages its distance argument. |
| `SceneActorState_StartRecovery` | 12 | Real operation. Start the idle action and set the recovery delay in the same place. |
| `SceneActorState_TestDispatchFlags` | 1 | Needed for the match. Single dispatch guard with an early mask. |
| `SceneActors_SetFacing` | 4 | Real operation. The preexisting helper now serves both units, writing the input angle and both facings. |
| `startEight` | 1 | Real operation. Preexisting transition helper, updated to use the shared playback fields. |
| `SceneActorState_EnterReactionPhase` | 5 | Real operation. The damage, death and sway paths enter a reaction phase and reset its timer together. |
| `GridAttack_BeginIdleWait` | 6 | Real operation. The grid actor enters idle and assigns its wait duration in one transition. |
| `SceneCharacter_EnterState` | 12 | Real operation. Set the state, reset ticks and mark a new command. |
| `SceneCharacter_InterruptForTarget` | 6 | Real operation. Preserve the current state and enter the proximity handler. |
| `ScenePool_EnterStateAndClearHistory` | 1 | Real operation. The same state/frame/workspace transition matches in two other selectors in scratch. |

## RANDOM TABLE

The ambient sample sequence uses a halfword cursor, a typed table base, and
`HitSpark_Sample` for both reads. The first advance is split before masking and the
second uses `HitSpark_NextIndex`. This preserves the early table load at 08021D8C.

Contact retreat 0804373C remains open. Direct removal, including an ambient-style
halfword cursor, gives 776 versus 788 bytes. It drops the unused 0x0403B714 load
and moves the retreat center from SL to R8. Relative reads over the polar draws,
later steering draws, and the full draw sequence change other allocations and
do not match. The next decision is what real source operation produces that
early literal. The direct-radius draft retains the current origin and misses
only four register operands at the radius mask.

## Concession exceptions

None.

## Tool changes

Matchsearch now falls back to local evaluation if the resident server is denied permission
to bind its Unix socket. Other startup failures stay visible. Verified with a real
deactivation proposal and the matchsearch suite (195 tests, one skipped).

The linter now reports deferred standalone literal assignments to otherwise unused
locals. The former ambient particle mask is a regression fixture. 37 linter tests passed.

Scope selects an annotated moved instruction for its diagnostic hint, so it no longer
suggests querying a missing RTL UID. Both moved-row tests passed.

The linter now reports identical braced assignments in both arms of a simple
conditional. The only hit in the worktree is the preexisting contracting-shake
branch at 08027AD4. All 38 concession tests passed.
