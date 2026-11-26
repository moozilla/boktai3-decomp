#include "global.h"
#include "system/game_state/game_state.h"
void PlayerBarrier_DecrementDuration(void)
{
    if (gGameState->unk77C <= 1)
        gGameState->unk77C = 0;
    else
        gGameState->unk77C--;
}
