#include "global.h"
/* Reconstructed from B3 instructions; SIIRTC correspondence: docs/RTC.md. */
// CFLAGS: -O0 -mthumb-interwork
u8 sub_08249184(void)
{
    u8 i;
    u8 bit;
    u8 value;
    for (i = 0; i < 8; i++) {
        *(vu16 *)0x080000C4 = 4;
        *(vu16 *)0x080000C4 = 4;
        *(vu16 *)0x080000C4 = 4;
        *(vu16 *)0x080000C4 = 4;
        *(vu16 *)0x080000C4 = 4;
        *(vu16 *)0x080000C4 = 5;
        bit = (u16)(*(vu16 *)0x080000C4 & 2) >> 1;
        value = (value >> 1) | (bit << 7);
    }
    return value;
}
