#include "scene_actor_terrain.h"
void SceneActorTerrain_ClearCollisionTimers(struct RoomActor *actor)
{
    struct ContactCollider *collider = &actor->colliderA;
    u16 zero = 0;
    collider->cost = zero;
    actor->reactionSoundVariant = zero;
    actor->field2E6 = zero;
}
