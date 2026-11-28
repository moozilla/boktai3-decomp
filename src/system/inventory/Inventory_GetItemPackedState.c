#include "system/game_state/game_state.h"

s32 Inventory_GetItemPackedState(s32 i)
{
    return *(i + gGameState->table100);
}
