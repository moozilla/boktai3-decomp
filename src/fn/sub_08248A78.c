#include "global.h"
/* Reconstructed from B3 instructions; SIIRTC correspondence: docs/RTC.md. */
// CFLAGS: -O0 -mthumb-interwork
extern u8 gUnk_030035C6;
u32 Rtc_WriteCommandByte(u8);
u32 Rtc_WriteDataByte(u8);
u8 Rtc_ReadDataByte(void);
struct Rtc {
    u32 year : 8, month : 8, day : 8, weekday : 8;
    u32 hour : 8, minute : 8, second : 8, status : 8;
    u32 alarmHour : 8, alarmMinute : 8;
};
u8 Rtc_WriteStatus(struct Rtc *);
u8 Rtc_Reset(void)
{
    u8 result;
    struct Rtc rtc;
    if (gUnk_030035C6 == 1)
        return 0;
    gUnk_030035C6 = 1;
    *(vu16 *)0x080000C4 = 1;
    *(vu16 *)0x080000C4 = 5;
    *(vu16 *)0x080000C6 = 7;
    Rtc_WriteCommandByte(0x60);
    *(vu16 *)0x080000C4 = 1;
    *(vu16 *)0x080000C4 = 1;
    gUnk_030035C6 = 0;
    rtc.status = 0x40;
    result = Rtc_WriteStatus(&rtc);
    return result;
}
