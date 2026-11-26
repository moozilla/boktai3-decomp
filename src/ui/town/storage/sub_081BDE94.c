#include "global.h"
#include "ui/widgets/scene_input_grid.h"
#include "ui/town/storage_screen.h"
#include "system/game_state/game_state.h"
#include "ui/town/shop_item_values.h"
struct SwapFlagWidget {
    u32 flags;
    u8 unknown4[0x5C];
};
struct SwapFlags {
    u8 unknown0[0x20];
    struct {
        u32 flags;
        u8 unknown4[0x5c];
    } widgets[40];
};
s32 Selection_GetDisplayFrame(s32);
s32 Storage_FindEquippedItemSlot(void *, s32);
s32 GameState_IsConsumableSlotEquipped(u32);
void Storage_GetSlotPosition(struct SceneInputPosition *, u32);
void Sound_PlaySong(u32);
#define SWAP_OFFSET(field) ((u32) & ((struct StorageScreen *)0)->field)

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
    s32 mode =
        ((struct StorageScreen *)p)->categoryKinds[((struct StorageScreen *)p)->category] >> 1;
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
        s32 item;
        slots = ((struct StorageScreen *)p)->slots;
        if (i == 0)
            selected = ((struct StorageScreen *)p)->savedSelection;
        else
            selected = ((struct StorageScreen *)p)->selection;
        ((struct SwapFlags *)p)->widgets[selected + 8].flags |= 1;
        switch (mode) {
            case 0: {
                item = Inventory_GetItemId(slots[selected]);
                enabledBase[i] = item >= 0;
                frameBase[i] = Selection_GetDisplayFrame(item);
                break;
            }

            case 1: {
                s32 slot = Storage_FindEquippedItemSlot(p, slots[selected]);
                if (slot >= 0)
                    ((struct SceneInputWidget *)(p + slot * sizeof(struct SceneInputWidget) +
                                                 SWAP_OFFSET(categorySlots)))
                        ->flags |= mode;
                item = Inventory_GetEquipmentId(slots[selected]);
                enabledBase[i] = item != 255;
                frameBase[i] = item + 0xF0;
                break;
            }

            case 2: {
                s32 slot;
                u8 *consumables;
                struct SwapFlagWidget *widget;
                for (slot = 0, consumables = ((struct StorageScreen *)p)->slots,
                    widget = (struct SwapFlagWidget *)&((struct StorageScreen *)p)
                                 ->categorySlots[0]
                                 .flags;
                     slot < 32; slot++) {
                    if ((Inventory_GetConsumableId(consumables[slot]) != (-1)) &&
                        GameState_IsConsumableSlotEquipped(consumables[slot])) {
                        if ((slot == ((struct StorageScreen *)p)->selection) ||
                            (slot == ((struct StorageScreen *)p)->savedSelection)) {
                            widget->flags |= 1;
                            widget++;
                        }
                    }
                }

                item = *(gGameState->table160 + Storage_SlotTable(p, SWAP_OFFSET(slots))[selected]);
                enabledBase[i] = item >= 0;
                frameBase[i] = item + 0x121;
                break;
            }
        }
        attributeBase[i] = 0x373;
        Storage_GetSlotPosition(&starts[i], selected);
    }

    for (i = 0; i < 2; i++)
        targetBase[i] = starts[1 - i];

    SceneInput_BeginPairTransition(&((struct StorageScreen *)p)->markers, enabledBase, starts,
                                   targetBase, frameBase, attributeBase, 12);
    Sound_PlaySong(0x38A);
}
