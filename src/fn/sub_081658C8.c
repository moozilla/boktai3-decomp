#define SceneHud_DrawSelectedClock sub_081658C8
#define SceneHud_DrawClockTimeB Hud_DrawSunClockTimeB
#define SceneHud_DrawClockTimeA Hud_DrawSunClockTimeA
#define SceneHud_DrawDecimalDigits sub_08165488
#include "global.h"
#include "scene_hud_display.h"
void sub_08165488(s32, s32, s32, s32, s32);
void Hud_DrawSunClockTimeB(s32, s32);
void Hud_DrawSunClockTimeA(s32, s32);
void sub_081658C8(struct SceneHudState *hud)
{
    if ((u8)(hud->mode - 1) <= 1 || hud->mode == 5 || hud->mode == 3 || hud->mode == 4 ||
        hud->mode == 6) {
        if (hud->clockSelection == 0) {
            sub_08165488(SunClock_GetBaseHour(), 10, 24, 0, 0);
            sub_08165488(SunClock_GetBaseMinute(), 10, 27, 0, 0);
        } else if (hud->clockSelection == 1) {
            Hud_DrawSunClockTimeA(24, 0);
        } else {
            Hud_DrawSunClockTimeB(24, 0);
        }
    }
}
