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

## Resumed particle-group batch

Tags: cx-sol08, particle-groups, compiler-debugger

- `0801404C` matched, 740 bytes, `ParticleGroup_UpdateBouncing` (tentative), register-pinned. Full reconstruction from the ordinary particle update and terrain helpers. Random evaluation must precede velocity loads; a memory boundary prevents reuse of the cell-index position load. Keeping the group pointer live after selecting the resource gives the ROM's caller-saved resource lifetime. Removed an unnecessary hint with depin; exact check passed after formatting.
- `08014330` matched, 360 bytes, `ParticleGroup_UpdateFrozen`, register-pinned. Pin the temporary type load to r0, then let its ordinary copy allocate to r3. Depin removed the redundant first barrier. Exact check passed after formatting.
- `08014634` drafted 77.7%, 664-byte target. Full randomized group initializer with terrain cell, lifetime/velocity randomization, sprite setup and inactive tail. Iterated evaluation order, loop induction, typed copy, stack-address operands, variants and compiler flips. Thirteen structural differences remain; primarily stack byte addressing, register/copy lifetimes and mask evaluation. Fixed-register loop-table experiments were discarded after invalid allocation; retained source uses an ordinary table pointer.
- `080148CC` drafted 59.0%, 860-byte target. Full radial particle spawner: angular spread, randomized horizontal/vertical speed, resource palette and normal group initialization. Several variant rounds reduced 109 structural differences to 34. Explicit direction-output copies prevent incorrect cross-branch tail merging; a byte-scaled angle offset reproduces the invariant lookup. Compiler flips did not help; constant repair found no improvement. Remaining differences are mostly group/table lifetimes and stack slots.
- `08014CA4` drafted 85.4%, 704-byte target. Full fragment-group initializer using the matched update's observed layout. Separate array cursors, a distinct group base and moving random initialization to its evaluation point raised the first 55.9% draft. Iterated variants, compiler flips and local-pointer sweep; animation retention and cursor setup remain, 22 structural differences. Discarded overlapping fixed-register speed/lifetime experiment; retained speed pointer uses an ordinary pseudo after a scoped hint.
- `081FF454` drafted 98.7% from 93.3%, 1,256-byte target. Retain a case-local value one in r5 for the initial flag, state transition and first flip; passing it into Face removes the extra high-register save. Delay pointer r2 and an unsigned height-store helper remove the remaining structure differences. Inline blink cursor setup retains the timer for increment. Full length and zero structural differences; eight register-only instructions remain (two trail table reloads, one final facing pointer, blink signed-load zero). Ran compiler flips, several source variant rounds and a 2,457-compile allocation diagnosis; a synthetic allocation solution got within six bytes. Scoped table helpers disturbed unrelated vector initializer lowering, so discarded them. Revisit this close miss after the full pass.
- `081FFAB8` drafted 86.8% from 84.4%, 704-byte target. Velocity pointer in ip improves the branch and integration stream, while keeping a readable vector helper. Tested explicit flags/manager pins, index lifetime and volatile index reads, helper argument copies and compiler flips. Combined pins and a counter rewrite regressed. The retained version still differs in 20 structural instructions, especially flags-pointer lifetime, slot retention and sequence mask scheduling; the exact-size 84.4% baseline remains in history.
- `081FFE94` matched, 808 bytes, `HomingProjectile_Spawn`, register-pinned. All eight initial differences were reload-register choices at the final sprite flags, priority and active-mask update. Scoped pointer/offset hints resolve those choices without changing the instruction stream; depin removed two hints, and the formatted source passed the exact check.
- `08017510` matched, 372 bytes, `Weather_UpdateTilemapScroll` (tentative), register-pinned. Full reconstruction of weather layer scroll and drift. Typed camera fields retain the shared base address. Two separate compound stores preserve the original step-then-drift arithmetic. Direct global random-cursor references let CSE reuse the updated seed; masked sample and read-before-save helpers fix register/ordering choices. Depin reduced nine hints to four; formatted exact check passed.
- `08017684` skipped at retained 98.3% draft after iteration. Only the animation-field address and loop-count initialization swap order. Tried for/do/while induction spellings, explicit field pointer, evaluated address, zero/count ordering, body pointer, volatile resource, pinned count, and an extracted initializer helper. None improved the baseline; no tracked C change.
- `080177AC` drafted 80.4%, 704-byte target. Full two-endpoint terrain particle spawner, including cooldown, flags, slope height, sprite setup and random delay. Began at 65.6%; iterated helpers, source layouts, compiler flips, direct random cursor access, fixed sample registers and loop variants. Flattened second-endpoint fields remove derived-pointer spills. Retaining one across the effect-link call matches the final active store. Eleven structural differences remain, mostly counter spill and delay-halfword address setup. A two-counter alternative has nine structural differences at 80.3%; an aggregate-argument experiment reaches the exact length but scores lower; both remain scratch candidates.

## Weather continuation

Tags: #matching #weather

- `08017A6C` matched, 332 bytes, `Weather_UpdateStormTrigger` (tentative), register-pinned. Full reconstruction of brightness/display reset, pause/light gates, cooldown clamp and randomized storm entry. A shared transition label restores branch layout. Scoped timer-address evaluation prevents keeping the 0x630 offset across calls. Passing the display address before its mask reproduces load order. Depin reduced eleven hints to three; formatted exact check passed.
- `08018028` matched, 724 bytes, `Weather_Initialize`, register-pinned. Completed the 90.9% draft by placing the final random table, cursor, index and masks at their evaluation points. Reading the sensor through a scoped address after the player-pointer load prevents keeping its offset across the clamp. This removes the extra r7 frame save. Depin reduced eighteen hints to three; formatted exact check passed.
- `080E6E08` matched, 680 bytes, `Mimic_UpdateHitRecovery`, register-pinned. Completed the 88.4% draft. Route loading must precede position/target pointer setup; retain the flags pointer in r8 immediately after the first navigation test. Shared failure label reproduces route-flag clearing. Separate stopped-control setup retains the cleared zero for the context mask; helper argument order preserves packet setup. Final field getters resolve local reload choices. Depin reduced 103 hints to 43; formatted exact check passed.
- `080E7134` matched, 912 bytes, `SceneActor_LeapTowardTarget`, register-pinned. Completed the 95.9% draft. Retain the heading pointer through the vector setup and separate the context-field and motion-vector address calculations. Removing the context offset barrier makes its constant use a normal reload and rotates the following signed-load scratch registers into the ROM's choices. Allocation diagnosis independently found the same two-slot reload rotation. Depin reduced nine hints to six; formatted exact check passed.

The real wrapup signal arrived before starting `080E7D9C`; its existing draft
was only inspected and remains untouched. The next pass starts there, then
continues the remaining assigned functions. The first-pass reviewed-only and
existing-draft skips still need the requested deeper revisit, closest scores
first, after the remaining functions have received full attempts.

This final batch adds four exact register-pinned matches, +0.1099 percentage
points (2,648 bytes). Combined with the two landed batches, this campaign
retains fourteen matches, eighteen new or improved drafts, and twenty-five
skips; thirty-two assigned functions remain unattempted at the stop signal.
