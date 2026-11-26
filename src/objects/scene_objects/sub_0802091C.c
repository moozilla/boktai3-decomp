#include "objects/scene_objects/scene_object_controller.h"

s32 SceneObjects_Init(struct SceneObjPool *, s32, s32);
s32 SceneObjects_UpdateController(struct SceneObjPool *);
s32 SceneObjects_Destroy(struct SceneObjPool *);

struct SceneObjPool *SceneObjects_CreateController(s32 messageId, s32 argument)
{
    struct SceneObjPool *pool = Task_Create(9, sizeof(*pool));
    if (pool) {
        Task_SetCallbacks(pool, (void (*)(void *))SceneObjects_UpdateController,
                          (void (*)(void *))SceneObjects_Destroy);
        if (SceneObjects_Init(pool, messageId, argument) < 0) {
            Task_Kill(pool);
            return NULL;
        }
    }
    return pool;
}
