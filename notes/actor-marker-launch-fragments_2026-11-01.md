# ActorMarker_LaunchFragments (08181514)

Tags: #matching #actor-marker #compiler

Matched all 288 bytes with stock agbcc and no concessions.

The offset table has two rows of nine signed, four-halfword records. Reading
its named const fields directly lets CSE retain the x and z displacements
across the stores to the particle, through to their velocity shifts. Writing
each position as origin plus table field lets combine fold the origin memory
operand into the add. Reload then produces the low-register copies of all three
origins and spills the z sum at sp+8.

A flat index selected before the loop gets the whole loop right, but computes
the row sum before the three table-address loads. Computing the entry index as
the difference between the selected row's current pointer and the table base
places that sum after the loads. All pointer arithmetic is within the same
18-record array.
