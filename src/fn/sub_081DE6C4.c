#include "global.h"
s32 SunClock_GetPackedDate(void);
s32 SunClock_GetBaseHour(void);
s32 SunClock_GetBaseMinute(void);
void Password_LoadDate(u8 *p)
{
    s32 v, t;
    s32 i;
    u32 *flags;
    v = SunClock_GetPackedDate() >> 16;
    *(s32 *)(p + 0x6ec) = ((u32)v >> 12) * 1000 + ((u32)(v & 0xf00) >> 8) * 100 +
                          ((u32)(v & 0xf0) >> 4) * 10 + (v & 0xf);
    t = SunClock_GetPackedDate() & 0xff00;
    v = t >> 8;
    *(s32 *)(p + 0x6f0) = ((u32)v >> 4) * 10 + (v & 0xf);
    v = SunClock_GetPackedDate() & 0xff;
    *(s32 *)(p + 0x6f4) = ((u32)v >> 4) * 10 + (v & 0xf);
    *(s32 *)(p + 0x6f8) = SunClock_GetBaseHour();
    *(s32 *)(p + 0x6fc) = SunClock_GetBaseMinute();
    t = ~1;
    flags = (u32 *)(p + 0x240);
    for (i = 11; i >= 0; i--) {
        *flags &= t;
        flags += 0x18;
    }
}
