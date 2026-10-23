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
Corrected combined validation: `build/boktai3.gba: OK`.

## Batch 3 — session h2 continuation

Tags: #matching #vanargand #animation #effects

- 081B2608 drafted 95.3% (was 94.7%). Retaining the consumed entry-byte value
  across a constraint improves entry allocation. Reset offset pins, typed
  state stores, a flag-taking helper and playback-mask constraints did not
  close the transition-address and entry-copy residue.
- 081B28D8 skipped at retained 99.9%. Four bytes reverse two independent
  copies in state four. Tried saved flag pointers, pointer/declaration
  bindings and address-input constraints. Compiler pass flips found no
  improvement; a bounded two-process focused permuter run saved no candidate.
  Revisit this closest draft before campaign completion.
- 081B2F38 drafted 96.0% (was 93.2%). Removing the entry-pointer binding
  halves the structural residue. Tried a volatile saved X, a Z binding,
  retained motion/one lifetimes and a mask binding. Remaining X spill,
  motion argument copies and playback constants.
- 081B3410 drafted 94.6% (was 94.5%). Ordered animation setup and loading
  base height before player height reduce structural differences from 18
  to 11. Ground-strike stepper transplant and motion-pointer variants regress.
- 081B3AAC drafted 96.6% (was 90.3%). Separate entry-value loads before
  retaining the entry pointer recover all three phase entries. Destination
  pointer aliases/bindings and zero-argument reuse do not close the remaining
  player-relative vector setup (15 total / four structural differences).
- 081B3DB8 drafted 89.3% (was 87.4%). An inline scale argument gives the
  expected shift/multiply pair; a state setter recovers transition ordering.
  Clear-vector helper, explicit position cursor and scale barriers regress.
- 081B4040 drafted 85.2% (was 81.0%). Reused the scale argument, release
  stepper, state setters, earlier loop-index initialization and reversed X
  addition. Eleven structural differences remain, chiefly loop cursors,
  stack addresses and state-zero entry lifetime.
- 081B431C drafted 92.2% (was 84.9%). State setter, vector scale argument,
  unbound owner, shared animation zero, reversed X addition and duration-mask
  temporary recover most of the body. Remaining 57 total / six structural
  differences include final completion-store sharing and vector cursors.
- 081B48A0 drafted 97.0% (was 93.9%). Move the first mask into playback,
  load entry separately, remove the sample constraint, reverse X/Z addition,
  and write origin Y outside the planar helper. State/entry bindings regress.
  Remaining 15 total / four structural differences.
- 081B4DA4 reviewed-only skip: confirmed prior boundary evidence. The listed
  extent includes another independent function at 081B512C; earlier scratch
  source is absent here. Needs a complete new draft and boundary recovery
  during the required revisit, rather than another review-only disposition.
- 081B562C drafted 90.0%, `Vanargand_UpdatePartEmergence` (tentative).
  Full new C models hidden/display flags, two owner keyframe tracks, radial
  emergence, contact setup and screen projection. Reused reveal-fade layout
  and packed stepper. Ordered animation data/zero setup is the strongest
  improvement. Projection uses implicit struct padding. Reviewed full extent
  1132 bytes through 081B5A98. Default fallback exact.
- 081B5E38 matched, `Vanargand_DestroyPartController` (tentative), 108 bytes.
  Removes owner animation and two embedded colliders/sprites/shadows/nodes,
  then clears the singleton. Binding the per-iteration stride to r0 prevents
  invariant hoisting. Depin removes three of four hints; one stride binding
  remains. Reviewed full body/pool through 081B5EA4.
- 081B5EA4 drafted 72.0%, `Vanargand_InitializePartController` (tentative).
  Full new resource/palette setup and two-part initialization, including
  hidden and visible modes. Multiple advancing field cursors, a call-clobbered
  stride, delayed keyframe pointers and earlier index initialization improve
  the first loop. Mode/palette/half/key bindings regress. Second-loop cursor
  addressing and action guard dominate the remaining residue. Reviewed full
  732-byte extent through 081B6180.
- 0818EA90 exploratory scratch reached 51.1%. Full C queues unused falling
  slots around player X/Z with 224-unit heading steps and ascending heights.
  Split flag/position/target cursors and constrained loop stride reduce
  structure but allocation is still far off. Needs deeper retry before list
  completion; scratch is ignored under wip/cx-sol02/.
