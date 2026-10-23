# cx-sol01 campaign

Tags: #player #matching

- 0813ADD0 matched: local typed facing fields reproduce the folded direction and side stores.
- 0813B418 skipped: response/null-check structure recovered; inline helper variants still differ in mask load order, register selection and boolean materialization. Candidate retained locally.
- 0813B4C8 matched (register-pinned): reused the mode-field offset for the normal derived-stat destination; explicit sum width keeps store before truncation. Reduced hints with depin.py.
- 0813B7C4 matched (register-pinned): collider body flags combine stored bits with mode bits; active scene mode eight forces 0x40. A defined zero level-derived amount reproduces otherwise discarded level arithmetic. Reduced hints to one barrier.
- 0813B950 skipped: short hidden-effect frame advance recovered; fixed-register experiments changed pointer allocation and retained-value shifts without reaching equality.
- 0813BAD8 drafted 63.9%: direction-specific paired frames and offsets plus signed three-over-thirty-two screen projection. Explicit vector setters avoid scalarized aggregate writes. Remaining stack-vector allocation and projection ordering differences.
- 0813C9B4 drafted 60.3%: preserves actor palette indices 5, 6 and 13; brightens source RGB channels, clamps each to 31, and blends with the base palette. Explicit branch-local loops improve spill and load shape; channel registers and frame size remain off.
- 0813CB8C matched (register-pinned): scoped overlay source separately from enchantment source; initialize counter and output-field pointer before palette pointers. A shared inline pulse-weight helper fixes the last five timer-register differences. Existing incoming-mode pin remains necessary.
- 0813CE20 matched (register-pinned), tentative Player_UpdateAlternateVisualPalette: mode-one ramp, mode-two pulse and fallback fade. Expand the overlay loops in their callers rather than through a helper to avoid PRE hoisting output addresses. Reuses the pulse-weight helper from 0813CB8C. Full player_3 unit exact check passes.
- 0813D3B0 matched: ordinary SceneVec locals reproduce packed stack-word writes without explicit reads of uninitialized values; three random draws and seven spawn arguments match directly.
- 0813D834 drafted 95.7%: state-zero queued-effect flush and state-one catalog rotation recovered. Separate no-argument random helper and flag/timer helpers improve the source; entry copy order and one indexed-load lifetime remain different.
- 0813DB60 matched (register-pinned): return the pinned incoming mode directly; an extra result alias loses the register constraint. Only the mode needs a pin.
- 0813DB98 matched (register-pinned): pointer-increment speed setter merges the two decay tails. A memory barrier retains the observed timer reload after animation-flag updates; status-mask helper reproduces mask-before-load ordering. Reduced hints with depin.py; combined player_3 check passes.
- 0813DF5C skipped: full initialization recovered locally, best 69.4%; remaining constant reuse and register order, especially packed tail resets. Vector-copy and scoped-constant variants regressed.
- 0813F8F8 matched (register-pinned): keep the request pointer in r2 and scope the active-mode pointer inside the valid-request branch. Reduced hints with depin.py.
- 0813FE30 drafted 83.1%: copies and relocates the animation header, initializes both channels, parses facing, folds directions and synchronizes flip flags. Delaying the header local until after relocation improves allocation; constructor pointer and constant lifetimes remain off.
- 08140748 drafted 90.7% (from 84.6%): widen the direction temporary and quantize through a small inline helper. Timer-address constant reuse and mask-load order remain different. Explicit timer-offset and constant pins regressed and were discarded.
- 08141A1C matched (register-pinned): signed history view, pointer traversal and separate sentinel lifetimes reproduce the selection loop. Reduced four hints to two with depin.py.

## Batch 1

Ten exact matches add approximately 0.127 percentage points of code coverage (3,064 bytes). Eight use register-pinning concessions (2,688 bytes); two need no hints (376 bytes). Five new or improved guarded drafts earn no exact credit. Three functions were skipped after bounded attempts. Per-function checks use reviewed extents; combined player_3 checks also pass. After rebasing, build/cxbuild printed `build/boktai3.gba: OK`.

The wrap-up marker appeared during integration; no further function attempts were started. Next function in assignment order: 08141BAC, Player_GetSmoothedDirection. The remaining 93 assigned functions were not attempted.

## Session h2 continuation

- 08141BAC drafted 94.4%: signed history smoothing loop recovered in the pending continuation commit. Three register-only instructions differ; no exact credit. Reviewed extent is 108 bytes.

