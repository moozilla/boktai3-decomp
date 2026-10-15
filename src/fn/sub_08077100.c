#include "scene_actor_terrain.h"
#define SceneActorTerrain_IsFacingPosition sub_08077100
u32 SceneActorTerrain_IsFacingPosition(struct SceneActorTerrainView *actor,
                                       const struct ContactPosition *position)
{
    struct ContactPosition difference;
    s32 angle;
    u32 direction;
    s32 cell = SceneActorTerrain_GetCellIndex(position);
    if (actor->terrainCell != cell)
        goto failed;
    SceneActorTerrain_Subtract(&difference, position, &actor->position);
    direction = actor->direction;
    angle = Math_Atan2(difference.x, difference.z) - direction;
    if (angle > 128)
        angle -= 256;
    else if (angle < -128)
        angle += 256;
    if ((angle < 0 ? -angle : angle) <= 48)
        goto success;
failed:
    return 0;
success:
    return 1;
}
