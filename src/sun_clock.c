#include "sun_clock.h"

/* Sun clock: time-of-day spans and the light phase (0x08228928-0x08228C10). */
/* Recompute the spans and the phase, initializing the clock first if needed. */
void SunClock_Refresh(void)
{
    if (!gClockSynced)
        Clock_SyncFromRtc();
    SunClock_Update();
}

/* Set time A (hour, minute), one hour later in daylight-saving mode. */
void SunClock_SetSunrise(s32 hour, s32 minute)
{
    struct SunClock *c = &gSunClock;
    c->hourA = hour;
    c->minuteA = minute;
    if (gSessionState[0xb]) {
        c->hourA = hour + 1;
        if (c->hourA > 23)
            c->hourA = hour - 23;
    }
    SunClock_Update();
}

/* Set time B (hour, minute), one hour later in daylight-saving mode. */
void SunClock_SetSunset(s32 hour, s32 minute)
{
    struct SunClock *c = &gSunClock;
    c->hourB = hour;
    c->minuteB = minute;
    if (gSessionState[0xb]) {
        c->hourB = hour + 1;
        if (c->hourB > 23)
            c->hourB = hour - 23;
    }
    SunClock_Update();
}
