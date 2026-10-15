#include "scene_steering_actor.h"
#define SceneSteeringActor_FinishSettledAll sub_0806CF18

/* Advance state-four actors without touching their entry latch. */
void SceneSteeringActor_FinishSettledAll(void)
{
    struct SceneSteeringActorPool *pool = gUnk_02000188;
    if (pool) {
        u32 replacement = 5;
        u8 *state = &pool->slots[0].state;
        u32 stride = sizeof(pool->slots[0]);
        s32 i = 7;
        do {
            if (*state == 4) *state = replacement;
            state = (void *)((u32)state + stride);
            i--;
        } while (i >= 0);
    }
}
