# Rotating contact actor and floating numbers, October 14

Continue the contiguous sequence/contact actor work with **08046880–08046B28**
and **08046B34–0804732C**. The intervening 12-byte reset helper and the helper at
0804732C are unrelated, already exact old_agbcc functions; they are not newly
credited or renamed.

Exact coverage rises **23.595% → 23.671%**, **+0.0754 percentage points**
(+1,820 bytes, thirteen new functions). There are **6,440 / 11,470 functions**
and **571,730 / 2,415,354 exact bytes**. Code without register pinning is
**19.380%**; Register-pinned code is **4.291%**, included in the total.
Seventy-two guarded drafts remain, including the decimal renderer below.

| Subsection | New exact functions | New bytes | Existing functions typed |
| --- | ---: | ---: | ---: |
| Rotating resource-contact actor | 4 | 488 | 4 |
| Floating number display | 9 | 1,332 | 6 |

Nine new functions need no hints. Four retain minimized concessions:
orbit update (nine hints), number initialization (four), bounce update (one),
and leaving update (nine). Every hinted exact candidate was minimized with
`tools/depin.py --write`; no handwritten instruction bodies were introduced.

## Rotating contact actor

`include/orbit_contact_actor.h` recovers a **0xC8 list-8 task**, with signed
radius at 0x18, angle/state/state flag at 0x1A/0x1B/0x1C, frame counter at 0x20,
center at 0x24, collider at 0x2C, graphics at 0x80 and sprite at 0x9C. The state
table gUnk_086059B4 selects inactive, expanding and contracting handlers.
The gameplay identity is tentative.

Inactive hides the sprite, resets radius to zero and sets angle 192 whenever
the state flag is nonzero. The flag is not consumed by these handlers.
Expansion unhides, advances angle by six and radius by seven per update.
After frame 59 it switches to contraction and resets the frame counter.
Contraction subtracts seven from signed radius, clamps below zero, and returns
to inactive after frame 59. Both moving states play song 0x3BC when frame modulo
20 equals eight. Resetting a counter and then incrementing it leaves it at one.

The update uses sine samples, signed division by 4096 truncating toward zero,
and the center vector to place the sprite. It positions and refreshes the
resource collider while state is nonzero. Graphics resource 4148 selects tile
15F. The collider uses flags 2001, 128/128/128 bounds and 0/128/0 offset; its
resource parameter is forwarded with 50, zero, 60000 and 30. The constructor
returns null on allocation or initialization failure. The destructor removes
the sprite only; no collider removal call was invented.

The actor is called from the scene owner at 0802A188. All eight functions in
this actor span are now exact; the four newly recovered bodies are 08046880,
08046900, 0804694C and 08046A10.

## Floating number display

`include/damage_number_display.h` recovers a **0x2F0 list-9 singleton** at
RAM gUnk_02000074. The active mask is at 0x18, glyph resource at 0x1C, and four
groups start at 0x20 with **0xB4 stride**. Each group has a source key, priority,
phase/frame, digit count, anchor fields, world vector and four **0x28 glyphs**.
Glyph x/y are at 0x18/0x1A and its rendering priority byte is at 0x0F.

Create reuses the existing singleton. Initialization loads resource 1C1C,
initializes all sixteen glyphs with 10010, -4/-4 offsets and glyph 20, gives them
rendering priority one, and starts groups at priority three with no digits.
Destroy removes all four groups and clears the singleton.

Show forms a key from the source pointer's low 16 bits and the source halfword
at +4. It reuses a matching active group unless the incoming priority is lower.
Otherwise it takes the first inactive group, or the first group with priority
at most the incoming value; it fails with -1 if none qualifies. The source
halfword is provisionally named id. Its callers include actor hit callbacks
080427FC, 0804293C, 08045D54, scene-owner damage callbacks, and 080638F8/08063F84.

Setup resets phase/frame, adds the offset vector, projects world position with
signed 3/32 scaling and camera offsets, and chooses glyph base 32/42 for
priority zero or two, otherwise 52/62. A nonzero kind picks the higher base.
The active-bit leaf preserves and returns its input display pointer; setup
forwards that pointer, and Show returns its integer representation on success.
The API names and interpretations are provisional; exact code preserves the
observed return behavior rather than adding a new status instruction.

The state table gUnk_086059D4 contains bounce and leaving handlers. Bounce uses
per-digit phase offsets and a sine displacement, clamps phase to 96, and
advances after twenty frames. Leaving uses the fixed sine-table sample at
08614A84 and removes the group after ten frames. Both phases reproject the
world vector and space digits by five pixels. This is static evidence, not a
new emulator coverage claim.

## Retained decimal renderer

**08046BC8**, reviewed extent **548 bytes**, is guarded by NONMATCHING with its
original INCLUDE_ASM fallback and recorded **63.1% fuzzy** score. It is excluded
from exact coverage. The readable C removes previous digits, reduces values
modulo 10000 by subtraction, selects thousands/hundreds/tens/ones glyphs,
suppresses leading zeroes while retaining a zero glyph, stores projected anchor
fields and registers the visible digits. Initial phase clamps at 128.

The original leaves r0 unspecified; both static calls discard its result.
The draft uses a non-void function with an unobserved result to preserve that
calling convention. The exact setup caller has a matching return of its own.

More heavily hinted local alternatives remain in ignored candidate checkpoints.
They confirm much of the decimal prefix but are not accepted. The best later
attempts still differ in camera aliases, signed-load scratch registers and
loop lifetime/branch layout. Explicit r7 pointer bindings cause extra halfword
address copies in agbcc; keep memory access bases naturally allocated when
resuming these attempts. The checked-in draft favors understandable C over
retaining a large set of ineffective hints.

## Validation

The combined build printed **`build/boktai3.gba: OK`** in
`build/orbit-damage-combined.log`. Thirty-three post-promotion checks passed
at reviewed complete extents: thirteen new functions, ten typed existing
functions and ten renamed callers. The renderer was separately checked at its
548-byte original extent with `--nonmatching --record`. Formatting, code
inventory, RAM literals and decoded actor-registry checks passed.

Continue contiguously at **08047338**, the sixteen-slot transient sprite-effect
pool called by the preceding actor's death effects.
