#include "scene_actor_terrain.h"
void SceneActorTerrain_ClearCollisionTimers(struct SceneActorTerrainCollisionView *actor)
{
    struct ContactCollider *collider = &actor->collider;
    u16 zero = 0;
    collider->cost = zero;
    actor->field2E4 = zero;
    actor->field2E6 = zero;
}
