# Phase 3 holdouts

Tags: #matching #phase3 #holdouts

One section per worker. Each row: the function, its best pin-free draft (a snippet to
`scope.py FILE.c put`), the differing instructions, the mechanism and the knob that matches,
what was tried, and the next idea.

## menus-link (Menus, HUD and dialogue; Link and wireless)

### StatusScreen_DrawClock (0800E5B8, src/status_screen.c)

Draft: `notes/phase3-menus-link/StatusScreen_DrawClock.c` (4 ROM / 4 draft insns).
* The pinned match keeps the digit-array address in a user variable `values` (pinned to ip).
  Without the pin it gets sb: two sets of `values = digits` each get a REG_EQUIV (the frame
  pointer is function-invariant), and update_equiv_regs (local-alloc.c) doubles the live length
  once per set, 48 -> 192, so its priority (625) falls below the hoisted `gBgRecords` constants
  (833), which take sb first; then pass 0 of find_reg picks sb over ip. Needs priority > 833
  (`AGBCC_PRI=values:+600`). The sibling NumericDisplay_DrawPair gets the same refs from an
  inline `do { row = 0; values = digits; } while (0);`, which the concession detector does not
  count (see the note to the coordinator).
* Indexing the digits (`int tile = digits[col] + 0x60;`) instead: loop.c's hoisted giv base
  lands in ip as in the ROM, and only the inner loop's shift and digit pointer swap r3/r6
  (shift 5 refs over 14 insns against the pointer's 5 over 13; `AGBCC_PRI=shift#2:+551`
  fixes the first loop). Tried: shift inline (hoisted by loop.c: right registers, but the
  `out = dst` copy then comes before it), `dst[col]` (giv order flips), `shift = row * 16`,
  the shift in the for init, operand orders of the add.
* Next: a form where shift lives two insns less (or the pointer two more) without moving the
  `out = dst` copy; or what made the original's `values` denser.
