#include "global.h"

extern u16 gUnk_03004BD8;
void BgGfx_LoadTilemapAtPosition(u32, u32, u32);

void ChallengeMenu_EnableBackground(void)
{
    u32 m = 0x100;
    gUnk_03004BD8 = m | gUnk_03004BD8;
    BgGfx_LoadTilemapAtPosition(1, 0, 0);
}
