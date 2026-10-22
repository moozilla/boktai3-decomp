#include "global.h"

u16 *Hud_GetTilemapCell(u32, u32, u32);
void SceneHud_FillDigitCells(s32 count, u32 x, u32 y, s32 sign)
{
    s32 tile = 1;
    u16 *p;
    s32 i;
    if (sign >= 0) {
        tile = 0x30;
        if (sign == 0)
            tile = 0x20;
    }
    p = Hud_GetTilemapCell(0, x, y);
    for (i = 0; i < count; i++)
        *p++ = (u16)(tile | 0xF000);
}
