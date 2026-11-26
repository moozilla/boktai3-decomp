#include "global.h"
extern u8 gUnk_08E8795C[][2];
extern u8 gSunClock[];

void sub_0822797C(u16 *first, u16 *second)
{
    *first = gUnk_08E8795C[gSunClock[14]][0];
    *second = gUnk_08E8795C[gSunClock[14]][1];
}
