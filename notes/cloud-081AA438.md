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

## sub_081ABA64 (Vanargand_RunDebugSlam), 66% fuzzy, below the draft bar

`cloud-081AA438/sub_081ABA64.c` is the best attempt: 1744 instructions vs 1738, 145
differing instructions with registers masked. All 11 phases are decoded. The inlined
animation step is the same as Actor_MoveAlongScriptedCurve's (src/actor_animation_controller.c).
Shapes that mattered:

* The direction helper takes a packed u32 (`*(u32 *)&v = packed` into a local 8-byte
  struct), so each call gets its own stack slot (sp+0x14/0x1C/0x24/0x34/0x3C). A constructor
  expression folds the arithmetic away instead.
* The phase-6 origin must be a struct written through a pointer, or it takes slot 0x14
  ahead of the directions.
* The sprite struct overlaps the anim struct: the sequence is read as sprite+0x74 while
  the anim pointer is body+0x10C.
* Pinning `input` to r0 in the first flag test reproduces the phase-3 reuse of the timer
  register (known 1) as the mask, but adds instructions elsewhere.

Still different: cross-jumping (the ROM shares the phase 4/9 `timer++` tails, the
`a->timer = a->rate` store at 0x081AC706 and the advance tail at 0x081AC91A); p/q/body sit
in r8/sb/r7 in the ROM and r7/r8/r6 here (pinning made it worse); phase-1 polar store order.
