#include "global.h"
#include "engine.h"

struct Pos {
    u16 x, pad2, z;
};
u8 *TerrainModifiers_FindCellOverride(s32, s32);
int NavMesh_FindRoute(u16 *, void *, void *);

int Terrain_ApplyBlocked(u16 *out, void *arg, struct Pos *pos)
{
    s32 x = (s16)pos->x >> 8;
    s32 z = (s16)pos->z >> 8;
    s32 cell;
    u8 *props;
    u32 mask, flags;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        cell = 0;
    else
        cell = gTerrainMap->rows[z] + x;
    props = TerrainModifiers_FindCellOverride(cell, 1);
    if (props)
        props += 4;
    else {
        struct EngineMap *map = gTerrainMap;
        u32 offset = cell * 4 + 12;
        props = map->cells + offset;
    }
    flags = *(u16 *)(props + 2);
    mask = 2;
    if (flags & mask) {
        int z = 0;
        *out = z;
        return z;
    }
    return NavMesh_FindRoute(out, arg, pos);
}
