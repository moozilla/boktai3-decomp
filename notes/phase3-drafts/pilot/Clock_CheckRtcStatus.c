#include "global.h"
void Clock_BeginRtcAccess(void);
u8 Rtc_CheckAndReset(void);
void Clock_EndRtcAccess(void);
s32 Clock_CheckRtcStatus(void)
{
    u8 status;
    Clock_BeginRtcAccess();
    status = Rtc_CheckAndReset();
    Clock_EndRtcAccess();
    if ((status & 15) == 1)
        return 1;
    return -1;
}
