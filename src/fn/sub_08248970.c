#include "global.h"
#include "runtime.h"
/* Reconstructed from B3 instructions; SIIRTC correspondence: docs/RTC.md. */
// CFLAGS: -O0 -mthumb-interwork
void Rtc_EnableGpioReads(void);
void Rtc_Initialize(void)
{
    Rtc_EnableGpioReads();
    gRtcProtected = 0;
}
