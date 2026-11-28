#include "system/game_state/game_state.h"

s32 Inventory_GetConsumableId(u32 i)
{
    return *(i + gGameState->table160);
}
