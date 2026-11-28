#include "system/game_state/game_state.h"
u32 Inventory_IsEquipmentAssigned(s32 slot)
{
    s32 i;

    for (i = 0; i < 8; i++) {
        if (gGameState->inventory[i] == slot)
            return 1;
    }
    return 0;
}
