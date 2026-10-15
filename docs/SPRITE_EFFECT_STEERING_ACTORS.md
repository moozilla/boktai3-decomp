# Sprite/effect controllers and steering actors (0806B2C4–0806CA3C)

This review continues the contiguous visual-manager work. Names describe observed
contracts; the game identity and runtime coverage of these managers remain
unconfirmed. The first family belongs to global `02000178` and the second to
`02000188`. They have distinct allocation, sprite ownership, and collider views.

## Sprite/effect controller

The shared `sprite_effect_controller.h`, recovered with the preceding worker,
describes a `0xBBC` task: used mask at `18`, then 24 records at `1C`, stride `7C`.
A record contains drawable pointer `0`, sequence state `4`, velocity `14`, collider
`1C`, flags `70`, state `72`, signed slot index `73`, lifetime `74`, frame `78`, and
state-entry byte `7A`. Flag `8` selects a sequence sprite (frame `10`, position
`1C`); otherwise the drawable has a tile sprite layout (tile `8`, dimensions
`A/B`, priority `F`, position `18`, graphics-resource pointer `28`). Both expose an
active byte at `4`. The constructor allocates `48` or `2C` heap bytes respectively.

The pool dispatcher (`6B8DC`) skips when `(030053F4 | 030053F0) & 400` or
`02000560 & 4`, invokes `6A60C`, then dispatches each used slot through table
`08605B84` using its state byte. The pool destructor (`6B96C`) selects
`Sprite_Remove` or `Effect_Remove` and frees the heap drawable when its active
byte is set. Initialization clears the used mask and publishes the singleton;
creation uses task list 10, and reset only clears that singleton pointer.

The fading state (`6B2C4`) resets its frame on entry and steps the corresponding
animation representation. Both walkers apply command flip bits and advance the
sequence index/duration. The sequence-sprite walker also stores frame weight and
clamps a zero computed duration to one; the tile walker does neither. The shared
blink helper `082212DC(drawable, frame, 60, 40)` uses a three-period table and reports completion after frame 60. For this
`(60,40)` call, it selects table index zero through frame 40, then index two;
index one is not selected because the helper tests its fourth argument first. Completion clears the pool bit
and slot state, removes/frees an active drawable, then writes index `-1` and zero
flags. Otherwise the frame increments. The otherwise redundant flags-bit-8 mask
before the blink call remains as an empty-asm lifetime concession.

The resource helper (`6B5B0`) takes three arguments: slot, graphics ID, context.
The graphics ID really is consumed by `AnimData_Load`; the old one-argument
loader declaration happened to leave `r1` unchanged. It returns resource lookup
`ScriptProc_Start(922E, context)`. The script wrapper (`6BA58`) reads packed
position keyword `70` and velocity keyword `76`, with missing components inside
each present triple defaulting to zero. An absent whole keyword leaves that
local vector unassigned, matching the observed wrapper; callers need to supply
the vector keywords for a defined vector input. The registry maps key `7314`
to Thumb callback `0806BA59`. All five literal-key callers in the ROM's GCX image
supply both `p` and `v`, with three operands in each triple:

| Native packet address | Procedure index | Present vector keywords |
|---|---:|---|
| 08DF2AEA | 8799 | p, v |
| 08DF3B6D | 1717 | p, v |
| 08E50ED4 | 6013 | p, v |
| 08E50F77 | 6015 | p, v |
| 08E540AA | 6144 | p, v |

These are native `B745` packets, whose handler `08225590` uses the same actor
registry lookup `08225884` before invoking the callback. The bytecode has a short
argument-list envelope containing key `7314`, followed by keyword envelopes
inside the full native packet; inspecting only `gcx.py chara` output misses
these callers and keyword tails. An exhaustive little-endian key-byte scan of
the GCX body has eight hits: these five callers and three unrelated compact
length/procedure-byte coincidences. The reviewed packet bounds and decoded
triples are retained privately in `build/7314-native-call-keywords.json` and the
raw-hit classification in `build/7314-key-occurrences.json`. This proves the
precondition for the five literal-key callers, without claiming runtime
coverage or proving the arguments of arbitrary dynamically computed dispatch.
Dynamic/nonliteral callers must also supply both vector keywords.

The wrapper then reads keywords `50`, `64`, `72`, and `6C` and calls the constructor with graphics ID `1C1E`, context
`D43F`, slot flags `11`, palette bank one, and two negative-four bias parameters.

The constructor (`6B5D4`) remains a private nonmatching draft. Its recovered
18-argument contract is position, velocity, graphics ID, context, sequence,
drawable flags, slot flags, palette, priority, variant, animation flags, lifetime,
four contact parameters, and two bias parameters. A nonzero velocity sets flag
`80`; positive first contact parameter initializes the collider and sets flag
`4`. The tile path selects state two for flag `20`, otherwise state three for
flag `200`. Heap allocation failure is not checked by the original body.

## Steering actor

`scene_steering_actor.h` describes an independent eight-slot task, with used mask
`38`, records at `3C`, stride `128`. Its embedded sequence sprite begins at zero
(flags `0`, priority `7`, signed scales `8/9`, frame `10`, position `1C`, sequence
`2C`). A shadow begins at `3C` and the actor collider at `A8`. Motion/control
fields are velocity `FC`, target `104`, initial direction `10C`, direction `10D`,
steering timer `10E`, turn step `10F`, speed `110`, parameter `112`, shadow status
`115`, scale timer `116`, sequence resource `118`, lifetime `11C`, flags `120`,
state `122`, signed index `123`, state frame `124`, entry byte `126`, and steering
delay `127`. Field names and the family identity remain tentative.

