#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void SceneArc_UpdateManager(void);
void SceneArc_DestroyManager(void);
s32 SceneArc_InitializeManager(void *);
void *SceneArc_CreateManager(void)
{
    void *r = Task_Create(10, 0x9a0);
    if (r) {
        Task_SetCallbacks(r, SceneArc_UpdateManager, SceneArc_DestroyManager);
        if (SceneArc_InitializeManager(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
