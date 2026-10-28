#include "global.h"
extern u8 gSunClock[];
s32 Clock_CheckRtcStatus(void);
s32 Clock_SyncFromRtc(void);
void Clock_WaitCycles(s32);
void SunClock_SetDateTime(s32, s32, s32, s32, s32, s32, s32);
void Clock_CheckRtcDate(void)
{
    s32 status = 0, i = 0, error = -1;
    while (i <= 4) {
        status = Clock_CheckRtcStatus();
        if (status != error)
            break;
        Clock_WaitCycles(200);
        i++;
    }
    if (status == 1) {
        if (Clock_SyncFromRtc() == 1)
            gSunClock[9] = 0;
        else {
            SunClock_SetDateTime(2005, 7, 28, 12, 0, 0, status);
            gSunClock[9] = status;
        }
    } else {
        s32 one;
        SunClock_SetDateTime(2005, 7, 28, 12, 0, 0, one = 1);
        gSunClock[9] = one;
    }
}
