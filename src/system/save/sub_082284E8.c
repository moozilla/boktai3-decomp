#include "global.h"

void Rtc_Protect(void);

void Clock_EndRtcAccess(void)
{
    Rtc_Protect();
}
