#ifndef GUARD_TERRAIN_PROBE_H
#define GUARD_TERRAIN_PROBE_H

#include "global.h"

struct TerrainProbe {
    u32 x : 16;
    u32 y : 16;
    u32 z : 16;
    u32 w : 16;
};

#endif
