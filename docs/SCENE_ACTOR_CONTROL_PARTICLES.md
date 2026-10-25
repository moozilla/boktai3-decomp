# Scene actor control and particle pools (08067148–08068500)

This contiguous review follows the inventory and attachment-manager cohorts. The
names describe observed contracts. The eight-slot actor controller's exact game
identity remains tentative; no runtime execution or screen identity is claimed.
Its global `02000128` and slot layout differ from the preceding attachment
manager at `02000124`. The adjacent particle managers use `02000130` (32 slots)
and `02000134` (8 anchored slots), respectively.

## Actor controller

`scene_actor_control.h` describes a `0x59C` task (list 10), an eight-bit used mask
at `38`, and eight `0xAC` records beginning at `3C`. Each record begins with a
sprite and animation state, followed by `stateFrame:3C`, `kind:3E`, `flags:40`,
`index:42`, `timer:44`, `scene:48`, cached parameters at `4C/4E`, a collider at
`50`, and a saved position at `A4`. Observed kinds are 0 free, 1 active, and 2
leaving. Graphics data begins at owner `18`, and the sequence resource is at `34`.
The resources are animation ID `1C1B` and resource lookup `(922E,3DC2)`.

The allocator (`67148`) selects the first clear bit, writes the record's signed
index, and sets that bit. The dispatcher (`67854`) calls the kind-indexed table
`08605B44` through the generic two-argument callback wrapper, then reconciles
unused player/tether parameters. The destructor removes each used record's
active sprite and collider and clears the singleton pointer.

The active state restores the saved position before applying visibility and
parameter logic. Flag `10` selects the player-counter path, flag `40` the tether
parameter path, flag `4` requests shaking, and flag `2` requests departure. Flag
`20` participates in conditional visibility. The cached counter uses an 8-bit
wrapping difference; the active routine compares the new difference with its
previous value and requests a shake on the relevant changes. The scene predicate
also checks system mode (excluding 0, 4, and 5), game-state `868 & 2`, and equality
between the record's scene and game-state `region:5A0`.

`674F0` tests player position minus saved position, then a copied tether position
minus saved position. Both use the same box: absolute Y at most 31, X from -64 to
160, and signed Z from -58 to 160. The player path requires a negative signed
player index at `4F3`; it stores the slot index there, sets slot flag `10` and
collider flag `4`, caches player counter `4F4`, and emits event `(2,1,[6])` plus
sound `115`. The tether path requires state one and parameter less than -1,
sets the parameter to the slot index, sets slot flag `40`, and plays the same
sound. The reconciliation helper resets an unused player index to -1 and an
unused tether parameter to -1; preserve that distinction from the activation
predicate's `< -1` test.

Departure (`67438`) ensures visibility, clears attachment flags at frame 18,
selects one of three blink periods from `08605B40` for frames 0–40, 41–60, and
61 onward, and releases the slot after frame 90. Release removes the collider,
disables the sprite, clears flags and the used bit, and sets index to -1.

The positioned constructor (`67680`) consumes an input direction byte at `9`
and an aligned position at `C`. `67658` maps `((direction+60)>>6)&3` into a folded
quarter and flip flag; this constructor uses the flip output and ignores the
folded output. The script constructor (`67990`) instead reads XYZ operands after
keyword `70`, defaulting each absent operand to zero, and reads flip from keyword
`72` with default zero. Both initialize the same sprite/sequence 1, step its
animation once, set priority 1, resolve the saved position's scene, and initialize
the collider. Their animation decoder stores frame weights and clamps a computed
zero duration to one; the particle decoder below does neither.

The old `67984` assembly symbol retained unrelated executable bytes after its
12-byte singleton reset. They contain three separately reviewed functions:

| Entry | Full extent | Evidence |
| --- | ---: | --- |
| `67990` | 536 B | independent constructor prologue, calls, return, and literal pools |
| `67BA8` | 100 B | independent request-all loop and return |
| `67C0C` | 4 B | separate empty `bx lr` leaf plus alignment |

The request-all helper visits all used slots and, when index is nonnegative and
kind is not 2, sets departure flag `2` and collider flag `4`. The new reviewed
boundaries are recorded in the inventory (`symbols/progress_functions.csv`); the whole tail
is never credited as part of the old 12-byte reset.

## Particle pools

