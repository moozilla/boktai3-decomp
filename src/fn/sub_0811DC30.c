#define ClockDisplay_DrawBaseTime sub_0811DC30
#define Text_DrawNumber sub_08215F38
#define gGameState gUnk_02000710
#include "global.h"
#include "game_state.h"
u32 SunClock_GetBaseHour(void);
u32 SunClock_GetBaseMinute(void);
void sub_08215F38(int, int, int, int, int, int, int);
void sub_0811DC30(s32 layer)
{
    s32 hour = SunClock_GetBaseHour();
    s32 minute = SunClock_GetBaseMinute();
    s32 value = gUnk_02000710->clockDisplayValue;
    sub_08215F38(layer, 24, 0, hour, 17, 2, 0);
    sub_08215F38(layer, 27, 0, minute, 17, 2, 0);
    sub_08215F38(layer, 24, 1, value, 17, 4, 0);
}
