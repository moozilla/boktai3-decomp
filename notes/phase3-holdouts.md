# Phase 3 holdouts

One section per worker. Each entry: the best pin-free draft, what blocks it, the knob that
matches (when one does) and the next idea.

## field-effects

### Scene_SpawnDirectionalDust (src/scene_object.c, 08004B64): deferred

Draft: `notes/phase3-drafts/field-effects/Scene_SpawnDirectionalDust.c` (117 ROM / 93 draft
insns differ; it relies on a local `u32 x:16` bitfield struct defined next to it in the unit copy).
The ROM writes the particle's x and z halves as SImode read-modify-write (`ldr; and #0xFFFF0000;
orr; str`), z through the particle pointer in r5, not through the stack. Probes:
* a struct of u16 fields gives `strh`;
* a `u32 x:16` bitfield struct gives the SImode RMW only while the struct is ADDRESSOF (its address
  is never stored in a pointer variable); once `q = &pt` exists the store becomes `strh`;
* `pt.x = pt.x op ...` written directly also narrows to `strh`; `t = ...; pt.x = t;` keeps the RMW.
The pinned version gets z's RMW from a union of words plus FIRST/LAST macros. Next idea: find
the struct the particle really has at that offset (a packed `u32` position word written whole by
its siblings) and recover it in `include/`.

### Scene_TryStepDown (src/scene_object.c, 08004CE4): deferred

Draft: `notes/phase3-drafts/field-effects/Scene_TryStepDown.c` (42 ROM / 40 draft insns differ,
allocation only). Fixed so far: parameters used directly (no `actor = input` copy) and the
sibling-style `neighbor = index + delta; dynamic = TerrainModifiers_FindCellOverride(neighbor, 1);
savedIndex = neighbor;` put actor in r5 and index in r4. Remaining: the ROM gives mask (0xF0) r4,
cell r6, type r7, direction r8; the draft gives mask r8 (pri 1875, 6 refs / 64), cell r4 (3243).
`AGBCC_PRI=mask:+1400` cuts it to 17/19 but does not match: the low-register choice in the
`heightByte`/`lowMask` block (ROM ldrb r1, movs r0 #0xf; draft r0/r1) and a `regmove` copy at
line 773 remain. The draft still has statement expressions and plain `register` locals from the
pinned form; strip them once the shape is right. The ROM's dead `subs r0, r7, #1` tail comes from
`type == 1 || type == 2` with both arms returning 0 (fact 57-style cross-jump).
