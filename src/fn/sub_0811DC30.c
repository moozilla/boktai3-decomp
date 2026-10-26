#include "global.h"
#include "game_state.h"
u32 SunClock_GetBaseHour(void);
u32 SunClock_GetBaseMinute(void);
void Text_DrawNumber(int, int, int, int, int, int, int);
void ClockDisplay_DrawBaseTime(s32 layer)
{
    s32 hour = SunClock_GetBaseHour();
    s32 minute = SunClock_GetBaseMinute();
    s32 value = gGameState->clockDisplayValue;
    Text_DrawNumber(layer, 24, 0, hour, 17, 2, 0);
    Text_DrawNumber(layer, 27, 0, minute, 17, 2, 0);
    Text_DrawNumber(layer, 24, 1, value, 17, 4, 0);
}
