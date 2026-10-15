#include "global.h"
#define SceneTilemap_GetWrappedCell sub_0806F1B0
struct SceneTilemapLayer {
    u8 unknown0[0x2c];
    u16 *map;
};
extern struct SceneTilemapLayer gUnk_03004C30[];
u16 *SceneTilemap_GetWrappedCell(s32 layer, u32 column, u32 row)
{
    struct SceneTilemapLayer *entry = &gUnk_03004C30[layer];
    u16 *p;
    p = entry->map;
    p += column & 31;
    p += (row & 31) * 32;
    return p;
}
