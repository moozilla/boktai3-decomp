#include "global.h"
/* Reconstructed from B3 instructions; SIIRTC correspondence: docs/RTC.md. */
// CFLAGS: -O0 -mthumb-interwork
extern u8 gUnk_030035C6;
void Rtc_EnableGpioReads(void);
void Rtc_Initialize(void)
{
    Rtc_EnableGpioReads();
    gUnk_030035C6 = 0;
}
