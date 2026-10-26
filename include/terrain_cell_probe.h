#ifndef TERRAIN_CELL_PROBE_H
#define TERRAIN_CELL_PROBE_H

#include "global.h"
#include "engine.h"

/* 16-bit world position: x and z hold cell index in the high byte. */
struct Pos16 {
    u16 x, y, z, w;
};

u8 *TerrainModifiers_FindCellOverride(s32, s32);

static inline s32 cellIndexAt(struct Pos16 *p)
{
    s32 x = (s16)p->x >> 8;
    s32 z = (s16)p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}

static inline u8 *cellProps(s32 cell)
{
    u8 *p = TerrainModifiers_FindCellOverride(cell, 1);
    if (p)
        return p + 4;
    return gTerrainMap->cells + (cell * 4 + 12);
}

/* Sets pos->y to the terrain height under pos. */
static inline void Pos16_SnapToTerrain(struct Pos16 *pos)
{
    u8 *props = cellProps(cellIndexAt(pos));
    u32 type = *props >> 4;
    u32 h = (*props & 0xf) << 8;
    switch (type) {
        case 1:
            h -= (u8)pos->z;
            break;
        case 2:
            h -= (u8)pos->x;
            break;
    }
    pos->y = h;
}

#endif
