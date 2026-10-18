// Tentative StatusPanel family and semantic names; based on static consumers.
#define Tilemap_GetCell Hud_GetTilemapCell
#include "global.h"
u16 *Hud_GetTilemapCell(s32, u32, u32);
int WirelessGame_GetRemoteOpponentHealthPercent(void);
void StatusPanel_DrawDecimalValue(void *, int, int, int);
void StatusPanel_DrawSecondValue(void *panel)
{
    u16 *p = Hud_GetTilemapCell(0, 20, 4);
    *p++ = 0xD00F;
    *p++ = 0xD010;
    *p = 0xD05B;
    StatusPanel_DrawDecimalValue(panel, 23, 4, WirelessGame_GetRemoteOpponentHealthPercent());
}
