#include "global.h"
#include "runtime.h"
/* SiiRtcSetAlarm: writes the BCD alarm hour (with AM/PM flag) and minute.
 * Reconstructed from B3 instructions; SIIRTC correspondence: docs/RTC.md.
 * The packed union makes the alarm buffer BLKmode, so agbcc forces its
 * address into a register before evaluating each bitfield store. */
// CFLAGS: -O0 -mthumb-interwork
u32 Rtc_WriteCommandByte(u8);
u32 Rtc_WriteDataByte(u8);

struct SiiRtcInfo {
    u8 year;
    u8 month;
    u8 day;
    u8 dayOfWeek;
    u8 hour;
    u8 minute;
    u8 second;
    u8 status;
    u8 alarmHour;
    u8 alarmMinute;
};

union AlarmData {
    struct {
        u8 hour : 8;
        u8 minute : 8;
    } __attribute__((packed)) f;
    u8 bytes[2];
} __attribute__((packed));

u8 sub_08248F0C(struct SiiRtcInfo *rtc)
{
    u8 i;
    union AlarmData alarmData;

    if (gRtcProtected == 1)
        return 0;
    gRtcProtected = 1;

    alarmData.f.hour = (rtc->alarmHour & 0xF) + 10 * (u8)((rtc->alarmHour >> 4) & 0xF);

    if (alarmData.f.hour < 12)
        alarmData.f.hour = rtc->alarmHour | 0;
    else
        alarmData.f.hour = rtc->alarmHour | 0x80;

    alarmData.f.minute = rtc->alarmMinute;

    *(vu16 *)0x080000C4 = 1;
    *(vu16 *)0x080000C4 = 5;
    *(vu16 *)0x080000C6 = 7;
    Rtc_WriteCommandByte(0x68);
    for (i = 0; i < 2; i++)
        Rtc_WriteDataByte(alarmData.bytes[i]);
    *(vu16 *)0x080000C4 = 1;
    *(vu16 *)0x080000C4 = 1;
    gRtcProtected = 0;
    return 1;
}
