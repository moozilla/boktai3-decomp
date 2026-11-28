#include "global.h"
#include "system/game_state/game_state.h"
#include "system/clock/sun_clock.h"
u32 SunClock_GetPackedDate(void);
void Clock_UnpackDate(s32 *, s32 *, s32 *, s32);
u32 Clock_CalculateDateDifference(s32, s32, s32, s32, s32, s32);
u32 Solar_HasExposureCooldownElapsed(void);
s32 sub_0822CAA0(void);
void sub_0822CB08(void)
{
    u32 date = SunClock_GetPackedDate();
    s32 hour = SunClock_GetBaseHour();
    s32 minute = SunClock_GetBaseMinute();
    s32 sunHour = gSunClock.hourB;
    s32 sunMinute = gSunClock.minuteB;
    s32 a, b, c, d, e, f;
    u32 diff;
    u32 reset;

    Clock_UnpackDate(&a, &b, &c, date);
    Clock_UnpackDate(&d, &e, &f, gGameState->unk4E0);
    diff = Clock_CalculateDateDifference(a, b, c, d, e, f);
    reset = 0;
    if (diff > 1)
        reset = 1;
    else if (diff == 1) {
        if (gGameState->unk4EC || hour > sunHour || (hour == sunHour && minute >= sunMinute))
            reset = 1;
    } else {
        u32 before, after;
        before = 0;
        if (gGameState->unk4E4 < sunHour)
            before = 1;
        else if (gGameState->unk4E4 == sunHour && gGameState->unk4E8 < sunMinute)
            before = 1;
        after = 0;
        if (hour > sunHour || (hour == sunHour && minute >= sunMinute))
            after = 1;
        if (before && after)
            reset = 1;
    }
    if (reset) {
        *(u16 *)gGameState->unk500 = 0;
        *(u16 *)&gGameState->playerParticleRate = 0;
        *(s32 *)gGameState->unk504 = 0;
        *(s32 *)&gGameState->unk504[4] = 0;
        *(u16 *)&gGameState->unk610 = 0;
        *(u16 *)&gGameState->unk612 = 0;
    }
    if (*(s32 *)gGameState->unk504 > 29999 && Solar_HasExposureCooldownElapsed()) {
        *(s32 *)gGameState->unk504 = 0;
        *(u16 *)&gGameState->playerParticleRate = 0;
    }
    sub_0822CAA0();
}
