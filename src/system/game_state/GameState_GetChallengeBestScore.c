#include "global.h"
#include "system/game_state/game_state.h"
s32 GameState_GetChallengeBestScore(u32 i)
{
    s32 result;

    if (i <= 11)
        result = *(i + gGameState->unk7C8);
    else
        result = -1;
    return result;
}
