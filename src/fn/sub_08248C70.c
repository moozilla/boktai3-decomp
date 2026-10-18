#include "global.h"
/* Reconstructed from B3 instructions; SIIRTC correspondence: docs/RTC.md. */
// CFLAGS: -O0 -mthumb-interwork
extern u8 gUnk_030035C6;
u32 Rtc_WriteCommandByte(u8);
u32 Rtc_WriteDataByte(u8);
u8 Rtc_ReadDataByte(void);
u8 Rtc_ReadDateTime(u8 *p)
{
    u8 i;
    if (gUnk_030035C6 == 1)
        return 0;
    gUnk_030035C6 = 1;
    *(vu16 *)0x080000C4 = 1;
    *(vu16 *)0x080000C4 = 5;
    *(vu16 *)0x080000C6 = 7;
    Rtc_WriteCommandByte(0x65);
    *(vu16 *)0x080000C6 = 5;
    for (i = 0; i < 7; i++)
        p[i] = Rtc_ReadDataByte();
    p[4] &= 0x7f;
    *(vu16 *)0x080000C4 = 1;
    *(vu16 *)0x080000C4 = 1;
    gUnk_030035C6 = 0;
    return 1;
}
