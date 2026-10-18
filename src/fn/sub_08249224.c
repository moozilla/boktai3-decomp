#include "global.h"
/* Reconstructed from B3 instructions; SIIRTC correspondence: docs/RTC.md. */
// CFLAGS: -O0 -mthumb-interwork
void Rtc_DisableGpioReads(void)
{
    *(vu16 *)0x080000C8 = 0;
}
