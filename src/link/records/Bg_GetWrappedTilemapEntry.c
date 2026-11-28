#include "global.h"
#include "engine/video/render.h"

u16 *Bg_GetWrappedTilemapEntry(u32 layer, u32 x, u32 y)
{
    struct BackgroundLayer *record = &gBgRecords[layer];
    u16 *map = record->map;

    map += x & 0x1F;
    map += (y & 0x1F) * 32;
    return map;
}
