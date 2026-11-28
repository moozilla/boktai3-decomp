#include "global.h"
#include "engine/math/scene_vec.h"
#include "scene/terrain/terrain_collision.h"

u32 Player_GetTerrainHeightAtPosition(const struct TerrainCell *cell, const struct SceneVec *pos)
{
    u32 height = (cell->shapeAndHeight & 0xf) << 8;

    switch (cell->shapeAndHeight >> 4) {
        case 1:
            height -= (u8)pos->z;
            break;
        case 2:
            height -= (u8)pos->x;
            break;
    }
    return height;
}
