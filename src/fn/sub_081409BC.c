#include "player.h"
#include "scene_object.h"
#include "scene_actor_terrain.h"
// confidence=low: tests whether the terrain cell under a position is flush with the actor height or flagged.
s32 Player_IsTurnTerrainBlocked(struct Player *player, const struct ContactPosition *position)
{
    s32 cell = SceneActorTerrain_GetCellIndex(position);
    if (SceneActorTerrain_GetHeight(cell, position) != player->position.y)
        return 1;
    cell = SceneActorTerrain_GetCellIndex(position);
    {
        const struct SceneCell *terrain = (const struct SceneCell *)SceneActorTerrain_GetCell(cell);
        u32 flags = terrain->flags;
        if (flags & 1)
            return 1;
    }
    return 0;
}
