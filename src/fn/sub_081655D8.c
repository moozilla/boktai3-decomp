#include "global.h"
struct G {
    u8 f[0x7B0];
    s16 v;
};
extern struct G *gGameState;
u16 *Hud_GetTilemapCell(s32, s32, s32);
void SceneHud_DrawDecimalDigits(s32, s32, s32, s32, s32);
void Hud_DrawCounter7B0(s32 a, s32 b)
{
    u16 *p = Hud_GetTilemapCell(0, a - 1, b);
    *p = 0xF001;
    if (gGameState->v > 9999)
        gGameState->v = 9999;
    SceneHud_DrawDecimalDigits(gGameState->v, 1000, a, b, 0);
}
