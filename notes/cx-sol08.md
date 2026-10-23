# cx-sol08 campaign notes

Tags: #matching #actors #particles

- 080AD75C: drafted 78.6%, tentative Vorn_UpdateStaggerRecovery. Reconstructed from the matched stagger and route relatives. Signed halfword masks and explicit flag pointers improved the first candidate. Remaining differences include flag-address lifetimes, state-store scheduling, and register allocation; reviewed extent 1076 bytes, candidate 1084.
- 080ADB90: matched, tentative Vorn_UpdatePursuitWait, 304 bytes. Entry clears context timer 69C; flag 6D4 bit 2 waits for distance at most 18FFF or a timer above 120 before restoring behavior 15. Separate byte and word zeros reproduce the reset sequence. Register pins and empty constraints preserve the observed address lifetimes; no instruction bodies. Register-pinned category.
- 080AE1C4: drafted 82.6% (previous recorded 82.1%). Absolute completion-latch access and a scoped store value remove four structural differences. Reset pointer lifetimes and collider scheduling remain.
- 080AE6F0: drafted 84.6% (previous recorded 83.9%). Retaining a separate byte-reset zero across the route call removes five structural differences. Flags and state-address reuse remain.
- 080AEB74: drafted 87.4% (previous 80.2%). Signed halfword clear helper plus the absolute completion-latch access improve address/register reuse. Terrain spill and reset scheduling remain.
- 080AEE08: drafted 88.3% (previous 83.4%). Explicit script-field offset lifetime improves subsequent register allocation. Eight structural differences and late state reset allocation remain.
- 080B0038: drafted 84.2%, tentative Vorn_RestoreRouteMovement. Context group base requires a distinct lifetime; callback result must truncate to byte. Remaining flag scheduling and shared callback-reset addresses.
- 080B03BC: skipped. Reconstructed action-entry and saved-control reset paths, but a clean candidate reached only 67.3%; carrying the known zero through a reset helper worsened scheduling. Prior attempts identify the same reset-address derivation residue.
- 080B162C: drafted 88.7% (previous 88.1%). Retained final mask and the action-mask helper improve instruction ordering; state-reset offsets and route flag pointers remain.
- 080B197C: drafted 80.0%, tentative Vorn_SelectReactionControl. First flag result needs the mask register; the motion result remains distinct from reset values. Motion-pointer lifetime and state reset scheduling remain.
- 080B1FA8: skipped. Reconstructed opposing-point scan, random fallback, height snap, and route attempt at 58.3%. Signed packed-vector aliases preserve the dot-product halfword loads. Moving selection outputs to the caller worsened it to 55.8%; stack vector lifetimes and loop-success layout remain too different.
- 080B23A0: drafted 89.0% (previous 83.4%). Volatile halfword reads preserve the two separately observed context flag loads. Seventeen structural differences remain.
- 080B2580: skipped, retained 94.5% draft. Separate success/failure loop exits and declaration swaps did not fix map-width rematerialization or stack-slot order; loop-exit alternatives worsened the candidate.
- 080B2858: skipped, retained 90.7% draft. Reviewed realigned structural differences. Volatile inline group, slot/vector order, and mask operand changes did not improve actor copies and group spills.
- 080B2EEC: skipped, retained 89.2% draft. Tested distinct owner lifetime, zero width, mask order, derived flag offset, and timer-result lifetime. Deriving the offset reduced one structural difference but worsened total differences; retained baseline.
- 080B3110: drafted 77.6%, tentative Vorn_BeginResponsePursuit. Reused the collision-reset prefix on the context object. Explicit saved zero values worsened 77.6% to 66.8%; retained the readable candidate. Reset scheduling and group-address lifetimes remain.
- 080B3464: skipped, retained 87.8% draft. Signed halfword masks and motion-mask width did not change generated code; duplicated resume/reset lifetimes remain.
- 080B3ADC: matched SceneActor_DispatchContextAction, 244 bytes. Retaining the incoming reset argument in its entry register until the actor input is copied fixes the final prologue scheduling difference. Register-pinned category.
- 080B3C1C: skipped. Contact acquisition/reaction candidate reached 67.5%; signed clears and carrying known reset values worsened it. Corrected the final observed flag store to flags2D6. Pointer lifetimes and reset scheduling remain below the prior 69.9% attempt.
- 08085DD0: skipped after complete condensed assembly, matched-relative review, and prior attempt review. Prior full saved-target throw candidate stopped at 62.3% with 188 structural differences; the repeated terrain scan and hoisted projectile argument addresses remain the unresolved shared structure.
- 08086400: drafted 88.1% (previous 87.8%). Direct actor direction access removes seven structural differences. Removing the inherited restart pin regresses substantially; animation tails remain unresolved.
- 08089168: skipped, retained 99.1% draft. First signed-source load uses r3 rather than r1 for its zero scratch. Pointer/source/offset constraint variants and explicit zero offsets did not help. Compiler-space solver tested 1,943 single priority/reload changes without improvement and classified the residue as instruction selection or CSE.
- 08089340: skipped after complete condensed assembly and prior attempt review. Shares terrain probing and projectile argument hoists with 08085DD0; prior complete candidate 63.2% has the same unresolved structure.
- 0808A664: skipped. Return-action reconstruction reached 57.2%; group/context/motion pointer pins worsened it to 44.5%. Distinct byte/word zero lifetimes and control-reset address derivations remain.
- 0808AF38: skipped. Ported the equivalent Vorn opposing-point candidate at 58.3%; repair confirms structure is too different for constant substitution (105 structural differences). Shared vector/scan layout remains unresolved.
- 0808B264: skipped. Decoded the raw-byte function boundary and reconstructed route-speed selection. Explicit saved zeros and flags improve the candidate from 26.4% to 51.6%, but reset scheduling and route-index addressing remain far apart.
- 0808CC48: skipped, retained 86.1% draft. Context depinning, delay parameter width, and split reset zeros did not improve route-slot and flags lifetimes.
- 080B77D0: skipped, retained 94.3% draft. Shared cardinal scan tails and a distinct failure-callback group address did not improve cached route/width addresses.
- 080B7A90: matched SceneActorTarget_ResolvePendingObjectSkeleton, 292 bytes. Distinct input and saved target-link pointer lifetimes reproduce the missing copy before the flag test. Register-pinned category.
- 080B8DE0: matched Skeleton_OnAttackCollision, 252 bytes. Keep initial context flags in their input register and retain the tested flags pointer separately for the second test. Avoid assigning it back to the pinned original pointer. Register-pinned category.
- 080BA268: skipped after full condensed assembly and prior attempt review. Packed stack-vector arithmetic, two long particle calls, and category reinitialization retain the previously unresolved stack and register scheduling.
- 080BA688: skipped, retained 98.8% draft. A step-base helper fixes the Z load shape but removes the required stack-base rematerialization. Pointer barriers and clobber variants introduce copies or spills; baseline retained.
- 080BA7E4: skipped, retained draft. Reviewed extent scores 98.6%. Step-Z base helper and DMA-control lifetime variants fail to fix both address shape and control-load placement.
- 080BAB80: skipped, retained draft. Reviewed extent scores 98.4%. Earlier spawn-pointer lifetime reduces structural differences from four to two but changes eight registers; explicit pointer constraints worsen alias scheduling. Baseline retained.
- 080BAE04: skipped after complete condensed assembly and prior attempt review. Skeleton variant of the large directional-turn state uses profile-selected animation entries and duplicated keyframe tails; prior complete attempt already identifies the unresolved helper and lifetime structure.
- 0809EF50: matched Crow_PlayEntry2, 212 bytes. Removing the fixed flag-register hint and retaining the flag value through the animation call reproduces the saved-register frame. Remaining tile/spare hints keep this in Register-pinned category.
- 0809F510: matched Crow_HopWithinHomeCell, 828 bytes, ordinary C. An entry-flag helper retains its pointer after the flag load; a zero-and-copy helper and DMA argument order reproduce the remaining store/load scheduling.
- 0809F84C: skipped, retained 91.5% draft. Minimum control-flow, motion-copy fields, and mask-order variants do not improve clamps and vector padding handling. Kept defined padding behavior.
- 0809FC74: skipped, retained 98.7% draft. Final wander-offset literal uses r2 rather than r3. Explicit offset pin, address macro, and retaining the RNG table through the store all disrupt earlier allocation.
- 080A09DC: drafted 97.3% (previous 96.1%). Removing the height pin fixes literal derivation and reduces structure to zero; seven terrain-height register choices remain. Explicit attribute/mask pins regress.
- 080A0E00: skipped, retained 99.2% draft. Explicit reset-zero lifetime, helper call, memory barrier, and approach argument-order variants do not fix the two scheduling residues without broader allocation changes.
- 080A11F4: skipped, retained 89.0% draft. Separate reset widths, direct state access, counter-load width, and ground-load lifetime do not alter the remaining scheduling and reset-offset differences.

## Wrapup

Stopped when build/CX_WRAPUP appeared before beginning the particle-group family.
The first pass covered all Vorn, Ghoul, Skeleton, and Crow assignments. Later
families remain unattempted in this campaign. Six exact matches and twelve new
or improved guarded drafts are retained; twenty-four functions were skipped.
Guarded drafts add no exact coverage. Five exact matches use register-pinning;
the 828-byte Crow hop match uses ordinary C.

Validation: rebased onto current main; all function commits applied without
conflicts. Shared history/index conflicts preserved both sides. Combined
build/cxbuild validation printed `build/boktai3.gba: OK`. Privacy range check
passed. Batch integration preserves the per-function commits.
