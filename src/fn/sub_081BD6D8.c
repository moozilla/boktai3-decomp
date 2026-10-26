#include "global.h"
extern u8 *gGameState;
s32 DarkLoan_GetDefaultRate(void)
{
    s16 v = *(s16 *)(gGameState + 0x876);
    s32 r;
    if (v <= 1)
        r = 9;
    else if (v <= 9)
        r = 11 - v;
    else
        r = 2;
    return r;
}
