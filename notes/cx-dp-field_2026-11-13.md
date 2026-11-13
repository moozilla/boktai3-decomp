# Field concession removal, 2026-11-13

Tags: #matching #phase3 #field

## Scene_SpawnDirectionalDust (08004B64)

Matched without concessions: function score 74.3% -> 100%, a 25.7 percentage-point gain
(384 bytes). The complete `src/scene_object.c` unit passes `tools/check.py`.

The old packed-word coordinate updates were retained: x and z are inserted into their
32-bit position words while retaining the upper half. Ordinary halfword fields narrowed
these to `strh`, which is not the ROM. The private view is now named `DustPoint`.

Three interacting source forms decided the allocation:

* Each direction has a first index and a separate final index. Reusing one index through
  the pointer arithmetic made it a global-allocation pseudo and let the sample take r2
  first. Separate indices let local allocation retain the first index while its sample
  takes r3; the final index and its sample reuse r2.
* The edge helpers initialize a `DustJitterMask` aggregate. Its clobber and set keep 15
  inside the loop (the same mechanism as scope fact 93). A scalar mask was hoisted on
  loop.c's second pass and occupied the actor's high register, spilling the actor.
* The repeated sample-address capture is a statement macro, like the existing sample
  capture macros. Using it for the table base as well as the eight draw addresses raises
  the table's reference density enough to precede the secondary point pointer. Without
  the base capture these two merely swap r4/r5: 14 ROM / 14 draft instructions differ,
  with identical instruction shape. `AGBCC_PRI=table:+97` proves that remaining decision.
  This is an allocation-sensitive macro/aggregate form and remains a phase-4 cleanup
  opportunity; no register pins, barriers, compiler settings, or single-use macros remain.

The intermediate shared scalar mask draft was 66 ROM / 60 draft instructions off.
`AGBCC_PRI=table:+2000` plus local sample allocations 67:r2, 90:r3, 144:r2,
167:r3 matched it exactly, establishing the allocation diagnosis before the C rewrite.
Simple loop reversal, pointer-vs-index arithmetic, signed/narrow helper arguments, local
actor unions, table half construction and scalar reuse were also tested. The persistent
search is `build/matchsearch/Scene_SpawnDirectionalDust`; scratch evidence is under
`build/cx/work/Scene_SpawnDirectionalDust/`.
