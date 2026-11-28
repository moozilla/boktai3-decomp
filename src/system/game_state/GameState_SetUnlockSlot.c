#include "global.h"
#include "system/game_state/game_state.h"
void GameState_SetUnlockSlot(u32 i, u16 v)
{
    *(i + gGameState->table4C0) = v;
}
