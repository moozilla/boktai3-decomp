#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void *, void *);
s32 SceneStimulus_InitFromScript(void *, u32, u32);
void Task_Kill(void *);
void SceneStimulus_UpdateTask(void);
void SceneStimulus_DestroyTask(void);

void *SceneStimulus_CreateFromScript(u32 a, u32 b)
{
    void *p = Task_Create(5, 0xD8);
    if (p) {
        Task_SetCallbacks(p, SceneStimulus_UpdateTask, SceneStimulus_DestroyTask);
        if (SceneStimulus_InitFromScript(p, a, b) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
