#include "scene_actor_terrain.h"
#define SceneActorTerrain_ClearCollisionTimers sub_080771C4
void SceneActorTerrain_ClearCollisionTimers(struct SceneActorTerrainCollisionView *actor)
{
    struct ContactCollider *collider = &actor->collider;
    u16 zero = 0;
    collider->cost = zero;
    actor->field2E4 = zero;
    actor->field2E6 = zero;
}
