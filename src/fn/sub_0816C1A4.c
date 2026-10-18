#include "global.h"
u16 *Hud_GetTilemapCell(s32, s32, s32);
void MapScreen_ClearMapLayer(void)
{
    s32 y;
    s32 x;
    for (y = 0; y <= 0x1f; y++) {
        for (x = 0; x <= 0x1f; x++)
            *Hud_GetTilemapCell(1, x, y) = 0xE002;
    }
}
