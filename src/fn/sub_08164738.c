#include "global.h"
extern u16 *Hud_GetTilemapCell(u32);
void Hud_DrawHealthLabel(void)
{
    u16 *p = Hud_GetTilemapCell(0);
    *p++ = 0xD040;
    *p++ = 0xD041;
    *p = 0xD042;
}
