# Scene panel interaction states

The reviewed cohort is `0805B28C–0805BCEC`. It contains five predicates and
the fifteen handlers in `gUnk_08605ABC`, in states 0–14. The next function,
`0805BCEC`, initializes presentation sprites. The new local layout is
`scene_panel_interaction.h`; the integrator's shared panel/rig views remain
unchanged. All names describe observed behavior; overall scene identity and
the player's word at `0x1C` remain tentative.

## Layout and predicates

| Offset | Observed field |
| --- | --- |
| `0x1E` | subject count |
| `0x250` | presentation callback |
| `0x254` | main sprite flags |
| `0x4FC/0x4FE` | interaction state/frame |
| `0x500/0x508/0x510` | origin, player approach position, tether center |
| `0xBE8` | subject pointer slots |
| `0xC10` | presentation mode |
| `0xC12` | unnamed transition gate byte |
| `0xC16` | acquisition variant |
| `0xC6C` | embedded rig; its state is also panel offset `0x12E8` |
| `0x1336` | all-active notification latch |
| `0x1337/0x1338` | arrival ramp enable/frame |
| `0x134A` | optional transition sound |

Subjects expose state byte `0xD3` and a changed/request byte at `0xF0`.
The local player view keeps `actionMode` at `0x418` distinct from `tetherMode`
at `0x457`. The existing tether view supplies position `0x50`, state `0x255`,
and heading `0x260`.

`0805B28C` subtracts signed X/Z halfwords, performs the original one-sided
component bounds checks, then compares the signed, wrapping 32-bit squared
sum with the wrapping radius square. It does not inspect Y or take absolute
values. Unsigned arithmetic before signed comparison preserves the original
multiplication/addition without signed-overflow undefined behavior.

Acquisition requires a nonnull tether, player word `0x1C` equal to one,
`tetherMode` six, and tether/center distance at most 70. Player acceptance
requires that same word equal one, `actionMode` outside 3–5, `tetherMode`
seven, and player/approach distance at most 100. The distant-tether predicate
negates the radius-800 test and assumes the tether is present. The settled
predicate requires every subject to have state two; an empty set passes.

## State flow

| State | Handler | Main behavior |
| --- | --- | --- |
| 0 | `0805B3CC` | Acquire tether with variant zero, enter state one |
| 1 | `0805B40C` | Interpolate/acquire, then dispatch by variant |
| 2 | `0805B658` | Wait for all subjects at state two, enter three, script argument zero |
| 3 | `0805B688` | Reacquire with variant one, or enter four when a subject is active |
| 4 | `0805B70C` | Track active subjects and accept the approaching player |
| 5 | `0805B7C8` | Submit weighted arrival position, choose state six or seven |
| 6 | `0805B8B4` | Timed subject departure; return to three |
| 7 | `0805B974` | Arrival script/ramp counters |
| 8 | `0805B9C8` | Wait thirty frames and clear gate, then enter nine |
| 9 | `0805BA18` | Wait for tether state thirteen; start rig state one and panel state ten |
| 10 | `0805BA50` | Reset timer while active; otherwise after 120 frames request rig state six/panel eleven |
| 11 | `0805BAE4` | Wait for rig state zero; begin directed tether action, state twelve, script five |
| 12 | `0805BB20` | Reacquire variant two, or initiate distant tether return |
| 13 | `0805BC10` | Timed return events |
| 14 | `0805BCE8` | Empty handler |

State one's frames 1–5 interpolate X/Z with divisor `6 - frame` and previous
weight `divisor - 1`. Only the packed X and Z fields are written; Y and padding
remain untouched. On odd frames, nonzero heading moves to an adjacent heading
and masks with seven. Frame six sets tether state twelve, copies the center
into its position, clears heading, sets main sprite flag one, and plays 249.
At frame twenty, variants other than two request callback `080582B8` and play
294. Frames 7–69 submit `(70 - frame) >> 1` to `082260A4`.

Afterward, variant zero requests subject state one, calls the player helper,
plays 250 and enters two. Variant one calls two player helpers, then selects
state four if any subject is at state four, or three otherwise. Other variants
play 490, set tether state fourteen, request rig state one and enter ten.

State three's subject scan runs even after reacquisition; an active subject can
therefore replace the just-selected state one with four. State four clears each
changed byte after its sound/callback calls by reloading the pointer slot, so a
callback can replace that subject. Counts are freshly loaded around those calls.
It sends script argument one once all subjects are active, including the empty
set, and fills a missing presentation callback with `08058444`.

State five submits the component-wise low sixteen bits of
`(3 * origin + playerPosition) >> 2`. Mode zero selects player helper argument
zero, sound 407 and state six; nonzero mode selects argument one, sound five,
callback `080581A0` and state seven. State six requests active subjects' state
five at frame thirty, performs its second event at ninety, and after 105 waits
for player `tetherMode` zero before state three/script two.

State seven caps its main counter at seventy and sends script argument three
when reaching that value. Its optional second counter caps at sixty and sends
half the current value to `082260A4`. This consumer only raises a halfword at
offset `0x7E` of the global `03005420` object when enabled; fade semantics have
not been established, so the fields and handler are named as an arrival ramp.

State ten evaluates active subjects only in nonzero mode. It calls `0805907C`
when game-state word `0x618` is positive, then uses the previously computed
active flag and fresh panel frame. State twelve computes player direction as
`(((u16)atan2(dx,dz) + 112) >> 5) & 7`. Player action modes zero, one and six use
one helper path; other modes use two helpers. The return path starts the tether
auxiliary state, submits its position, selects state thirteen and script six.
State thirteen performs events at frames 33, 64, 300, 316 and 360; the last
sends script argument seven. Frame 316 optionally plays `transitionSound`.

## Validation

All twenty full reviewed extents are exact. Eleven new functions cover 2,172
bytes, adding 0.0899 percentage points: 1,120 bytes of ordinary C and 1,052
bytes with temporary concessions. The four conceded functions retain one,
six, one and one minimized hints respectively (`B34C`, `B40C`, `B70C`, `B8B4`).
Nine already-C functions contribute 484 typed bytes with no new credit.
The existing hidden four-byte leaf `BCE8` retains its metadata and local alias.
The former no-argument declaration of acquisition predicate `B2CC` in `B3CC`
has been corrected to pass the panel explicitly. No handwritten instruction
bodies or explicit r7 bindings are present. Full combined validation belongs
to the integrator.
