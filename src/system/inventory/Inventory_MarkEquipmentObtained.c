#include "system/game_state/game_state.h"

/* Set bit `index` of the 64-bit game-state flag set in flags72C and flags730. */
void Inventory_MarkEquipmentObtained(s32 index)
{
    if (index > 31)
        gGameState->flags730 |= 1 << (index - 32);
    else
        gGameState->flags72C |= 1 << index;
}
