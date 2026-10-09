#include "global.h"
/* Reconstructed from B3 instructions; SIIRTC correspondence: docs/RTC.md. */
// CFLAGS: -O0 -mthumb-interwork
extern u8 gUnk_030035C6;
u32 sub_08249040(u8);
u32 sub_082490E4(u8);
u8 sub_08249184(void);
struct Rtc { u32 year:8, month:8, day:8, weekday:8; u32 hour:8, minute:8, second:8, status:8; u32 alarmHour:8, alarmMinute:8; };
u8 sub_08248AFC(struct Rtc *);
u8 sub_08248A78(void);
u8 sub_08248DBC(struct Rtc *);
u8 sub_082489A0(void)
{
    u8 errors;
    struct Rtc rtc;
    if (!sub_08248AFC(&rtc)) return 0;
    errors = 0;
    if ((u8)(rtc.status & 0xc0) == 0x80 || (u8)(rtc.status & 0xc0) == 0) {
        if (!sub_08248A78()) return 0;
        errors++;
    }
    sub_08248DBC(&rtc);
    if ((u8)(rtc.second & 0x80)) {
        if (!sub_08248A78()) return (errors << 4) & 0xf0;
        errors++;
    }
    return (errors << 4) | 1;
}