- 0818ECFC drafted 73.5%, `ScenePanelEffect_UpdatePool` (tentative).
  Full new C dispatches 24 simple and 48 collision effects, clears occupancy
  bits, applies anchored/delta motion and staggers collider updates by parity.
  Based on ScenePanelEffect_InitPool's observed layout. Separate motion/position
  cursors and pool/remaining bindings regress. Reviewed 316-byte extent through
  0818EE38.
- 0818EE38 matched, `Nidhoggr_StepPoolAnimation`, 288 bytes. Applying the
  Vanargand packed-stepper operand, mask and duration patterns closes the
  retained draft in one check. Depin removes one of twelve hints.
- 0818EF64 drafted 80.8%, `Nidhoggr_StepMovingSpriteEffect`.
  New full C combines packed playback, position integration, signed velocity
  damping/gravity and lifetime completion. Union word/bitfield views retain
  the stack acceleration vector; explicit low/high packing recovers paired
  stores. Owner binding improves fuzzy to 85.6% but adds structural copies;
  retained the full-sized 80.8% form with seven structural differences.
- 0818F0FC matched, `ScenePanelEffect_TickSpriteLifetime` (tentative), 284 bytes.
  Reused the complete stepper and lifetime tail from the moving-effect draft.
  Reviewed body plus alignment through 0818F218.
  Eleven hints remain after depin.
- 0818F6A4 drafted 87.9% (was 87.1%). Deeper iteration after steering:
  move age-address creation after playback, retain separate X/Z field pointers
  and a saved position through terrain work, then load the owner before the
  state-255 sprite argument. Reduces structure from 35 to 18. Tested age/state
  bindings, unsigned-coordinate views and owner unbinding; these do not close
  it. Pass flips find no improving compiler switch. The optional causes mode
  needs an absent local causebook database; no tool changes made.

This batch adds three exact matches: +0.028 percentage points (680 bytes), all
Register-pinned. Thirteen guarded drafts are new or improved. Resume at
0818FA54 after integration, checking origin/main and the wrapup marker first.
Required revisit queue includes all earlier-session skips plus 081B28D8,
081B4DA4 and 0818EA90; prioritize the closest retained drafts.

Batch 3 integration: the initial build caught five redundant extra-boundary
rows. These entries already have decoded starts in gen/code.s despite lacking
CSV names. Removed the rows from the corresponding function commits before
rebuilding; no generated files changed. Documentation conflicts preserve both
workers' history entries, followed by index regeneration.
The next build caught a named fallback using its raw address token. Removed
that draft-only alias so both the recorded candidate and default fallback use
the existing descriptive symbol. Both modes rechecked. Corrected combined
validation: `build/boktai3.gba: OK`.


## Session h2, batch 4: seeking effects and script combat actors

Tags: #matching #nidhoggr #effects #script-combat-actor

- 0818FA54 matched, `Nidhoggr_CreateFallingAttackEffect` (tentative),
  full 428-byte extent. Reused the matched sibling constructor, advancing the
  animation cursor to recover collider allocation. Grouped callback loads,
  reversed vector operand order and a mask argument helper recover scheduling.
  Inverting a retained collision mask before the final flag load avoids bics.
  Two register hints remain after depin.
- 0818FEE4 drafted 64.3%, `Nidhoggr_UpdateSeekingAttackEffect` (tentative),
  full 912-byte extent. New complete C models playback, acceleration, seeking
  angles, randomized steering, proximity arming and both impact exits. Flat
  playback, explicit phase labels and releasing the initial state pointer
  reduce the core difference to 38 instructions. Tested volatile motion,
  vector arrays, direct coordinates, cursor helpers, phase bindings and
  compiler-pass flips; these plateau or regress. A superficially higher fuzzy
  form had materially worse structure and was not retained.
- 08190274 matched, `Nidhoggr_CreateSeekingAttackEffect` (tentative),
  full 392-byte extent. Reused the falling constructor with seeking callbacks,
  shared bounds and enabled default shadow. Declaring the shared bounds table
  without const prevents word caching across calls, recovers its saved pointer
  and makes the constructor exact. Two register hints remain after depin.
- 082015D4 drafted 96.7% (was 82.4%),
  `ScriptCombatActor_ApplyHitReaction`. Typed recipient fields reproduce the
  first half exactly apart from argument-copy order. Batched cursor updates,
  explicit index/count lifetimes, preserving the blocked recipient pointer,
  reversed flag intersection and HUD argument bindings reduce the residue to
  eight total / two structural differences (22 bytes). A focused permutation
  search identifies a redundant HUD value binding; removing it is the last
  improvement. Mask barriers, final-sample
  bindings, pointer bindings and alternate loop forms regress. Compiler-pass
  flips find no improvement. Default fallback remains exact over 432 bytes.
