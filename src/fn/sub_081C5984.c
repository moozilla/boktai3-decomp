#include "global.h"
#include "render.h"

void BgGfx_LoadTilemapAtPosition(u32, u32, u32);

void ChallengeMenu_EnableBackground(void)
{
    u32 m = 0x100;
    gDispCntShadow = m | gDispCntShadow;
    BgGfx_LoadTilemapAtPosition(1, 0, 0);
}
