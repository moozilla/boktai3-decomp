#include "global.h"

/* Sun clock: time-of-day spans and the light phase (0x08228A10-0x08228C10). */
struct SunClock {
    u8 pad0[4];
    u8 baseHour, baseMinute; /* 0x04 */
    u8 pad6[4];
    u8 hourA, minuteA;         /* 0x0A */
    u8 hourB, minuteB;         /* 0x0C */
    u8 phase;                  /* 0x0E */
    u8 level;                  /* 0x0F */
    u8 spanBHour, spanBMinute; /* 0x10: B - base */
    u8 spanAHour, spanAMinute; /* 0x12: A - base */
};
extern struct SunClock gUnk_03005430;
extern u8 *gUnk_030053F8;
s32 Div(s32, s32);
u8 sub_08228DB8(void);
static inline void SunClock_Update(void)
{
    struct SunClock *c = &gUnk_03005430;
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
    c->phase = sub_08228DB8();
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
/* Set time A (hour, minute), one hour later in daylight-saving mode. */
void sub_08228A10(s32 hour, s32 minute)
{
    struct SunClock *c = &gUnk_03005430;
    c->hourA = hour;
    c->minuteA = minute;
    if (gUnk_030053F8[0xb]) {
        c->hourA = hour + 1;
        if (c->hourA > 23)
            c->hourA = hour - 23;
    }
    SunClock_Update();
}

/* Set time B (hour, minute), one hour later in daylight-saving mode. */
void sub_08228B10(s32 hour, s32 minute)
{
    struct SunClock *c = &gUnk_03005430;
    c->hourB = hour;
    c->minuteB = minute;
    if (gUnk_030053F8[0xb]) {
        c->hourB = hour + 1;
        if (c->hourB > 23)
            c->hourB = hour - 23;
    }
    SunClock_Update();
}
