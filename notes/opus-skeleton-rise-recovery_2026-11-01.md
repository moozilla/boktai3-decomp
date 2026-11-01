# sub_080BF0E4 (Skeleton rise and recovery): holdout, allocation order

Tags: #matching #skeleton #global-alloc #priority #pre #stack-slots #holdout

**Not matched.** `sub_080BF0E4` (3040 bytes) stays `INCLUDE_ASM`. The best
ordinary-C draft is in the unit's `#ifdef NONMATCHING` block: fuzzy 74.6% (the
unit's earlier draft was 72.5%), scope 344 ROM / 345 draft instructions
differing, shape 71 (the earlier draft: 381 / 370, shape 241, 12 bytes short). It has no register pins or
inline assembly. It has one `DEBUG_NOP()` per directional expansion (see
below). With the what-if `AGBCC_PRI=cell:+90` the same draft is 102 / 103,
shape 67. The matchsearch store is `build/matchsearch/opus_080BF0E4` (node
`best-t1` is the landed draft; `flip-*` are the order-flipping variants).

## What the draft settles

* The arrival tail is `Skeleton_ArriveAfterDelay` from the matched neighbor
  080BFCC4. Every relative's version (080B9158, 080BBDD0, 080C09F0, 080C7E18,
  080D6608, 080D7568) scores the same or worse.
* The entry flag uses `SceneActorControl_TakeStateEntry`, as in 080BFCC4
  (shape 81 to 71).
* The completion uses `SceneActorChase_SelectState(actor, 0, 0, 12)` and then
  `SceneActorChase_ResetStateEntry`, as in the matched 080B9158. With
  `SetState(actor, 0, 0, 12)` (or `SetBehavior(actor, 12)`) the zero is a
  parameter copy at the inline's start. Here SelectState's readonly zeros are
  substituted and materialized at the first store, after the address, as in
  the ROM. The SUBREG copy of `behavior` still comes first.
* The three directional expansions are a block macro, `PLAY_DIRECTIONAL`. Each
  expansion has its own `mirror`/`variant` stack bytes (sp+4/5, 6/7, 8/9). As
  an inline function, the expansions share one temp slot and the frame drops
  to 0x5C.
* The macro has `DEBUG_NOP()` right after `entryFlags = entry->flags;`. That
  is the same statement where the matched `SceneActorAnimation_SelectDirectionalEntry`
  (08076034, same body as a standalone function) has its `DEBUG_NOP()`. Two
  separately compiled copies of one source need a cse split at the same
  statement. That is the evidence for a compiled-out macro in the original.
  Without it (q0), cat 6's sequence path cross-jumps differently, the frame
  becomes 0x5C, and the draft is 403 / 335.

## The remaining mechanisms, largest first

1. **Global allocation order, actor (pseudo 22) against cell (899).**
   * Actor: priority 4794 (97 refs, live 1214). Cell: 4705 (8 refs, live 51).
   * The ROM allocates cell first, giving cell r5 and actor r6. That one flip is
     about 240 of the 344 differing instructions.
   * Any of these would flip it: actor with 95 refs, or living 1237+ insns;
     cell with 9 refs, or living 50 or fewer.
   * With the knob, every instruction from the arrival tail to the end matches.
     So the ROM's source has the same final code, and the difference is in
     pre-combine refs or live length.
   * Ordinary C that flips the order changes the code elsewhere:
     * `group->common.gate.counter = group->common.gate.duration` (-2 actor
       refs, but loads `group` from its stack slot): 134 / 133, shape 97;
     * `restart = &actor->field2B3` set once before the `drawableKind` test:
       145 / 142, shape 127;
     * the final if/else swapped or written as a guard clause: 181 / 186,
       shape 145.
   * Seven actor mentions are in insns that combine or jump2 delete: three
     `drawableKind` loads, and cat 6's sequence-path tail and `field2B0` store,
     which are cross-jumped into cat 7's. The ROM cross-jumps the same tails,
     so its source has those copies as well.
2. **Directional locals in cat 5.**
   * The ROM gives quadrant r3, variantPointer r2 and the direction-byte
     address (pseudo 47, live across the category tests) r3.
   * The draft allocates quadrant (10000) first into r2. variantPointer (1764)
     then takes r3, and pseudo 47 (1449) takes r5.
3. **selection (1764) and restart (1333).**
   * The ROM has restart in r7 and selection in r8 (`movs r3, #0x63; mov r8, r3`).
   * In the draft, selection's live length is doubled by its constant first set
     (update_equiv_regs). It is allocated first and takes r7.
4. **Cat 7's mask.**
   * In the ROM the mask is r1: reload_cse reuses r0 (== sl) for `*flags`, and
     move2add derives 0x2B7 as `adds r1, #0xb7`.
   * The draft's mask is r0, so `mov r2, sl` and a literal load appear. The
     operand order of the AND has no effect (four spellings tried).
5. **PRE stack slots.**
   * The category address (`[r51]`, line 323) is at sp+0x34 instead of sp+0x24.
   * The BeginTurn address `(plus r769 706)` is at sp+0x44 instead of sp+0x54.
   * Both follow pseudo numbering and insn count (tools/scope/PRE_STACK_ORDER.md),
     so they come last.

## Ruled out

* The arrival helper variants of nine matched relatives, transplanted whole
  (348-351, none flips the order).
* 759 matchsearch moves on the function and its helpers (`build/scope/drafts/sk/mv`,
  scored with `scope.py try`). Only the final-if swap flips the order.
* The 080B9158 gate helpers (`Skeleton_GateFlags`, `Skeleton_EnableMotionFlags`,
  `Skeleton_RestartGate`): 367 / 365.
* `SetTypedControl` for the completion's control: no change.
* `selection` placed before the barrier or after the variant read (a1, a2):
  no change to the r7/r8 split.

Next: the actor/cell order needs an ordinary-C difference that removes two
pre-combine actor refs (or adds a cell ref) without moving any instruction.
Look first at forms whose extra insns combine merges, in the three category
blocks.
