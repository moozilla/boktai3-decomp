#include "sun_clock.h"

/* The light phase from the clock: whether the base (current) time lies in the
   window from time A to time B (which may wrap past midnight, or lie inside
   one hour), then which of the three in- or out-of-window states it is: the
   hour before A (spanAHour == 0), the hour before B (spanBHour == 0), or
   neither. SunClock_Update turns the phase into a light level.

   Every comparison reads its first operand before the result is set, and the
   four `day = 0` presets sit in empty do/while (0) blocks. flow.c weights a
   register's references by loop depth, so the blocks raise day's allocation
   priority above the clock address's (0.383 against 0.351) and day takes r3,
   as in the ROM; without them they swap r3/r4 (docs/REGALLOC.md). */

/* A and B in the same hour, B first: in the window unless between B and A */
#define WRAP_WINDOW(day)                                                                           \
    {                                                                                              \
        u8 m = gSunClock.baseMinute;                                                               \
        if (m < gSunClock.minuteB) {                                                               \
            day = 1;                                                                               \
        } else {                                                                                   \
            if (m < gSunClock.minuteA)                                                             \
                day = 0;                                                                           \
            else                                                                                   \
                day = 1;                                                                           \
        }                                                                                          \
    }

/* A and B in the same hour, A first: in the window between A and B */
#define PLAIN_WINDOW(day)                                                                          \
    {                                                                                              \
        u8 m = gSunClock.baseMinute;                                                               \
        if (m < gSunClock.minuteA) {                                                               \
            day = 0;                                                                               \
        } else {                                                                                   \
            if (m < gSunClock.minuteB)                                                             \
                day = 1;                                                                           \
            else                                                                                   \
                day = 0;                                                                           \
        }                                                                                          \
    }

u8 SunClock_GetPhase(void)
{
    int day;
    if (gSunClock.baseHour < gSunClock.hourA) {
        if (gSunClock.hourB < gSunClock.hourA) {
            if (gSunClock.baseHour < gSunClock.hourB) {
                day = 1;
            } else if (gSunClock.baseHour == gSunClock.hourB) {
                u8 m = gSunClock.baseMinute;
                do {
                    day = 0;
                } while (0);
                if (m < gSunClock.minuteB)
                    day = 1;
            } else {
                day = 0;
            }
        } else if (gSunClock.hourB == gSunClock.hourA) {
            u8 mB = gSunClock.minuteB;
            do {
                day = 0;
            } while (0);
            if (mB < gSunClock.minuteA)
                day = 1;
        } else {
            day = 0;
        }
    } else if (gSunClock.baseHour == gSunClock.hourA) {
        if (gSunClock.hourB == gSunClock.hourA) {
            if (gSunClock.minuteB < gSunClock.minuteA) {
                WRAP_WINDOW(day);
            } else if (gSunClock.minuteA < gSunClock.minuteB) {
                PLAIN_WINDOW(day);
            } else {
                day = 0;
            }
        } else {
            u8 m = gSunClock.baseMinute;
            day = 1;
            if (m < gSunClock.minuteA)
                day = 0;
        }
    } else {
        if (gSunClock.baseHour < gSunClock.hourB) {
            day = 1;
        } else if (gSunClock.baseHour == gSunClock.hourB) {
            if (gSunClock.hourB == gSunClock.hourA) {
                if (gSunClock.minuteB < gSunClock.minuteA) {
                    WRAP_WINDOW(day);
                } else if (gSunClock.minuteA < gSunClock.minuteB) {
                    PLAIN_WINDOW(day);
                } else {
                    day = 0;
                }
            } else {
                u8 m = gSunClock.baseMinute;
                do {
                    day = 0;
                } while (0);
                if (m < gSunClock.minuteB)
                    day = 1;
            }
        } else if (gSunClock.hourB < gSunClock.hourA) {
            day = 1;
        } else if (gSunClock.hourB == gSunClock.hourA) {
            u8 mB = gSunClock.minuteB;
            do {
                day = 0;
            } while (0);
            if (mB < gSunClock.minuteA)
                day = 1;
        } else {
            day = 0;
        }
    }
    if (!day) {
        if (gSunClock.spanAHour == 0)
            return 0;
        if (gSunClock.spanBHour == 0)
            return 4;
        return 5;
    } else {
        if (gSunClock.spanAHour == 0)
            return 1;
        if (gSunClock.spanBHour == 0)
            return 3;
        return 2;
    }
}
