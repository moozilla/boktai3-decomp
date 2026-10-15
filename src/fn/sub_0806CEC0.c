#include "scene_steering_actor.h"
#define SceneSteeringActor_EnsurePool sub_0806CEC0
s32 sub_0806CE98(struct SceneSteeringActorPool *);
s32 sub_0806CDC0(struct SceneSteeringActorPool *);
s32 sub_0806CE24(struct SceneSteeringActorPool *);
struct SceneSteeringActorPool *SceneSteeringActor_EnsurePool(void)
{
    struct SceneSteeringActorPool *pool;
    if (gUnk_02000188) return gUnk_02000188;
    pool = Task_Create(10, sizeof(*pool));
    if (pool) {
        Task_SetCallbacks(pool, (void (*)(void *))sub_0806CDC0,
                          (void (*)(void *))sub_0806CE24);
        if (sub_0806CE98(pool) < 0) {
            Task_Kill(pool);
            return 0;
        }
    }
    return pool;
}
