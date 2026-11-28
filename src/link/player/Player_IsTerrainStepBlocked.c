#include "global.h"
#include "player/player.h"
#include "scene/terrain/terrain_sampling.h"
#include "scene/terrain/terrain_cell_probe.h"

// confidence=low: rejects a candidate position if terrain height changes or its cell blocks movement.
s32 Player_IsTerrainStepBlocked(struct Player *p, struct ContactPosition *position)
{
    if (Terrain_SampleHeight(position) != p->position.y)
        return 1;
    {
        u32 flags = ((struct TerrainCell *)cellProps(Terrain_GetCellIndex(position)))->flags;

        if (flags & 1)
            return 1;
    }
    return 0;
}
