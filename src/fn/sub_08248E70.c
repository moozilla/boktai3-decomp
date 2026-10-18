#include "global.h"
/* Rtc_WriteTime: Writes hours minutes and seconds from record offsets four through six */
#define Rtc_WriteTime sub_08248E70
// CFLAGS: -O0 -mthumb-interwork
extern u8 gUnk_030035C6;
u32 Rtc_WriteCommandByte(u8);
u32 Rtc_WriteDataByte(u8);
u8 Rtc_ReadDataByte(void);
u8 sub_08248E70(u8 *p)
{
    u8 i;
    if (gUnk_030035C6 == 1)
        return 0;
    gUnk_030035C6 = 1;
    *(vu16 *)0x080000C4 = 1;
    *(vu16 *)0x080000C4 = 5;
    *(vu16 *)0x080000C6 = 7;
    Rtc_WriteCommandByte(0x66);
    for (i = 0; i < 3; i++)
        Rtc_WriteDataByte(p[i + 4]);
    *(vu16 *)0x080000C4 = 1;
    *(vu16 *)0x080000C4 = 1;
    gUnk_030035C6 = 0;
    return 1;
}
