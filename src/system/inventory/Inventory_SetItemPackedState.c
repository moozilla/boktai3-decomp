#include "system/game_state/game_state.h"

void Inventory_SetItemPackedState(s32 i, u16 value)
{
    *(i + gGameState->table100) = value;
}
