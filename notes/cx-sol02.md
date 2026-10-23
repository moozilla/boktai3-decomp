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
