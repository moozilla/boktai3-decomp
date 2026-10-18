#define Hresvelgr_HasAdjacentOverride sub_0819BA7C
#define gTerrainMap                   gUnk_030052F4
#define gTerrainMapWidth              gUnk_030052F8
#define gTerrainMapHeight             gUnk_030052FC
#include "global.h"
#include "engine.h"
#include "scene_vec.h"
struct Hresvelgr {
    u8 unknown0[0x54];
    struct SceneVec position;
    u8 unknown5C[0x32B - 0x5C];
    u8 side;
};
u8 *TerrainModifiers_FindCellOverride(s32, s32);
static inline s32 cellIndex(struct SceneVec *pos)
{
    s32 x = pos->x >> 8, z = pos->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        return 0;
    return gUnk_030052F4->rows[z] + x;
}
s32 sub_0819BA7C(struct Hresvelgr *actor)
{
    s32 cell = cellIndex(&actor->position);
    s32 i;
    if (TerrainModifiers_FindCellOverride(cell, 0))
        return 1;
    for (i = 0; i <= 2; i++) {
        s32 direction = actor->side + i;
        s32 offset = gUnk_030052F4->neighborOffsets[direction & 3];
        if (TerrainModifiers_FindCellOverride(cell + offset, 0))
            return 1;
    }
    return 0;
}
