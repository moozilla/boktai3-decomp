#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 ScenePool_InitializeManager(void *);
void Task_Kill(void *);
void sub_0803D430(void);
void ScenePool_DestroyManager(void);
void *ScenePool_CreateManager(void)
{
    void *p = Task_Create(8, 0x2360);
    if (p) {
        Task_SetCallbacks(p, sub_0803D430, ScenePool_DestroyManager);
        if (ScenePool_InitializeManager(p) >= 0)
            goto ok;
        Task_Kill(p);
        return 0;
    }
ok:
    return p;
}
