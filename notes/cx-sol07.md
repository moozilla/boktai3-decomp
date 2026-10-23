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
