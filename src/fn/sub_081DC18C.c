#include "global.h"

void Bg_SetLayerMap(u32, u32, u32);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);

void Menu_SetBackgroundMap(u32 p, u32 b)
{
    Bg_SetLayerMap(3, p, b);
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
}
