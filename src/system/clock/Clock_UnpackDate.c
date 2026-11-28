#include "global.h"
void Clock_UnpackDate(s32 *year, s32 *month, s32 *day, s32 date)
{
    *year = ((u32)date >> 28) * 1000 + ((date & 0x0f000000) >> 24) * 100 +
            ((date & 0x00f00000) >> 20) * 10 + ((date & 0x000f0000) >> 16);
    *month = ((date & 0x0000f000) >> 12) * 10 + ((date & 0x00000f00) >> 8);
    *day = ((date & 0x000000f0) >> 4) * 10 + (date & 0xf);
}
