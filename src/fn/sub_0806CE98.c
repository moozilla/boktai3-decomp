#include "scene_steering_actor.h"
#define SceneSteeringActor_InitializePool sub_0806CE98
s32 SceneSteeringActor_InitializePool(struct SceneSteeringActorPool *pool)
{
    pool->used = 0;
    AnimData_Load(pool->graphics, 0xF422);
    gUnk_02000188 = pool;
    return 0;
}
