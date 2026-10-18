#include "global.h"
extern u16 *Hud_GetTilemapCell(u32);
void Hud_DrawEnergyLabel(void)
{
    u16 *p = Hud_GetTilemapCell(0);
    *p++ = 0xD050;
    *p++ = 0xD051;
    *p = 0xD052;
}
