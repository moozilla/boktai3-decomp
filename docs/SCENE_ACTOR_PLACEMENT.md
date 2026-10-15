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
