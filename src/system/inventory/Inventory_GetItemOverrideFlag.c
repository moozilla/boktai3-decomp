#include "system/game_state/game_state.h"

u32 Inventory_GetItemOverrideFlag(u32 i)
{
    return *(i + gGameState->table100) & 0x8000;
}
