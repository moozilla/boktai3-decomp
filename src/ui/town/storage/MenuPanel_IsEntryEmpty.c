#include "global.h"
#include "ui/town/shop_states.h"
#include "ui/town/storage_screen.h"
#include "system/game_state/game_state.h"
s32 Inventory_GetItemId(u32);
s32 Inventory_GetEquipmentId(u32);
s32 MenuPanel_IsEntryEmpty(struct StorageScreen *screen, s32 index)
{
    u32 entry = screen->slots[index];
    switch (screen->categoryKinds[screen->category] >> 1) {
        case 0:
            if (Inventory_GetItemId(entry) < 0)
                return 1;
            break;
        case 1:
            if (Inventory_GetEquipmentId(entry) == 0xFF)
                return 1;
            break;
        case 2:
            if (*(entry + gGameState->table160) < 0)
                return 1;
            break;
    }
    return 0;
}