- 08201784 matched, `ScriptCombatActor_InitializeColliders` (tentative),
  full 276-byte extent, without register hints. Explicitly preserving the
  upper packed word when clearing the first collider offset recovers the
  target word load/mask/store. Removing the owner binding then recovers all
  register allocation. Pointer, size and collider bindings regress.

Batch 4 adds three exact matches: +0.0455 percentage points (1096 bytes),
including 820 Register-pinned bytes and 276 bytes without hints. Two guarded
new/improved drafts remain. Resume at 08201A78 after integration, then continue
through 08201D38 and 0820212C before the tagged NPC family. The required revisit
queue from earlier batches remains outstanding.

Batch 4 combined validation after rebase: `build/boktai3.gba: OK`.


## Session h2, batch 5: script combat actors and tagged NPCs

Tags: #matching #script-combat-actor #scene-npc #palette

- 08201A78 matched, `ScriptCombatActor_UpdateProjectileAttack`, full 564-byte
  extent, without hints. Reused the attack-start handler layout and a matched
  orbit-particle RNG sequence. An explicit signed-byte aim temporary retains
  the target sign extension; an inline vector copy emits the destination
  address before copying. Typed origin/position fields and staged RNG cursor
  updates recover the tail exactly.
- 08201D38 matched, `ScriptCombatActor_UpdateState` (tentative), full
  1000-byte extent. Reused the packed sprite-playback helper, with first-bit,
  mask, duration and weight lifetimes adapted to this actor. Retaining the
  damage offset recovers constant reuse. A temporary disabled-collider pointer
  and a bound receiving pointer recover the final missing copy. Fourteen
  hints remain after depin. Callback use is confirmed by 08202608.
- 0820212C matched, `ScriptCombatActor_InitializeFromScript` (tentative), full
  1244-byte extent. Ported the matched PoisonHand initializer, mapped every
  script field and added period/delay options. A complete typed actor layout
  moves the first draft from 85.9% to 98.8%. A sprite-load clobber, frame pointer
  and retained initial-death field offset close the remaining bytes. Nineteen
  hints remain after depin. All script, collision, terrain and scene-body paths
  are retained; no extra split boundary is required.
- 082015D4 matched, `ScriptCombatActor_ApplyHitReaction`, full 432-byte extent.
  Required revisit closes the prior 96.7% draft. A forward-counting three-pass
  loop produces the reversed count initialization after invariant hoisting.
  Explicit index increments, a table binding after vector copies and separate
  blocked-state zero recover scheduling. Passing the final sample to an inline
  coordinate setter permits destructive reuse of the dead index register.
  Four hints remain after depin. The script combat actor family is now exact.
- 081D1908 drafted 44.5%, `SceneNpc_BlendPalette`, full new C for sixteen
  RGB555 palette entries. Explicit packed-color shifts, a palette-binding
  helper and retained blue channel reduce structure to 22 differences.
  Tested separate result variables, phase/divisor bindings, scratch records,
  helper argument order, loop forms and pass flips; none closes the channel
  spill and loop allocation plateau. The strongest structural form is retained
  rather than a superficially higher fuzzy form. Default fallback is exact
  over 264 bytes. The observed palette binding is at actor offset 0x148.
- 081D1A44 matched, `SceneNpc_EnterIdleFacing` (tentative), full 72-byte extent,
  without hints. Initial matching constraints are all removed by reusing the
  signed-short facing helper from 081D1BA8; this preserves arithmetic shifts
  and mask operand order naturally.
- 081D1A8C drafted 61.2%, full new walking-handler C. Reused the signed-short
  facing helper, fixed-point scale helper and target-distance pattern. Keeping
  the speed and heading lifetimes improves structure to six differences.
  Tested alternate helper argument orders, position/target bindings, owner and
  distance-result bindings, a typed velocity view and an enclosing velocity
  helper; these plateau or regress. A bounded permutation search produced only
  a weaker, more complicated candidate. Default fallback is exact over 284
  bytes. Source remains guarded; the unguarded legacy check otherwise compares
  only the candidate length, which can distort these short-layout scores.

The real wrapup marker reappeared before 081D1C38. Its assembly was inspected,
without starting a C draft; it remains unattempted for the continuation.
Resume at 081D1C38, then 081D2160, 081D2318 and 081D2610. Continue the original
function list afterward and perform the recorded earlier-session revisit queue.
This batch has five exact matches: +0.1374 percentage points (3312 bytes),
including 2676 Register-pinned bytes and 636 bytes without hints. Two new full
NONMATCHING drafts are retained. No function is newly classified as skipped.
