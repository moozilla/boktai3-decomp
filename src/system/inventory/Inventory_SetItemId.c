#include "system/game_state/game_state.h"

void Inventory_SetItemId(u32 i, u16 id)
{
    *(i + gGameState->table0A0) = id;
}