- 08141C40 skipped: existing 99.9% draft differs in one null-branch target. Explicit bypasses, nested guards and empty barriers disturb redundant checks and constant reuse; retained the original draft.
- 081432DC matched (register-pinned): explicit facing local preserves load order and pointer reuse. Holding block follows the cue tail; narrow reset and field-read helpers resolve register allocation. Depin reduces the hints from 26 to 10.
- 08143604 matched without hints: separate parameter setter controls the 366 constant lifetime; depin removes every hint.
- 08143B54 drafted 90.8% (from 89.0%): inline actor-cell calculation preserves the direct field loads and an explicit saved x coordinate. A memory barrier retains the later z reload. Two structural and 39 total instruction differences remain; fixed-register experiments did not help.
- 08143ED0 drafted 99.9% (from 93.9%): safe flag helpers and byte-valued mode/global predicates recover the tail. One null-branch target remains, as in 08141C40. Explicit bypass variants regress.
- 08144AAC matched without hints, tentative Player_UpdateBarrierRecovery: ported the resource-recovery sibling and retained the positive-input early break in phase two to reproduce its branch trampoline.
- 08145254 matched without hints, tentative Player_UpdateInterpolatedMotion: duration and elapsed pointer locals plus a retained duration value reproduce the deltas and divisions. A nested rounding helper returns through a separate local. Depin removes every hint; broad coordinate pins had regressed.
- 081462DC matched without hints, tentative Player_UpdateTimedScreenTransition: six animation/transition phases; declaring the flag pointer before the zero local fixes the last ordering difference.
- 08147FB8 matched (register-pinned): explicit resource load, timer address and final latch addresses repair the tail. Depin reduces hints from 29 to 17.
- 08148318 skipped: existing 87.6% wavering draft has RNG allocation, even-direction scheduling and literal-pool differences. Masked RNG helper and explicit offset/return constraints regressed to 84.7%; original retained.
- 08148B64 skipped: existing 99.4% draft differs only in duration-offset register r0 versus r3. Direct pointer pins alter earlier allocation; readonly offset constraints add a copy. Clobber and memory barriers do not improve it.
- 081491E4 matched (register-pinned), tentative Player_UpdateRearSpiralCue: reused the sprite stepper; a packed position copy reproduces the paired coordinate writes without uninitialized values. Sixteen-frame exit completes the body.
- 0814957C skipped: existing 97.6% draft differs in facing-address scheduling and final state-offset register. An explicit facing pointer fixes the first order but breaks side-store tail merging; inline setter and tail pointer constraints also regress.
- 08149DF0 skipped: existing 99.3% draft has the same two duration-offset register differences as 08148B64. Natural parameter setter, readonly offsets and scoped duration helpers did not improve it.

- 0814A854 matched (register-pinned): an empty loop-counter barrier retains the observed ascending three-entry callback loop; depin removes the register binding.
- 0814A9D4 matched (register-pinned), tentative Player_UpdateSelectedSpinAttack: adapted the exact 0823F928 sibling. Added the intermediate charge stage, standard-player callbacks, selected projectile and attack-facing APIs, and the timer-zero spin cue. Reviewed all nine phases and the packed particle/collider vectors; no new instruction hints were required beyond the sibling's pending-value binding.
- 0814B078 drafted 80.2%: nearest visible list target and distance-derived motion duration recovered. Typed player accesses and the exact projection helper improve the shape; a source-pointer barrier prevents inappropriate coordinate reuse. Remaining duplicated-flag and projection-temporary scheduling differs. Natural-only and broad high-register variants regress.
- 0814C248 drafted 77.8%: related fixed-range target scan, with signed squared-distance comparison and no duration calculation. Ported the retained target/projection layout; register and frame scheduling remain different.

## Batch 2

Nine exact matches add approximately 0.241 percentage points of code coverage (5,812 bytes). Five use register-pinning concessions (3,476 bytes); four need no hints (2,336 bytes). Four new or improved guarded drafts earn no exact credit. Five other functions retain their earlier drafts after bounded attempts. The initial 08141BAC continuation draft was validated and merged separately before this batch. Next assigned function: Player_ComputeActionScale.

Batch 2 validation: rebase changed only the generated documentation index conflict, resolved by regeneration with both sides present. The combined build printed `build/boktai3.gba: OK`.


