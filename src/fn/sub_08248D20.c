#include "global.h"
#include "runtime.h"
/* Reconstructed from B3 instructions; SIIRTC correspondence: docs/RTC.md. */
// CFLAGS: -O0 -mthumb-interwork
u32 Rtc_WriteCommandByte(u8);
u32 Rtc_WriteDataByte(u8);
u8 Rtc_ReadDataByte(void);
u8 Rtc_WriteDateTime(u8 *p)
{
    u8 i;
    if (gRtcProtected == 1)
        return 0;
    gRtcProtected = 1;
    *(vu16 *)0x080000C4 = 1;
    *(vu16 *)0x080000C4 = 5;
    *(vu16 *)0x080000C6 = 7;
    Rtc_WriteCommandByte(0x64);
    for (i = 0; i < 7; i++)
        Rtc_WriteDataByte(p[i]);
    *(vu16 *)0x080000C4 = 1;
    *(vu16 *)0x080000C4 = 1;
    gRtcProtected = 0;
    return 1;
}
