#include "global.h"
struct G {
    u8 f[0x77C];
    s32 v;
};
extern struct G *gGameState;
u16 *Hud_GetTilemapCell(s32, s32, s32);
void SceneHud_DrawDecimalDigits(s32, s32, s32, s32, s32);
void Hud_DrawHeaderCounter(void)
{
    u16 *p = Hud_GetTilemapCell(0, 0x18, 1);
    *p = 0xF001;
    if (gGameState->v > 9999)
        gGameState->v = 9999;
    SceneHud_DrawDecimalDigits(gGameState->v, 1000, 0x19, 1, 0);
}
