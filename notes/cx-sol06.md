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
