#include "menu_display.h"
s32 GameState_HasUnlockedInventoryEntry(u32);
s32 SolarFruit_IsInventorySlotEquipped(u32);
s32 Inventory_IsEquipmentAssigned(u32);
s32 GameState_IsConsumableSlotEquipped(u32);

u32 MenuDisplay_CanSwapCategoryEntries(void *hud, u32 first, u32 second, u32 category)
{
    if (first == second)
        return 0;
    if (PauseMenu_IsCategoryEntryEmpty(hud, first, category) == 0)
        return 1;
    if (PauseMenu_IsCategoryEntryEmpty(hud, second, category) == 0)
        return 1;
    return 0;
}

s32 MenuDisplay_GetCategoryEntryValue(u32 entry, s32 category)
{
    switch (category) {
        case 1:
            if (entry - 8 <= 0xb)
                return GameState_HasUnlockedInventoryEntry(entry - 8);
            break;
        case 0:
            if (entry <= 7)
                return SolarFruit_IsInventorySlotEquipped(entry);
            break;
        case 2:
            if (entry <= 0xf)
                return Inventory_IsEquipmentAssigned(entry);
            break;
        case 3:
            if (entry <= 0xf)
                return GameState_IsConsumableSlotEquipped(entry);
            break;
    }
    return 0;
}

s32 MenuDisplay_CountAvailableEntries(void *unused, s32 category)
{
    s32 count = 0;
    s32 i;
    switch (category) {
        case 2:
            for (i = 0; i < 16; i++)
                if (MenuDisplay_GetCategoryEntryValue(i, category))
                    count++;
            break;
        case 1:
            for (i = 8; i < 20; i++)
                if (MenuDisplay_GetCategoryEntryValue(i, category))
                    count++;
            break;
        case 0:
            for (i = 0; i < 8; i++)
                if (MenuDisplay_GetCategoryEntryValue(i, category))
                    count++;
            break;
    }
    return count;
}
