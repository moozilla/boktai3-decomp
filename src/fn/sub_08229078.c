#include "global.h"
#include "sun_clock.h"

/* Tentative: the date-dependent lunar age is rounded to tenths by the caller. */
void SunClock_SetMoonAge(s32 whole, s32 tenth)
{
    u32 age;
    age = gSunClock.moonAgeTenths = whole * 10 + tenth;
    if (age <= 34)
        gSunClock.moonPhase = 0;
    else if (age <= 69)
        gSunClock.moonPhase = 1;
    else if (age <= 104)
        gSunClock.moonPhase = 2;
    else if (age <= 149)
        gSunClock.moonPhase = 3;
    else if (age <= 159)
        gSunClock.moonPhase = 4;
    else if (age <= 209)
        gSunClock.moonPhase = 5;
    else if (age <= 244)
        gSunClock.moonPhase = 6;
    else
        gSunClock.moonPhase = 7;
}
