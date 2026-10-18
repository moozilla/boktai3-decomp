#include "global.h"
void BgGfx_LoadTilemapAtPosition(u32, u32, u32);
void ScenePaletteFade_SetBackgroundOffsets(void)
{
    BgGfx_LoadTilemapAtPosition(2, 8, 0x34);
    BgGfx_LoadTilemapAtPosition(3, 8, 0x34);
}
