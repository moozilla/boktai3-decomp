#include "global.h"
struct B {
    u8 f[0xc];
    u8 a;
    u8 b;
};
extern struct B gSunClock;
void SceneHud_DrawDecimalDigits(s32, s32, s32, s32, s32);
void Hud_DrawSunClockTimeB(s32 x, s32 y)
{
    SceneHud_DrawDecimalDigits(gSunClock.a, 10, x, y, 0);
    SceneHud_DrawDecimalDigits(gSunClock.b, 10, x + 3, y, 0);
}
