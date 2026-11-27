#include "global.h"
void Clock_BeginRtcAccess(void);
u8 Rtc_CheckAndReset(void);
void Clock_EndRtcAccess(void);

// The probe byte keeps its top-aligned copy for the power-failure test (bit 7), as the SDK's
// RtcInit does with SIIRTCINFO_POWER; a power failure still reports the clock as usable.
s32 Clock_CheckRtcStatus(void)
{
    u32 probe, status;
    Clock_BeginRtcAccess();
    probe = Rtc_CheckAndReset() << 24;
    status = probe >> 24;
    Clock_EndRtcAccess();
    if ((status & 15) != 1)
        return -1;
    if ((s32)probe < 0)
        return 1;
    return 1;
}
