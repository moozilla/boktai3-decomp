// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_input_grid.h"
struct SwapWidget {
    u8 unknown0[8];
    u32 flags;
    u8 unknownC[0x54];
};
struct SwapFlagWidget {
    u32 flags;
    u8 unknown4[0x5C];
};
struct SwapFlags {
    u8 unknown0[0x20];
    struct {
        u32 flags;
        u8 unknown4[0x5c];
    } widgets[32];
};
s32 Inventory_GetItemId(u32);
s32 Inventory_GetEquipmentId(u32);
s32 Inventory_GetConsumableId(u32);
u32 Selection_GetDisplayFrame(s32);
s32 Storage_FindEquippedItemSlot(void *, s32);
s32 GameState_IsConsumableSlotEquipped(u32);
void Storage_GetSlotPosition(struct SceneInputPosition *, u32);
void Sound_PlaySong(u32);
struct SwapMenu {
    u8 unknown0[0x10F8];
    struct SwapWidget markers[8];
    u8 unknown13F8[0x1558 - 0x13F8];
    u8 transition[0x1754 - 0x1558];
    u8 bank;
    u8 unknown1755[3];
    u8 categories[3];
    u8 unknown175B[0x1763 - 0x175B];
    u8 firstSelection;
    u8 unknown1764[2];
    u8 secondSelection;
    u8 unknown1767[0x1790 - 0x1767];
    u8 slots[32];
};
#define SWAP_OFFSET(field) ((u32) & ((struct SwapMenu *)0)->field)
struct SwapInventory {
    u8 unknown0[0x160];
    s16 consumables[432];
};
#define SWAP_CONSUMABLE_OFFSET ((u32) & ((struct SwapInventory *)0)->consumables)

static inline struct SwapWidget *Storage_EquipmentWidget(u8 *menu, s32 slot)
{
    u32 base = slot * sizeof(struct SwapWidget);
    register u32 offset;
    base += (u32)menu;
    offset = SWAP_OFFSET(markers);
    asm("" : "+r"(offset));
    return (struct SwapWidget *)(base + offset);
}

static inline u8 *Storage_ConsumableSlots(u8 *menu)
{
    register u32 address = SWAP_OFFSET(slots);
    asm("" : "+r"(address));
    address += (u32)menu;
    return (u8 *)address;
}

static inline struct SwapFlagWidget *Storage_ConsumableWidgets(u8 *menu)
{
    register u32 offset asm("r1") = SWAP_OFFSET(markers[0].flags);
    asm("" : "+r"(offset));
    return (struct SwapFlagWidget *)(menu + offset);
}

static inline u8 *Storage_SlotTable(u8 *menu, s16 offset)
{
    return menu + offset;
}

void Storage_BeginSwapTransition(u8 *p)
{
    struct SceneInputPosition starts[2];
    struct SceneInputPosition targets[2];
    u32 enabled[2];
    u32 frames[2];
    u32 attributes[2];
    s32 mode = ((struct SwapMenu *)p)->categories[((struct SwapMenu *)p)->bank] >> 1;
    u8 *slots;
    u32 *enabledBase;
    u32 *frameBase;
    u32 *attributeBase;
    struct SceneInputPosition *targetBase;
    s32 i;
    for (i = 0, enabledBase = enabled, frameBase = frames, attributeBase = attributes,
        targetBase = targets;
         i < 2; i++) {
        u32 selected;
        u32 frame;
        slots = p + SWAP_OFFSET(slots);
        if (i == 0)
            selected = ((struct SwapMenu *)p)->secondSelection;
        else
            selected = ((struct SwapMenu *)p)->firstSelection;
        ((struct SwapFlags *)p)->widgets[selected + 8].flags |= 1;
        switch (mode) {
            case 0: {
                register s32 id asm("r2") = Inventory_GetItemId(slots[selected]);
                enabledBase[i] = id >= 0;
                frame = Selection_GetDisplayFrame(id);
                break;
            }

            case 1: {
                s32 slot = Storage_FindEquippedItemSlot(p, slots[selected]);
                register s32 id asm("r2");
                if (slot >= 0)
                    Storage_EquipmentWidget(p, slot)->flags |= mode;
                id = Inventory_GetEquipmentId(slots[selected]);
                enabledBase[i] = id != 255;
                frame = id + 0xF0;
                break;
            }

            case 2: {
                s32 slot;
                register s32 id asm("r2");
                u8 *consumables;
                struct SwapFlagWidget *widget;
                for (slot = 0, consumables = Storage_ConsumableSlots(p),
                    widget = Storage_ConsumableWidgets(p);
                     slot < 32; slot++) {
                    if ((Inventory_GetConsumableId(consumables[slot]) != (-1)) &&
                        GameState_IsConsumableSlotEquipped(consumables[slot])) {
                        if ((slot == ((struct SwapMenu *)p)->firstSelection) ||
                            (slot == ((struct SwapMenu *)p)->secondSelection)) {
                            widget->flags |= 1;
                            widget++;
                        }
                    }
                }

                id = *((s16 *)((((u8 *)gGameState) + SWAP_CONSUMABLE_OFFSET) +
                               (Storage_SlotTable(p, SWAP_OFFSET(slots))[selected] * 2)));
                enabledBase[i] = id >= 0;
                frame = id + 0x121;
                break;
            }
            default:
                goto noFrame;
        }
        frameBase[i] = frame;
    noFrame:
        attributeBase[i] = 0x373;
        Storage_GetSlotPosition(&starts[i], selected);
    }

    for (i = 0; i < 2; i++)
        targetBase[i] = starts[1 - i];

    SceneInput_BeginPairTransition((void *)(p + SWAP_OFFSET(transition)), enabledBase, starts,
                                   targetBase, frameBase, attributeBase, 12);
    Sound_PlaySong(0x38A);
}
