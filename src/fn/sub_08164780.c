#include "global.h"
extern u16 *Hud_GetTilemapCell(u32);
void Hud_DrawAlternateMeterLabel(void)
{
    u16 *p = Hud_GetTilemapCell(0);
    *p++ = 0xD08D;
    *p++ = 0xD08E;
    *p = 0xD08F;
}
