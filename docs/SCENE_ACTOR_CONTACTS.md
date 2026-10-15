# Scene actor contacts and profiles (tentative)

The reviewed adjoining span begins at `08073A3C`; the contact-candidate manager
continues through `08073C2C`, where the registry's profile-record helpers begin.
The candidate-list singleton is `020001A0`, allocated with size `0x2C` on task
list 8. Its narrow manager view has fields at `0x18/0x1C`, frame at `0x20`, a
halfword resource ID at `0x24`, and node-list head at `0x28`. Candidate nodes
begin with an object pointer, flags/kind bytes, halfword cooldown, actor owner
pointer and next pointer at `0xC`. The preceding cohort establishes any tail.

`73A3C` skips selection while game-state flags contain `0x40`. It scans enabled
object candidates whose kind intersects its mask, applies availability and
distance predicates, then links kind 1 to its actor owner or dispatches kinds
2/4 through `72B50`. It clears actor byte `0xEB` bit 1 and returns the selected
node. The signed pointer-valued selection sentinel is preserved as observed.
The 252-byte extent is exact; `depin.py --write` removed 9 of 11 hints.
This contributes about 0.0104 percentage points, in the register-pinned category.

A pre-existing ABI draft needs care: `72BA8` clears manager fields and exits
with its link address in r0 (`pop {r0}; bx r0`). Initializer `73B84` does not
produce a defined C status, although creator `73BA0` compares that incidental
r0 value with zero. The valid void initializer draft also differs in its epilogue, so both
initializer and creator retain assembly until a defined representation is
established. The parent will remove the legacy initializer fall-through C
from exact credit. No output-only empty operand,
fall-through return or fabricated status is used to claim a match.

Profile records occupy `0x80` bytes. The registry's two-word subsystem at
`0x280` holds the head and count; at most twelve records are allocated. A record
has next/id/mode/category/entry-count fields at `0/4/5/6/7`, followed by thirty
four-byte entries. Each entry's first three bytes are initialized as value,
flags and state; the fourth is unknown. These names remain provisional.
`73CC4` uses 26 category cases and 228 entry assignments; the semantic data was
recovered from the full switch and retained privately for review. Its complete 2,364-byte extent matches as ordinary C, contributing about
0.0979 percentage points. Allocation/linking occurs
before category validation, so category 0 or an out-of-range category can leave
a linked record while returning -1. This original quirk must remain.

The large profile constructor needs no hints: keeping a typed `{head, count}`
list view at registry offset `0x280` naturally produces the shared pointer
lifetime, and retaining category 0 explicitly gives the original switch table
bounds. The inline entry setter writes three bytes within each four-byte entry;
all indexes stay within the allocated 128-byte record.

The update callback explicitly returns its new frame count, matching retained
r0. The destruction leaf clears the singleton and returns its original manager
argument unchanged, also matching its actual r0 value. Callback consumers
discard these outputs. The manager count is signed; the preceding registration
helper compares it with five using a signed branch (six candidate capacity).

Final reviewed span: `08073A3C` through `0807462C` exclusive (3,056 bytes).
Eleven worker functions are exact: four new functions total 2,728 bytes, about
0.1130 percentage points (2,432 ordinary C and 296 register-pinned), and seven
existing functions total 212 typed bytes. The independently corrected
`Memory_ClearWordPair` at `73BFC` contributes its existing eight bytes. The
remaining 108 bytes are initializer `73B84` (28) and creator `73BA0` (80).
The parent's integration removes the legacy initializer's false 28-byte C
credit and retains assembly. Its valid void worker draft is
`wip/sub_08073B84.c`, 84.6% over all 28 bytes: only the epilogue differs.
An explicit `__builtin_return_address(0)` model in the neighboring worker did
not match and was not promoted.

`73C2C` finds the first category match with mode zero, considering at most twelve
records. Its ordinary-C scan/found/missing labels retain the cold return outside
the loop backedge; structured loops otherwise move the preheader. `74600`
uses this default lookup only for mode zero and allocates on a miss; nonzero
modes allocate directly. Its forwarded argument lifetimes retain three of six
initial hints after `depin.py --write`. All eleven worker extents are recorded
in `build/scene-actor-contacts-final.json`; parent owns combined ROM validation.
