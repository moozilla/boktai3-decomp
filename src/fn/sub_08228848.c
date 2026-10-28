#include "global.h"
#include "runtime.h"
extern u8 gSunClock[];
void Clock_UnpackDate(s32 *, s32 *, s32 *, u32);
u32 Clock_PackDate(s32, s32, s32);
s32 Clock_DaysInMonth(s32, s32);
s32 Clock_CalculateWeekday(s32, s32, s32);
void SunClock_TickFrame(void)
{
    if (++gSunClock[7] > 59) {
        u8 *p;
        gSunClock[7] = 0;
        p = gSunClock;
        if (++p[6] > 59) {
            p[6] = 0;
            if (++p[5] > 59) {
                p[5] = 0;
                if (++p[4] > 23) {
                    s32 year, month, day;
                    p[4] = 0;
                    Clock_UnpackDate(&year, &month, &day, *(u32 *)p);
                    day++;
                    if (day >= Clock_DaysInMonth(year, month)) {
                        day = 1;
                        month++;
                        if (month > 12) {
                            month = 1;
                            year++;
                            if (year > 2098)
                                year = 2099;
                        }
                    }
                    *(u32 *)p = Clock_PackDate(year, month, day);
                    p[8] = Clock_CalculateWeekday(year, month, day);
                }
            }
            gSleepResumePending = 1;
            {
                u8 *other = gSunClock;
                if (*(u16 *)(other + 12) == *(u16 *)(p + 4)) {
                    u16 *counter = (u16 *)(other + 20);
                    if (++*counter > 280)
                        *counter = 0;
                }
            }
        }
    }
}
