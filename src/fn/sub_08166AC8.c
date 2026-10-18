#include "global.h"
s32 GameState_HasUnlockedInventoryEntry(u32);
s32 SolarFruit_IsInventorySlotEquipped(u32);
s32 Inventory_IsEquipmentAssigned(u32);
s32 GameState_IsConsumableSlotEquipped(u32);
s32 PauseMenu_GetCategoryEntryValue(u32 a, s32 b)
{
    switch (b) {
        case 1:
            if (a - 8 <= 0xb)
                return GameState_HasUnlockedInventoryEntry(a - 8);
            break;
        case 0:
            if (a <= 7)
                return SolarFruit_IsInventorySlotEquipped(a);
            break;
        case 2:
            if (a <= 0xf)
                return Inventory_IsEquipmentAssigned(a);
            break;
        case 3:
            if (a <= 0xf)
                return GameState_IsConsumableSlotEquipped(a);
            break;
    }
    return 0;
}
