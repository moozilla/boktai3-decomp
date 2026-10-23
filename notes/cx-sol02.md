# Campaign worker sol02

Tags: #matching #vanargand

## Batch 1

- 081A6A08 matched: `Vanargand_TestProjectedHorizontalRange` (tentative).
  Projects the position at 54 and target via 110, then tests a strict horizontal
  margin of 96. Reused camera scaling with an explicit signed-shift result.
  A pointer-to-pointer local fixes evaluation order; no hints remain after depin.
- 081A6AEC skipped: reviewed the full symbol extent and earlier attempts.
  The visible 312-byte body projects lateral separation and returns an
  unprojected planar distance, but the 736-byte symbol includes undecoded
  executable tails from 081A6C24. Needs boundary recovery before exact credit.
- 081A7204 matched: `Vanargand_ApplyPartDamage`. Observed local part layout
  recovers pending damage/flags, accumulated threshold damage, animation and
  flash updates. Assigning the narrow damage field and using its assignment
  result reproduces the original truncation and allocation.
- 081A76C8 matched: `Vanargand_BeginExitTransition` (tentative).
- 081A7760 matched: `Vanargand_UpdateIdleTransition` (tentative).
- 081A77B0 matched: `Vanargand_UpdateActiveExitTransition` (tentative).
  These reuse the existing pending-entry helper returning u8 and local state
  held over the action-sequence call.
- 081A7D80 matched: `ScenePanel_ApplyPairTransition`. Inline state-request
  helpers at each branch reproduce shared tails; a single selected-state
  variable produced different conditional materialization and switch layout.
- 081A84B0 matched: `Vanargand_UpdateMode3Transition`. The same helper technique
  fixes the retained draft's duplicated stores and counter-address lifetime.
- 081A8888 matched: `Vanargand_UpdateTimedAlternateTransition`.
  Sequential byte-pointer writes and reusing the selected state for the pending
  byte recover the original. Two empty compiler barriers remain after depin;
  categorized as Register-pinned. Removed an inherited unsupported flag comment.
- 081A8A0C matched: `Vanargand_UpdateMode23Transition`.
- 081A8AE4 matched: `Vanargand_UpdateMode24Transition`.

## Session 2 integration

- The combined build exposed missing linker aliases when an exact worker
  definition retains `sub_XXXXXXXX` after its descriptive name is in the CSV.
  Updated `tools/split.py` to preserve both function and even-address aliases;
  focused fixtures cover both naming directions and interior references.
- Combined batch validation: `build/boktai3.gba: OK`.

## Batch 2

- 081A8B28 matched: `Vanargand_UpdateMode25Transition` (tentative).
  Reused pending-entry helper and a typed inline mode request. Direct typed
  field stores recovered constant scheduling and address reuse; sequential
  byte-pointer stores introduced allocation differences. No concessions.
- 081A8E08 drafted 76.4%: `Vanargand_UpdateCaptureSetup` (tentative).
  Recovered mode-dependent animation 12/13/14, collision copies at 7B8/7D8/7F8,
  and the ready-state position request. Differences: stack-vector aliases,
  zero lifetimes and constant scheduling. Owner/zero pins helped; explicit
  stack-vector pins regressed. No Ghidra export is available in this checkout.
- 081A8F48 drafted 90.6% (was 88.7%): `Vanargand_UpdatePlayerDrain`.
  Loading the player pointer before the frame fields fixes load order. Tried
  a first-coordinate pointer alias and reversing middle vector operands;
  these reduce structural differences but lower overall similarity. Returning
  the first-vector pointer from its initializer also regresses. Remaining
  initial vector aliases, temporary registers and constant scheduling.
- 081A9178 drafted 72.1%: `Vanargand_UpdateTargetDash` (tentative).
  Recovered phase 0 animation/frame gate, target-distance test around center
  (640,768,640), 1024-unit radial alternative, mode-3 track and afterimages.
  Positive-first signed scale helper and materialized completion byte recover
  branch shapes. Center pointer barrier prevents constant folding of the X
  load. Remaining timer/track spills, vector aliases and allocation; pinning
  the timer pointer to r8 produced correct extent but lower similarity.
- 081A93A0 drafted 70.4%: `Vanargand_UpdateMovingPairAttack` (tentative).
  Recovered active-half selection, selected-half commands, offset track and
  completion phases. A barrier after stack-vector initialization recovers its
  loads. Explicit owner/result pins and a retained phase value regressed.
  Remaining high-register lifetimes, vector aliases and timer spills.