The allocator (`6BC8C`) chooses the first clear bit and writes its index, without
clearing the rest of the slot. Collider initialization uses bounds `(64,128,64)`
and zero offset, configuration `2403`, priority 16, callback `6BBB8`, and position
`1C`. The callback requests state five and sets the entry byte. The recovered
packed-vector/ordinary-position union preserves the observed aggregate layout
without the prior uninitialized scalar mask operation. Three unused trailing
constructor-supplied collider arguments are preserved in its declaration.

Steering (`6BCD8`) first counts down its delay. While the steering timer is
nonzero, it computes `atan2(target.x-position.x, target.z-position.z) - direction`,
wraps the error into `[-128,128]`, and compares its absolute value with 18. For a
larger error it adds or subtracts the turn step and decrements the steering timer;
otherwise it clears the timer. The sign of this turn is the observed engine
angle convention, without a guessed world-direction interpretation.

Random emission (`6BDB8`) consumes three RNG samples: byte angle, radial magnitude
`(sample & 31)-16`, and vertical velocity `(sample & 63)-48`. It computes X/Z with
the sine table and truncates signed products toward zero at 12 fractional bits,
then calls `08125EC4(position, velocity, 60, 3)`. The dead literal `0403B714`, also
seen in other RNG callers, survives only through an explicitly defined empty-asm
lifetime; no C computation consumes it.

The two scale-in states (`6BE84`, `6C044`) initialize scales to one, count down a
15-frame interpolation timer toward scales 64, emit every eighth state frame,
and advance animation. Completion clears flag two, requests state four or three
respectively, and sets the entry byte. Their final signed timer load retains one
asm-defined scratch token: only empty-asm inputs consume it, and the defining
output must remain. The other two timer loads use explicit empty-asm clobbers.
The initial depin pass removed the token definition while retaining an input;
the semantic audit restored a valid definition before the final exact checks.

The bobbing state (`6C204`) clears horizontal velocity, derives Y velocity from
`4*sin(stateFrame)`, adds velocity to position, emits every 128 phase units,
refreshes its collider, and advances animation. Its phase increments by two and
wraps to eight bits. Slot teardown (`6C708`) emits through `69D50`, with two
collider-associated parameters each defaulting from zero to one, removes the
shadow when its status byte is zero, removes the sprite, clears the used bit,
and writes index `-1`, state zero, and entry zero.

Travel state `6C3A0` and constructor `6C7B0` remain assembly-backed gaps. The latter
creates state one with lifetime 120, animation sequence seven, a shadow and
collider, and copies target/direction/steering parameters. Its adjacent state-two
constructor begins at the next worker boundary `6CA3C`.

## Validation and accounting

The frozen cohort contains 19 exact functions covering 3,708 reviewed bytes.
Ten newly recovered bodies contribute **+0.1356 percentage points** (3,276 bytes):
**+0.0238 points ordinary C** (576 bytes) and **+0.1118 points register-pinned**
(2,700 bytes). Nine pre-existing exact bodies were reviewed and typed (432 bytes)
without new matching credit. The three outstanding body spans are `6B5D4` (776),
`6C3A0` (872), and `6C7B0` (652). Global progress is authoritative only after the
parent's combined import, name promotion, and complete ROM build.

| Address | Reviewed bytes | Contract | Category |
|---|---:|---|---|
| 0806B2C4 | 748 | Fade and destroy drawable | New, pinned |
| 0806B5B0 | 36 | Load sprite resources | Prior exact |
| 0806B8DC | 144 | Update pool | New, pinned |
| 0806B96C | 132 | Destroy pool | New, pinned |
| 0806B9F0 | 16 | Initialize pool | Prior exact |
| 0806BA00 | 76 | Ensure pool | Prior exact |
| 0806BA4C | 12 | Reset pool | Prior exact |
| 0806BA58 | 352 | Create from script | New, ordinary |
| 0806BBB8 | 24 | Request departure on contact | Prior exact |
| 0806BBD0 | 156 | Initialize collider | Prior exact |
| 0806BC6C | 32 | Refresh collider | Prior exact |
| 0806BC8C | 76 | Allocate actor | Prior exact |
| 0806BCD8 | 224 | Turn toward target | New, ordinary |
| 0806BDB8 | 200 | Emit random particle | New, pinned |
| 0806BE80 | 4 | No-op | Prior exact |
| 0806BE84 | 448 | Appear before travel | New, pinned |
| 0806C044 | 448 | Appear before bobbing | New, pinned |
| 0806C204 | 412 | Bob in place | New, pinned |
| 0806C708 | 168 | Destroy actor slot | New, pinned |

Full reviewed sizes passed the fast checker after the final formatting and
semantic audit (`build/visual-steering-final.json`, private verification output).
The parent runs a combined build before integration. Register hint minimization
included `6B2C4` 169→70, `6B8DC` 10→2, `6B96C` 15→3, `6BDB8` 33→12,
`6C204` 57→29, and `6C708` 20→13. The scale-in states started at 61→34 hints,
then replaced two output-token pairs with clobbers and repaired the final token's
definition; no handwritten instruction bodies or explicit `r7` bindings are used.
