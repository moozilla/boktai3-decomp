// MATCHING_CONCESSION: register-pinning
#include "scene_inventory_panel.h"
#define SceneInventory_PopulateEquipmentGrid sub_0805FD20
void SceneInventory_PopulateEquipmentGrid(struct SceneInventoryPanel *input)
{
    struct SceneInventoryPanel *panel = input;
    volatile s32 column;
    volatile s32 nextRow;
    struct SceneInventoryWidgetFlags *volatile flagsBase;
    volatile u32 rowBytes;
    register s32 rowSlot asm("r10");
    s32 row;
    s32 slot;
    sub_08219728(&panel->display.heading, &panel->display.widgetResource, 0x65);
    row = 0;
    {
        register struct SceneInventoryWidgetFlags *initial asm("r1") =
            ((struct SceneInventoryFlagsView *)(void *)panel)->widgets;
        flagsBase = initial;
    }
    rowBytes = 0;
    do {
        register struct SceneInputWidget *widget asm("r6");
        register struct SceneInventoryWidgetFlags *entry asm("r4");
        {
            register u32 zero asm("r1") = 0;
            column = zero;
        }
        {
            u32 shifted = row;
            asm("" : "+r"(shifted));
            shifted *= 4;
            rowSlot = shifted;
        }
        {
            register struct SceneInputWidget *base asm("r0") = panel->display.primary;
            {
                register u32 offset asm("r1");
                offset = rowBytes;
                widget = (void *)(offset + (u32)base);
            }
        }
        {
            u32 index = rowSlot + 5;
            register u32 scaled asm("r0");
            register struct SceneInventoryWidgetFlags *base asm("r1");
            scaled = index * 0x60;
            base = flagsBase;
            entry = (void *)(scaled + (u32)base);
        }
        do {
            register s32 kind asm("r5");
            slot = column;
            slot += rowSlot;
            kind = sub_0822D6E0(slot);
            if (kind != 255) {
                entry->flags &= ~1;
                {
                    register u32 frame asm("r2") = kind;
                    frame = (u16)(frame + 240);
                    {
                        struct SceneInputWidget *argument = widget;
                        Anim_LoadKeyframe(argument, &panel->display.gridResource, frame, 1);
                    }
                }
            } else
                entry->flags |= 1;
            if (sub_0822DFDC(slot))
                SceneHud_SetKeyframePaletteBank(widget, 0x373);
            else
                SceneHud_SetKeyframePaletteBank(widget, 0x374);
            widget++;
            entry++;
            {
                s32 next = column + 1;
                column = next;
                if (next > 3)
                    break;
            }
        } while (1);
        {
            register u32 offset asm("r1") = rowBytes;
            u32 step = 0x180;
            rowBytes = offset + step;
        }
    } while (++row <= 3);
    row = 0;
    do {
        struct SceneInputWidget *widget;
        struct SceneInventoryWidgetFlags *entry;
        {
            u32 zero = 0;
            column = zero;
        }
        {
            register u32 shifted asm("r1") = row;
            asm("" : "+r"(shifted));
            shifted *= 4;
            rowSlot = shifted;
        }
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
                register u32 offset asm("r0");
                const s8 *catalog;
                slot = column;
                slot += rowSlot;
                offset = panel->page * 8;
                offset = slot + offset;
                catalog = panel->catalog;
                catalog += offset;
                kind = *catalog;
            }
            if (kind >= 0) {
                struct SceneValueEquipmentDefinition *definition = sub_08227F5C(kind);
                entry->flags &= ~1;
                {
                    register u32 frame asm("r2") = kind;
                    frame = (u16)(frame + 240);
                    {
                        struct SceneInputWidget *argument = widget;
                        Anim_LoadKeyframe(argument, &panel->display.gridResource, frame, 1);
                    }
                }
                if (gUnk_02000710->unk7B0 < definition->baseValue)
                    SceneHud_SetKeyframePaletteBank(widget, 0x374);
                else
                    SceneHud_SetKeyframePaletteBank(widget, 0x373);
            } else
                entry->flags |= 1;
            entry++;
            widget++;
            {
                register s32 next asm("r1") = column + 1;
                column = next;
                if (next > 3)
                    break;
            }
        } while (1);
        {
            register s32 next asm("r0") = nextRow;
            row = next;
            if (next > 1)
                break;
        }
    } while (1);
    {
        register struct GameState *state asm("r0") = gUnk_02000710;
        register const s16 *materials asm("r4");
        struct ContactPosition *position;
        struct SceneInventoryWidgetFlags *entry;
        s32 remaining;
        materials = state->inventory;
        position = &panel->display.digits[0].position;
        {
            struct SceneInventoryWidgetFlags *base = flagsBase;
            u32 offset = 41 * 0x60;
            asm("" ::"r"(base), "r"(offset));
            entry = (void *)((u32)base + offset);
        }
        remaining = 7;
        do {
            u32 packed = *(const u16 *)materials;
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
            remaining--;
        } while (remaining >= 0);
    }
}