## Batch 3

Tags: #player #matching #charge #terrain

- 0814D104 matched without hints: separate null-aware action/state flag helpers preserve the redundant checks. A signed flag-return helper fixes the sign branch; explicit halfword and global-bonus readers preserve field lifetimes. Depin removes all sixteen original hints.
- 0814D264 drafted 76.3%: full attack-collider motion draft covers animation-specific radial paths, coordinate transforms, contact modes and attack properties. Typed motion context, unsigned sentinel and branch-local angle barriers reduce structural differences from 185 to 34. Broad table/coordinate pins and declaration swaps regress.
- 0814D9C4 drafted 77.2%: full weapon-cue/combo draft has no structural differences and 82 register differences. Raw player accesses, phase-offset locals, explicit sentinel/reset helpers and a record pointer binding recover the shape. A bounded permuter run produces an invalid best candidate with an unreachable alias assignment; rejected. A repaired, initialized alias alternative reaches 81.4% with three structural differences and remains local for a later attempt.
- 0814DF0C matched (register-pinned): an empty r3 clobber prevents deriving the final field offset from an earlier constant. Hint flips identify reload behavior; no compiler decision flip improves the source directly. Depin retains one hint.
- 0814E3D8 matched (register-pinned): full selected charge handler. A memory barrier after the second tier's sound call prevents CSE from merging distinct phase tails. Parameter setter, explicit cost and particle-mask lifetimes finish allocation; depin retains six hints.
- 0814E990 matched (register-pinned), tentative Player_UpdateAttachedChargeTiers: adapt the selected-charge sibling to task-reported tiers, attached-position updates and release spawning. An entry zero local reused for the minus-one task sentinel resolves the final three instructions. Depin retains five hints.
- 0814EEE0 matched (register-pinned), tentative Player_UpdateSustainedSpinAttack: full energy-consuming spin collider and animation handler. Explicit amount branches, cost locals and facing temporaries improve 87.6% to exact; a remaining-energy local resolves the last register pair. Depin retains four hints.
- 0814F328 matched (register-pinned): attached attack charge uses the verified entry reset, parameter setter and particle-mask layout. Null-safe bonus helper with pointer-before-mask initialization removes materialized booleans; scoped input offset/pointer bindings finish the reload sequence. Depin retains eight hints.
- 08156DAC drafted 89.5%: full terrain-clearance probe, signed radial projection, cell override lookup and slope-height adjustment. Only two structural differences remain, with heading and shared facing-store allocation also off. Explicit branches, common-tail restructuring, clobbers and heading bindings regress. Hint reports that disabling CSE jump following would match; preserve the default compiler and revisit the source CFG later.
- 08158C14 drafted 65.3%: decoded the reviewed raw tail and recovered all link-state arrays/counters and both clearing loops. Signed integer-address loop reproduces the descending pair clear. Explicit global pointer/register experiments reach closer instruction shapes but trigger an unsafe hard-register lifetime transformation or compiler spill failure; rejected. Retain the natural, full draft.
- 08159164 matched (register-pinned), tentative Player_AddExperience: opposing bonus flags double-adjust or halve the reward, then clamp accumulated experience. A separate predicate returning true for a null player reproduces the initial direct bypass while preserving later safe flag checks. Local sum avoids an unwanted alias reload. Depin retains only its mask binding.
- 0815977C matched without hints: return the masked flag word directly and initialize the mask after the field pointer. This removes boolean materialization and recovers the original load order, improving 74.7% to exact.
- 0815A048 matched (register-pinned), tentative Player_TryStartStun: forced variant clears motion state before starting or refreshing stun. Separate byte/halfword zero values and explicit control-bit lifetimes reproduce the reset sequence; a guard barrier preserves the signed branch. Depin retains three hints.
- 08237ED8 matched without hints, tentative PlayerOrbitEffect_UpdateMovingParticle: moving collider stops on terrain then runs variant-specific impact frames and flip flags. Sprite view, signed variant and pointer-before-load initialization reproduce every instruction.

Ten exact matches add approximately 0.263 percentage points of code coverage (6,336 bytes). Seven use register-pinning concessions (5,540 bytes); three need no hints (796 bytes). Four new guarded drafts earn no exact credit. The next assigned function is 08238110. Earlier reviewed-only skips still require a second pass after the remaining list.

