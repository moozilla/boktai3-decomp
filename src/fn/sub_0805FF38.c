// MATCHING_CONCESSION: register-pinning
#include "scene_inventory_panel.h"
void SceneInventory_PopulateItemGrid(struct SceneInventoryPanel *input)
{
    struct SceneInventoryPanel *panel = input;
    volatile s32 nextRow;
    struct SceneInventoryWidgetFlags *volatile flagsBase;
    volatile u32 rowBytes;
    s32 column;
    register s32 rowSlot asm("r9");
    s32 row;
    Anim_SelectWidgetGlyph(&panel->display.heading, &panel->display.widgetResource, 0x63);
    row = 0;
    flagsBase = ((struct SceneInventoryFlagsView *)(void *)panel)->widgets;
    {
        register u32 zero asm("r1") = 0;
        rowBytes = zero;
    }
    for (; row <= 3; row++) {
        struct SceneInputWidget *widget;
        struct SceneInventoryWidgetFlags *entry;
        column = 0;
        rowSlot = row * 4;
        {
            struct SceneInputWidget *base = panel->display.primary;
            register u32 offset asm("r1");
            offset = rowBytes;
            widget = (void *)(offset + (u32)base);
        }
        {
            u32 index = rowSlot + 5;
            u32 scaled;
            struct SceneInventoryWidgetFlags *base;
            scaled = index * 0x60;
            base = flagsBase;
            entry = (void *)(scaled + (u32)base);
        }
        do {
            register s32 kind asm("r5");
            {
                register s32 slot asm("r1") = rowSlot + column;
                kind = Inventory_GetItemId(slot);
            }
            if (kind >= 0) {
                register u32 frame asm("r2");
                entry->flags &= ~1;
                frame = Selection_GetDisplayFrame(kind);
                asm("" : "+r"(frame));
                frame = (u16)frame;
                {
                    struct SceneInputWidget *argument = widget;
                    Anim_LoadKeyframe(argument, &panel->display.gridResource, frame, 1);
                }
            } else
                entry->flags |= 1;
            SceneHud_SetKeyframePaletteBank(widget, 0x373);
            widget++;
            entry++;
            column++;
        } while (column <= 3);
        {
            u32 offset = rowBytes;
            u32 step = 0x180;
            rowBytes = offset + step;
        }
    }
    row = 0;
    for (; row <= 1;) {
        struct SceneInputWidget *widget;
        struct SceneInventoryWidgetFlags *entry;
        column = 0;
        rowSlot = row * 4;
        nextRow = row + 1;
        {
            u32 flagIndex = rowSlot + 21;
            u32 offset;
            struct SceneInputWidget *base;
            offset = ((row << 3) + rowSlot) << 5;
            base = panel->display.secondary;
            widget = (void *)(offset + (u32)base);
            offset = flagIndex * 0x60;
            {
                struct SceneInventoryWidgetFlags *flags = flagsBase;
                entry = (void *)(offset + (u32)flags);
            }
        }
        do {
            s32 kind;
            {
                u32 slot = rowSlot + column;
                register u32 offset asm("r0");
                const s8 *catalog;
                offset = panel->page * 8;
                offset = slot + offset;
                catalog = panel->catalog;
                catalog += offset;
                kind = *catalog;
            }
            if (kind >= 0) {
                s32 currency;
                entry->flags &= ~1;
                {
                    register u32 frame asm("r2") = Selection_GetDisplayFrame(kind);
                    asm("" : "+r"(frame));
                    frame = (u16)frame;
                    {
                        struct SceneInputWidget *argument = widget;
                        Anim_LoadKeyframe(argument, &panel->display.gridResource, frame, 1);
                    }
                }
                currency = gGameState->unk7B0;
                if (currency < (s32)SceneItemValue_GetItemBaseValue(kind))
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
        u32 mask = 1;
        struct SceneInventoryWidgetFlags *entry;
        register s32 remaining asm("r1");
        {
            struct SceneInventoryWidgetFlags *base = flagsBase;
            u32 offset = 41 * 0x60;
            entry = (void *)((u32)base + offset);
        }
        remaining = 7;
        do {
            entry->flags |= mask;
            entry++;
            remaining--;
        } while (remaining >= 0);
    }
}
