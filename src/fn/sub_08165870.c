#include "global.h"
u16 *Hud_GetTilemapCell(u32, u32, u32);
void ResultPanel_DrawClockLabel(u32 x, u32 y)
{
    u16 *p = Hud_GetTilemapCell(0, x, y);
    *p = 0xF07C;
    p++;
    *p = 0xF07D;
    p[1] = 0xF07F;
}
