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
