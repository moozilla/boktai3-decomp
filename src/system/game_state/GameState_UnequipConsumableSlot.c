#include "global.h"
#include "system/game_state/game_state.h"
void GameState_UnequipConsumableSlot(s32 slot)
{
    s32 i;

    for (i = 0; i < 4; i++) {
        if (*(i + gGameState->party) == slot)
            *(i + gGameState->party) = -1;
    }
}
