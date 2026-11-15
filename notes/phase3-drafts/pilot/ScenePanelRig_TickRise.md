# ScenePanelRig_TickRise (08059AC4): holdout

Tags: #matching #phase3

The landed match uses `asm("" : "+r"(counter), "+r"(frame) : : "memory")` so that cse forgets
`stateFrame` after the increment. The ROM increments in SImode on the zero-extended load
(`ldrh r0; adds r1, r0, #1; strh r1`), compares the new value twice (`lsls/lsrs r1` before
each `cmp`) and reloads `stateFrame` (`ldrh r0, [r3]`) for the arithmetic in both `> 7` and
`<= 7` arms. Every plain spelling tried keeps the old value live and cse folds
`(stateFrame - 8)` into `old - 7` (`why 242`: src_folded through the path 101T 223N).

* Best draft `ScenePanelRig_TickRise.c` (`u16 frame = ++rig->stateFrame`, compares on frame,
  arithmetic on `rig->stateFrame`): 18 / 12. With `CSE_NOFOLLOW=2` (cse does not follow the
  `frame > 7` jump into the `<= 7` arm) it is 11 / 9; the `> 7` arm still folds.
* Tried: `++` in the condition (17/13), `x = stateFrame + 1; stateFrame = x` with u16, u32, s32
  and `& 0xFFFF` compares (18-27), `(s16)` reads in the arms (no change).
* The sibling ScenePanelRig_TickReturn (`++rig->stateFrame > 15`, then `16 - rig->stateFrame`)
  does reload in its matched C: the difference is the second compare on the new value.
