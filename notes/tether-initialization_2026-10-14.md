# Tether initialization, charge and flagged terrain

Eight new exact functions add **0.0818 percentage points**, reaching **24.059%**
(6,498 / 11,471 functions; 581,122 / 2,415,354 bytes).
Without register pinning: **19.546%**; Register-pinned: **4.513%**.
New exact bytes: 1,976, including 320 without hints. Actor identity remains tentative.

The contiguous **0804A430–0804AF80** subsection is fully exact: 2,896 bytes,
comprising eight new matches and seven existing functions with descriptive names.
The following world-position update at 4AF80 has a typed 80.0% scratch candidate.

## Initialization and shared layout

- 4A430 (152): stores entry 18, reads `k` or the external status at
  GameState+5AA, reads `c`, selects the kind table at 086059F0, applies `a`
  as state (default 0), and reads `A` as two halfwords (defaults 50/50).
- 4A4C8 (312): reads `r` as XYZ target and `O` as a word, with zero defaults.
  Kind table 08605A04 scales the primary pair 40C/40E by 120 and secondary
  pair 3F8/3FA by 60. Kinds 2/9 disable interaction and directly reset states
  3/4 to 0; this direct store preserves the frame counter. Other kinds enable
  interaction and reselect states 3/4. Recovery duration402 is300; recovery countdown400 is0 (names refined by4C304).
- 4A600 (256): default palette index 10. Kind 9 initializes keyframes from
  resource 35E3; others initialize sequence resource 2499 / sequence 3D95,
  set entry 0, select the palette, copy 16 RGB555 colors, and point the display
  at the actor buffer. Priority is 2; `d` sets heading or defaults to 0.
- 4A700 (464): reads XYZ but replaces Y with map slope height, copies display,
  origin and target positions, initializes the body at 44 with category 6
  and owner actor, attaches sequence/keyframe draw pointers, sets body angle
  from heading, and registers the 244 node except for kind 3. Kind 9 attaches
  its movement cache. The body ID is copied to **1FC**, not interaction lock 3FC.

The movement cache at **120 is 16 bytes**, confirmed by 0821BC68's writes
through +E and the first collider beginning at 130. It contains paired
cell/height/slope/attribute fields. The two **0x54-byte colliders** are at
130 and 184. They are separate objects, not part of the movement cache.
The path-probe storage begins at 1D8; its body-ID halfword is at +24 (1FC).

The body prefix at 44 holds ID, flags, category and angle. Helpers also operate
on subsequent actor fields. **58 is a per-frame movement delta**, rather than
another position: 08225B74 adds XYZ to position then clears the delta. Its fourth
halfword is initialized to 16, with purpose still unknown. Existing palette
and body consumers were adapted to these views without extra exact credit.

The already exact charge-collider initializer now uses the shared actor view
and the callback's actual **three-argument ABI**. Several other initializers
retain byte/packed-word views. An attempted secondary-collider padding cleanup
changed its store width and lost the match; the existing body is retained,
and the failed attempt is in ignored scratch work.

## Charge, cooldown and contact

384/386 are now named **chargeReady / charge**, replacing the earlier pending
interpretation. Existing 4AAB4 accumulates a truncated halfword, clamps it to
1,200, and marks ready only when an addition **exceeds** 1,200 and the lock/state
conditions allow it. Exactly 1,200 does not newly mark ready. Existing 4AB40
reduces charge and resets readiness/effect when the decremented value reaches 1.

- New 4AB94 (56) decrements interaction cooldown by two in state 1, saturating
  at zero, and by one otherwise.
- New 4ABCC (140, no hints) skips those ticks when global flag 030053F4 bit
  0x200 is set and resets the attached effect instead. Otherwise contact
  cooldown 392 sets/clears collider flag 4 and ticks down.
- New 4ACA8 (180, no hints after minimization) receives contact data, an unused
  second argument and the actor owner. It skips contact flag 0x800, actor byte
  1B equal to 1, cooldown, and zero power at contact+3C. The private contact view
  has response flags at **34**, distinct from the shared collider's other word
  at 38. Kind-value zero requires type flag 1 or response flag 0x200.

Response bits 1/2/4/16 select charge additions 200/300/500/500, otherwise 400.
Bit 2 selects cooldown 12; other cases use 8. Every accepted contact sets palette
flash timer 390 to 4. No source pointer is consumed after its lifetime, and no
caller-clobbered register binding is relied on across a call.

## Flagged terrain handler

New **4ADE0 (416)** checks terrain attribute 0x80 and GameState flags868 bit
0x2000, then clamps X/Z to the previous cell's interior `[base+1, base+255]`.
It calculates a cardinal direction from displacement toward the cell center
and calls the kind-9 action API with **both** arguments `(100, direction)`.
The resulting direction can be 256 before the action API truncates its byte.
It optionally plays 3F4/4ED and decrements byte 3E9, sets 3E8, then calls the
script resource ID at3EC if present (confirmed by the0821ACDC table lookup).

A bounded permuter run improved allocation after two pointer aliases were
introduced. One early run stopped on an implicit `Mod` declaration; explicit
Mod/Div prototypes fixed that tooling failure. The final match came from
correct field views, delaying the Z pointer and row calculation, and preserving
two scalar lifetimes. The final C was reviewed and checked exactly. Six new
matches retain minimized concessions (2/1/4/1/2/3 hints); two need none.
No explicit r7 binding or handwritten instruction body.

## Validation and continuation

The combined build printed **`build/boktai3.gba: OK`** in
`build/tether-initialization-combined.log`. **67 complete post-promotion extent checks passed**, covering every exact actor-header user and the retained named initializers.
Format, inventory, RAM literals, registry, ten archive and text-bank checks pass.
No new emulator claims.

Claude's PRs 127–129 are incorporated: scene listing, real palettes for 165
stages, experimental warps and the RunFrame-hook `callproc` helper. Their warp
still fails to create the player object; this limitation is documented in
CENTAUR.md. These changes do not alter the default C/ROM build pipeline.

Continue at **0804AF80**, world position/display/map-cache update, then **4B1CC**
and the state routines. AF80 confirms that 380 is a vertical display offset;
its older `auxiliaryTimer` field name should be corrected with that consumer.
Earlier guarded terrain/interpolation drafts remain non-exact. Continue until
25% exact or 60% account usage remaining; latest check is 74% remaining.
