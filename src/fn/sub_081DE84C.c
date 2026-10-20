#include "global.h"
/* AnimPanel_UpdateDigits: redraws the year/month/day/hour/minute digit animations whose value changed and clamps the day to the month length */
#define AnimPanel_UpdateDigits sub_081DE84C

s32 Mod(s32, s32);
s32 Div(s32, s32);
s32 Anim_LoadKeyframe(void *anim, void *data, u32 index, u32 mode);
s32 Clock_DaysInMonth(s32 year, s32 month);

/* Tentative clock-setting layout of the AnimPanel task. */
struct AnimPanelClock {
    u8 task[0x18];
    u8 mainAnimations[4][0x88];
    u8 yearDigits[4][0x60];
    u8 monthDigits[2][0x60];
    u8 dayDigits[2][0x60];
    u8 hourDigits[2][0x60];
    u8 minuteDigits[2][0x60];
    u8 digitData[0x6EC - 0x6B8];
    s32 year;
    s32 month;
    s32 day;
    s32 hour;
    s32 minute;
    s32 prevYear;
    s32 prevMonth;
    s32 prevDay;
    s32 prevHour;
    s32 prevMinute;
};

void AnimPanel_UpdateDigits(struct AnimPanelClock *p)
{
    s32 v, i;
    u8(*digits)[0x60];

    if (p->year != p->prevYear) {
        digits = p->yearDigits;
        v = p->year;
        for (i = 0; i <= 3; i++) {
            Anim_LoadKeyframe(digits[3 - i], p->digitData, (u16)Mod(v, 10), 0);
            v = Div(v, 10);
        }
    }
    if (p->month != p->prevMonth) {
        digits = p->monthDigits;
        v = p->month;
        for (i = 0; i <= 1; i++) {
            Anim_LoadKeyframe(digits[1 - i], p->digitData, (u16)Mod(v, 10), 0);
            v = Div(v, 10);
        }
    }
    if (p->year != p->prevYear || p->month != p->prevMonth) {
        v = Clock_DaysInMonth(p->year, p->month);
        if (p->day > v)
            p->day = v;
    }
    if (p->day != p->prevDay) {
        digits = p->dayDigits;
        v = p->day;
        for (i = 0; i <= 1; i++) {
            Anim_LoadKeyframe(digits[1 - i], p->digitData, (u16)Mod(v, 10), 0);
            v = Div(v, 10);
        }
    }
    if (p->hour != p->prevHour) {
        digits = p->hourDigits;
        v = p->hour;
        for (i = 0; i <= 1; i++) {
            Anim_LoadKeyframe(digits[1 - i], p->digitData, (u16)Mod(v, 10), 0);
            v = Div(v, 10);
        }
    }
    if (p->minute != p->prevMinute) {
        digits = p->minuteDigits;
        v = p->minute;
        for (i = 0; i <= 1; i++) {
            Anim_LoadKeyframe(digits[1 - i], p->digitData, (u16)Mod(v, 10), 0);
            v = Div(v, 10);
        }
    }
}
