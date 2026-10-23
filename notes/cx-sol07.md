# Campaign worker sol07

Tags: #matching #ratatosk #particles #nonmatching #register-pinning

## Batch 1 (2026-10-23)

Eight exact matches add +0.1487 percentage points of code coverage (3,584
reviewed bytes): +0.0433 pp without hints and +0.1054 pp register-pinned.
Fast checks use reviewed full extents. Combined build and merge are pending.

| Address | Result | Notes |
| --- | --- | --- |
| 081F2E34 | drafted 97.6% | Count pinned to r6; only the two loop initializers remain reversed. Byte-offset alternatives and an offset pin did worse. |
| 081F3414 | deferred | The 120-byte retained extent contains two apparent 60-byte bodies, the second starting at 081F3450. Boundaries need repair before full-extent acceptance. |
| 081F3C68 | deferred | Reviewed attack setup and prior 69.7% attempt; no Ghidra export or retained candidate available locally. Large radial-spawn body remains assembly. |
| 081F423C | drafted 98.1% | Passing the shared zero into the palette helper fixes most differences. Remaining zero initialization occurs before the direction load. Returning the zero from the facing helper and pins did worse. |
| 081F4378 | skipped | Retained 98.6% baseline. Six differing instructions: polar destination scheduling, final component load order and a reload register. Helper argument variants and high-byte scheduling worsened it. Bounded permuter setup could not resolve the inline helper symbol. |
| 081F5120 | already matched | Existing unguarded source checks exactly over 184 bytes; no new credit or change. |
| 081F55B0 | drafted 82.4% | New orbiting-particle pose. Timer/radius lifetimes and vector load order remain; helper argument permutations and radius pins did not improve the retained candidate. |
| 081F5E60 | matched | Ratatosk_UpdateDecayingShake, 200 bytes, no hints. Derived from the adjacent spawn-shake callback. Compute remaining duration before angle/speed shifts. |
| 081F6748 | matched | SceneActors_FireTrackingProjectileSpread, 332 bytes, register-pinned. High-byte facing temporary and one barrier place the vector address before truncation; step bound to r8. Depin retains two hints. |
| 081F79F8 | matched | Ratatosk_ProcessDefeatState, 172 bytes, no hints. Inline argument order loads the callback before state ID and the mask before the flags address. |
| 081F7DAC | matched | Ratatosk_UpdateCloneAttack, 548 bytes, register-pinned. Give the random helper its table pointer before incrementing the index; retain the same coordinate temporary across both modulo calls. Depin retains three hints. |
| 081F8600 | matched | Ratatosk_UpdateSunOrbPlacement, 672 bytes, no hints. Typed 24-byte point records fix address arithmetic. A sample temporary and RANDOM_TABLE_ADDR fix random-load scheduling and reuse the existing field offset. |
| 081FB8A0 | matched | Ratatosk_UpdateParticlePool, 520 bytes, register-pinned. Sprite/config declaration order, mask-first helpers and a copied velocity pointer reproduce loop scheduling. Depin retains eleven hints. |
| 081FBBA8 | matched | SceneParticles_SpawnAtPosition, 564 bytes, register-pinned. Return the sprite link pointer through ip from the inlined sequence stepper; keep the byte-store address in r1. Depin retains twelve hints. |
| 081FBF2C | matched | Ratatosk_UpdateAnimatedParticlePool, 576 bytes, register-pinned. Ported the related loop to 32 records of 120 bytes. Signed lift handling and appearance bits gate the sequence stepper. Depin retains ten hints. |
| 081FC26C | drafted 72.9% | New randomized animated-particle spawn. Full control flow and flags recovered; random-source lifetimes and sprite/sequence allocation still differ. Pins and explicit random-source parameters worsened the baseline. |

Remaining Ratatosk targets, including the larger callbacks temporarily deferred
while establishing the reusable particle loops: 081F4CF8, 081F5290, 081F6428,
081F6B90, 081F7FD0, 081F92C8 and 081F9BEC. Resume these before the next family.
No totals files, symbol registry or shared generated files were edited.

