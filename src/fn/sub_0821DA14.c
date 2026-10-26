#include "global.h"
#include "engine.h"
// confidence=low: returns higher-cell flags used by SceneIndicator_CreateFromScript.
struct Position {
    s16 x, y, z, pad;
};
struct Sample {
    u32 x : 16, y : 16, z : 16, pad : 16;
};
u8 *TerrainModifiers_FindCellOverride(s32, s32);
static inline s32 CellIndex(struct Position *p)
{
    s32 x = p->x >> 8, z = p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline u8 *CellData(s32 index)
{
    u8 *p = TerrainModifiers_FindCellOverride(index, 1);
    if (p)
        return p + 4;
    return gTerrainMap->cells + (index * 4 + 12);
}
u32 Terrain_GetHigherNeighborMask(struct Position *p)
{
    struct Sample center, sample;
    u32 result = 0;
    u32 height;
    center.x = (p->x & ~255) | 128;
    center.y = p->y & ~255;
    center.z = (p->z & ~255) + 128;
    height = *CellData(CellIndex((struct Position *)&center)) & 15;
    sample.x = center.x - 256;
    sample.y = center.y;
    sample.z = center.z + 256;
    if ((*CellData(CellIndex((struct Position *)&sample)) & 15u) > height)
        result |= 1;
    sample.x = center.x + 0;
    sample.y = center.y;
    sample.z = center.z + 256;
    if ((*CellData(CellIndex((struct Position *)&sample)) & 15u) > height)
        result |= 2;
    sample.x = center.x + 256;
    sample.y = center.y;
    sample.z = center.z + 256;
    if ((*CellData(CellIndex((struct Position *)&sample)) & 15u) > height)
        result |= 4;
    sample.x = center.x + 256;
    sample.y = center.y;
    sample.z = center.z + 0;
    if ((*CellData(CellIndex((struct Position *)&sample)) & 15u) > height)
        result |= 8;
    sample.x = center.x + 256;
    sample.y = center.y;
    sample.z = center.z - 256;
    if ((*CellData(CellIndex((struct Position *)&sample)) & 15u) > height)
        result |= 16;
    return result;
}
