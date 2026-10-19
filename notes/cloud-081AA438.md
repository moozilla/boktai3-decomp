# Cloud task 081AA438 (Vanargand phase scripts)

Matched: sub_081AA438 (Vanargand_UpdateDescendTrack), sub_081AAE10 (Vanargand_UpdateLeapTrack),
sub_081AB50C (Vanargand_RunDefeatSequence). Draft on main: sub_081AB078 (88.0%).

## sub_081AA788 (Vanargand_RunProjectileVolley), 75.0%, below the draft bar

`cloud-081AA438/sub_081AA788.c` is the best attempt (same instruction count as the ROM, 714
vs 715). Shapes that mattered:

* Origin fill: `setOrigin(s16 *origin, s32 x, s32 y, s32 z)` with the `s16 *q = origin` copy;
  s32 parameters keep the ROM's `ldrsh` loads (s16 parameters narrow them to `ldrh`).
* Random values: the `offset = index * 2; *(u16 *)(offset + (u32)table)` inline gives the
  ROM's late table load and `asrs` shifts; the firing helpers take the table as a local.
* The volley is an inline `fireVolley(p, obj, position)` whose `self = obj` copy is what the
  ROM spills to sp+0x30; `p` stays the outer register. Target and aim are evaluated before
  the heading (they are the first inline parameters).
* `heading + offset` needs `offset` in its own statement, or fold reassociates the `- 16`.

Still different: the half-select copy lands at sp+0x24 instead of sp+0x1C (after the
descent origin), the position spill slot, and p/q in r5/r8 instead of r6/sb. Pinning p/q
made it worse (52-67%).
