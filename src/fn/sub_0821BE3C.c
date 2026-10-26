#include "global.h"
#include "engine.h"

struct TerrainPos {
    s16 x, y, z;
};
u8 *TerrainModifiers_FindCellOverride(s32, s32);

static inline s32 cellIndex(struct TerrainPos *pos)
{
    s32 x = pos->x >> 8, z = pos->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline s32 groundHeight(struct TerrainPos *position)
{
    s32 index = cellIndex(position), height;
    u8 *cell = TerrainModifiers_FindCellOverride(index, 1);
    u32 kind;
    if (cell)
        cell += 4;
    else
        cell = gTerrainMap->cells + 12 + index * 4;
    kind = *cell >> 4;
    height = (*cell & 15) << 8;
    switch (kind) {
        case 1:
            height -= (u8)position->z;
            break;
        case 2:
            height -= (u8)position->x;
            break;
    }
    return height;
}

u16 sub_0821BE3C(struct TerrainPos *a, struct TerrainPos *b)
{
    s32 ha = groundHeight(a);
    s32 d = ha - groundHeight(b);
    if (d < 0)
        d = -d;
    return d;
}
