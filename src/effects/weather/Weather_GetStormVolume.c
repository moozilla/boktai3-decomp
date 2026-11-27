#include "global.h"
extern u8 *gGameState;
s32 SunClock_GetPhase(void);
s32 Weather_GetStormVolume(s16 *p)
{
    s32 r, v;
    if (*(s16 *)(gGameState + 0x612) != 0)
        r = p[0x19];
    else
        r = (p[0x18] * p[0x19]) >> 6;
    v = SunClock_GetPhase();
    if ((u32)(v - 4) <= 1 || v == 0)
        r >>= 1;
    return r;
}
