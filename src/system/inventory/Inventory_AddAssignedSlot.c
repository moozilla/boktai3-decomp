#include "system/game_state/game_state.h"

u32 Inventory_IsEquipmentAssigned(s32);
s32 ItemRecord_MeetsLevelRequirement(u32);

s32 Inventory_AddAssignedSlot(s32 slot)
{
    s32 list[8];
    s32 n;
    s32 i;
    s32 found;
    if (!ItemRecord_MeetsLevelRequirement(slot))
        return 0;
    n = 0;
    found = -1;
    for (i = 0; i < 16; i++) {
        if (i == slot) {
            if (n > 7)
                return 0;
            list[n] = i;
            found = n;
            n++;
        } else if (Inventory_IsEquipmentAssigned(i)) {
            if (n > 7)
                return 0;
            list[n] = i;
            n++;
        }
    }
    if (found < 0)
        return 0;
    gGameState->unk50 = found;
    for (; n < 8; n++)
        list[n] = -1;
    for (i = 0; i < 8; i++)
        *(i + gGameState->inventory) = list[i];
    return 1;
}
