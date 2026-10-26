#include "global.h"

extern u8 *gGameState;

void ChallengeMenu_SetStoredRank(s32 a, s32 b, u32 c)
{
    s32 i = a * 3 + b;
    s32 h = i / 2;
    s32 odd = i & 1;
    u8 *q;
    u32 k = 0xf;
    c &= k;
    q = gGameState + 0x658 + h;
    if (odd) {
        c <<= 4;
        *q &= k;
    } else {
        *q &= 0xf0;
    }
    *q += c;
}
