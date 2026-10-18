#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void *), void (*)(void *));
s32 SceneActorSequence_InitFromScript(void *, void *);
void Task_Kill(void *);
void SceneActorSequence_Update(void *);
void SceneActorSequence_OnDestroy(void *);
void *SceneActorSequence_CreateFromScript(void *a)
{
    void *p = Task_Create(8, 0x60);
    if (p != 0) {
        Task_SetCallbacks(p, SceneActorSequence_Update, SceneActorSequence_OnDestroy);
        if (SceneActorSequence_InitFromScript(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
