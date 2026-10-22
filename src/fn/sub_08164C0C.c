#include "global.h"

extern u32 gUnk_030053F4;
u16 *Hud_GetTilemapCell(u32, u32, u32);
void ResultPanel_ClearFiveTiles(void)
{
    if (!(gUnk_030053F4 & 0x1000)) {
        u16 *tiles = Hud_GetTilemapCell(0, 25, 0);
        s32 i;
        for (i = 0; i < 5; i++)
            *tiles++ = 0xD000;
    }
}
