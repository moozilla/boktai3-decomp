#include "system/game_state/game_state.h"

u32 Inventory_GetItemOverrideFlag(u32);
s32 Inventory_GetItemId(u32 i)
{
    s32 id;

    if (Inventory_GetItemOverrideFlag(i) == 0)
        id = *(i + gGameState->table0A0);
    else
        id = 9;
    return id;
}
