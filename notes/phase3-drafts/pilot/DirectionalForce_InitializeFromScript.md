# DirectionalForce_InitializeFromScript (080513B8): holdout

Draft: `DirectionalForce_InitializeFromScript.c` (no concessions; put the original's
`#include` lines on top). Off by a single swap: table r6/mask r4 instead of r4/r6.

* Blocker: the table pseudo (`table = gRandomTable`) gets a REG_EQUIV note
  (update_equiv_regs, fact 20), which doubles its live length: priority 625 vs mask 882.
  `AGBCC_NOEQUIV=table` -> MATCH; `AGBCC_PRI=table:+258` also matches.
* The mask is loaded inside the first sample (`index = *seed + 1; mask = 0x3FF;
  index &= mask;`) as in ScenePanelRig_RandomizeMovement; this fixed the ldr order.
* Tried against the equivalence: slim-1's halves union (ParticleRandomTableSource)
  in case 1 does not fold here. Its pseudo reads its own uninitialized value, so it is
  live from function entry across four calls, and cse makes it canonical over table
  (`CSE_NOCANON=<union pseudo>` -> 0 ROM / 4 draft, a stray r8 push). Union at the top
  of the function, union with `.table = 0` first, designated initializer, and
  RandomTableView: all keep the equivalence or are worse. Setting table after
  `*seed = index` shortens its life enough for the right registers, but moves the ldr.
* Expected construct: something that sets the table pointer from a non-constant
  first in insn order (fact 20), or a copy from a pseudo that combine (not cse) folds
  to the constant, with the copy before the gRandomIndex load.
