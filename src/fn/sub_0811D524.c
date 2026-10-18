#include "global.h"
extern u8 *gUnk_02000710;
s32 GameState_GetChallengeBestRank(u32 i)
{
    s32 r;
    if (i <= 11)
        r = *(s16 *)(i * 2 + gUnk_02000710 + 0x800);
    else
        r = 0;
    return r;
}
