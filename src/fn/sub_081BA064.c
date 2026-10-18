#include "global.h"

void BgGfx_LoadTilemapAtPosition(s32, s32, s32);
void Hud_SetMode(s32);

void Registry2388_ConfigureDisplay(void)
{
    BgGfx_LoadTilemapAtPosition(3, 8, 0x48);
    BgGfx_LoadTilemapAtPosition(2, 0, 0);
    BgGfx_LoadTilemapAtPosition(0, 0, 0);
    Hud_SetMode(7);
}
