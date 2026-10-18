// Tentative StatusPanel family and semantic names; based on static consumers.
#define StatusPanel_DrawSecondValue sub_08169B74
#define Tilemap_GetCell Hud_GetTilemapCell
#define StatusPanel_DrawDecimalValue sub_08169A9C
#include "global.h"
u16 *Hud_GetTilemapCell(s32, u32, u32);
int WirelessGame_GetRemoteOpponentHealthPercent(void);
void sub_08169A9C(void *, int, int, int);
void sub_08169B74(void *panel)
{
    u16 *p = Hud_GetTilemapCell(0, 20, 4);
    *p++ = 0xD00F;
    *p++ = 0xD010;
    *p = 0xD05B;
    sub_08169A9C(panel, 23, 4, WirelessGame_GetRemoteOpponentHealthPercent());
}
