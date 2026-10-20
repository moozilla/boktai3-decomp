#include "sun_clock.h"

#ifdef NONMATCHING
// fuzzy: 72.7%
/* Structure matches exactly; only the day flag (r4 here, r3 in the ROM) and
   the clock address (r3/r4) are swapped: the address outranks day in global
   allocation (0.351 vs 0.317, tools/rasolve.py). */
#define C gUnk_03005430

/* A and B in the same hour */
#define WRAP_WINDOW(day)                                                                           \
    {                                                                                              \
        u8 m = C.baseMinute;                                                                       \
        if (m < C.minuteB)                                                                         \
            day = 1;                                                                               \
        else {                                                                                     \
            if (m < C.minuteA)                                                                     \
                day = 0;                                                                           \
            else                                                                                   \
                day = 1;                                                                           \
        }                                                                                          \
    }
#define PLAIN_WINDOW(day)                                                                          \
    {                                                                                              \
        u8 m = C.baseMinute;                                                                       \
        if (m < C.minuteA)                                                                         \
            day = 0;                                                                               \
        else {                                                                                     \
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
                day = 0;
                if (m < C.minuteB)
                    day = 1;
            } else {
                day = 0;
            }
        } else if (C.hourB == C.hourA) {
            u8 mB = C.minuteB;
            day = 0;
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
                day = 0;
                if (m < C.minuteB)
                    day = 1;
            }
        } else if (C.hourB < C.hourA) {
            day = 1;
        } else if (C.hourB == C.hourA) {
            u8 mB = C.minuteB;
            day = 0;
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
#else
INCLUDE_ASM("asm/nonmatching", SunClock_GetPhase);
#endif
