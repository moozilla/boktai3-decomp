#include "global.h"
#include "system/game_state/game_state.h"
s32 GameState_GetChallengeBestRank(u32 i)
{
    s32 result;

    if (i <= 11)
        result = *(i + gGameState->unk800);
    else
        result = 0;
    return result;
}
