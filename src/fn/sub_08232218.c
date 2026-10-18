#include "global.h"

extern void *gUnk_0200005C;
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 SceneActorEffects_InitManager(void *, void *);
void Task_Kill(void *);
void SceneActorEffects_UpdateManager(void);
void SceneActorEffects_DestroyManager(void);

void *SceneActorEffects_CreateManager(void *a)
{
    void *p;
    if (gUnk_0200005C != 0)
        return gUnk_0200005C;
    p = Task_Create(9, 0x24);
    if (p != 0) {
        Task_SetCallbacks(p, SceneActorEffects_UpdateManager, SceneActorEffects_DestroyManager);
        if (SceneActorEffects_InitManager(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
