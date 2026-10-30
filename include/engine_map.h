#ifndef GUARD_ENGINE_MAP_H
#define GUARD_ENGINE_MAP_H

#include "global.h"

/* Terrain/collision map header of the current scene. Terrain_LoadHP (08225C48)
 * fills it from the stage's "HP" resource (type AE6C, the third argument of the
 * script's proc_884(map, tileset, hp); format in docs/ASSETS.md, decoder
 * tools/assets/hp.py): +4 the grid, +0xC named boxes (doors, camera area),
 * +0x10 patrol paths, +0x14 nav meshes. +0x1C..+0x22 are the neighbour index
 * offsets -width, +1, +width, -1 (Terrain_SetNeighborOffsets). */
struct TerrainTrap {
    u32 serial;
    u16 f4, f6, id, fa, flags, fe;
    u16 w[4], s[4];
    u32 f20, command;
    void *f28;
};

struct EngineMap {
    u16 trapCount;
    u16 unknown2;
    u8 *cells; /* grid: +0 u32 0, +4 u16 width (the row stride), +6 u16 height, +8 s16 origin x/y
                  (BG map pixels * 16), cells (struct SceneCell) from +12 */
    void *resource8;
    void *boxes;
    void *routes;
    void *navigation;
    void *boxList;
    s16 neighborOffsets[4];
    u16 rows[256]; /* first cell index of each row */
    struct TerrainTrap traps[64];
    void *trapTops[64];
};


#endif
