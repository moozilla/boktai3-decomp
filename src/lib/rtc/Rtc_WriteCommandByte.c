#include "global.h"
/* Reconstructed from B3 instructions; SIIRTC correspondence: docs/RTC.md. */
// concession-ok(cflags): SDK RTC driver (SIIRTC), built unoptimized as the SDK built siirtc.c
// CFLAGS: -O0 -mthumb-interwork
u32 Rtc_WriteCommandByte(u8 value)
{
    u8 i;
    u8 bit;
    for (i = 0; i < 8; i++) {
        bit = (value >> (7 - i)) & 1;
        *(vu16 *)0x080000C4 = (bit << 1) | 4;
        *(vu16 *)0x080000C4 = (bit << 1) | 4;
        *(vu16 *)0x080000C4 = (bit << 1) | 4;
        *(vu16 *)0x080000C4 = (bit << 1) | 5;
    }
}
