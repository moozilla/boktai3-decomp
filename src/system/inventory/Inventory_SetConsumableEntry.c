#include "system/game_state/game_state.h"

void Inventory_SetFlag(u32);
void Inventory_SetConsumableEntry(u32 i, u32 id)
{
    Inventory_SetFlag(id);
    *(i + gGameState->table160) = id;
}
