#include "global.h"
void SceneHud_DrawActiveStatValue(s32, s32, s32, s32, s32, s32);
void Hud_DrawDefaultResourceCounts(s32 a, s32 b, s32 c, s32 d, s32 e)
{
    SceneHud_DrawActiveStatValue(a, b, c, d, e, 0);
}
