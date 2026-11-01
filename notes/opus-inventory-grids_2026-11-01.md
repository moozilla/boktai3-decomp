# SceneInventory_Populate{Equipment,Item,Consumable}Grid: still pinned; a goto outer loop is needed

Tags: #loop-reduction #goto #reload-rotation #inventory

**Not depinned.** All three units (0805FD20, 0805FF38, 080600DC) still match
only in their pinned form. I gave each the two or three tries the queue
allowed. The best pin-free drafts:

| Unit | Best pin-free draft | Differing (ROM / draft) | Shape |
|---|---|---|---|
| PopulateItemGrid 0805FF38 | goto outer loop, local struct, base temporaries (below) | 26 / 26 | 2 |
| PopulateConsumableGrid 080600DC | goto outer loop, plain locals | 67 / 67 | 68 |
| PopulateEquipmentGrid 0805FD20 | `do` / `while` loops with goto exits | 72 / 76 | 68 |

## What decided it so far: the first section's outer loop is invisible to loop.c

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
* What remains (26 insns):
  * In memory (a local struct or `volatile`), `rowBytes = 0` stores
    `row`'s register (`str r7`). CSE replaces the zero with the pseudo that
    already holds 0, where the ROM has `movs r1, #0`. The pinned unit uses
    `register u32 zero asm("r1")` for this.
  * As plain (spilled) locals, `rowBytes = 0` matches. Reload's spill
    rotation is then one step off at `column = 0`. `scope.py spill` reports
    that the rotation alone does not explain it: 34 differing at best.
  * The rest are low-register choices (r0/r1/r2) in reloads and local
    allocation.

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

## Best item grid draft (26 / 26, shape 2)

```c
#include "scene_inventory_panel.h"
void SceneInventory_PopulateItemGrid(struct SceneInventoryPanel *panel)
{
    struct {
        s32 nextRow;
        struct SceneInventoryWidgetFlags *flagsBase;
        u32 rowBytes;
    } v;
    s32 column;
    s32 rowSlot;
    s32 row;
    s32 slot;
    s32 kind;
    Anim_SelectWidgetGlyph(&panel->display.heading, &panel->display.widgetResource, 0x63);
    row = 0;
    v.flagsBase = ((struct SceneInventoryFlagsView *)(void *)panel)->widgets;
    v.rowBytes = 0;
primaryRow: {
        struct SceneInputWidget *widget;
        struct SceneInventoryWidgetFlags *entry;
        column = 0;
        rowSlot = row * 4;
        {
            struct SceneInputWidget *base = panel->display.primary;
            widget = (void *)(v.rowBytes + (u32)base);
        }
        entry = (void *)((rowSlot + 5) * 0x60 + (u32)v.flagsBase);
        do {
            slot = rowSlot + column;
            kind = Inventory_GetItemId(slot);
            if (kind >= 0) {
                entry->flags &= ~1;
                Anim_LoadKeyframe(widget, &panel->display.gridResource, (u16)Selection_GetDisplayFrame(kind), 1);
            } else
                entry->flags |= 1;
            SceneHud_SetKeyframePaletteBank(widget, 0x373);
            widget++;
            entry++;
            column++;
        } while (column <= 3);
        v.rowBytes += 0x180;
        row++;
        if (row <= 3)
            goto primaryRow;
    }
    for (row = 0; row <= 1;) {
        s32 column2;
        struct SceneInputWidget *widget;
        struct SceneInventoryWidgetFlags *entry;
        u32 flagIndex;
        u32 offset;
        column2 = 0;
        rowSlot = row * 4;
        v.nextRow = row + 1;
        flagIndex = rowSlot + 21;
        offset = ((row << 3) + rowSlot) << 5;
        {
            struct SceneInputWidget *base = panel->display.secondary;
            widget = (void *)(offset + (u32)base);
        }
        entry = (void *)(flagIndex * 0x60 + (u32)v.flagsBase);
        do {
            slot = rowSlot + column2;
            kind = panel->catalog[slot + panel->page * 8];
            if (kind >= 0) {
                s32 currency;
                entry->flags &= ~1;
                Anim_LoadKeyframe(widget, &panel->display.gridResource, (u16)Selection_GetDisplayFrame(kind), 1);
                currency = gGameState->unk7B0;
                if (currency < (s32)SceneItemValue_GetItemBaseValue(kind))
                    SceneHud_SetKeyframePaletteBank(widget, 0x374);
                else
                    SceneHud_SetKeyframePaletteBank(widget, 0x373);
            } else
                entry->flags |= 1;
            entry++;
            widget++;
            column2++;
        } while (column2 <= 3);
        row = v.nextRow;
    }
    {
        u32 mask = 1;
        struct SceneInventoryWidgetFlags *entry = &v.flagsBase[41];
        s32 i;
        for (i = 0; i < 8; i++) {
            entry->flags |= mask;
            entry++;
        }
    }
}
```
