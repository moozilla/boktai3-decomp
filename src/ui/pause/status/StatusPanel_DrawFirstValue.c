// Tentative StatusPanel family and semantic names; based on static consumers.
#define Tilemap_GetCell Hud_GetTilemapCell
#include "global.h"
u16 *Hud_GetTilemapCell(s32, u32, u32);
int WirelessGame_GetRemotePlayerHealthPercent(void);
void StatusPanel_DrawDecimalValue(void *, int, int, int);
void StatusPanel_DrawFirstValue(void *panel)
{
    u16 *p = Hud_GetTilemapCell(0, 19, 3);
    *p++ = 0xD04D;
    *p++ = 0xD04E;
    *p = 0xD05B;
    StatusPanel_DrawDecimalValue(panel, 22, 3, WirelessGame_GetRemotePlayerHealthPercent());
}
