#include "global.h"
/* Reconstructed from B3 instructions; SIIRTC correspondence: docs/RTC.md. */
// CFLAGS: -O0 -mthumb-interwork
extern u8 gUnk_030035C6;
void sub_08249210(void);
void sub_08248970(void)
{
    sub_08249210();
    gUnk_030035C6 = 0;
}
