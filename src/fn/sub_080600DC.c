#include "scene_inventory_panel.h"
void SceneInventory_PopulateConsumableGrid(struct SceneInventoryPanel *panel)
{
    s32 row;
    struct SceneInventoryWidgetFlags *flagsBase;
    s32 nextRow;
    s32 rowSlot;
    u32 cursor;
    s32 column;
    s32 slot;
    s32 kind;
    Anim_SelectWidgetGlyph(&panel->display.heading, &panel->display.widgetResource, 0x66);
    row = 0;
    flagsBase = ((struct SceneInventoryFlagsView *)(void *)panel)->widgets;
    for (; row <= 3; row++) {
        rowSlot = row * 4;
        for (column = 0; column <= 3; column++) {
            struct SceneInputWidget *widget;
            s32 flagIndex;
            slot = rowSlot + column;
            flagIndex = slot + 5;
            kind = ((struct GameState *)((u8 *)gGameState + slot * 2))->table160[0];
            widget = &panel->display.primary[slot];
            if ((s16)kind >= 0) {
                flagsBase[flagIndex].flags &= ~1;
                Anim_LoadKeyframe(widget, &panel->display.gridResource, (u16)(kind + 289), 1);
            } else
                flagsBase[flagIndex].flags |= 1;
            if (!gUnk_08E60F80[(s16)kind].baseValue || GameState_IsConsumableSlotEquipped(slot))
                SceneHud_SetKeyframePaletteBank(widget, 0x374);
            else
                SceneHud_SetKeyframePaletteBank(widget, 0x373);
        }
    }
    for (row = 0; row <= 1;) {
        struct SceneInputWidget *widget;
        struct SceneInventoryWidgetFlags *entry;
        u32 flagIndex;
        u32 offset;
        column = 0;
        cursor = row * 4;
        nextRow = row + 1;
        flagIndex = cursor + 21;
        offset = ((row << 3) + cursor) << 5;
        {
            struct SceneInputWidget *base = panel->display.secondary;
            widget = (void *)(offset + (u32)base);
        }
        entry = (void *)(flagIndex * 0x60 + (u32)flagsBase);
        do {
            slot = cursor + column;
            kind = panel->catalog[slot + panel->page * 8];
            if (kind >= 0) {
                entry->flags &= ~1;
                Anim_LoadKeyframe(widget, &panel->display.gridResource, (u16)(kind + 289), 1);
                if (gGameState->unk7B0 < gUnk_08E60F80[kind].baseValue)
                    SceneHud_SetKeyframePaletteBank(widget, 0x374);
                else
                    SceneHud_SetKeyframePaletteBank(widget, 0x373);
            } else
                entry->flags |= 1;
            entry++;
            widget++;
            column++;
        } while (column <= 3);
        row = nextRow;
    }
    {
        struct GameState *state = gGameState;
        struct ContactPosition *position = &panel->display.digits[0].position;
        struct SceneInventoryWidgetFlags *entry = &flagsBase[41];
        const s16 *party = state->party;
        for (slot = 0; slot < 4; slot++) {
            s32 packed = *party;
            if (packed < 0)
                entry->flags |= 1;
            else {
                entry->flags &= ~1;
                position->x = (packed & 3) * 24 + 112;
                position->y = (packed >> 2) * 24 + 32;
            }
            position = (void *)((u8 *)position + 0x60);
            entry++;
            party++;
        }
    }
    slot = 4;
    {
        u32 mask = 1;
        struct SceneInventoryWidgetFlags *hidden = &flagsBase[45];
    hiddenLoop:
        hidden->flags |= mask;
        hidden++;
        if (++slot <= 7)
            goto hiddenLoop;
    }
}
