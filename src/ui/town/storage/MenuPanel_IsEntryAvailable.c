#include "global.h"
#include "ui/town/storage_screen.h"
s32 Storage_FindEquippedItemSlot(struct StorageScreen *, s32);
s32 GameState_IsConsumableSlotEquipped(u32);
s32 MenuPanel_IsEntryAvailable(struct StorageScreen *screen, s32 index)
{
    switch (screen->categoryKinds[screen->category] >> 1) {
        case 0:
        default:
            break;
        case 1:
            if (Storage_FindEquippedItemSlot(screen, screen->slots[index]) >= 0)
                return 1;
            break;
        case 2:
            if (GameState_IsConsumableSlotEquipped(screen->slots[index]))
                return 1;
            break;
    }
    return 0;
}
