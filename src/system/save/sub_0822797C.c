#include "global.h"
#include "system/clock/sun_clock.h"
extern u8 gUnk_08E8795C[][2];
void sub_0822797C(u16 *first, u16 *second)
{
    *first = gUnk_08E8795C[gSunClock.phase][0];
    *second = gUnk_08E8795C[gSunClock.phase][1];
}
