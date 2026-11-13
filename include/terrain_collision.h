#ifndef GUARD_TERRAIN_COLLISION_H
#define GUARD_TERRAIN_COLLISION_H
#include "engine.h"
#include "contact_position.h"
#include "terrain_probe.h"
struct TerrainCell {
    u8 shapeAndHeight, attribute;
    u16 flags;
};
struct TerrainGrid {
    u32 unknown0;
    s16 width, height, originX, originZ;
    struct TerrainCell cells[1];
};
struct TerrainCellOverride {
    u16 flags, index;
    struct TerrainCell cell;
    struct TerrainCellOverride *previous, *next;
};
struct SurfaceTrack {
    u8 blocked, unknown1[3];
    u16 previousCell, cell;
    u8 previousHeight, height, previousShape, shape;
    u16 previousFlags, flags;
};
struct TerrainSignedProbe {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
#endif
