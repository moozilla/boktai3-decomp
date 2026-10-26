#include "global.h"
#include "scene_vec.h"
#include "engine.h"

struct TerrainMap {
    u32 pad;
    u8 *cells;
    u8 pad8[28];
    u16 rows[1];
};
void SceneVec_OffsetPolar(struct SceneVec *out, const struct SceneVec *in, s32 angle, s32 distance);
u8 *TerrainModifiers_FindCellOverride(int, int);
s32 Player_GetTerrainHeightAtPosition(void *, struct SceneVec *);

s32 Terrain_ClassifyOffsetHeight(struct SceneVec *origin, s32 angle, s32 distance)
{
    struct SceneVec pos;
    s32 x, z, index;
    u8 *cell;
    u8 *over;
    SceneVec_OffsetPolar(&pos, origin, angle, distance);
    x = pos.x >> 8;
    z = pos.z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        index = 0;
    else
        index = gTerrainMap->rows[z] + x;
    if (gTerrainMap->cells == 0)
        cell = 0;
    else
        cell = gTerrainMap->cells + 12 + index * 4;
    if (Player_GetTerrainHeightAtPosition(cell, &pos) > pos.y)
        return 1;
    over = TerrainModifiers_FindCellOverride(index, 1);
    if (over != 0 && Player_GetTerrainHeightAtPosition(over + 4, &pos) > pos.y)
        return 2;
    return 0;
}
