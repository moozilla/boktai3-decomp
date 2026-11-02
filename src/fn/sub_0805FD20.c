#include "scene_inventory_panel.h"
void SceneInventory_PopulateEquipmentGrid(struct SceneInventoryPanel *panel)
{
    s32 column;
    s32 nextRow;
    struct SceneInventoryWidgetFlags *flagsBase;
    u32 rowBytes;
    s32 rowSlot;
    s32 row;
    s32 slot;
    s32 kind;
    Anim_SelectWidgetGlyph(&panel->display.heading, &panel->display.widgetResource, 0x65);
    row = 0;
    flagsBase = ((struct SceneInventoryFlagsView *)(void *)panel)->widgets;
    rowBytes = 0;
    do {
        struct SceneInputWidget *widget;
        s32 flagIndex;
        column = 0;
        rowSlot = row * 4;
        {
            struct SceneInputWidget *base = panel->display.primary;
            widget = (void *)(rowBytes + (u32)base);
        }
        flagIndex = rowSlot + 5;
        do {
            slot = column + rowSlot;
            kind = Inventory_GetEquipmentId(slot);
            if (kind != 255) {
                flagsBase[flagIndex].flags &= ~1;
                Anim_LoadKeyframe(widget, &panel->display.gridResource, (u16)(kind + 240), 1);
            } else
                flagsBase[flagIndex].flags |= 1;
            if (Inventory_CanAssignEquipment(slot))
                SceneHud_SetKeyframePaletteBank(widget, 0x373);
            else
                SceneHud_SetKeyframePaletteBank(widget, 0x374);
            widget++;
            flagIndex++;
            column++;
        } while (column <= 3);
        rowBytes += 0x180;
    } while (++row <= 3);
    for (row = 0; row <= 1; row = nextRow) {
        struct SceneInputWidget *widget;
        u32 flagIndex;
        u32 offset;
        column = 0;
        rowSlot = row * 4;
        nextRow = row + 1;
        flagIndex = rowSlot + 21;
        offset = ((row << 3) + rowSlot) << 5;
        {
            struct SceneInputWidget *base = panel->display.secondary;
            widget = (void *)(offset + (u32)base);
        }
        do {
            slot = column + rowSlot;
            kind = panel->catalog[slot + panel->page * 8];
            if (kind >= 0) {
                struct SceneValueEquipmentDefinition *definition = Equipment_GetDefinition(kind);
                flagsBase[flagIndex].flags &= ~1;
                Anim_LoadKeyframe(widget, &panel->display.gridResource, (u16)(kind + 240), 1);
                if (gGameState->unk7B0 < definition->baseValue)
                    SceneHud_SetKeyframePaletteBank(widget, 0x374);
                else
                    SceneHud_SetKeyframePaletteBank(widget, 0x373);
            } else
                flagsBase[flagIndex].flags |= 1;
            flagIndex++;
            widget++;
            column++;
        } while (column <= 3);
    }
    {
        const s16 *materials = gGameState->inventory;
        struct ContactPosition *position = &panel->display.digits[0].position;
        struct SceneInventoryWidgetFlags *entry = &flagsBase[41];
        s32 i;
        for (i = 0; i < 8; i++) {
            u16 packed = *materials;
            if (*materials < 0)
                entry->flags |= 1;
            else {
                entry->flags &= ~1;
                position->x = (packed & 3) * 24 + 112;
                position->y = (*materials >> 2) * 24 + 32;
            }
            materials++;
            position = (void *)((u8 *)position + 0x60);
            entry++;
        }
    }
}
