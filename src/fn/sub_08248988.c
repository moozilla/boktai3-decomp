#include "global.h"
/* Reconstructed from B3 instructions; SIIRTC correspondence: docs/RTC.md. */
// CFLAGS: -O0 -mthumb-interwork
extern u8 gUnk_030035C6;
void sub_08249224(void);
void sub_08248988(void)
{
    sub_08249224();
    gUnk_030035C6 = 1;
}
