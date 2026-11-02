#include "global.h"
/* Reconstructed from B3 instructions; SIIRTC correspondence: docs/RTC.md. */
// concession-ok(cflags): SDK RTC driver (SIIRTC), built unoptimized as the SDK built siirtc.c
// CFLAGS: -O0 -mthumb-interwork
void Rtc_EnableGpioReads(void)
{
    *(vu16 *)0x080000C8 = 1;
}
