#include "global.h"
// CFLAGS: -O0 -mthumb-interwork
extern u8 gUnk_030035C6;
u32 sub_08249040(u8);
u32 sub_082490E4(u8);
u8 sub_08249184(void);
struct Rtc { u32 year:8, month:8, day:8, weekday:8; u32 hour:8, minute:8, second:8, status:8; u32 alarmHour:8, alarmMinute:8; };
u8 sub_08248F0C(struct Rtc *p)
{
    u8 i;
    struct { u8 value:8; } alarm[2];
    if (gUnk_030035C6 == 1) return 0;
    gUnk_030035C6 = 1;
    alarm[0].value = (p->alarmHour & 0xf) + 10 * (u8)((p->alarmHour >> 4) & 0xf);
    if (alarm[0].value < 12) alarm[0].value = p->alarmHour | 0;
    else alarm[0].value = p->alarmHour | 0x80;
    alarm[1].value = p->alarmMinute;
    *(vu16 *)0x080000C4 = 1;
    *(vu16 *)0x080000C4 = 5;
    *(vu16 *)0x080000C6 = 7;
    sub_08249040(0x68);
    for (i = 0; i < 2; i++) sub_082490E4(*((u8 *)&alarm + i));
    *(vu16 *)0x080000C4 = 1;
    *(vu16 *)0x080000C4 = 1;
    gUnk_030035C6 = 0;
    return 1;
}
