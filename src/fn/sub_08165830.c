#include "global.h"
struct B {
    u8 f[0xa];
    u8 a;
    u8 b;
};
extern struct B gUnk_03005430;
void SceneHud_DrawDecimalDigits(s32, s32, s32, s32, s32);
void Hud_DrawSunClockTimeA(s32 x, s32 y)
{
    SceneHud_DrawDecimalDigits(gUnk_03005430.a, 10, x, y, 0);
    SceneHud_DrawDecimalDigits(gUnk_03005430.b, 10, x + 3, y, 0);
}
