# Route sprite actor pool (tentative)

The `0806D000–0806E130` subsection belongs to the four-slot pool at singleton
`02000198`. Its identity in the game remains unverified; names describe the
observed route, sequence, movement, and repetition mechanisms. The adjacent
manager lifecycle begins at `6E130` and is owned by the integrator.

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

## Matching evidence and remaining work

Current new exact bodies are `6D000` (388), `6DB20` (452), and `6DFC8` (360):
**1,200 bytes / +0.0497 percentage points**, all temporarily register-pinned.
The allocator, wrapper, event-script callback, and empty state total 132
previously credited bytes, independently reviewed and checked in full.
Temporary hints were minimized: completion TU 4→2 plus eleven shared playback
hints, pausing 60→26, and creation 52→24. No source binds `r7`.

Unmatched bodies remain `6D184` (692), waypoint helper `6D438` (776), movement
state `6D740` (992), and emission/repetition state `6DCE4` (636). A spreading-state
candidate is retained privately and is being matched across its full extent.
The bounded permuter search produced an apparent 455 score by deleting a
register temporary's initialization before an asm input; that output was
rejected. It is not a valid improvement over the initialized source.
