# Motion callback family at 0818D754

October 12, 2026, solo investigation. This is a static, address-qualified
subsystem slice, not identification of a particular character, attack or effect.
No new emulator observation or semantic function rename is claimed.

## Entry and callback chain

The already exact dispatcher `0818E574` checks bit 4 at object +119, decrements
its countdown when nonzero, calls `gUnk_0824F158[state]`, invokes callback +1D0,
calls `0818E4BC`, then invokes callback +1CC if present. This establishes how
these callbacks participate in the surrounding object's update, without
identifying the object from screen coverage.

The exact `0818E244` installs `0818D754` after its reset helper succeeds.
The following callback assignments and operations are directly in exact C:

| Function | Observed role | Transition |
|---|---|---|
| 0818D754 | Capture current position/owner-relative offsets; calculate an endpoint around the global object's position with table-scaled X/Z offsets and Y+220; reset timer/spin | Installs D8E0 |
| 0818D8E0 | Decrease signed spin when owner +5C2 low two bits are clear; clamp magnitude to 15; apply spin; manage signed timer | Installs D960 once timer is negative and spin magnitude is 15 |
| 0818D960 | Interpolate three coordinates over divisor 100; advance timer | After timer exceeds 100, installs DB10 and captures new start/end/delta and owner-position snapshots |
| 0818DB10 | Add owner displacement since the snapshot to another interpolated segment; advance timer; write completion/status fields | No new callback in this function |

DB10's role is supported by disassembly and the candidate, not accepted C.
The assembler body and literal pool occupy [0818DB10,0818DC50), 320 bytes.
A surrounding handler also compares +1D0 against D8E0, D960 and DB10 before
handling state transitions (see code around 0818C7B4). This strengthens the
family relationship; it does not establish an exclusive owner or actor name.

## Shared layout and phase-dependent fields

`include/motion_0818.h` is shared by D754, D8E0 and D960, with unchanged exact
objects (396, 128 and 432 bytes). Offsets refer to the 32-bit GBA ABI.

| Offset | Evidence-backed interpretation |
|---|---|
| 006 | Wrapping orientation byte updated by spin |
| 01C/01E/020 | Current three-coordinate position |
| 119 | Flags; dispatcher tests bit 4, finishing stage sets bit 0 |
| 11E/11F | Status/state bytes; precise enum still unresolved |
| 134/13C/144 | Segment endpoint / start / signed delta vectors, each with 8-byte stride |
| 14C | Initial owner-relative position difference |
| 154 | Reused scratch: correction vector, then owner-position snapshot |
| 15C | Owner pointer; these routines read its position at +5AC/+5AE/+5B0 |
| 1D0 | Motion-stage callback |
| 1D6 | Signed 16-bit timer |
| 1DB | Written to 1 on finishing; named `done` provisionally |
| 1DD | Signed spin increment in D8E0; adding it to an 8-bit orientation wraps |

Keep `a/b/c` as phase-dependent scratch rather than naming them permanently
as one semantic vector. The header deliberately retains padding and uncertain
status names. Whole-object equivalence with other nearby families is unproven.

## Candidate improvement and limits

The retained DB10 draft used signed bitfields plus u16 pointer casts to read
X/Z. Expressing X/Z as unsigned 16-bit bitfields and Y as signed removes those
casts and changes register lifetimes. The resulting 324-byte candidate matches
the motion calculations through the timer branch; its reviewed 320-byte
comparison has 32 differing bytes, versus 155 for the retained source.
These are raw byte differences including literal-pool/layout effects, not a
percentage of semantic correctness or an estimate of remaining effort.

The sibling `080F5B68` uses a target-position pointer at +15C, different owner/
callback layout and a signed 32-bit timer. Its body/pool ends at 080F5C9C
(308 bytes). Applying the same vector representation matches the calculation
prefix; finishing writes/callback setup remain different. Do not impose the
0818 layout on it. The retained field named callback224 really is at +224;
a trial increasing its padding was a mistaken hypothesis and is discarded.

Mask-local, operand-commutation, inline flag-helper, signed flag-byte and
one-bit flag-field experiments did not solve DB10's finishing writes. Avoid
repeating them without new compiler evidence. No exact new functions are
credited in this pass. Source snapshots and machine-readable checks are local
under build/candidates; selected sources and concise attempt metadata are
preserved separately on codex/wip-motion-2026-10-12.

## Next bounded questions

1. Inspect compiler RTL for the duplicated constant 1 in the finishing writes;
   determine which conversion/lifetime causes it before more source shuffling.
2. Review enum/flag access idioms in exact callers using +119/+11E/+11F.
3. Follow the state-table entry and constructor to identify an observable game
   context; then design a targeted replay. Until then keep address-qualified names.

The useful result of this slice is shared layout, explicit callback/phase
relationships and better retained candidates. Existing exact byte coverage is
unchanged. Tooling and understanding are reported separately from matching.
