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

#define C gUnk_03005430

/* A and B in the same hour, B first: in the window unless between B and A */
#define WRAP_WINDOW(day)                                                                           \
    {                                                                                              \
        u8 m = C.baseMinute;                                                                       \
        if (m < C.minuteB) {                                                                       \
            day = 1;                                                                               \
        } else {                                                                                   \
            if (m < C.minuteA)                                                                     \
                day = 0;                                                                           \
            else                                                                                   \
                day = 1;                                                                           \
        }                                                                                          \
    }

/* A and B in the same hour, A first: in the window between A and B */
#define PLAIN_WINDOW(day)                                                                          \
    {                                                                                              \
        u8 m = C.baseMinute;                                                                       \
        if (m < C.minuteA) {                                                                       \
            day = 0;                                                                               \
        } else {                                                                                   \
            if (m < C.minuteB)                                                                     \
                day = 1;                                                                           \
            else                                                                                   \
                day = 0;                                                                           \
        }                                                                                          \
    }

u8 SunClock_GetPhase(void)
{
    int day;
    if (C.baseHour < C.hourA) {
        if (C.hourB < C.hourA) {
            if (C.baseHour < C.hourB) {
                day = 1;
            } else if (C.baseHour == C.hourB) {
                u8 m = C.baseMinute;
                do {
                    day = 0;
                } while (0);
                if (m < C.minuteB)
                    day = 1;
            } else {
                day = 0;
            }
        } else if (C.hourB == C.hourA) {
            u8 mB = C.minuteB;
            do {
                day = 0;
            } while (0);
            if (mB < C.minuteA)
                day = 1;
        } else {
            day = 0;
        }
    } else if (C.baseHour == C.hourA) {
        if (C.hourB == C.hourA) {
            if (C.minuteB < C.minuteA) {
                WRAP_WINDOW(day);
            } else if (C.minuteA < C.minuteB) {
                PLAIN_WINDOW(day);
            } else {
                day = 0;
            }
        } else {
            u8 m = C.baseMinute;
            day = 1;
            if (m < C.minuteA)
                day = 0;
        }
    } else {
        if (C.baseHour < C.hourB) {
            day = 1;
        } else if (C.baseHour == C.hourB) {
            if (C.hourB == C.hourA) {
                if (C.minuteB < C.minuteA) {
                    WRAP_WINDOW(day);
                } else if (C.minuteA < C.minuteB) {
                    PLAIN_WINDOW(day);
                } else {
                    day = 0;
                }
            } else {
                u8 m = C.baseMinute;
                do {
                    day = 0;
                } while (0);
                if (m < C.minuteB)
                    day = 1;
            }
        } else if (C.hourB < C.hourA) {
            day = 1;
        } else if (C.hourB == C.hourA) {
            u8 mB = C.minuteB;
            do {
                day = 0;
            } while (0);
            if (mB < C.minuteA)
                day = 1;
        } else {
            day = 0;
        }
    }
    if (!day) {
        if (C.spanAHour == 0)
            return 0;
        if (C.spanBHour == 0)
            return 4;
        return 5;
    } else {
        if (C.spanAHour == 0)
            return 1;
        if (C.spanBHour == 0)
            return 3;
        return 2;
    }
}
