#include "objects/sliding_doors/scene_object_controller.h"

s32 SlidingDoors_Init(struct SceneObjPool *, s32, s32);
s32 SlidingDoors_UpdateController(struct SceneObjPool *);
s32 SlidingDoors_Destroy(struct SceneObjPool *);

struct SceneObjPool *SlidingDoors_CreateController(s32 messageId, s32 argument)
{
    struct SceneObjPool *pool = Task_Create(9, sizeof(*pool));
    if (pool) {
        Task_SetCallbacks(pool, (void (*)(void *))SlidingDoors_UpdateController,
                          (void (*)(void *))SlidingDoors_Destroy);
        if (SlidingDoors_Init(pool, messageId, argument) < 0) {
            Task_Kill(pool);
            return NULL;
        }
    }
    return pool;
}
