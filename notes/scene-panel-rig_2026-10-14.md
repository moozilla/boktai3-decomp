# Scene panel rig and contact/movement contracts (tentative)

The same parent ScenePanel owns subjects and a separate sprite rig. The reviewed
59428–59C48 span contains sixteen functions /2,080 bytes: ten new exact bodies
add1,920 bytes. The five old parent helpers and16-byte scale leaf receive typed
contracts without new matching credit. No game identity or execution coverage
is inferred solely from these offsets.

Rig layout: fifteen104-byte sprite records (main, eight primary, six secondary),
position618, origin674, state67C/frame67E, angle680/turnFrame682, signed turnRate684,
turnDuration686, distance688/speed68A, verticalStep68C, scale68E, motionFlag690,
contact direction691/force692/effect694, resource accumulator69A, effect payloads
6A0/6A8 and parent6B0. Parent fields1E/1F count subjects/active participants;
subject pointersBE8 point to stateD3/requestE6. Parent BF8 scales resource drain;
modeC10 and activated1342 are observed separately from warningStage1343 and
warningSounds1344. These are partial views, not complete allocation-size claims.

Contact responses require source reactionFlags&7F, copy direction and play243.
The nonnegative source-minus-target power difference is increased according to
source typeFlags priority1,2,4,10,200 (hex); corresponding effects are32,24,128,
128,128 (decimal). Force is capped at200 after halfword storage. State5 clears
the effect; absent type bits retain previous force/effect.

Hide/show toggle bit0 on all fifteen sprites. Leaving state0 first restores
origin plus128 Y; entering state2 from another state changes parent presentation
and sets global panel activation. The state is stored as a halfword, frame clears,
and state0 hides while other states show. Movement initialization computes X/Z
angle, stores angle<<4, and derives radial distance by dividing the dominant
axis by the sine table; no new division guards are introduced.

Active-subject selection consumes one general RNG sample only if activeCount is
nonzero, selects an ordinal modulo that count, and scans state4 subjects. It may
return-1 if parent counts and actual eligible entries disagree. Existing helpers
query/set subject state, expose parent counts/mode and set requestE6.

Movement randomization first initializes polar movement, chooses speed192..447,
compares current angle with the player direction and chooses signed turn rate
12..19, then duration64..127 and clears turnFrame/motionFlag. Rise restores origin
on each tick: frames1..7 add64..256 Y, frames8..15 add256..144; the next tick
adds128 and enters state2 if parent mode/activeCount are nonzero, otherwise it
randomizes movement, sets speed448/verticalStep96 and enters state3. Return uses
sixteen-frame weighted coordinate interpolation toward origin, then enters state0.

Resource drain requires positive activeCount and parent mode. State2 drains at
four times their product and may emit an effect at position; states3..5 and7 use
the base product and may emit at origin. The parent percentage factor scales
positive rates with a minimum of1. A halfword accumulator converts amounts above
40 into integer drain units, preserving remainder. The global remaining value
clamps at zero. On first crossing of one quarter of the reference value, warning
stage becomes1, its indexed sound plays if nonzero, a screen transition starts,
and pending script4 is called. All observed RNG advancement, wrapped halfword
arithmetic and post-store global reloads are retained.

Offsets/addresses omit0x; sizes/timings are decimal. Fast checks use each full
reviewed extent. Register concessions on movement setup, rise and resource drain
are minimized before promotion; no explicit r7 binding or instruction bodies.
