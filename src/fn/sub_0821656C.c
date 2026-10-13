#include "global.h"

void BgGfx_LoadTilemap(u32, u32, u32, u32, u32, u32);

void BgGfx_LoadTilemapSimple(u32 a, u32 b, u32 c, u32 d, u32 e)
{
    BgGfx_LoadTilemap(a, b, c, d, e, 0);
}
