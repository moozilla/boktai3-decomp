#include "scene/motion/scene_actor_terrain.h"
void SceneActorTerrain_ClearCollisionTimers(struct RoomActor *actor)
{
    struct ContactCollider *collider = &actor->colliderA;
    collider->cost = 0;
    actor->reactionSoundVariant = 0;
    actor->field2E6 = 0;
}
