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
u8 Rtc_ReadStatus(struct Rtc *);
u8 Rtc_Reset(void);
u8 Rtc_ReadTime(struct Rtc *);
u8 Rtc_CheckAndReset(void)
{
    u8 errors;
    struct Rtc rtc;
    if (!Rtc_ReadStatus(&rtc))
        return 0;
    errors = 0;
    if ((u8)(rtc.status & 0xc0) == 0x80 || (u8)(rtc.status & 0xc0) == 0) {
        if (!Rtc_Reset())
            return 0;
        errors++;
    }
    Rtc_ReadTime(&rtc);
    if ((u8)(rtc.second & 0x80)) {
        if (!Rtc_Reset())
            return (errors << 4) & 0xf0;
        errors++;
    }
    return (errors << 4) | 1;
}