Both pools use `0x28` effect records with flags at `0`, tile at `8`, dimensions
at `A/B`, priority at `F`, and an aligned position at `18`. Their separate
`DisplaySequenceState` arrays have stride `10`. An effect's tile is the packed
command's frame index times `(width>>3)*(height>>3)`, plus the graphics resource's
halfword at `4`. The decoder applies packed flip bits, advances elapsed/index
state in forward or reverse mode, and respects the end/hold flag. It computes
duration directly from the command's low four bits and sequence speed; it does
not store `frameWeight` or apply the actor decoder's minimum-duration clamp.
Completion sets slot state 2, and each non-delayed update increments its frame.

The 32-slot burst pool is a `0xA28` task (list 9). Effects begin at `20`, sequence
states at `520`, used mask at `720`, an initialization mode at `724`, and `0x18`
slot records at `728`. The slot contains position `0`, velocity `8`, flags `10`,
state `12`, signed index `13`, frame `14`, and signed delay `16`. Its updater
adds velocity to position, damps each signed component by `7/10` using signed
division, and copies position to the effect. The allocator scans 32 bits; release
removes the effect, clears state and the used bit, clears all `18` record bytes,
and restores signed index -1. The destructor removes used effects and clears the
global pointer. Resources are animation ID `1C1A` and lookup `(922E,D1B8)`.

The burst dispatcher pauses flag-bit-0 slots when either `030053F4|030053F0` has
bit `200`, or `030053F4` has bit `800` while `0300523C & 7` is nonzero. Other
slots continue. Wrapper `68108` passes flag 1 (pause-sensitive), and `68124`
passes flag 0 (continues during pause).

The burst spawner (`67E3C`, still a private nonmatching candidate at this snapshot)
accepts position, optional velocity range, required position-jitter range, count,
priority, flip flags, and slot flags. It may create the singleton even for a
nonpositive count. It returns -1 for a null position or allocation failure;
earlier particles survive a later allocation failure. Nonzero jitter components
consume RNG values modulo twice the range, then subtract the range. Optional
velocity components consume values modulo the component range, with zero
components explicitly zeroed. The shared RNG increments `03005308` modulo 1024
before reading table `0203B400`. Delay uses modulo 16. Animation sequence is 14
when game-state `868 & 8`, otherwise 13; flip combines the argument with a random
modulo-2 bit. Effect flags gain `201`, and the slot becomes state 1.

The eight-slot anchored pool has effects at `20`, sequence states at `160`, used
mask at `1E0`, and `0x1C` records at `1E4`. A record contains an anchor pointer at
`0`, offset at `4`, a velocity-shaped vector at `C`, state `16`, signed index
`17`, frame `18`, and signed delay `1A`. The updater recomputes effect position as
anchor plus offset. It does not consume the vector at `C`, although the following
spawner initializes that storage. This is an observed gap, not a claimed motion
meaning. Allocation and release mirror the larger pool using eight mask bits and
`1C` bytes per record. Wrappers `684D0/684E8` forward to the following spawner;
`684E8` fixes priority to 2. Both now explicitly return its signed result, removing
the old default-priority wrapper's C fallthrough while preserving its 24 bytes.

Both spawners call `08217EC4(effect,-8,-8)`: the callee writes separate bytes at
`C/D`. Its third argument must be preserved; a two-argument declaration hid part
of the contract and produced misleading register-allocation differences.

## Validation and remaining work

The current committed cohort contains 33 exact functions across 4,332 reviewed
bytes: 3,164 net new exact bytes, approximately **+0.1310 percentage points**, and
1,168 previously credited bytes reviewed explicitly or for types/names. The
640-byte tail formerly credited under `67984` receives no second byte credit.
New ordinary C contributes 684 bytes; register-pinning concessions contribute
2,480 new bytes. The recovered 536/100-byte bodies are now classified as
register-pinned, while their preceding reset retains only its 12-byte extent.
Every affected full extent passed `tools/check.py --target-size`; the private
summary is `build/control-pools-final.json`. The integrator performs the combined
build and name promotion before merging. Worker commits do not claim ROM-wide
validation or runtime coverage.

The only remaining body in this assigned range is `67E3C` (716 bytes), currently
89.5% fuzzy with no exact credit. Its private source and bounded permuter search
are retained separately. All other bytes through `68500` have been reviewed.

Local hints were minimized with `tools/depin.py`. Manual semantic review remains
necessary: the minimizer can remove an empty-asm definition while retaining its
input use. `674F0` deliberately retains a defining output for its compiler-only
scratch token; the token is consumed only by another empty asm and never by C
arithmetic, a branch, an address, or a store. No instruction bodies or explicit
`r7` bindings were introduced. Permuter mutations that made two simultaneously
live values share a pinned register were rejected even when their byte score
improved.