- 081A96E0 matched: `Vanargand_UpdateSequencedPairAttack`.
  The retained candidate differed by one byte in a final pair-owner load.
  Use the saved owner for that load while retaining the original owner as an
  empty-constraint input. Plain substitution shortened the entire switch.
- 081A9BB4 retained/re-recorded at 99.2%: `Vanargand_UpdateFacingPairAttack`.
  Only pair-slot offset register choices differ. Explicit r0 offset fixes that
  point but changes seventeen later temporary-register choices. Pointer casts,
  owner aliases, declaration swaps and input barriers did not close it.
- 081AA178 skipped at retained 99.5%: `Vanargand_UpdateRepeatedPairAttack`.
  One-byte phase-zero reset store remains. Result-only and local-zero output
  constraints changed later allocation; explicit result/zero pins regressed.
  Both-input constraints, memory barrier, constant reset and input-order
  variations retain the same one-byte difference. Original source retained.
- 081AA788 skipped after renewed full condensed-listing and prior-attempt review.
  The earlier complete candidate was below 80% and is absent here. The large
  randomized-projectile setup still has no new structural hypothesis; no new
  source or credit claimed.
- 081AB438 matched: `Vanargand_UpdateOffsetMotion`.
  Pinning the vector destination to r4 plus a separate first-store pointer
  reproduces all lifetimes, including the mode constant held in r8. Pinning
  the mode directly regressed. Full reviewed extent is 212 bytes.
- 081AB904 matched: `Vanargand_UpdatePartDebugControls` (tentative).
  Decoded the reviewed hidden function. Five key-selected auxiliary-animation
  modes include a case-2 fallthrough into case 3. Named mode and one constants
  recover scheduling and stop an inappropriate common tail. Plain C.
- 081AD3E8 drafted 85.6%: `Vanargand_UpdateAnimationAndTargets` (tentative).
  Reused the Hresvelgr target-cache loop. Keeping the final distance read on
  the original actor while the loop uses the saved owner prevents induction
  strength reduction. Owner/base bindings and the render-Z store reproduce
  structure and full extent; 26 temporary-register differences remain.
- 081AD564 drafted 85.1%: `Vanargand_UpdatePresentation` (tentative).
  Complete camera, collider, palette restoration/interpolation, impact shake,
  heading-relative approach and palette-state update. Corrected the observed
  palette-transition fields after the three saved palette banks. Replacing
  the interpolation inline with the leap handler's goto macro regressed.
  Remaining allocation, stack-vector aliases and constant scheduling.
- 081ADBF8 matched: `Vanargand_OnDamage`.
  Reused the damage callback's action/state inline pattern. Putting one, zero
  and action ID in that order in the inline signature recovers all constants.
  Plain C; 276-byte reviewed extent.
- 081AF540 matched: `Vanargand_ProjectEffectOffset`.
  An eight-byte packed-coordinate union recovers the stack layout. Bind its
  unpacked coordinates to r5/r6, and retain the two high-half packing values
  before shifting the low half back down. This recovers the interleaved
  argument packing; changing parameter signedness or OR order alone did not.
- 081AF7F4 skipped: complete exploratory part-damage candidate reached 45.8%.
  Recovered damage channels, special-hit bit, defeat/recoil requests and
  palette update. Changing a local vector struct to an array fixed packed
  stack writes, but request-state guards and lifetimes still differ widely.
  Scratch candidate stays under ignored `wip/cx-sol02/`; no draft credit.
- 081B1F74 skipped at retained 94.5%: `Vanargand_UpdatePartPairedStrike`.
  Explicit narrow vector-add temporaries and reversed X/Z operands regressed.
  Local-pointer sweep found no improving edit. Entry copies and vector load
  order still account for the structural residue. Original source retained.

Wrapup marker observed before starting 081B2608; no edits made to that function.
This batch adds six exact matches (0.067 percentage points, 1620 bytes), with
868 bytes using register-pinning concessions. Seven guarded draft records are
new or updated; four functions were skipped after attempts or renewed review.

Batch integration: the first full build exposed a duplicated binary tail after
081AB904. Its reviewed body contains several symbolized incbin fragments;
recorded replacement end 081ABA64 in `symbols/build_extra_functions.csv` and
folded that boundary correction into the function's matching commit. The
352-byte exact C object now replaces the whole reviewed body.
