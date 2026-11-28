#include "global.h"
#include "ui/widgets/scene_input_grid.h"
#include "ui/town/storage_screen.h"
#include "system/game_state/game_state.h"
#include "ui/town/shop_item_values.h"
s32 Selection_GetDisplayFrame(s32);
s32 Storage_FindEquippedItemSlot(void *, s32);
s32 GameState_IsConsumableSlotEquipped(u32);
void Storage_GetSlotPosition(struct SceneInputPosition *, u32);
void Sound_PlaySong(u32);
void Storage_BeginSwapTransition(struct StorageScreen *screen)
{
    struct SceneInputPosition starts[2];
    struct SceneInputPosition targets[2];
    u32 enabled[2];
    u32 frames[2];
    u32 attributes[2];
    s32 mode = screen->categoryKinds[screen->category] >> 1;
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
        slots = screen->slots;
        if (i == 0)
            selected = screen->savedSelection;
        else
            selected = screen->selection;
        screen->controls[selected + 8].flags |= 1;
        switch (mode) {
            case 0: {
                item = Inventory_GetItemId(slots[selected]);
                enabledBase[i] = item >= 0;
                frameBase[i] = Selection_GetDisplayFrame(item);
                break;
            }

            case 1: {
                s32 slot = Storage_FindEquippedItemSlot(screen, slots[selected]);
                if (slot >= 0)
                    AnimSprite_Hide(&screen->categorySlots[slot]);
                item = Inventory_GetEquipmentId(slots[selected]);
                enabledBase[i] = item != 255;
                frameBase[i] = item + 0xF0;
                break;
            }

            case 2: {
                s32 slot;
                u8 *consumables;
                u32 *flags;
                for (slot = 0, consumables = screen->slots, flags = &screen->categorySlots[0].flags;
                     slot < 32; slot++) {
                    if ((Inventory_GetConsumableId(consumables[slot]) != (-1)) &&
                        GameState_IsConsumableSlotEquipped(consumables[slot])) {
                        if ((slot == screen->selection) || (slot == screen->savedSelection)) {
                            *flags |= 1;
                            flags += sizeof(struct AnimSprite) / sizeof(*flags);
                        }
                    }
                }

                item = *(gGameState->table160 + screen->slots[selected]);
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

    SceneInput_BeginPairTransition(&screen->markers, enabledBase, starts, targetBase, frameBase,
                                   attributeBase, 12);
    Sound_PlaySong(0x38A);
}
