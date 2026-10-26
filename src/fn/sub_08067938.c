#include "scene_actor_control.h"
#include "ram.h"
s32 SceneActorControl_Initialize(struct SceneActorControlManager *);
s32 SceneActorControl_Update(struct SceneActorControlManager *);
u32 SceneActorControl_Destroy(struct SceneActorControlManager *);
struct SceneActorControlManager *SceneActorControl_EnsureSingleton(void)
{
    struct SceneActorControlManager *p;
    if (gSceneActorControlManager)
        return gSceneActorControlManager;
    p = Task_Create(10, sizeof(*p));
    if (p) {
        Task_SetCallbacks(p, (void (*)(void *))SceneActorControl_Update,
                          (void (*)(void *))SceneActorControl_Destroy);
        if (SceneActorControl_Initialize(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
