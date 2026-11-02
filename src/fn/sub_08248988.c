#include "global.h"
#include "runtime.h"
/* Reconstructed from B3 instructions; SIIRTC correspondence: docs/RTC.md. */
// concession-ok(cflags): SDK RTC driver (SIIRTC), built unoptimized as the SDK built siirtc.c
// CFLAGS: -O0 -mthumb-interwork
void Rtc_DisableGpioReads(void);
void Rtc_Protect(void)
{
    Rtc_DisableGpioReads();
    gRtcProtected = 1;
}
