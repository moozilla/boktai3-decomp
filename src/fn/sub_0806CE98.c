#include "scene_steering_actor.h"
s32 SceneSteeringActor_InitializePool(struct SceneSteeringActorPool *pool)
{
    pool->used = 0;
    AnimData_Load(pool->graphics, 0xF422);
    gSceneSteeringActorPool = pool;
    return 0;
}
