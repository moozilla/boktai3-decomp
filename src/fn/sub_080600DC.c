// MATCHING_CONCESSION: register-pinning
#include "scene_inventory_panel.h"
void SceneInventory_PopulateConsumableGrid(struct SceneInventoryPanel *input)
{
    register struct SceneInventoryPanel *panel asm("r8") = input;
    volatile s32 row;
    struct SceneInventoryWidgetFlags *volatile flagsBase;
    volatile s32 nextRow;
    volatile u32 saveByteOffset;
    volatile u32 slotBase;
    volatile u32 rowBytes;
    u32 cursor;
    register s32 column asm("r10");
    s32 slot;
    register u32 previous asm("r1");
    Anim_SelectWidgetGlyph(&panel->display.heading, &panel->display.widgetResource, 0x66);
    {
        u32 zero = 0;
        row = zero;
        {
            register struct SceneInventoryWidgetFlags *base asm("r1") =
                ((struct SceneInventoryFlagsView *)(void *)panel)->widgets;
            flagsBase = base;
        }
        {
            register u32 other asm("r2") = 0;
            saveByteOffset = other;
        }
        {
            register u32 other asm("r3") = 0;
            slotBase = other;
        }
        rowBytes = zero;
    }
    do {
        struct SceneInputWidget *widget;
        struct SceneInventoryWidgetFlags *entry;
        {
            register u32 zero asm("r1") = 0;
            asm("" ::"r"(zero));
            column = zero;
        }
        {
            struct SceneInputWidget *base = panel->display.primary;
            register u32 offset asm("r2");
            offset = rowBytes;
            widget = (void *)(offset + (u32)base);
        }
        {
            register u32 offset asm("r3") = saveByteOffset;
            cursor = offset;
        }
        {
            u32 index = slotBase + 5;
            register u32 scaled asm("r0");
            struct SceneInventoryWidgetFlags *base;
            scaled = index * 0x60;
            base = flagsBase;
            entry = (void *)(scaled + (u32)base);
            previous = (u32)base;
        }
        do {
            s32 kind;
            struct GameState *indexed;
            slot = slotBase;
            slot += column;
            indexed = (void *)((u32)gGameState + cursor);
            {
                u32 offset = 0x160;
                s16 *values;
                values = (void *)((u32)indexed + offset);
                kind = *values;
                asm("" ::"r"(offset), "r"(previous));
            }
            if (kind >= 0) {
                entry->flags &= ~1;
                {
                    u32 frame = (u16)(kind + 289);
                    {
                        register struct SceneInputWidget *argument asm("r0") = widget;
                        Anim_LoadKeyframe(argument, &panel->display.gridResource, frame, 1);
                    }
                }
            } else
                entry->flags |= 1;
            {
                /* The original also reads the preceding record for an empty (-1) slot. */
                u32 base = (u32)gUnk_08E60F80;
                const struct SceneValueConsumableDefinition *definition;
                definition = (void *)(((u32)kind << 4) + base);
                if (!definition->baseValue || GameState_IsConsumableSlotEquipped(slot))
                    SceneHud_SetKeyframePaletteBank(widget, 0x374);
                else
                    SceneHud_SetKeyframePaletteBank(widget, 0x373);
            }
            widget++;
            {
                u32 step = 2;
                cursor += step;
            }
            entry++;
            {
                u32 step = 1;
                column += step;
            }
            {
                register s32 check asm("r1") = column;
                previous = check;
                if (check > 3)
                    break;
            }
        } while (1);
        {
            register u32 value asm("r2") = saveByteOffset;
            value += 8;
            saveByteOffset = value;
        }
        {
            register u32 value asm("r3") = slotBase;
            value += 4;
            slotBase = value;
        }
        {
            u32 value = rowBytes;
            u32 step = 0x180;
            rowBytes = value + step;
        }
        {
            register s32 next asm("r2") = row + 1;
            row = next;
            if (next > 3)
                break;
        }
    } while (1);
    {
        register u32 zero asm("r3") = 0;
        row = zero;
    }
    do {
        register struct SceneInputWidget *widget asm("r6");
        struct SceneInventoryWidgetFlags *entry;
        {
            register u32 zero asm("r0") = 0;
            asm("" ::"r"(zero));
            column = zero;
        }
        {
            register u32 shifted asm("r1") = row;
            shifted *= 4;
            cursor = shifted;
        }
        {
            register s32 next asm("r2") = row + 1;
            nextRow = next;
        }
        {
            u32 flagIndex = cursor + 21;
            register u32 offset asm("r0");
            register struct SceneInputWidget *base asm("r1");
            {
                register u32 current asm("r3") = row;
                offset = ((current << 3) + cursor) << 5;
            }
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
            slot = cursor;
            slot += column;
            {
                register u32 offset asm("r0") = panel->page * 8;
                const s8 *catalog;
                offset = slot + offset;
                catalog = panel->catalog;
                kind = catalog[offset];
            }
            if (kind >= 0) {
                const struct SceneValueConsumableDefinition *definition;
                {
                    u32 flags = entry->flags;
                    u32 mask = ~1;
                    u32 frame;
                    flags &= mask;
                    entry->flags = flags;
                    frame = (u16)(kind + 289);
                    {
                        struct SceneInputWidget *argument = widget;
                        Anim_LoadKeyframe(argument, &panel->display.gridResource, frame, 1);
                    }
                }
                {
                    struct GameState *state = gGameState;
                    u32 offset = (u32) & ((struct GameState *)0)->unk7B0;
                    s16 *currency;
                    const struct SceneValueConsumableDefinition *record;
                    s32 money, value;
                    currency = (void *)((u32)state + offset);
                    {
                        u32 base = (u32)gUnk_08E60F80;
                        record = (void *)(((u32)kind << 4) + base);
                    }
                    money = *currency;
                    value = record->baseValue;
                    if (money < value)
                        SceneHud_SetKeyframePaletteBank(widget, 0x374);
                    else
                        SceneHud_SetKeyframePaletteBank(widget, 0x373);
                }
            } else
                entry->flags |= 1;
            entry++;
            widget++;
            {
                u32 step = 1;
                column += step;
            }
            {
                s32 check = column;
                if (check > 3)
                    break;
            }
        } while (1);
        {
            register s32 next asm("r2") = nextRow;
            row = next;
            if (next > 1)
                break;
        }
    } while (1);
    {
        register struct GameState *state asm("r0") = gGameState;
        register struct ContactPosition *position asm("r4");
        register const s16 *party asm("r5");
        struct SceneInventoryWidgetFlags *entry;
        position = &panel->display.digits[0].position;
        {
            struct SceneInventoryWidgetFlags *base = flagsBase;
            u32 offset = 41 * 0x60;
            entry = (void *)((u32)base + offset);
        }
        party = state->party;
        slot = 3;
        do {
            s32 packed = *party;
            if (packed < 0)
                entry->flags |= 1;
            else {
                entry->flags &= ~1;
                position->x = (packed & 3) * 24 + 112;
                {
                    s32 vertical = packed >> 2;
                    position->y = vertical * 24 + 32;
                }
            }
            position = (void *)((u8 *)position + 0x60);
            entry++;
            party++;
            slot--;
        } while (slot >= 0);
        slot = 4;
        {
            u32 mask = 1;
            register struct SceneInventoryWidgetFlags *hidden asm("r1");
            register struct SceneInventoryWidgetFlags *base asm("r3");
            u32 offset;
            base = flagsBase;
            offset = 45 * 0x60;
            hidden = (void *)((u32)base + offset);
            do {
                asm("" : "+r"(slot));
                hidden->flags |= mask;
                hidden++;
                slot++;
            } while (slot <= 7);
        }
    }
}
