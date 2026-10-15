#include "scene_actor_control.h"
#define SceneActorControl_EnsureSingleton sub_08067938
extern struct SceneActorControlManager *gUnk_02000128;
s32 sub_08067900(struct SceneActorControlManager *);
s32 sub_08067854(struct SceneActorControlManager *);
u32 sub_080678B0(struct SceneActorControlManager *);
struct SceneActorControlManager *SceneActorControl_EnsureSingleton(void)
{
    struct SceneActorControlManager *p;
    if (gUnk_02000128)
        return gUnk_02000128;
    p = Task_Create(10, sizeof(*p));
    if (p) {
        Task_SetCallbacks(p, (void (*)(void *))sub_08067854, (void (*)(void *))sub_080678B0);
        if (sub_08067900(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
