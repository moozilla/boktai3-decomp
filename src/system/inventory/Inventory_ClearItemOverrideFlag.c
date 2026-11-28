#include "system/game_state/game_state.h"

void Inventory_ClearItemOverrideFlag(u32 i)
{
    *(i + gGameState->table100) &= 0x7FFF;
}
