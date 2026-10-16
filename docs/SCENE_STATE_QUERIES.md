# Scene state, spatial queries and pair records (tentative)

This cohort continues the grid helpers toward the scene actor registry. The
180-byte reset at `0806EF04` ends before an independent 180-byte rectangle
query at `0806EFB8`. The split introduces no extra byte credit. Reset calls the
scene subsystem reset helpers and clears the scene singleton at `030004B4`.

`SceneActorQuery_FindInRectangle` asks the registry for at most twenty candidates,
advancing the caller's ID cursor before filtering each returned actor. It rejects
flag `2D4 & 40`, flag `2D0 & 20000`, and nonpositive health. X/Z comparisons
include both ends of the rectangle described by minimum and size. The adjacent
212-byte XYZ query at `0806F06C` remains assembly-backed: its cursor behavior
also differs when `2D0 & 400` is set. Do not fold the two predicates together.
`SceneActorQuery_FindPlayerInBounds` rejects absent players, slot flag 8, and
players lacking status `4000`, then tests inclusive center/radius X/Z bounds.
These names describe static code interpretation, not observed execution.

The independent leaves at `0806F1B0` and `0806F1D0` obtain a wrapped tilemap cell
and fill rows of a rectangle. Layer records are `0x30` bytes with the map at
`+0x2C`; the starting column and row wrap with `&31`. Each filled row writes
consecutive halfwords after resolving its starting cell; the inner span does
not wrap each individual column. Nonpositive width or height performs no writes.
The counter updater at `0806F218` preserves signed comparisons and the original
caps at 9999, including the separately clamped increment of field `592`.

The six-byte pair records at game-state `+670` contain key, ID and a provisional
counter named ticks. Twenty records occupy 120 bytes; the clear helper clears
128 bytes, including eight bytes whose use remains unknown. ID zero marks a
free record. Pair lookup compares incoming word arguments directly to the
loaded halfwords, while insertion first narrows its inputs to halfwords.
Insertion checks for an existing pair, a free slot and the registry manager,
then copies manager byte `30` into the counter. That byte is initialized to 3;
its unit is not established. When game-state flag `868 & 4` is set, occupied
records decrement the halfword counter and clear only the ID upon reaching
zero. An initial zero wraps to `FFFF`; it must not be clamped. The capacity
query only reports availability and does not reserve an entry.

The new exact functions in this document total 972 bytes. The reset, tilemap
leaves, counter updater and insertion are ordinary C; rectangle/player queries
and pair lookup/ticking retain initialized register-lifetime concessions.
The old clear, empty-slot and capacity helpers are typed without new credit.

## Runtime field reset:0806F2CC

The full220-byte reset now matches ordinary C, adding **+0.0091 percentage
points**. It clears halfwords7A0/7A2/7A4, writes-1 to7A6, clears81A and
7B2/7B4/7B6, then clears7FA/7FC/7FE. It writes2000 to818,1 to each word
7B8/7BC/7C0,0 to word7C4,1 to halfword7F8, and0 to word81C. Calls811D358,
811DB6C and811D54C follow, then word820 is cleared. The field roles remain
provisional; offsets describe observations rather than inferred gameplay.

A four-hour frozen permuter search improved weighted score805→210 by applying
an ordinary inline halfword-address helper to the clear stores. The reviewed
result had only two differing instruction bytes. Auditing the cached address
arithmetic exposed three transcribed offsets in the private baseline:800/804/800
should have been7FA/7FE/7F8. Correcting those source offsets produced a full exact
match with the helper; the mutations had not introduced the transcription error.
The obsolete search was stopped. No register hints, empty asm, unfinished scalar
reads, or raw mutation output were promoted. The final fast proof is
build/scene-state-F2CC-final.json; parent combined validation owns integration.
