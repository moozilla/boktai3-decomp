#include "global.h"
#include "game_state.h"
struct Context {
    u8 pad[0x4f0];
    u32 date;
    s32 hour, minute, second;
};
u32 SunClock_GetPackedDate(void);
s32 SunClock_GetBaseHour(void), SunClock_GetBaseMinute(void), SunClock_GetSecond(void);
void Solar_SaveExposureTimestamp(void)
{
    gGameState->date = SunClock_GetPackedDate();
    gGameState->hour = SunClock_GetBaseHour();
    gGameState->minute = SunClock_GetBaseMinute();
    gGameState->second = SunClock_GetSecond();
}
