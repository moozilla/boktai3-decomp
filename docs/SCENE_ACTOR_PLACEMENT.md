# Scene actor placement and event feedback

The phase view in `scene_actor_placement.h` covers the contiguous reset,
callback, shake and sound consumers beginning at `080772C0`. Names describe
observed behavior; the actor category and variant meanings remain tentative.
The halfword at `1E6` selects the accepted sound variants zero, one, three and
five; this is the same physical field previously named paletteOffset. The
reset-control pointer at `220` is distinct from the saved pointer at `22C`.

`77848` dispatches event values zero through six to songs, filtering the
variant for most events. Event one plays song529 when field2E4 is zero;
otherwise it checks flags2E6 bit4 and reloads the variant before playing318.
`779B8` and `77B1C` select songs by actor category. Both consume flags2D0
bit400000 as a one-shot suppression flag. The response dispatcher additionally
returns when flags2E6 bit4 is set. Category three forwards to the variant
dispatcher with event zero or one. These functions do not change animation
state and the sound/variant names remain provisional.

The three complete reviewed extents are 368,356 and300 bytes respectively.
The event-by-category dispatcher is ordinary C. The other two each retain
one local register-zero mask allocation hint after `depin.py --write`; no
empty barriers remain. Their combined exact credit is1024 bytes:356 ordinary
and668 with temporary register concessions.

Remaining reset/callback/shake and particle-emission routines in this range
are private drafts and do not count as matches. The ordinary disassembler
start at `77654` hides a real helper boundary at `77444`; `772C0` ends there
with388 bytes and the hidden helper has528 bytes.

`77654` consumes callback-request flag80 at2D6, invokes the optional pointer
at23C through0824923C, then clears180 and returns one. It returns zero for
actors marked unavailable (2D4 bit40,2D0 bit20000,or nonpositive signed health),
zero flags,or absent request. The original retains a second flags read; an
inline test also saves its pointer for the post-callback clear. Its144-byte
extent is exact with two empty lifetime barriers after minimization. No
callback is dereferenced by authored C; dispatch retains the engine ABI.

`776E4` uses the same availability gate and consumes drawable-shake flags10
and20 at2D6. The first takes precedence and adds independent random offsets
from minus5 through4 to X/Z; the second uses minus8 through7. The position
belongs to the sequence drawable at1C or tile drawable at40. Each random
read advances the shared index modulo1024 before loading the table value.
Halfword arithmetic wraps on the store, matching the original unsigned loads.
The356-byte extent is exact. Minimization removed all nineteen allocation
and coordinate hints, leaving only the same two lifetime barriers used by
the callback consumer.

`77C48` is a timed particle emitter, not an initializer. Halfword2BC is
decremented while nonzero; emission runs when the new value is divisible
by four. Byte2B6 selects an eight-way base angle,2BA supplies an angular
offset,and signed byte2BB changes that offset by four times its value.
Values above64 clamp to64 and reverse the signed step. Category2 selects
particle kind9 or10 according to paletteOffset; category14 selects kind11
and adds40 to world Y. Each selected path issues three particle-group calls
with independently randomized counts,angles and durations. Its eleven-arg
particle API at148CC is preserved; the first two random-index advances do
not publish the index before the later third/fourth samples. Each subsequent
particle call can change the shared RNG index,so the next group reloads it.

The emitter reproduces1072 bytes. `depin.py --write` reduced152 exploratory
hints to21; all are local,no explicit r7 binding is present. Coordinate
copy and word masking are defined: the position union is assigned from the
actor before the first word's initialized X/Y pair is read. The remaining
padding halfword is passed through the aggregate; it is never used as a
scalar in authored arithmetic. The preserved separate branches/scopes keep
three stack positions and RNG lifetimes consistent with the reviewed code.
