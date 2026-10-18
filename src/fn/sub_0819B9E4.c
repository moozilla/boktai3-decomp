#define Hresvelgr_HasCellOverride sub_0819B9E4
#define gTerrainMap               gUnk_030052F4
#define gTerrainMapWidth          gUnk_030052F8
#define gTerrainMapHeight         gUnk_030052FC
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
s32 sub_0819B9E4(struct Hresvelgr *actor)
{
    s32 cell = cellIndex(&actor->position);
    if (!TerrainModifiers_FindCellOverride(cell, 0)) {
        s32 offset;
        if (actor->side == 0)
            offset = gUnk_030052F4->neighborOffsets[1];
        else
            offset = gUnk_030052F4->neighborOffsets[2];
        cell += offset;
        if (!TerrainModifiers_FindCellOverride(cell, 0))
            return 0;
    }
    return 1;
}
