# Scene panel rig states and animation (tentative identities)

Reviewed update span `08059C48–0805AD18`: twelve functions /4,304 bytes,
eleven new exact bodies /4,248 bytes (+0.1759 percentage points). The existing
56-byte contact pause and the separately reviewed 32-byte subject-request helper
receive typed contracts without new credit. Of the new bytes, 712 are ordinary
C and 3,536 retain minimized register concessions. No game identity or execution
coverage is inferred from these addresses.

The ten pointers at ROM `08605A94` establish this rig state dispatch:

| State | Callback | Observed behavior |
| --- | --- | --- |
| 0 | 08059AB4 | Hold collider timer at 16 while hidden |
| 1 | 08059AC4 | Rise from origin, then hover or wander |
| 2 | 08059C48 | Hover and periodically select a state-four subject |
| 3 | 08059DF8 | Randomized turning, radial motion and subject selection |
| 4 | 0805A044 | Approach the selected subject's angle and radius |
| 5 | 0805A1FC | Request subject action, emit effects and return to wandering |
| 6 | 08059BA4 | Interpolate back to origin, then hide |
| 7 | 0805A2BC | Contact knockback and force decay |
| 8 | 0805A3D4 | Wait for contact-effect duration, then wander |
| 9 | 0805A40C | Hide and emit a shrinking burst for 64 frames |

The earlier names `scale`/`ResetScale` were provisional and incorrect: offset
`68E` is now `colliderHold`, and the hidden callback is `TickHidden`. The parent
driver sets collider flag four and decrements this timer while nonzero; at zero
it clears that bit and calls `Collider_SetPosition`. No stronger meaning for
flag four is claimed here. Offset `682`, incremented/decremented toward the
signed target turn rate, is now `turnStep`. Shared rig field `620` is a typed
`ContactCollider`, preserving its existing 84-byte extent. The setup and update
paths now share one 104-byte `SceneRigSprite`: render mode at 7, animation frame
at 10, position at 1C, graphics at 2C, sequence at 48 and resource at 58.

Hover contracts X/Z toward origin by quarter steps, consumes a fresh random
sample for Y at origin+128..191, and caps its state frame at 300. From frame 60,
a zero selection timer resets to 300..599 and selects an active subject. A
valid index initializes polar movement and enters state four. Otherwise the
300-frame timeout can start wandering with speed 448 and vertical step 96.
The observed unused constant `0403B714` remains a value retained by a minimized
constraint; it is never dereferenced, and no hardware role is inferred.

Wandering smooths `turnStep` toward `turnRate`, periodically chooses magnitude
12..19 and sign, advances a wrapped 12-bit angle, smooths radial distance toward
speed, and adds a phase-modulated planar oscillation. Random-turn durations are
32..95 and speed durations 64..127. First samples remain local until the second
index write where the ROM does so. A selection timer at or below two resets to
600 and may enter approach; other ticks subtract two.

Approach uses angles 1920, 3968, 3200 and 1152 for the four selected indices,
with radii 480, 400, 480 and 400. It turns by the shortest wrapped direction in
48-unit steps, smooths distance by eighths, and enters subject-action state with
sound 244 once aligned and within 31 distance units. While a subject remains in
state four, the action has a one-in-four effect chance and requests state five.
The request helper stores only the request flag; its observed third argument
is unused and remains in the typed ABI. After 90 ticks it sets the subject to
state five and returns to wandering.

Knockback applies the contact direction through the sine table. Outside squared
radius 490,000 it clamps force to at least 30 and rotates direction by 128;
inside, force decays to three quarters. Zero force enters the contact pause.
Contact-transition checks exclude hidden, rising, returning and burst states.
They can leave hover when parent mode is zero, or return from knockback to hover
near the offset origin while parent mode is positive. Remaining nonzero force
selects knockback. The burst hides on its first frame, sets parent activation,
and emits payload effects at rig and secondary-sprite positions before state zero.

The renderer has three layers. The central sprite starts at rig position plus
(44,64,44); pause state eight consumes one RNG sample to add opposite X/Z jitter
in -8..7. Eight primary sprites orbit with radius 60..100, opposite angular
steps on alternating indices and faster radius phases while hovering. Six
secondary sprites interpolate current position toward origin using weights
(i+2)/8, add a height-derived X/Z lift, and orbit with amplitudes 24,28,...44.
Their angular phase advances by three and radius phase by five each tick.
All divisions by powers of two in these position formulas truncate toward zero.
Packed coordinate padding is preserved, with no explicit r7 binding.

All fifteen sprites inline the same sequence decoder already seen in the
contact actors. Command bits above five choose the frame; bits four/five XOR
with sequence flags to control sprite flags four/eight. The low nibble is frame
weight. Sequence flags four/eight select reverse traversal and hold-at-end;
otherwise indices wrap. The byte duration is `(speed * weight) >> 6`, with a
zero byte replaced by one. Each layer applies the selected palette bank.

The parent driver checks contact transition, dispatches the callback, handles
the collider timer and position, and applies resource drain unless parent byte
C0F suppresses it. Palette 91 changes to 92 when a scheduled action exists and
is due, or on alternating four-frame groups during its final 90 frames.
Frame and interval reads retain their observed reloads.

Full reviewed extents passed before integration. Primary animation hints were
minimized to 12, central sprite hints to 12, and the driver to 21. The secondary
chain was minimized from 76 to 28 hints. The existing hidden-state boundary is
unchanged; its descriptive alias stays file-local because it is not a raw
function label. Build and post-promotion evidence are recorded in HANDOFF.md.
