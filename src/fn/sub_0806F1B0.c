#include "global.h"
#include "render.h"
/* SceneTilemap_GetWrappedCell: Returns a layer tilemap cell pointer with column and row wrapped to the 32 by 32 map */
#define SceneTilemap_GetWrappedCell sub_0806F1B0
#define SceneTilemap_GetWrappedCell sub_0806F1B0
u16 *SceneTilemap_GetWrappedCell(s32 layer, u32 column, u32 row)
{
    struct BackgroundLayer *entry = &gBgRecords[layer];
    u16 *p;
    p = entry->map;
    p += column & 31;
    p += (row & 31) * 32;
    return p;
}
