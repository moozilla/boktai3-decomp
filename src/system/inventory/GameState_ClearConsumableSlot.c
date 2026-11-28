#include "system/game_state/game_state.h"

void GameState_ClearConsumableSlot(u32 i)
{
    *(i + gGameState->table160) = -1;
}
