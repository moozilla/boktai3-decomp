#include "global.h"
extern u8 *gGameState;
s32 GameState_GetChallengeBestScore(u32 i)
{
    s32 r;
    if (i <= 11)
        r = *(s32 *)(i * 4 + gGameState + 0x7c8);
    else
        r = -1;
    return r;
}
