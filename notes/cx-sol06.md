# cx-sol06 campaign notes

Tags: #matching #hresvelgr

- 0819BB18: drafted 81.0%. Decoded retained tail directly; facing-relative polar offset, unused target position, sixteen randomized motion emissions. Remaining table-address lifetime and loop allocation differences. Reviewed extent 416 bytes.
- 0819BCB8: skipped after side-emitter reconstruction. Best 65.4%; signed random remainders, opaque dead literal and cursor lifetime differ. Reviewed body ends at 0819BE28 (368 bytes); the listed 636 bytes includes another retained tail. Local attempt retained under ignored build directory.

## Batch 1

Exact matches (reviewed body sizes):

- 0819E25C: ActorAction_FinishRecoveryAnimation, 120 bytes. Ported the matched adjacent recovery-state layout and added the cooldown store.
- 0819E3B4: ActorAction_BeginLinkedAnimation, 84 bytes. Entry-flag helper and a saved actor alias reproduce the linked callback and timer update.
- 0819F55C: DirectionBurst_EmitOffset, 88 bytes. An opaque offset local prevents arithmetic reassociation. Listed retained extent also contains a separate emission-loop tail.
- 0819F638: Hresvelgr_UpdateRandomVolleyPhase, 336 bytes. Structured vector, no redundant vector pointer, RNG operand order and constrained table/spacing locals.
- 081A09C4: Hresvelgr_PlayDescent, 196 bytes. Opaque actor alias retains the original second actor-pointer lifetime.
- 081A0EA4: Hresvelgr_EmitRandomMotionBurst, 332 bytes. Ascending source loop compiles to the observed countdown; constrained per-iteration RNG table keeps its load inside the loop.
- 081A0FF0: Hresvelgr_UpdateOscillatingMotionBurst, 328 bytes. Ported the same burst helper; scoped flag-mask local reproduces the flag-update order.
- 081A1D18: Hresvelgr_UpdateFlaggedMotionBurst, 324 bytes. Same burst helper, enabling flags before entry processing.
- 081A2858: Hresvelgr_UpdateCollisionPresentation, 868 bytes. Ascending collider loops, explicit camera fallback and mask, and reversed reset-pointer declaration order.
- 081A388C: EffectPool_AllocateEntry, 256 bytes. Correct five-argument Anim_SetSequence declaration is essential; its zero fifth argument accounts for the outgoing stack slot and allocation throughout the function.

Guarded drafts improved or added:

- 0819C5D4: 91.6%; target calculation is close; table load, argument-copy order and redundant angle mask remain.
- 0819C6AC: 89.7%; target-vector address and polar-helper lifetimes remain.
- 0819C870: 88.2%; sine-table pointer copy and scaled-speed register differ.
- 0819C8F8: 71.4%; cosine-angle common subexpressions and scaled-speed register lifetimes differ.
- 0819FE9C: 87.5% from 86.1%; removing the position pointer local reduced realigned differences from 123 to 114 and structural differences from 35 to 30.
- 081A3554: 95.1% from 91.5%; literal X coordinate and constrained coordinate copies leave four register differences around lifetime/collider arguments.
- 081A39E0: 97.4% from 96.9%; a shared trail-count pointer removes two register differences. Animation mask reload and trail-setup scratch choices remain.

Unchanged or deferred:

- 0819CD60: pointer-local deletion sweep regressed all candidates; retained 81.9% draft. Dot-product sharing and second normalization register lifetimes remain.
- 081A0BC8: retained 99.4% draft. Eight arithmetic variants and a bounded single-process permuter run did not improve the two remainder-register differences.
- 081A2378: retained 85.7% draft; changing radial-array address association regressed to 72.8%.
- 081A2430: retained 87.5% draft; typed trigger indexing regressed to 81.5%.
- 081A14FC, 081A1EFC, 081A2C80: inspected entry/control structure and existing notes; deferred large defeat/damage/script-initialization bodies for a later pass. No new draft claimed.

All accepted matches passed check.py with explicit reviewed target extents. Full batch build and integration result are recorded below when complete. Register constraints use only empty compiler barriers and are marked as matching concessions; no instruction bodies were added.

