#include "global.h"
/* AnimPanel_HandleClockInput: up/down (with hold repeat) changes the selected year/month/day/hour/minute field with wraparound, then refreshes the digit animations */

s32 Mod(s32, s32);
s32 Clock_DaysInMonth(s32 year, s32 month);
void Sound_PlaySong(u32);
struct AnimPanelClock;
void AnimPanel_UpdateDigits(struct AnimPanelClock *);

struct AnimPanelKeys {
    u16 held;
    u16 pressed;
};
extern struct AnimPanelKeys gPadState;

/* Tentative clock-setting layout of the AnimPanel task. */
struct AnimPanelClock {
    u8 pad0[0x6D8];
    s32 cursor;
    u8 pad6DC[0x6EC - 0x6DC];
    s32 year;
    s32 month;
    s32 day;
    s32 hour;
    s32 minute;
    s32 prev[5];
    s32 repeatTimer;
};

enum { CLOCK_DIR_NONE, CLOCK_DIR_UP, CLOCK_DIR_DOWN };

void AnimPanel_HandleClockInput(struct AnimPanelClock *p)
{
    s32 dir = CLOCK_DIR_NONE;
    s32 days;

    if (gPadState.pressed & 0x40) {
        dir = CLOCK_DIR_UP;
    } else if (gPadState.pressed & 0x80) {
        dir = CLOCK_DIR_DOWN;
    } else if (gPadState.held & 0xC0) {
        if (++p->repeatTimer > 14 && Mod(p->repeatTimer, 5) == 0) {
            if (gPadState.held & 0x40)
                dir = CLOCK_DIR_UP;
            else
                dir = CLOCK_DIR_DOWN;
        }
    } else {
        p->repeatTimer = dir;
    }

    switch (p->cursor) {
        case 0:
            if (dir == CLOCK_DIR_UP) {
                if (p->year <= 2098) {
                    p->year++;
                    Sound_PlaySong(0x19A);
                }
            } else if (dir == CLOCK_DIR_DOWN) {
                if (p->year > 2000) {
                    p->year--;
                    Sound_PlaySong(0x19A);
                }
            }
            break;
        case 1:
            if (dir == CLOCK_DIR_UP) {
                if (p->month > 11)
                    p->month = 1;
                else
                    p->month++;
                Sound_PlaySong(0x19A);
            } else if (dir == CLOCK_DIR_DOWN) {
                if (p->month <= 1)
                    p->month = 12;
                else
                    p->month--;
                Sound_PlaySong(0x19A);
            }
            break;
        case 2:
            days = Clock_DaysInMonth(p->year, p->month);
            if (dir == CLOCK_DIR_UP) {
                if (p->day >= days)
                    p->day = 1;
                else
                    p->day++;
                Sound_PlaySong(0x19A);
            } else if (dir == CLOCK_DIR_DOWN) {
                if (p->day <= 1)
                    p->day = days;
                else
                    p->day--;
                Sound_PlaySong(0x19A);
            }
            break;
        case 3:
            if (dir == CLOCK_DIR_UP) {
                if (p->hour > 22)
                    p->hour = 0;
                else
                    p->hour++;
                Sound_PlaySong(0x19A);
            } else if (dir == CLOCK_DIR_DOWN) {
                if (p->hour <= 0)
                    p->hour = 23;
                else
                    p->hour--;
                Sound_PlaySong(0x19A);
            }
            break;
        case 4:
            if (dir == CLOCK_DIR_UP) {
                if (p->minute > 58)
                    p->minute = 0;
                else
                    p->minute++;
                Sound_PlaySong(0x19A);
            } else if (dir == CLOCK_DIR_DOWN) {
                if (p->minute <= 0)
                    p->minute = 59;
                else
                    p->minute--;
                Sound_PlaySong(0x19A);
            }
            break;
    }
    AnimPanel_UpdateDigits(p);
}
