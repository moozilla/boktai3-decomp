# Actor terrain targets and effect cleanup

The first reviewed subsection is0807E334–0807E9D0. It contains a968-byte cardinal
terrain scan, followed by randomized effect offsets180 bytes, an existing48-byte
advance helper, route-position resolution268 bytes, an existing32-byte position
forwarder, and resource cleanup200 bytes. The three new complete matches add
**648 bytes /+0.0268 percentage points**, all Register-pinned. Standard hint
minimization reduced the random-offset helper17→7, route resolver14→5, and
cleanup10→4. No explicit r7 binding or instruction bodies are used. Static
contracts do not imply runtime coverage.

## Random offsets and shared effect context

E6FC takes actor, signed count, mode, horizontal span, vertical span, and a word
value. With positive count it advances03005308 twice per iteration, wrapping by
3FF and publishing each index before reading the shared unsigned16 random table.
The X offset is Mod(sample>>4,horizontalSpan)-(horizontalSpan>>1); Y is the wrapped
word negation of Mod(sample>>3,verticalSpan), minus6. Z and the unused final
halfword are zero. Spans must satisfy the external Mod operation's caller
contract. Signed16 bitfield members reproduce the observed storage updates; each
member is initialized before the call, without a C whole-word read of unfinished
storage. The reviewed extent is exactly180 bytes.
The final mode/value/vector call targets082315A8 using actor+47C. Its callee copies
the complete8-byte offset and updates the context's effect entries. The existing
E7B0 helper advances that context count times only when its first byte equals1.
The existing E8E8 helper forwards actor position5C with three zero word arguments
to08232264, again using context47C.

## Route target resolution

E7DC reads X/Z from the current embedded route point at actor450. This point is
an X/Z pair, distinct from an XYZ position. It derives a terrain cell using signed
coordinates shifted8, rejecting negative or out-of-range coordinates to cell0.
The row offsets come from the map's rows24. Terrain lookup prefers dynamic
0821D64C(cell,1)+4, otherwise base cells+12+cell*4. Low-nibble height is scaled256;
slope kinds1/2 subtract the low Z/X byte. That Y store remains in the original
code even though it is immediately replaced with the actor's current Y.

The local target has halfword alignment and only halfword field accesses. It
passes fully initialized XYZ to the three-argument collision queries0821F204
and0821F7A8, with context actor5D0 and actor position as the other arguments.
The latter query's result2 preserves velocity; another nonzero result clears
the8-byte velocity record. A zero from either query clears flags2D4 mask200.
The unused padding halfword is never read as a C scalar.

## Resource teardown

E908 removes a kind0 drawable through Sprite_Remove, or another drawable's +20
animation through Anim_Remove, then frees the drawable allocation. Flags2C0 mask100
requests receiver removal at actor+C. Active shadow320 and effect context47C are
released through13684 and823233C. When flags2D0 mask02000000 is clear, terrain
ownership for identity50 is removed through03C10. Flags2C0 mask200 also release
the control object at404. Finally225938 processes identity50, Collider_Remove
removes collider134, and SceneActorRegistry_UnlinkActor removes the registry node.
The function does not itself free the actor allocation. Callback/API names beyond
these observed effects remain provisional.

## Remaining cardinal scan

E334 remains private semantic C with no exact credit. It tries up to four cardinal
directions, starting opposite the rounded heading quadrant. It scans cells of the
same low-nibble height whose flags halfword excludes43. A blocking cell selects
the preceding clear cell; exhausting the map limit instead restores the starting
cell. If the selected cell is still the actor's current terrain cell, it tries the
next direction. East/west derive their initial column by Mod(start±1,width), so
there is no added check preventing that initial step from entering an adjoining
row. The selected cell center is passed through the same two collision queries
and failure/velocity rules as E7DC. Private variants retain the observed control
flow and compiler-lifetime experiments; none contributes progress.

The adjoining 80008 callback is exact over84 bytes. It consumes prepared2B2,
sets control414 bit1 when the unsigned wordF0 is at mostFFFFF (otherwise clears
it), and selects directional animation entry1. The wordF0 interpretation remains
unknown; the comparison is preserved without assigning it a gameplay meaning.
Three minimized allocation hints remain.

A separate restoration callback begins7FE80 inside the previous retained tail.
FDC0 ends after192 bytes; FE80 has its own prologue, full200-byte body, return
and pool ending7FF48. Whole-ROM Thumb-pointer and decoded direct-branch scans
found no references to the new entry. This boundary review adds one function
without changing the code-byte denominator or claiming unmatched C credit.

FDC0/FE80/FF48 are now exact over192/200/192 bytes, with2/1/1 minimized
register hints respectively. At signed frame >= embedded threshold46C, they
restore active state/phase/behavior from468/469/46A, control21C from470, set
prepared2B2 and changed2B1, clear2B0/2C8/104/counter2AC, install substate14,
and clear flags114 bit1. FE80 also clears restart2B3 unconditionally at entry.
They forward table0824E128 and indices74/8B/A0 to the embedded460 reset helper;
the existing helper ignores those latter arguments and only clears its local
state bytes8/9/A and wordsC/10. Its definition now records the observed
three-word argument contract, still exact over16 bytes. No runtime coverage is
claimed by these static observations.