Batch 1 validation: the first combined build exposed an alias-related splitter bug that removed retained assembly after 0819F55C. The splitter now compares generated exact names; regression tests cover both alias directions and ensure the retained blob appears exactly once. Registered independent tail 0819F5B4 in build/progress inventories so its 132 bytes remain assembly credit. Focused splitter tests passed (nine test methods), and the corrected combined build printed `build/boktai3.gba: OK`.

Batch 1 exact gain: 0.1216 percentage points (2,932 bytes), including 0.1026 percentage points register-pinned (2,472 bytes) and 0.0191 percentage points without pins (460 bytes). Eight guarded drafts gained fuzzy coverage only.

## Batch 2, in progress

Batch 1 merged as PR 577 after the verified combined build.

- 081A3E48: matched EffectPool_CreateEntries, 188 bytes. Ascending three-entry loop; opaque entry pointer prevents unwanted loop-strength reduction of member addresses. Returns the defined final collider-initializer result. The field cleared at 159 is speed, not trail count.
- 081A4064: matched MotionEntry_UpdatePosition, 156 bytes. Reversing the commutative vector-add source operands closed the register differences without pins.
- 081A4100: drafted 95.1%, MotionEntry_Create. Defined null-origin local improves scratch allocation; nine register differences remain. Bounded permuter output with an uninitialized entry read was rejected; the defined null-origin alternative was reviewed and retained.
- 081A42AC: drafted 90.7%, MotionEntry_UpdateFalling (tentative), reviewed 372-byte body. Second flip-flag pointer lifetime and animation-mask allocation remain. Registered independent sibling tail 081A4420, also 372 bytes; it remains assembly.
- 081A4AB4: drafted 71.0%, Hresvelgr_EmitImpactExplosions (tentative). One centered and seven offset explosions. Observed Z jitter masks bit 9 rather than taking a 512 modulus. RNG register lifetimes and offset scheduling remain.
- 081A4DF0: matched Hresvelgr_InitializeProjectilePool (tentative), 232 bytes. Retains a defined animation-call result in r0. An inline clear helper taking both fill and destination pointers reproduces CpuSet argument scheduling.
- 081A4BF8: matched Hresvelgr_UpdateFallingProjectile (tentative), 404 bytes. Typed motion and terrain helpers match; loading phase before the resource-address local fixes the last four ordering differences.
- 080C47DC: unchanged 70.2%; pinning initial dx/dz to the diagnosed registers regressed to 63.2%.
- 080C5930: deferred after reviewing its entry/turn-state assembly and prior below-threshold reconstruction (37.1%, 443 structural differences). Repeated animation helper tails remain the obstacle; no new draft claimed.
- 080C725C: unchanged 91.3%; pointer-local sweep and RNG alias/table forms regressed. Removing RNG barriers trades extra seed stores for different allocation and instruction selection rather than a useful improvement.
- 080C6A68: unchanged 86.5%; three pointer-local deletions and eight declaration swaps did not improve its 93 realigned differences.
- 080CF3E4: unchanged 88.3%; no removable pointer locals in the target body; declaration swaps did not improve the 112 realigned differences.

The campaign wrap-up marker appeared before starting the Slime family. Stopped new matching work and began batch 2 integration immediately. Remaining assigned families were not attempted.

Batch 2 exact gain: 0.0407 percentage points (980 bytes), including 0.0174 percentage points register-pinned (420 bytes) and 0.0232 percentage points without pins (560 bytes). Three new guarded drafts add fuzzy coverage only.

Batch 2 validation: rebased onto current main, regenerated the documentation index after its merge conflict, and ran the combined ROM build. It printed `build/boktai3.gba: OK`. No code conflicts required resolution.

Campaign result at wrap-up: 14 matched functions, 11 new or improved guarded drafts, 13 unchanged/deferred functions; the remaining 52 assigned functions were not attempted after the stop marker. Exact gain is 0.1623 percentage points (3,912 bytes). Register-pinned gain is 0.1200 percentage points (2,892 bytes), and gain without pins is 0.0423 percentage points (1,020 bytes). Both batches received full ROM validation before submission.
