#include "global.h"
#include "system/game_state/game_state.h"
u32 SunClock_GetPackedDate(void);
s32 SunClock_GetBaseHour(void), SunClock_GetBaseMinute(void), SunClock_GetSecond(void);
void Solar_SaveExposureTimestamp(void)
{
    gGameState->date = SunClock_GetPackedDate();
    gGameState->hour = SunClock_GetBaseHour();
    gGameState->minute = SunClock_GetBaseMinute();
    gGameState->second = SunClock_GetSecond();
}
