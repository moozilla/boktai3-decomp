#include "scene_hud_display.h"
void SceneHud_DrawClock(u32 mode)
{
    u16 *output = SceneHud_GetMapCell(0, 20, 0);
    if (mode == 0) {
        *output++ = 0xF0BD;
        output[0] = 0xF0B1;
        output[1] = 0xF0BB;
        SceneHud_DrawDecimal(SunClock_GetBaseHour(), 10, 24, 0, mode);
        SceneHud_DrawDecimal(SunClock_GetBaseMinute(), 10, 27, 0, mode);
    } else if (mode == 1) {
        *output++ = 0xF07C;
        output[0] = 0xF07D;
        output[1] = 0xF07E;
        SceneHud_DrawDecimal(gSunClock.hourA, 10, 24, 0, 0);
        SceneHud_DrawDecimal(gSunClock.minuteA, 10, 27, 0, 0);
    } else {
        *output++ = 0xF07C;
        output[0] = 0xF07D;
        output[1] = 0xF07F;
        SceneHud_DrawDecimal(gSunClock.hourB, 10, 24, 0, 0);
        SceneHud_DrawDecimal(gSunClock.minuteB, 10, 27, 0, 0);
    }
}
