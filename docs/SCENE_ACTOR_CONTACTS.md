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
r0 value with zero. The initializer will be void; the creator remains assembly
until a defined representation is established. No output-only empty operand,
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
