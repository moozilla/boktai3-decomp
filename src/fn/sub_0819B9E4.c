#define gTerrainMapWidth  gTerrainWidth
#define gTerrainMapHeight gTerrainHeight
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
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
s32 Hresvelgr_HasCellOverride(struct Hresvelgr *actor)
{
    s32 cell = cellIndex(&actor->position);
    if (!TerrainModifiers_FindCellOverride(cell, 0)) {
        s32 offset;
        if (actor->side == 0)
            offset = gTerrainMap->neighborOffsets[1];
        else
            offset = gTerrainMap->neighborOffsets[2];
        cell += offset;
        if (!TerrainModifiers_FindCellOverride(cell, 0))
            return 0;
    }
    return 1;
}
