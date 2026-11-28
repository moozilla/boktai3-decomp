#include "global.h"
#include "scene/areas/area_effect_link.h"
u16 *Hud_GetTilemapCell(int, int, int);
void SceneHud_DrawSlotMarkers(void)
{
    u8 *kinds;
    u16 *cell;
    s32 i;
    if (gLinkSession) {
        u8 unavailable = gLinkSession->unavailable;
        kinds = (u8 *)gLinkSession->playerKinds;
        if (unavailable)
            kinds = 0;
    } else {
        kinds = 0;
    }
    cell = Hud_GetTilemapCell(0, 25, 2);
    for (i = 0; i < 5; i++) {
        if (*kinds == 0)
            *cell = 0xE0C0;
        else if (*kinds == 1)
            *cell = 0xE0C3;
        else if (*kinds == 2)
            *cell = 0xE0C2;
        else if (*kinds == 3)
            *cell = 0xE0C1;
        else
            *cell = 0xE0C4;
        cell++;
        kinds++;
    }
}
