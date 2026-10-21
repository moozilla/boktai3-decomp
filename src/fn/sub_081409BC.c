// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_terrain.h"
// confidence=low: tests whether the terrain cell under a position is flush with the actor height or flagged.
s32 sub_081409BC(u8 *p, struct ContactPosition *position)
{
    s32 cell = SceneActorTerrain_GetCellIndex(position);
    if (SceneActorTerrain_GetHeight(cell, position) != *(s16 *)(p + 0x32))
        return 1;
    cell = SceneActorTerrain_GetCellIndex(position);
    {
        u32 flags = *(u16 *)(SceneActorTerrain_GetCell(cell) + 2);
        u32 mask = 1;
        if (flags & mask)
            return 1;
    }
    return 0;
}
