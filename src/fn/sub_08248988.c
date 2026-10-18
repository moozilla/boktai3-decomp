#include "global.h"
/* Reconstructed from B3 instructions; SIIRTC correspondence: docs/RTC.md. */
// CFLAGS: -O0 -mthumb-interwork
extern u8 gUnk_030035C6;
void Rtc_DisableGpioReads(void);
void Rtc_Protect(void)
{
    Rtc_DisableGpioReads();
    gUnk_030035C6 = 1;
}
