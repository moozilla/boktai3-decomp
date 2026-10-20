#include "global.h"

u16 *Hud_GetTilemapCell(u32, u32, u32);
void ResultPanel_FillTileRun(s32 count, u32 x, u32 y)
{
    u16 *p = Hud_GetTilemapCell(0, x, y);
    s32 i;
    for (i = 0; i < count; i++)
        *p++ = 0xF090;
}
