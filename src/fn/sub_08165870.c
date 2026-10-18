#define ResultPanel_DrawClockLabel sub_08165870
#include "global.h"
u16 *Hud_GetTilemapCell(u32, u32, u32);
void sub_08165870(u32 x, u32 y)
{
    u16 *p = Hud_GetTilemapCell(0, x, y);
    *p = 0xF07C;
    p++;
    *p = 0xF07D;
    p[1] = 0xF07F;
}
