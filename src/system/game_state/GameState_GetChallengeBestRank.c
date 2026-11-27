#include "global.h"
extern u8 *gGameState;
s32 GameState_GetChallengeBestRank(u32 i)
{
    s32 r;
    if (i <= 11)
        r = *(s16 *)(i * 2 + gGameState + 0x800);
    else
        r = 0;
    return r;
}
