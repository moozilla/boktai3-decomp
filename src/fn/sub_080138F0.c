#include "global.h"

void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 ScaledShadowManager_Init(void *, void *);
void Task_Kill(void *);
void ScaledShadowManager_UpdateAll(void);
void ScaledShadowManager_OnTaskDestroy(void);

void *ScaledShadowManager_Create(void *a)
{
    void *p = Task_Create(9, 0x20);
    if (p != 0) {
        Task_SetCallbacks(p, ScaledShadowManager_UpdateAll, ScaledShadowManager_OnTaskDestroy);
        if (ScaledShadowManager_Init(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
