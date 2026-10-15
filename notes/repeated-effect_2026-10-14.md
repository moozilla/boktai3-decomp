# Contiguous repeated effect cohort (Sol, 2026-10-14)

New exact progress: **+0.0616 percentage points** (eight functions, 1,488 bytes),
relative to worker base 36ec8e7c. Of these, 684 bytes use no hints and 804 retain
minimized register hints or empty barriers. No explicit r7 binding or instruction
bodies. All reviewed extents pass fast checks; combined ROM validation belongs to
the integrator. No execution/visual identity claims.

The complete **0804F6C4–0805002C** cohort is now exact: 2,408 bytes, including
920 previously exact bytes. The worker assignment ends before 5002C. Names use
`RepeatedEffect` provisionally; the lower/upper state behavior suggests an
intensity-responsive object, but its gameplay identity is unconfirmed.

## Layout and interfaces

`include/repeated_effect.h` recovers manager size 0x7FC: task prefix 0x18,
count halfword at 0x18, entries at 0x1C, eight entries of 0xFC bytes. InitEntry
uses `entries[count]` without incrementing count. The adjacent script creator
owns the capacity check and increment. Its seven arguments are manager, tag,
position pointer (two packed words), initial state, bodyEnabled, behaviorFlags,
and constrainCell. Scalar parameters retain word-sized ABI arguments; the
initializers consume their low byte/halfword where stored.

Each entry has a 0x2C-byte sprite, graphics at 0x2C, body at 0x48 (0x48 bytes),
and collider at 0x90 (0x54 bytes). Sprite animation frame is halfword 0x10 and
position XYZ starts at 0x1C. Tail fields: state E4, behaviorFlags E5, request E7,
tag E8, transitionHold EA, bodyRegistered EC, flashing ED, constrainCell EE,
constrained EF, cached cell F0, signed balance F4, state frame F6, and callback
F8. `constrained` is a tentative label for the byte tested and cleared by the
cell gate; no writer setting it nonzero appears in this cohort.

SetState writes state/callback and clears balance/state frame. Entry callbacks
accept the entry pointer and return void. Manager task callbacks return zero.
The collision callback accepts (contact collider, other collider, entry).
InitParameters clears flashing ED and request E7, initializes transitionHold,
and optionally records CellQuery(position). It does not clear EF itself.

InitSprite selects graphics resource A680 and tile selection 0x62, render mode
3, and copies both position words. Optional body registration passes tag,
position, 0, 7, and the entry pointer. Collider init uses flags 0x4003/type0x10,
size (128,50,128), zero offset, callback/position registration, and value600.

## Behavior

RequestNearby scans in order for state0 with entry Y >= supplied Y and an XZ
radius <=128 (axis checks then squared distance <=16384). It sets request=1
and returns1; a null manager or no entry returns0. GetStateFromScript seeks
keyword n/tag and returns the matching state, otherwise -1.

Collision response bit4 gates contact handling, and the other collider's
cooldown must be zero. Contact bit4 sends state0 to upper transition/state2,
or state1 to lower recovery/state0, then sets a 30-frame cooldown. Bit8 sends
state0 to lower transition/state1 and sets the same cooldown. Other contacts
request neutral flashing while state0.

Neutral flashing normalizes an existing frame counter, then alternates sprite
frames1/2 with 8-frame periods through63 and 16-frame periods through95.
Past95 it clears flashing and frame count. GameState+876 signed intensity0
moves balance downward; intensity>7 moves it upward; intermediate values do
nothing. Flags2/1 clamp the respective motion at zero; otherwise crossing
-300/+300 starts the respective state1/state2 transition. Signed wrap/truncation
and strict comparisons are preserved.

Lower transition uses frames3/4/5, Sound_PlaySong19F at start, then lower state.
Lower state increments balance only when intensity>0; balance>299 enters
recovery frames4/3/2, playing1A0 at both ends and returning to neutral.
Upper transition plays15C at start and uses frames6/7/8. It emits particles
(type1, args0/1/64) every fourth frame, perturbing X/Z by the low bytes of two
successive random-table halfwords minus127. Its second phase increments the
frame counter a second time; both local position copies and this extra increment
are preserved. Upper state advances only with GameState+868 bit1000 and returns
through frames7/6/2 when balance>299.

The cell gate dispatches normally with constraint disabled, or while cached
cell equals GameState+5A0. In the matching-cell branch a zero EF clears collider
bit4. When outside the cell and EF is nonzero it resets the state callback,
sets collider bit4, and clears EF. Original state1 specifically calls SetState
with state2 and the lower-state callback; this unusual pairing is retained.
Nonzero cooldown decrements after the gate, including when no callback runs.

Destruction reloads manager count on each loop, removes optional registered
bodies, sprites, and colliders, clears the singleton, and returns0. Creation
allocates task priority9/size7FC and installs manager tick/destruction callbacks.

## Reviewed extents

New: F6D0=164, F7E8=160, F9C4=328, FB48=316, FC84=236, FD70=68,
FDB4=108, FF70=108. Pinned/concession category: F7E8/F9C4/FB48.

Existing helpers named and rechecked: F6C4=12 (old_agbcc), F774=92,
F7D0=24, F888=68, F8CC=80, F91C=84, F970=84, FB0C=60, FE20=136,
FEA8=84, FEFC=76, FF48=40, FFDC=16, FFEC=64. Naming/type work adds no byte credit.

No gaps or guarded drafts remain in this cohort. Across all five completed
worker subsections: 35 new exact functions, **+0.2734 percentage points**
(6,604 bytes: 2,520 without hints and 4,084 concession bytes).
