#include "sun_clock.h"
u32 Clock_PackDate(void);
u8 Clock_CalculateWeekday(s32, s32, s32);
s32 sub_08229180(s32, s32, s32, u8, u8, u8);
/* Initialize the sun clock for a date (a, b, c) and base time; in mode 1 the
 * result comes from sub_08229180, otherwise 1. */
s32 SunClock_SetDateTime(s32 a, s32 b, s32 c, s32 hour, s32 minute, s32 extra, s32 mode)
{
    struct SunClock *k;
    s32 result;
    u32 stamp = Clock_PackDate();
    k = &gSunClock;
    k->stamp = stamp;
    k->field8 = Clock_CalculateWeekday(a, b, c);
    k->baseHour = hour;
    k->baseMinute = minute;
    k->extra6 = extra;
    k->extra7 = 0;
    if (mode == 1)
        result = sub_08229180(a, b, c, k->baseHour, k->baseMinute, k->extra6);
    else
        result = 1;
    SunClock_Update();
    return result;
}
