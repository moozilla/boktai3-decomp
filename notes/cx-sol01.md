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
