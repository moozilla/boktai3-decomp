# SceneInventory_Populate{Equipment,Item,Consumable}Grid: item grid matched pin-free; a goto outer loop is needed

Tags: #loop-reduction #goto #reload-rotation #inventory #spilled-locals

**PopulateItemGrid (0805FF38) matches with no pins** (commit 3f28b7dd2,
`src/fn/sub_0805FF38.c`). The other two units still match only in their
pinned form. Best pin-free drafts:

| Unit | Best pin-free draft | Differing (ROM / draft) | Shape |
|---|---|---|---|
| PopulateItemGrid 0805FF38 | **MATCH** (plain spilled locals, `slot` as the hide-loop counter, goto outer loop) | 0 | 0 |
| PopulateConsumableGrid 080600DC | goto outer loop, plain locals | 67 / 67 | 68 |
| PopulateEquipmentGrid 0805FD20 | `do` / `while` loops with goto exits | 72 / 76 | 68 |

## What decided the item grid

* `nextRow`, `flagsBase` and `rowBytes` are plain locals that reload spills,
  declared in that order: spill slots follow pseudo order, so they land at
  sp+0, sp+4 and sp+8 as in the ROM. Any other declaration order swaps slots
  (6 to 10 differing). In memory (a local struct or `volatile`), CSE stores
  `row`'s register for `rowBytes = 0` (`str r7`) where the ROM loads a fresh
  zero. That was the pinned unit's `register u32 zero asm("r1")`.
* The closing hide loop counts with the function's `slot`
  (`for (slot = 0; slot < 8; slot++)`), not a counter of its own. A fresh
  `i` is set first from a constant, so update_equiv_regs doubles its live
  length (fact 20). It came out at priority 7777 and lost r1 to the entry
  pointer (27000). `slot`'s first set is `rowSlot + column`, so there is no
  doubling, and its refs from all three sections add up (17 refs over 16
  insns, 42500). This one change took the struct draft from 26 to 7
  differing, and with it the plain-locals form matches outright. The
  earlier plain-locals draft, with `i`, was 34 differing, its reload
  rotation one step off at `column = 0`; `scope.py spill` had already said
  that rotation alone did not explain it.
* The second section's loop can be `for (row = 0; row <= 1; row = nextRow)`
  and the first `if (++row <= 3) goto primaryRow;` with the label inside the
  section's block. Both are the same RTL.
* Still required: per-section column variables (one shared `column` is 15
  differing) and the base temporaries below.

## Why a goto outer loop: the first section's outer loop is invisible to loop.c

In all three ROMs, the first section recomputes the flag-entry offset
`(rowSlot + 5) * 0x60` (`(slotBase + 5) * 0x60` in the consumable grid) from
the row on every row. As C this is `entry = (void *)((rowSlot + 5) * 0x60 +
(u32)flagsBase)`.

* With a `do`/`while`/`for` outer loop, loop.c strength-reduces that multiply
  into a new induction variable: a stack slot that steps by 0x180, an extra
  frame word (`sub sp, #0x14` for 0x10). The only exception is when the giv
  chain is cut, which is what the pinned units do with `register s32 rowSlot
  asm("r9")` (a hard register is never a giv) or an `asm("" : "+r"(...))`.
* Writing the index in the inner loop (`flagsBase[slot + 5]`) does not help.
  The rerun of loop (-frerun-loop-opt) sees the init insns that the inner
  loop's reduction emitted as ordinary pseudos, and reduces them in the
  outer loop. You can see it in `u.i.loop`'s second pass.
* A label plus `if (++row <= 3) goto primaryRow;` outer loop has no loop notes,
  so loop.c leaves it alone. The multiply then comes out per row as in the
  ROM. Consumable grid: 67 / 67 that way against 101 / 105 with `do`/`for`.
  Item grid: 53 / 57 against 58 / 62.
* `rowBytes` (+0x180 per row) has to be explicit for the same reason.
* In the consumable grid, `slotBase` (+4) and `saveByteOffset` (+8) have to
  be explicit too.

## Smaller properties found on the item grid

* Compute the member base before adding the row offset:
  `struct SceneInputWidget *base = panel->display.primary; widget = (void *)(rowBytes + (u32)base);`.
  Folded into one expression, it becomes `(rowBytes + 0x288) + panel`.
  The same holds for `display.secondary` in the second section.
* Both a per-section `column` and a per-section `rowSlot` give rowSlot sb and
  column sl, as in the ROM. With one shared pair, column comes first in
  global-alloc's priority order and takes sb.
* For the closing hide loop, `u32 mask = 1;` declared before `entry` puts
  `movs r3, #1` first, as in the ROM.

## Consumable grid notes

* The function-start stores take r0, r1, r2 and r3, then reuse r0 for
  `rowBytes = 0`. That is reload inheritance, so its stack variables are
  spilled pseudos, not memory.
* The hidden-flags loop counts 4..7 upward in the ROM. A goto exit keeps it
  that way (README fact 40), so the pinned unit's `asm("" : "+r"(slot))` is
  not needed. `slot = 4;` goes before the `mask`/`hidden` block.
* What remains is section 1's allocation. `panel` gets sb instead of r8, and
  one variable gets r8 instead of a stack slot (frame 0x1c for 0x18).

## Equipment grid notes

* The `do`/`while` form (goto inner exits) is 72 / 76. The outer loop
  reduces the entry offset, and column and rowSlot swap sl and the stack.
* The goto outer loop is 99 / 101: panel gets r8 and row gets sb, where the
  ROM has the reverse.
* Not tried there yet: the item grid's base temporaries and per-section
  variables.
