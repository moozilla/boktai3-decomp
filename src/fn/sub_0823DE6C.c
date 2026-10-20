#include "global.h"
#include "scene_vec.h"

#define Terrain_ClassifyOffsetHeight sub_0823DE6C

struct TerrainMap {
    u32 pad;
    u8 *cells;
    u8 pad8[28];
    u16 rows[1];
};
extern struct TerrainMap *gUnk_030052F4;
extern u32 gUnk_030052F8, gUnk_030052FC;
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
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        index = 0;
    else
        index = gUnk_030052F4->rows[z] + x;
    if (gUnk_030052F4->cells == 0)
        cell = 0;
    else
        cell = gUnk_030052F4->cells + 12 + index * 4;
    if (Player_GetTerrainHeightAtPosition(cell, &pos) > pos.y)
        return 1;
    over = TerrainModifiers_FindCellOverride(index, 1);
    if (over != 0 && Player_GetTerrainHeightAtPosition(over + 4, &pos) > pos.y)
        return 2;
    return 0;
}
