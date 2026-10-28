/* Provisional family and semantic names; inferred from static accesses and callers. */
#include "global.h"
#include "scene_pool.h"
#include "ram.h"
struct TerrainCoordinate {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
};
s32 Terrain_GetHeightAtPosition(struct ScenePoolVec32 *position)
{
    struct TerrainCoordinate point;
    s32 row, column, cell, index, offset;
    u8 *data;
    point.x = position->x >> 12;
    point.z = position->z >> 12;
    column = point.x < 0 ? 0 : point.x;
    row = point.z < 0 ? 0 : point.z;
    index = column >> 5;
    data = ((u8 *)gTerrainColumns) + gTerrainColumnIndices[index] * 160;
    cell = (column & 31) >> 3;
    offset = row >> 3;
    if (offset > 7)
        offset = 7;
    offset = offset * 4 + 48;
    offset += cell;
    return data[offset * 2];
}
