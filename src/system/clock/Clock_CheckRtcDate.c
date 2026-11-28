#include "system/clock/sun_clock.h"
s32 Clock_CheckRtcStatus(void);
void Clock_WaitCycles(s32);
void SunClock_SetDateTime(s32, s32, s32, s32, s32, s32, s32);
void Clock_CheckRtcDate(void)
{
    s32 status = 0;
    s32 i;
    for (i = 0; i < 5; i++) {
        status = Clock_CheckRtcStatus();
        if (status != -1)
            break;
        Clock_WaitCycles(200);
    }
    if (status == 1) {
        if (Clock_SyncFromRtc() == 1)
            gSunClock.pad9 = 0;
        else {
            SunClock_SetDateTime(2005, 7, 28, 12, 0, 0, status);
            gSunClock.pad9 = status;
        }
    } else {
        SunClock_SetDateTime(2005, 7, 28, 12, 0, 0, 1);
        gSunClock.pad9 = 1;
    }
}
