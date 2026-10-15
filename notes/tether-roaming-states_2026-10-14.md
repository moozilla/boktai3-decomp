# Tether roaming, recovery and lifecycle states

Sixteen new exact functions add **0.2045 percentage points** (+4,940 bytes),
reaching **24.388%** (6,522 functions; 589,058 / 2,415,354 bytes).
Of the new bytes, 992 need no hints and 3,948 retain minimized concessions.
The **0804C104–0804D620** subsection is completely exact: 5,404 bytes,
including five existing functions with typed views and provisional names.
Actor identity remains tentative. No new runtime coverage is claimed.

## State contracts

- **4C104, state 4 (512):** wandering. Global flag 0x200 freezes it;
  charge readiness clears glow/frame and updates shake. At frame90 it turns
  randomly, using a different distribution after blocking. Later frames try
  three-unit steps with mask843. A blocked step sets frame30 and the turn flag.
  Frame120 reselects interaction state. Heading selects entry36 for moving kind5.
- **4C304, state 2 (688):** recovery countdown at400, configured duration402.
  Above64, it phases secondary collider bounds from256 to128, samples shake,
  emits effects every8 ticks and consumes charge to shorten the timer with a
  signed clamp to64. At64 or below it uses shrinking shake, clears charge/glow,
  and periodically plays sound2B9. Expiry either enters directed state16 with
  timer30 or reselects interaction state. Preserve both RNG advances and the
  final global-index store, rather than treating them as independent calls.
- **4C5B4, 4C658 (164/240):** departure and return visual phases. These update
  display flags, the existing three-parameter display API and displayOffsetY.
  The departure increments its frame first; return increments afterwards.
  Interpretation of the three display parameters remains tentative.
- **4C748, state 5 (640):** auxiliary transition. Smooth Y toward auxiliary
  origin for64 frames, then shake and emit point effects, then flicker with
  increasing frequency. On expiry remove the auxiliary sprite and set state0
  on the global actor. Consume the interaction script ID before calling it,
  then request destruction and hide the display in the original order.
- **4C9C8 / 4CA94, states6/7 (204/184):** create an attached effect on frame1,
  detach it on frame6, descend by30/60 per tick, request destruction after8/29.
  State6 respects the global freeze flag; state7 deliberately does not.
- **4CB4C, state8 (356):** descend to ground, then bounce through a short
  height sequence, sample shake and emit a ring on frame2. After29 frames
  return to state0, set ready, charge287 and attach its effect.
- **4CCB0, state11 (312):** kind9 recoil. Ensure movement probe, play entry
  sound, derive deltas from packed angle241 and magnitude242, decay magnitude
  by3/4 and lower displayOffsetY. Complete when magnitude<=19 and height0.
- **4CDE8 / 4CE88 (84/128):** passive state12 and special-entry state13.
  Both set heading0 and collider flag4. State13 selects glow/blend by kind and
  entry27. Existing CE3C/CF08 transitions use the playback result of SetEntry;
  SetEntry dispatches to heading/cardinal helpers, which return playback status.
- **4CF5C / 4D120 (312/336):** timed directed motion at speed5/6. Recompute
  travel heading at timer30, move on subsequent ticks, then restart the timer
  at a random45–108. Reverse movement faces opposite travel and calls the
  retained distance-limiter helper. Signed scaling truncates toward zero.
- **4D270, state17 (264):** copies target position before comparing X/Z
  distance to that same target. Preserve this apparently redundant ordering.
  Select entry36 for moving kind5. Lock/charge select shake paths; otherwise
  run reverse directed movement and update the secondary lock.

## Lifecycle and corrected layout

**4D378 (312, no hints)** is the main tick. It clears transient flags, handles
segment-only mode and pending destruction, optionally requests the kind9 player
transition, clears movement/shake, dispatches the selected state, then updates
slope/world/charge/effects/secondary collider values. It attaches the actor's
position pointer at200 and updates the interaction probe beginning at1FC.
A final destruction check catches requests raised during the state callback.

**4D500 (204, no hints)** calls the eight initialization helpers in order,
clears glow/frame and **contactCooldown392**, sets parameter408 to-1, resets
request/action/destruction flags, reads kind9 keywords `t` and `b`, and installs
the singleton. Existing D4B0 destroys body/display/collider/segments/auxiliary
resources and clears the singleton; D5CC allocates a0x484-byte task in list9.

The following prior provisional interpretations are corrected by consumers:

- **3EC and418 are u32 script resource IDs**, named onActionScriptId and
  interactionScriptId. Script_Call ultimately masks the high bit, subtracts1
  and indexes the script table in0821ACDC; these fields are not pointers.
- **241 is packed action angle;242 is magnitude**, established by recoil trig
  indexing and decay. **400 is recoveryTimer;402 is recoveryDuration**.
- **473 is destroyRequested**, checked by the main tick and followed by
  Task_Kill. It is unrelated to the offset position at474. The latter is still
  named sampledPosition provisionally: AD5C offsets it128 units using heading+1.
- **47C remains a true effect pointer**: 08065C50 dereferences its identity and
  manager fields, destroys resources, clears0x138 bytes, and returns null or the
  retained pointer. It is distinct from the indexed effect addressed by408.
- Player word1C and fields458/45A remain explicitly unknown. The transition
  performs repeated global pointer reloads between stores; preserve them.

## Retained gap and verification

**4BF04 (512 bytes)** is a guarded **93.5%** roaming draft, with no exact credit.
It follows the interaction probe's target with mask841, records moving/heading,
conditionally calls interactionScriptId and updates the probe. Sixteen bytes
of scratch-register allocation differ, with unchanged body extent. Hint removal
reduced27 concessions to16 while retaining that score. The packed delta's
partial stores preserve padding; no explicit uninitialized read or r7 pin was
introduced. Candidate checkpoint:
`build/candidates/0499cba6e34f834d/attempt-ijwlo2ic`.

Fast complete-extent checks authorize individual commits. Combined-build and
post-promotion results are recorded in HANDOFF.md after batch validation.
