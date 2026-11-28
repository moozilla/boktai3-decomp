#include "global.h"
#include "system/game_state/game_state.h"
#include "system/clock/sun_clock.h"
u32 SunClock_GetPackedDate(void);
s32 sub_0822CAA0(void)
{
    s32 minute;
    u32 beforeSunrise;
    s32 sunHour, sunMinute;
    struct GameState *state;

    gGameState->unk4E0 = SunClock_GetPackedDate();
    gGameState->unk4E4 = SunClock_GetBaseHour();
    minute = SunClock_GetBaseMinute();
    state = gGameState;
    state->unk4E8 = minute;
    sunHour = gSunClock.hourB;
    sunMinute = gSunClock.minuteB;
    beforeSunrise = 0;
    if (state->unk4E4 < sunHour || (state->unk4E4 == sunHour && minute < sunMinute))
        beforeSunrise = 1;
    gGameState->unk4EC = beforeSunrise;
}
