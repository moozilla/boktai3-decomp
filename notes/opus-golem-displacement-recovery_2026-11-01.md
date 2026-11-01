# Golem_ResumeBehaviorAfterDisplacement (080C8774): matched in ordinary C

Tags: #matching #golem #random-table #reg-equiv #pre #stack-slots #gcse

**Matched.** `Golem_ResumeBehaviorAfterDisplacement` (4304 bytes) is
byte-identical in ordinary C (commit `09082f98a`, PR 936). It has no register
pins, barriers, selector macros or compiler overrides. The unit keeps local
Golem variants of the shared chase step (`Golem_Arrive`, `Golem_BeginTurn`,
`Golem_CellIndex`, `Golem_CellCenter`, `Golem_SaveControl`) and Dog-style
displacement helpers (from the matched 080E0750) under a `Golem_Drift` inline.

## What decided it

1. **The random table has no equivalence.** In `Golem_Arrive`,
   `const u16 *randomSamples = gRandomTable;` is set at the top, many blocks
   before the turn, and `Golem_BeginTurn` reads it through the shared
   `SceneActorChase_RandomDirectionExcept`, which copies it into a
   `struct SceneActorChaseRandomTable`. The copy that loop hoists has no
   REG_EQUAL note and so no REG_EQUIV. Global alloc gives it r3. Reload then
   evicts it for the 0x3FF mask's reload register (`ldr r3, =0x3FF; mov r8, r3`),
   which gives the table the frame's last slot (sp+0xa4, frame 0xa8), as in the
   ROM. With the pool load as the table's own set, the pseudo gets REG_EQUAL,
   then REG_EQUIV: its life doubles, its priority falls, and the frame and the
   random loop's registers differ.
2. **The zeros come from the shared SetControl and SetState.** Byte and halfword
   field stores expand as bitfield stores (`(and (mem) 0) | value`), and cse
   reuses their zero pseudos for later zero stores. Only the shared
   `SceneActorChase_SetControl` / `SceneActorChase_SetState` give the zero
   pseudos the ROM's allocation order; the unit's earlier local copies did not.
3. **PRE's slot order needs T 853 and later base pseudos.** With items 1 and 2
   in place, the drafts differed only in SP displacements, 4 to 32 slots
   permuted. PRE makes its field-address pseudos in gcse hash-table order,
   (bucket, index), with bucket (13772 + base + offset) % T and
   T = max(11, N // 2) | 1 (fact 29 in `tools/scope/README.md`). The draft with
   only the WIDE_MAP_WIDTH selector (`sel2/v0000010`) had N 1696, T 849 and three
   pairs out of order; no table size alone gave
   the ROM's order. The match has N 1705, T 853, and its bases from `actor#7` on
   are 38 to 51 pseudos later than that draft's. The ordinary-C changes that
   moved them:
   * the Golem variants: cells looked up from scalar axes, the map width read at
     full width, the saved control written directly. These are the shared
     header's SCALAR_CELLS, WIDE_MAP_WIDTH and DIRECT_SAVED_CONTROL selectors
     written as local inlines: with those three selectors, the shared
     `SceneActorChase_Arrive` and the changes below, the function also matches
     (`build/scope/drafts/golem/MATCH_h7_selectors.c`), but selector macros are
     not allowed, so the landed unit has the local versions;
   * the Dog-style displacement helpers (`Golem_ReadDisplacementAxis`,
     `Golem_QuarterDisplacement`, `Golem_ClampAxis`, `Golem_DisplacementStopped`):
     about +30 pseudos;
   * the `Golem_Drift` inline around the displacement step;
   * `SceneActorChase_MotionFlags` for the 0x80 test.

`scope.py FILE.c preorder` (commit `a7641496b`) now does the search in point 3.
On the N 1696 draft it reports a cut before `actor#7` of +35..+55 at T 851 or
+43..+57 at T 853.

## Ruled out

* The header's selector combinations alone (128 compiled, `build/scope/drafts/golem/sel2`):
  the best, SCALAR_CELLS + DIRECT_SAVED_CONTROL + WIDE_MAP_WIDTH (N 1702, T 851),
  left 4 slots permuted. None matched, and the landed unit uses none of them.
* The Dog helpers with the shared arrive and no Drift inline (d1, d5), a
  Skeleton-style local arrive (d2, d3), and the Drift inline without the selector
  choices (f1, f2), with the e1-e6 and h1-h9 variants in
  `build/scope/drafts/golem/`. Each fixed part of the slot order and moved other
  slots.
* Reading the random table straight from `gRandomTable` in the turn loop: that
  table pseudo has REG_EQUIV, and the frame and the loop's registers differ.
