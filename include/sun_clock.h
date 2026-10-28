#ifndef GUARD_SUN_CLOCK_H
#define GUARD_SUN_CLOCK_H

#include "global.h"
#include "ram.h"
#include "runtime.h"

/* Sun clock state at gSunClock: a base time, two times A and B, their
 * spans from the base, and the light phase/level derived from them. */
struct SunClock {
    u32 stamp;               /* 0x00: Clock_PackDate */
    u8 baseHour, baseMinute; /* 0x04 */
    u8 extra6, extra7;       /* 0x06 */
    u8 field8;               /* 0x08: Clock_CalculateWeekday */
    u8 pad9;
    u8 hourA, minuteA;         /* 0x0A */
    u8 hourB, minuteB;         /* 0x0C */
    u8 phase;                  /* 0x0E */
    u8 level;                  /* 0x0F */
    u8 spanBHour, spanBMinute; /* 0x10: B - base */
    u8 spanAHour, spanAMinute; /* 0x12: A - base */
    u16 moonAgeTenths;         /* 0x14: tentative lunar age, in tenths of a day */
    u8 moonPhase;              /* 0x16: eight ranges of moonAgeTenths */
    u8 pad17;
    s32 sunriseSunsetArgs[3]; /* 0x18: arguments of Time_CalculateSunriseSunset */
};
/* 0x03005430 */
extern struct SunClock gSunClock;
u32 SunClock_GetBaseHour(void);
u32 SunClock_GetBaseMinute(void);
s32 Div(s32, s32);
u8 SunClock_GetPhase(void);
void Clock_SyncFromRtc(void);
static inline void SunClock_Update(void)
{
    struct SunClock *c = &gSunClock;
    s32 bh, bm, ah, am, t;
    bh = c->hourB - c->baseHour;
    bm = c->minuteB - c->baseMinute;
    if (bh < 0)
        bh += 24;
    if (bm < 0) {
        if (bh > 0)
            bh--;
        else
            bh = 23;
        bm += 60;
    }
    ah = c->hourA - c->baseHour;
    am = c->minuteA - c->baseMinute;
    if (ah < 0)
        ah += 24;
    if (am < 0) {
        if (ah > 0)
            ah--;
        else
            ah = 23;
        am += 60;
    }
    c->spanBHour = bh;
    c->spanBMinute = bm;
    c->spanAHour = ah;
    c->spanAMinute = am;
    {
        s32 left;
        if ((u8)(c->phase - 1) <= 2)
            left = 60 - bm;
        else
            left = 60 - am;
        t = left << 6;
    }
    c->phase = SunClock_GetPhase();
    switch (c->phase) {
        case 0:
            c->level = Div(t, 60) >> 1;
            break;
        case 1:
            c->level = (Div(t, 60) >> 1) + 0x20;
            break;
        case 3:
            c->level = Div(t, 60);
            break;
        case 4:
            c->level = Div(t, 60);
            break;
        case 2:
        case 5:
            c->level = 0;
            break;
    }
}
/* 0x03005460 */
extern u8 gRtcDateTime
    [8]; /* RTC date/time bytes (BCD year, month, day, weekday, hour, minute, second) read and written by the Clock functions */

#endif
