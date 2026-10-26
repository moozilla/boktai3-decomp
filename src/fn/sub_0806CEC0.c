#include "scene_steering_actor.h"
s32 SceneSteeringActor_InitializePool(struct SceneSteeringActorPool *);
s32 SceneSteeringActor_UpdatePool(struct SceneSteeringActorPool *);
s32 SceneSteeringActor_DestroyPool(struct SceneSteeringActorPool *);
struct SceneSteeringActorPool *SceneSteeringActor_EnsurePool(void)
{
    struct SceneSteeringActorPool *pool;
    if (gSceneSteeringActorPool)
        return gSceneSteeringActorPool;
    pool = Task_Create(10, sizeof(*pool));
    if (pool) {
        Task_SetCallbacks(pool, (void (*)(void *))SceneSteeringActor_UpdatePool,
                          (void (*)(void *))SceneSteeringActor_DestroyPool);
        if (SceneSteeringActor_InitializePool(pool) < 0) {
            Task_Kill(pool);
            return 0;
        }
    }
    return pool;
}
