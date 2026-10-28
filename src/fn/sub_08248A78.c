#include "global.h"
#include "runtime.h"
/* Reconstructed from B3 instructions; SIIRTC correspondence: docs/RTC.md. */
// CFLAGS: -O0 -mthumb-interwork
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
    if (gRtcProtected == 1)
        return 0;
    gRtcProtected = 1;
    *(vu16 *)0x080000C4 = 1;
    *(vu16 *)0x080000C4 = 5;
    *(vu16 *)0x080000C6 = 7;
    Rtc_WriteCommandByte(0x60);
    *(vu16 *)0x080000C4 = 1;
    *(vu16 *)0x080000C4 = 1;
    gRtcProtected = 0;
    rtc.status = 0x40;
    result = Rtc_WriteStatus(&rtc);
    return result;
}