Tooling: the private instrumented compiler is available for hint flips. Cause searches require a causebook corpus that is not present; guarded drafts may need an unguarded local scratch copy for these diagnostics. Source semantics remain the acceptance requirement alongside byte equality.

Batch 3 validation: the rebase conflict was limited to docs/INDEX.md and resolved by regeneration. After rebasing, build/cxbuild printed `build/boktai3.gba: OK`.


## Batch 4

Tags: #player #matching #terrain #camera

- 08238110 matched (register-pinned), tentative PlayerOrbitEffect_InitMovingParticle: heading-based velocity and impact variant setup. Scoped velocity pointers and zero locals recover the stores; a pointer increment with a binding reproduces the otherwise folded halfword store. Depin retains one hint.
- 08238238 matched without hints, tentative Player_UpdateActionFacingSide: identical body to the verified facing-fold helper at 0813ADD0.
- 08238580 matched (register-pinned), tentative Player_ApplyLinkedContactResponse: adapt the standard contact-response sibling without the position-reset gate. Separate impact-direction call paths using a barrier before the shared store, and explicit deferred-response pointer/value lifetimes, reproduce the tail. Depin retains one hint.
- 082386DC drafted 84.0%: full brief sprite animation updater. Comparing the stored byte after increment improves the original full-temporary draft. Explicit timer/pointer constraints, truncated and full-width temporaries, barriers and hint flips do not close the gap; reject hard-register variants that disturb pointer lifetimes. Five structural and eight total instruction differences remain.
- 0823B308 matched without hints, tentative Player_SpawnActionSpark: packed bitfield vectors reproduce the partial stack-word stores; modes three and four use the stored interaction position, other modes raise the normal position.
- 0823B47C skipped at the existing 99.4% draft: forcing the comparison copy before the facing store fixes that pair but changes else-branch allocation and prevents the shared flip store. Tested narrow/wide copies, explicit destination pointer, readonly constraints, comparison-only alias, clobber and retained-value bindings; all regress. Original retained for the final close-draft pass.
- 0823B5D8 matched without hints, tentative Player_InitFacingDriftOffset: reuse the heading-drift layout and signed scale helper; fixed radius 800 produces the observed shift/add multiplication directly.
- 0823B7A4 matched without hints, tentative Player_SmoothCameraAnchor: explicit per-axis input loads before destination pointer formation remove the extra persistent address. An inline controller-ID reader preserves its null arm and literal-pool split. Depin removes the final pointer barrier; improves 52.3% to exact.
- 0823BB48 matched without hints, tentative Player_IsTerrainStepBlocked: height helper and repeated cell lookup preserve the terrain override and slope logic. Separate attribute reader and masked local fix the final load/mask ordering; depin removes every hint.
- 0823C280 matched (register-pinned), tentative Player_RestoreLinkedActionState: full linked action reset restores resources, position, cell object and facing. Explicit first-copy offset, destination and distinct zero lifetimes reproduce the reset sequence. Retain natural root allocation: a forced r7 root produces unsafe stack-relative accesses in this compiler and was rejected. Depin retains three narrow hints.
- 0823C368 matched (register-pinned): identical to the verified history selector at 08141A1C. Pointer traversal and separate sentinel lifetimes preserve the early returns; both inherited hints remain after depin.
- 0823C424 drafted 93.0% (from 83.3%): separate safe state and flag predicates restore redundant null checks and the full-sized body. Remaining boolean materialization occurs around the state predicate instead of the flag predicate. Raw field-return, narrow predicate and explicit outer-guard alternatives regress.
- 0823C4B4 drafted 94.4% (from 83.3%): adapt the signed history smoothing sibling. Only the final pointer/zero load registers are swapped. Explicit reload-pointer pin, index-zero pin and clobber alternatives disturb earlier reloads or retain worse control flow; best source retained.
- 0823C8FC matched without hints, tentative Player_IsWithinSidestepCellMargin: decoded the reviewed raw tail; choose the perpendicular coordinate, mask to a byte and reject values outside the cell margin. An inverse early-return form fixes branch layout. Depin removes both temporary hints.

Ten exact matches add approximately 0.073 percentage points of code coverage (1,768 bytes). Four use register-pinning concessions (808 bytes); six need no hints (960 bytes). One new and two improved guarded drafts earn no exact credit. One existing close draft is retained unchanged. Next assigned function: Player_UpdateTerrainSidestep at 0823C930.
