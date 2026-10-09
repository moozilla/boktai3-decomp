#include "global.h"
// CFLAGS: -O0 -mthumb-interwork
extern u8 gUnk_030035C6;
u32 sub_08249040(u8);
u32 sub_082490E4(u8);
u8 sub_08249184(void);
struct Rtc { u32 year:8, month:8, day:8, weekday:8; u32 hour:8, minute:8, second:8, status:8; u32 alarmHour:8, alarmMinute:8; };
u8 sub_08248BC8(struct Rtc *p)
{
    u8 status;
    if (gUnk_030035C6 == 1) return 0;
    gUnk_030035C6 = 1;
    *(vu16 *)0x080000C4 = 1;
    *(vu16 *)0x080000C4 = 5;
    status = 0x40 | ((p->status & 4) << 3) | ((p->status & 2) << 2) | ((p->status & 1) << 1);
    *(vu16 *)0x080000C6 = 7;
    sub_08249040(0x62);
    sub_082490E4(status);
    *(vu16 *)0x080000C4 = 1;
    *(vu16 *)0x080000C4 = 1;
    gUnk_030035C6 = 0;
    return 1;
}
