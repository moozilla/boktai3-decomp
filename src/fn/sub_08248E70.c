#include "global.h"
#include "runtime.h"
/* Rtc_WriteTime: Writes hours minutes and seconds from record offsets four through six */
#define Rtc_WriteTime sub_08248E70
// CFLAGS: -O0 -mthumb-interwork
u32 Rtc_WriteCommandByte(u8);
u32 Rtc_WriteDataByte(u8);
u8 Rtc_ReadDataByte(void);
u8 sub_08248E70(u8 *p)
{
    u8 i;
    if (gRtcProtected == 1)
        return 0;
    gRtcProtected = 1;
    *(vu16 *)0x080000C4 = 1;
    *(vu16 *)0x080000C4 = 5;
    *(vu16 *)0x080000C6 = 7;
    Rtc_WriteCommandByte(0x66);
    for (i = 0; i < 3; i++)
        Rtc_WriteDataByte(p[i + 4]);
    *(vu16 *)0x080000C4 = 1;
    *(vu16 *)0x080000C4 = 1;
    gRtcProtected = 0;
    return 1;
}
