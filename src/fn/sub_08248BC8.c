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
u8 Rtc_WriteStatus(struct Rtc *p)
{
    u8 status;
    if (gRtcProtected == 1)
        return 0;
    gRtcProtected = 1;
    *(vu16 *)0x080000C4 = 1;
    *(vu16 *)0x080000C4 = 5;
    status = 0x40 | ((p->status & 4) << 3) | ((p->status & 2) << 2) | ((p->status & 1) << 1);
    *(vu16 *)0x080000C6 = 7;
    Rtc_WriteCommandByte(0x62);
    Rtc_WriteDataByte(status);
    *(vu16 *)0x080000C4 = 1;
    *(vu16 *)0x080000C4 = 1;
    gRtcProtected = 0;
    return 1;
}
