# Route sprite actor pool (tentative)

The reviewed `0806CFE4–0806E34C` subsection belongs to the four-slot pool at singleton
`02000198`. Its identity in the game remains unverified; names describe the
observed route, sequence, movement, and repetition mechanisms. The manager lifecycle begins at `6E130`; the following terrain helpers begin
at the independently recovered `6E34C` boundary.

## Storage and callbacks

The manager is `600` bytes: task header `0–17`, sequence resource at `18`,
occupied mask at `1C`, and four `178`-byte slots beginning at `20`. Each slot
contains an ordinary sprite at `0`, graphics resource storage at `2C`, sequence
state at `48`, a twelve-byte route record at `58`, and scaled-shadow storage at
`64`. Packed positions occur at `D0`, `140`, and `148`. Their broader roles are
still under review; `D0.y` is initialized to anchor Y plus `2C8` and is later a
movement target. The route-selection helper `6D438` replaces `D0` with the
player's current packed position.

The final control fields include flags `150`, state `152`, signed pool index
`153`, frame count `154`, entered flag `156`, position-change flag `157`,
animation-ended byte `158`, heading `159`, signed bob angle `15A`, and signed
shadow-initialization result `15B`. Entry `168` is an unsigned repetition count,
not a height. Initial/current timers are at `16A`/`16C`, movement timer at `16E`,
integer event-script ID at `170`, and forwarded context at `174`.

ROM table `08605BB0` provides the six state callbacks:

| State | Callback | Observed role |
| --- | --- | --- |
| 0 | `6CFFC` | Empty inactive handler |
| 1 | `6D184` | Spread toward a random radial X/Z target |
| 2 | `6D740` | Route movement; calls waypoint helper `6D438` |
| 3 | `6DB20` | Timed cosine bob and animation wait |
| 4 | `6DCE4` | Jitter, emit another pooled actor, run event script, repeat |
| 5 | `6D000` | Finish animation and release slot |

This table is static evidence, not an execution-coverage claim. The allocator
`6DF60` scans four mask bits, clears the complete slot, and records its signed
index. The wrapper `6DFB8` forwards the incoming manager. Both were previously
exact and receive typed contracts without new matching credit.

## Exact bodies and behavior

`6D000` starts sequence `17` with option 4 on state entry. It steps packed
sequence commands, updating sprite frame and flip flags, and releases the
occupied bit when playback completes. It removes a present sprite and removes
the shadow only when shadow initialization returned zero. It then writes index
`FF`, state zero, and flags zero.

`6DB20` starts sequence `E`, resets its frame count and bob angle, and advances
playback. While the signed angle is nonnegative it sets sprite Y to saved Y
minus a cosine-table displacement with amplitude 64, then advances the angle
by four. It waits through frame 60 and for playback completion before entering
state 4. Its packed playback is expanded locally to preserve the distinct
primary and secondary owner-pointer lifetimes; no instruction bodies are used.

`6DFC8` creates a manager if needed, allocates a slot, and installs state 1,
zeroed flags/frame count, repetition count, timers, emission parameters, script
ID, context, and route. The unused incoming R1 formal is retained. It initializes
the route helper and scaled shadow, loads graphics `D45E`, starts sequence `17`,
selects palette bank `140`, copies the anchor, and assigns priority 1. Failure to
load graphics leaves the already-allocated record as the original does; no new
rollback is added. `6CFE4` invokes a nonzero integer script ID with null arguments.

`6DCE4` restores the saved position on each tick and jitters X/Z for the first
ten frames. At frame 4 it selects palette bank `143`, emits through `1287D4`
using a stack copy whose signed packed Y field is reduced by 60, then runs the
slot event script. On playback completion it decrements the unsigned repetition
count and restores palette bank `140`; a nonzero count returns to state 2,
otherwise it enters final state 5. The packed adjustment uses the established
`ContactPackedVector` bitfield layout in a local union with `ContactPosition`.

## Manager continuation and recovered boundaries

`6E130` dispatches occupied slots only when the combined engine pause flags
`030053F0/F4 & 400` and mode flag `02000560 & 4` are clear. Its state cursor is
an unsigned address so the unused final stride does not form a pointer beyond
the pool object. `6E1C0` removes present sprites and successfully initialized
shadows, then clears the singleton. `6E224` looks up the sequence resource with
hashes `(922E,1DC3)`; the legacy callee name `ScriptProc_Start` is misleading
here. `6E250` allocates a category-10 task with size `600`, and `6E29C` only
resets the singleton.

`6E2A8` requests final state 5 on every occupied slot that is not already in that
state. Its full body is **96 bytes**, not the old 552-byte inventory span. Two
independent functions followed its return inside a raw-byte tail: `6E308–6E34C`
is a **68-byte capacity query**; `6E34C–6E4D0` is a **388-byte terrain sampler**.
Both have their own prologues and complete returns. An addressed ROM audit
found no stored Thumb pointers or halfword-aligned direct B/Bcc/BL encoding
targeting either start, including data. No execution coverage is claimed. Both
build/progress extra-boundary tables now record the full reviewed ends. The
function denominator increases by two; the code-byte denominator is unchanged.
The terrain body belongs to the integrator's next subsection and remains
unmatched in this worker. `6E308` returns true when all four slots are occupied
or manager creation fails, and false when any slot is free.

## Matching evidence and remaining work

Current new exact bodies are `6D000` (388), `6DB20` (452), `6DCE4` (636), and `6DFC8` (360):
**1,836 bytes** before the manager lifecycle extension. `6E130` (144),
`6E1C0` (100), `6E2A8` (96), and recovered `6E308` (68) bring the cohort to
**2,244 bytes / +0.0929 percentage points**: 164 ordinary C bytes and 2,080
temporarily register-pinned bytes.
The allocator, wrapper, event-script callback, empty state, initializer,
constructor, and singleton reset total **264 previously credited bytes**,
independently reviewed and checked in full.
Temporary hints were minimized: completion TU 4→2 plus eleven shared playback
hints, pausing 60→26, emission 79→31, creation 52→24, updater 11→1,
destructor 2→1, and finish request 4→0. No source binds `r7`.

Unmatched bodies remain `6D184` (692), waypoint helper `6D438` (776), movement
state `6D740` (992). A spreading-state
candidate is retained privately and is being matched across its full extent.
The bounded permuter search produced an apparent 455 score by deleting a
register temporary's initialization before an asm input; that output was
rejected. It is not a valid improvement over the initialized source.
