# Round 7: recovered work and continued monster matching

Tags: #matching #monsters #compiler #structs #tooling

Eleven functions match in ordinary C, covering 24,720 bytes of the supplied
Round 7 queue. Five were recovered from the interrupted run; six were completed
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

The four original integrated commits retain their object IDs (`40e221b`,
`ac405f8`, `6211771`, `2423916`). The additional Skeleton result came from the
original worker commit `52b90ab`. The recovery export also preserves the original
worker branches, source experiments and source snapshots from the saved search
databases. These solved matches were rechecked, not reconstructed again.

## Shared operations and initializer ownership

`scene_actor_turn.h` now owns seven identical animation/turn operations used by
080C5930 and 080BAE04. Both complete extents remain exact. The delayed-route
functions reuse the chase leaf operations; some local arrival compositions still
differ in their field-owner boundaries and remain candidates for later sharing.

Tentacle_Init (080ABDCC), Mummy_Init (080DCC98) and Axe_Init (080FE68C) now embed
the canonical actor prefix, preserving 12,032 existing exact bytes. The canonical
view occupies 0x6EC bytes. Other species have private data beginning at 0x3D4,
0x558 or 0x624, so embedding the entire view would overlap those fields. Attempts
to split those records changed register allocation; the original matches remain.
The Bat-specific initializer helpers were already file-local in the supplied base.

The [command-table note](round7-actor-command-tables_2026-10-30.md) records the
eleven editable tables, disjoint Studio atlas ranges and the macOS formatter-test
repair. The targeted tool suite passes 23 tests.

## Verification and continuation

Each checkpoint contains a cumulative Git bundle, ordered patches, stock-compiler
checks with complete extents and hashes, and the combined ROM/RAM-literal build
log. Acceptance requires zero differing bytes and a byte-identical 16 MiB ROM.
No compiler overrides, new selectors, register pins or assembly barriers are used
by the new matches.

Preserved unresolved work includes Dog 080E0750 (4,232-byte instruction stream
with two PRE slot swaps), Golem 080C8774 (PRE ordering and one reload decision),
and the Ghoul 08085DD0/Dog 080DEF78 scan-loop ownership problems. Golem 080C725C
and Dog 080DF908 retain an unused RNG-table literal in the ROM; existing relatives
use concessions to retain it, so those spellings were not adopted. Resume from
the exported candidates and causal reports rather than their old rough drafts.
