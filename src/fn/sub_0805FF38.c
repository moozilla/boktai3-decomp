#include "scene_inventory_panel.h"
void SceneInventory_PopulateItemGrid(struct SceneInventoryPanel *panel)
{
    s32 nextRow;
    struct SceneInventoryWidgetFlags *flagsBase;
    u32 rowBytes;
    s32 column;
    s32 rowSlot;
    s32 row;
    s32 slot;
    s32 kind;
    Anim_SelectWidgetGlyph(&panel->display.heading, &panel->display.widgetResource, 0x63);
    row = 0;
    flagsBase = ((struct SceneInventoryFlagsView *)(void *)panel)->widgets;
    rowBytes = 0;
    {
        struct SceneInputWidget *widget;
        struct SceneInventoryWidgetFlags *entry;
    primaryRow:
        column = 0;
        rowSlot = row * 4;
        {
            struct SceneInputWidget *base = panel->display.primary;
            widget = (void *)(rowBytes + (u32)base);
        }
        entry = (void *)((rowSlot + 5) * 0x60 + (u32)flagsBase);
        do {
            slot = rowSlot + column;
            kind = Inventory_GetItemId(slot);
            if (kind >= 0) {
                entry->flags &= ~1;
                Anim_LoadKeyframe(widget, &panel->display.gridResource,
                                  (u16)Selection_GetDisplayFrame(kind), 1);
            } else
                entry->flags |= 1;
            SceneHud_SetKeyframePaletteBank(widget, 0x373);
            widget++;
            entry++;
            column++;
        } while (column <= 3);
        rowBytes += 0x180;
        if (++row <= 3)
            goto primaryRow;
    }
    for (row = 0; row <= 1; row = nextRow) {
        s32 column2;
        struct SceneInputWidget *widget;
        struct SceneInventoryWidgetFlags *entry;
        u32 flagIndex;
        u32 offset;
        column2 = 0;
        rowSlot = row * 4;
        nextRow = row + 1;
        flagIndex = rowSlot + 21;
        offset = ((row << 3) + rowSlot) << 5;
        {
            struct SceneInputWidget *base = panel->display.secondary;
            widget = (void *)(offset + (u32)base);
        }
        entry = (void *)(flagIndex * 0x60 + (u32)flagsBase);
        do {
            slot = rowSlot + column2;
            kind = panel->catalog[slot + panel->page * 8];
            if (kind >= 0) {
                s32 currency;
                entry->flags &= ~1;
                Anim_LoadKeyframe(widget, &panel->display.gridResource,
                                  (u16)Selection_GetDisplayFrame(kind), 1);
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
    }
    {
        u32 mask = 1;
        struct SceneInventoryWidgetFlags *entry = &flagsBase[41];
        for (slot = 0; slot < 8; slot++) {
            entry->flags |= mask;
            entry++;
        }
    }
}
