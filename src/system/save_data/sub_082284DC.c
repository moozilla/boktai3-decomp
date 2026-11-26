#include "engine/shared/global.h"

void Rtc_Initialize(void);

void Clock_BeginRtcAccess(void)
{
    Rtc_Initialize();
}