## Batch 2 (2026-10-23)

Tags: #matching #ratatosk #tentacle #nonmatching

Session 2 found no new exact matches: +0.0000 percentage points of exact code
coverage. Two new guarded drafts preserve recovered behavior. The wrap-up flag
arrived during tentacle-family triage, so the remaining list is unattempted in
this session. A final branch audit confirmed Batch 1 was still unmerged;
its eight matches and four drafts are included in this integration.

| Address | Result | Notes |
| --- | --- | --- |
| 081F2E34 | skipped, retained 97.6% | Tried input-only and read/write barriers, byte offsets and offset bindings to reverse the two initializers. These disturbed strength reduction or allocation. Bounded one-process permuter search found no exact candidate. |
| 081F423C | skipped, retained 98.1% | Tried delaying the shared zero behind explicit angle/address operands and a high-register clobber. This introduced an extra high register and pointer copies. The hint tool could not compile the isolated source. |
| 081F4CF8 | skipped, retained 86.1% | Passed random table/mask explicitly, moved the spawn pointer out of the loop and tested actor/table/mask bindings. Stack layout and random-source lifetimes worsened. |
| 081F5290 | skipped | Reconstructed recoil, scale decay, particle emission and recovery/defeat transitions. Tick/owner bindings and explicit scale pointers remained below the prior recorded attempt; no draft promoted. |
| 081F6428 | drafted 64.8% | Ratatosk_ApplyQueuedDamage (tentative), reviewed extent 800 bytes. Consumes queued damage, advances health phases, applies vulnerability scaling, emits three sparks and transitions to recoil or defeat. Random-source lifetimes, particle field accesses and callback setup differ. |
| 081F6B90 | deferred | Inspected command dispatcher and its jump table. No Ghidra export is present; no dominant full-function port identified. No new candidate. |
| 081F7FD0 | deferred | Inspected the attack-state entry and four-phase dispatcher. Large projectile/position body remains assembly; no new candidate. |
| 081F92C8 | deferred | Inspected callback dispatch and interrupt gating. Large state-selection body remains assembly; no new candidate. |
| 081F9BEC | drafted 67.9% | Ratatosk_OnDamageCollision (tentative), reviewed extent 544 bytes. Queues collision damage, derives impact direction, tracks repeated attacks and triggers feedback. Attack/flags lifetimes, masks and address scheduling differ. Broad bindings reduced instruction residue but worsened byte similarity and were discarded. |
| 080A7C84 | skipped, retained source | Reviewed-extent check reports 98.0%. Tried offset bindings, preserving its lifetime through the callback and an explicit shared state exit. The callback-offset and zero-state scheduling differences remain. |
| 080A8280 | skipped, retained 90.3% | Tested motion/x bindings, a split turn temporary, vector operand order and removing the motion alias. No coherent structural improvement; retained the original draft. |
| 080A8904 | skipped, retained 87.6% | Tested indexed stack initialization, random-table spelling, surface widths and facing signedness. No useful improvement. |
| 080A8F38 | skipped, retained 95.5% | Tested a local pinned mask to stop hoisting and removed the secondary sprite-pointer binding. This changed the frame and mask schedule without resolving the sprite-pointer spill. |
| 080AA688 | skipped, retained 96.1% | Three stride-register instructions remain. Explicit collider/flag pointers, stride bindings, clobbers and a bounded one-process permuter run found no exact candidate. The permuter base score did not reproduce the fast checker baseline; no output accepted. |
| 080AA744 | skipped | Reconstructed motion-dependent scatter mask and active-segment velocity generation. Sequential dot-product terms and loop/owner bindings improved the new local attempt to 62.0%, below prior documented work. No draft promoted. |

Ten functions received matching attempts and were skipped, two received new
drafts, and three larger callbacks received static triage only. No exact credit
is claimed. Remaining function-list entries, including the earlier deferred
large Ratatosk callbacks, still need future work. All unaccepted experiments
remain in ignored local scratch storage.
