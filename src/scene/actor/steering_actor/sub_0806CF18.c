#include "scene/motion/scene_steering_actor.h"
/* SceneSteeringActor_FinishSettledAll: Changes all state-four travel actors to departure state five without modifying entry latches */

/* Advance state-four actors without touching their entry latch. */
void SceneSteeringActor_FinishSettledAll(void)
{
    struct SceneSteeringActorPool *pool = gSceneSteeringActorPool;
    if (pool) {
        s32 i;
        for (i = 0; i < 8; i++) {
            if (pool->slots[i].state == 4)
                pool->slots[i].state = 5;
        }
    }
}
