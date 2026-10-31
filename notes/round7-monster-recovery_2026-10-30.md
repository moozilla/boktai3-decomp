# Round 7: recovered work and continued monster matching

Tags: #matching #monsters #compiler #structs #tooling

Fifteen functions match in ordinary C, covering 33,332 bytes of the supplied
Round 7 queue. Five were recovered from the interrupted run; ten were completed
after recovery. Every accepted function has its own commit. PROGRESS files are
unchanged.

| Address | Bytes | Result and deciding source property |
|---|---:|---|
| 080DE6E0 | 588 | Recovered Dog animation-particle callback; typed motion and flag operations. |
| 080C5930 | 1,584 | Recovered Golem route-facing turn; animation and duration operations now shared with Skeleton. |
| 080C6A68 | 1,264 | Recovered Golem charge recovery with typed movement state. |
| 080B7C10 | 3,656 | Recovered Skeleton distance selection using canonical actor/context ownership. |
| 080B9158 | 3,768 | Recovered the worker's unmerged Skeleton hop/arrival match. |
| 080BA268 | 784 | Skeleton bone emission and attack restoration; the emission call owns its constant arguments. |
| 080C7E18 | 2,396 | Golem delayed impact recovery; typed delay, snap and turn operations preserve PRE ordering. |
| 080BAE04 | 1,752 | Skeleton route-facing turn; a signed-halfword octant input reproduces the pointer allocation. |
| 080BBDD0 | 4,116 | Skeleton displacement convergence; axis-field ownership and direct group access fix PRE address order. |
| 080BFCC4 | 2,452 | Skeleton delayed recovery; taking the entry flag through its component fixes address discovery order. |
| 080E3874 | 2,360 | Dog delayed route/waiting recovery; the same entry operation and a typed route-flag query preserve PRE copies and mask order. |
| 080CBF24 | 2,300 | Golem delayed route recovery; component entry handling and existing value-domain interfaces recover PRE ordering. |
| 080CE188 | 2,364 | Golem route restoration after control; a word-flag field owner preserves its load/store lifetime. |
| 080E4B34 | 1,976 | Dog delayed pursuit recovery; component mask order and measured cell/saved-control interfaces reproduce the PRE slot rotation. |
| 080CF3E4 | 1,972 | Golem arrival/restoration; the shared terrain-center operation changes group/coordinate lifetimes, then the existing scalar-cell interface fixes PRE order. |

The four original integrated commits retain their object IDs (`40e221b`,
`ac405f8`, `6211771`, `2423916`). The additional Skeleton result came from the
original worker commit `52b90ab`. The recovery export also preserves the original
worker branches, source experiments and source snapshots from the saved search
databases. These solved matches were rechecked, not reconstructed again.

## Shared operations and initializer ownership

`scene_actor_turn.h` now owns seven identical animation/turn operations used by
080C5930 and 080BAE04. Both complete extents remain exact. Seven functions share
the delayed-response record and preparation operation; six share the state-entry
component operation. All seven complete extents remain exact after extraction.
The delayed-route functions reuse the chase leaf operations; some local arrival compositions still
differ in their field-owner boundaries and remain candidates for later sharing.

Golem's three-function tail moved to `src/golem_3.c` before extracting 080CBF24.
The remaining 9,952-byte unit and the 240-byte tail retain their existing matches.

Tentacle_Init (080ABDCC), Mummy_Init (080DCC98) and Axe_Init (080FE68C) now embed
the canonical actor prefix, preserving 12,032 existing exact bytes. The canonical
view occupies 0x6EC bytes. Other species have private data beginning at 0x3D4,
0x558 or 0x624, so embedding the entire view would overlap those fields. Attempts
to split those records changed register allocation; the original matches remain.
The Bat-specific initializer helpers were already file-local in the supplied base.

The [command-table note](round7-actor-command-tables_2026-10-30.md) records the
eleven editable tables, disjoint Studio atlas ranges and the macOS formatter-test
repair. Scope's frame report now uses physical instruction offsets when the
complete listings differ only by a bijective permutation of stack displacements.
This avoids misleading alignment votes for repeated loads and stores; other
instruction differences retain the cautious aligned fallback. The targeted
tool suite passes 41 tests, including eight frame-correspondence regressions.

## Verification and continuation

Each checkpoint contains a cumulative Git bundle, ordered patches, stock-compiler
checks with complete extents and hashes, and the combined ROM/RAM-literal build
log. Acceptance requires zero differing bytes and a byte-identical 16 MiB ROM.
No compiler overrides, new selectors, register pins or assembly barriers are used
by the new matches.

Preserved unresolved work includes Golem 080C8774 (PRE ordering and one reload
decision) and the Ghoul 08085DD0 scan-loop ownership problem. Golem 080C725C
and Dog 080DF908 retain an unused RNG-table literal in the ROM; existing relatives
use concessions to retain it, so those spellings were not adopted. Resume from
the exported candidates and causal reports rather than their old rough drafts.
Skeleton 080BF0E4 retains an animation selection/restart-pointer allocation and
tail-sharing gap. Golem 080CCD7C retains context-address PRE and charge-tail
sharing differences. Their ordinary sources and diagnostic traces are preserved.

## Dog displacement recovery — 080E0750

All 4,232 bytes match in ordinary C. Canonical actor fields and the shared chase
operations replace the duplicated raw-offset draft. Signed axis getters, a
reference to the current displacement value, magnitude scaling, and an
early-return clamp preserve the recovery arithmetic and the native expansion
identities needed for PRE stack ordering. The final source uses the existing
chase selectors only; the stock check reports zero differing bytes.

## Dog retreat scan — 080DEF78

All 800 bytes match. Distinct symbolic and fixed-address map receivers preserve
the completed and interrupted vertical scans. A retained route-position output
separates the later route query from the slope-position owner, reproducing the
three spill slots and their initialization order. Shared height and cell-center
operations remain in use; the stock check reports zero differing bytes.

## Dog item contact — 080DF6EC

All 540 bytes match. Typed item-context, collider and player-position fields
replace raw accesses. The contact-cost value is reused for the subsequent zeroed
response words; their signed-offset field operation preserves the two response
views. This reproduces the health clamp and later collider stores without
register hints or compiler overrides. The stock check covers the complete extent.

## Poison Hand damage and defeat particles — 0820E2A4

All 1,140 bytes match. The particle loop retains the random bank while its first
three samples use the shared sample-capture operation. Reusing the data owner
for the final actor age update preserves the bank's stack lifetime. Animation
playback uses the canonical display-sequence record without volatile reads.
Private actor fields begin at 0x108, preventing a full canonical actor prefix.

## Screamer damage particles — 0820C9AC

All 840 bytes match. Separate direct and captured sample reads preserve the
particle direction and speed operands. The shared data owner holds the random
bank through the loop and the actor for the final age increment. The native
particle count and copied position reproduce the full stack layout without
register pins, assembly barriers or compiler overrides.
