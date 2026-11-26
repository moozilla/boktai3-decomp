#ifndef GUARD_TERRAIN_SAMPLING_H
#define GUARD_TERRAIN_SAMPLING_H

#include "scene/terrain/terrain_collision.h"

u8 *TerrainModifiers_FindCellOverride(s32, s32);

/* Sample the current grid, including temporary cell overrides and sloped cells. */
static inline s32 Terrain_GetCellIndex(struct ContactPosition *pos)
{
    s32 x = pos->x >> 8, z = pos->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}

static inline s32 Terrain_SampleHeight(struct ContactPosition *position)
{
    s32 index = Terrain_GetCellIndex(position), height;
    u8 *cell = TerrainModifiers_FindCellOverride(index, 1);
    u32 kind;
    if (cell)
        cell = (u8 *)&((struct TerrainCellOverride *)cell)->cell;
    else
        cell = (u8 *)&((struct TerrainGrid *)gTerrainMap->cells)->cells[index];
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

#endif
