# Five-entry projected display, 08045A20–08045D18

Five new exact functions add **0.0245 percentage points** (592 bytes). The
existing entry removal, manager update/destructor, factory and height accessors
gain shared types/names with no additional credit. Gameplay identity remains
tentative; the observed object is a 0x238-byte task in list 10 with five 0x60-byte
animation entries and a copied/relocated 32-byte resource header.

| Function | Address | Exact bytes | New credit |
|---|---|---:|---|
| PositionEntry | 08045A20 | 188 | Yes |
| UpdateEntries | 08045ADC | 44 | Yes, no hints |
| InitializeEntries | 08045B08 | 108 | Yes, no hints |
| RemoveEntries | 08045B74 | 28 | Existing |
| UpdateScreenPosition | 08045B90 | 144 | Yes, no hints |
| Update | 08045C20 | 24 | Existing |
| Destroy | 08045C38 | 12 | Existing |
| Initialize | 08045C44 | 108 | Yes, no hints |
| Create | 08045CB0 | 68 | Existing |
| GetHeight / SetHeight | 08045CF4 / 08045D04 | 16 / 20 | Existing |

## Coordinates and entry roles

Object +18 is a signed height offset, +20 the source/world position, +28 the
height-adjusted position, +30 its projected screen position, +38 the resource
header and +58 the five entries. Each entry's position is +20. Packed positions
are four halfwords and copied as two words. Other entry fields stay unknown.

UpdateScreenPosition copies source to adjusted position and adds height to its
Y halfword. For adjusted X/Y/Z, with each term truncated toward zero:

* screen X = 3(X-Z)/32 - camera[0] + 120;
* screen Y = 3(X+Z)/32 - 3Y/32 - camera[1] + 80;
* screen Z = 3(X+Z)/32 + 3Y/32 - camera[2].

The camera halfwords are read from 03005418. Entry 1 projects the original
unraised position with the same formula; all other entries copy the adjusted
screen vector. Each then adds its signed depth bias from table 0860594C. A
separate base/ground component is supported by this behavior, but an exact
visual/game identity is not established.

PositionEntry preserves its incoming object in r0 through the entire original
body and returns through r1. The recovered C models this observed return as the
input display pointer. Earlier void/pointer-return attempts let the compiler use
r0 as scratch or restore it later; a temporary r0 hint preserves the actual ABI
and greatly simplifies the remaining allocation. Hint minimization left seven
hints and no r7 binding. UpdateScreenPosition's temporary hints were all removed.

## Resource and lifecycle

Initialize clears height, copies input position and computes screen coordinates,
then looks up extension/name hashes CB05/7516. Missing resource returns -1. On
success it copies eight words and relocates the resource header, copies position
again into world/adjusted storage, and initializes entries. It **ignores** the
entry initializer's failure result and returns zero when the resource exists.
This behavior is retained, not corrected.

InitializeEntries initializes each animation with shared resource/screen storage,
then selects its halfword frame from the five word-stride rows at 08605960 using
mode 1/flags 4 and positions it. An animation initialization failure returns -1
immediately. UpdateEntries runs Anim_Step and PositionEntry for each index 0–4.
RemoveEntries visits all five through Anim_Remove. Create allocates 0x238 bytes,
sets update/destroy callbacks, and kills the task only when Initialize returns a
negative value.

SetHeight's non-null fallthrough retains r0. The observed owner at 0802A188 uses
only its side effect on the object pointer stored at owner+900: it raises height
by counter*4 and later lowers it by (32-counter)*4. GetHeight returns zero for
null, otherwise the word at +18. No new emulator coverage is claimed.

## Matching checkpoints

Every new body and all six typed existing units passed reviewed full-extent
checks before name promotion. The initializer/entry-setup matches came from
preserving original failure/success block order and a common return. No hints
were required. PositionEntry reduced 27 hints to seven. A rejected attempt bound
the world pointer and scale result both to r1; its local checkpoint is marked
known-problem and was never accepted. The final world pointer survives in r8.
All failed alternatives stay in ignored build/projected-* and build/candidates.

Next contiguous block starts at 08045D18: already-exact flag/state helpers and
contact callbacks/state actors with health, cooldown and flashing fields from
08045D54 onward. Their gameplay identity is still to be recovered.
