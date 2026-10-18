// Tentative StatusPanel family and semantic names; based on static consumers.
#define StatusPanel_DrawFirstValue sub_08169B38
#define Tilemap_GetCell Hud_GetTilemapCell
#define StatusPanel_DrawDecimalValue sub_08169A9C
#include "global.h"
u16 *Hud_GetTilemapCell(s32, u32, u32);
int WirelessGame_GetRemotePlayerHealthPercent(void);
void sub_08169A9C(void *, int, int, int);
void sub_08169B38(void *panel)
{
    u16 *p = Hud_GetTilemapCell(0, 19, 3);
    *p++ = 0xD04D;
    *p++ = 0xD04E;
    *p = 0xD05B;
    sub_08169A9C(panel, 22, 3, WirelessGame_GetRemotePlayerHealthPercent());
}
