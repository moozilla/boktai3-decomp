#include "system/game_state/game_state.h"

u32 Inventory_GetItemStateValue(u32 i)
{
    return *(i + gGameState->table100) & 0x7FFF;
}
