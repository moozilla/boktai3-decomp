#include "global.h"
#include "engine/runtime.h"
#include "system/clock/sun_clock.h"
void Clock_UnpackDate(s32 *, s32 *, s32 *, u32);
u32 Clock_PackDate(s32, s32, s32);
s32 Clock_DaysInMonth(s32, s32);
s32 Clock_CalculateWeekday(s32, s32, s32);
void SunClock_TickFrame(void)
{
    gSunClock.extra7++;
        if (gSunClock.extra7 > 59) {
        struct SunClock *clock;
        gSunClock.extra7 = 0;
        clock = &gSunClock;
        clock->extra6++;
        if (clock->extra6 > 59) {
            clock->extra6 = 0;
            clock->baseMinute++;
        if (clock->baseMinute > 59) {
                clock->baseMinute = 0;
                clock->baseHour++;
        if (clock->baseHour > 23) {
                    s32 year, month, day;
                    clock->baseHour = 0;
                    Clock_UnpackDate(&year, &month, &day, clock->stamp);
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
                    clock->stamp = Clock_PackDate(year, month, day);
                    clock->field8 = Clock_CalculateWeekday(year, month, day);
                }
            }
            gSleepResumePending = 1;
            if (gSunClock.hourB == clock->baseHour && gSunClock.minuteB == clock->baseMinute) {
                gSunClock.moonAgeTenths++;
                if (gSunClock.moonAgeTenths > 280)
                    gSunClock.moonAgeTenths = 0;
            }
        }
    }
}
