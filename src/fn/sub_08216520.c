#include "global.h"
void BgGfx_LoadTilemap(u32, u32, u32, u32, u32, u32);
static inline s32 truncateTile(s32 value)
{
    s32 result;
    if (value >= 0)
        result = value >> 3;
    else
        result = -((-value) >> 3);
    return result;
}
void BgGfx_LoadTilemapAtPosition(u32 layer, s32 x, s32 y)
{
    s32 bottom = y + 80;
    s32 tileX = truncateTile(x - 8);
    s32 tileY = truncateTile(bottom - 128) & ~1;
    BgGfx_LoadTilemap(layer, tileX, tileY, x, y, 0);
}
