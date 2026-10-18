// MATCHING_CONCESSION: register-pinning
#include "global.h"
void Clock_BeginRtcAccess(void);
u32 Rtc_CheckAndReset(void);
void Clock_EndRtcAccess(void);
s32 Clock_CheckRtcStatus(void)
{
    register u32 shifted asm("r5");
    register u32 status asm("r4");
    Clock_BeginRtcAccess();
    shifted = Rtc_CheckAndReset() << 24;
    status = shifted >> 24;
    Clock_EndRtcAccess();
    status &= 15;
    if (status == 1)
        return 1;
    return -1;
}
