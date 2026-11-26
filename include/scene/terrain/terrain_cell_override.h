#ifndef GUARD_TERRAIN_CELL_OVERRIDE_H
#define GUARD_TERRAIN_CELL_OVERRIDE_H
#include "global.h"
/* TerrainModifiers_InitCell packs the tile and height in the same byte. */
struct TerrainCellOverride {
    u16 id, cell;
    u8 tileHeight, kind;
    u16 flags;
    struct TerrainCellOverride *previous, *next;
};
#endif
