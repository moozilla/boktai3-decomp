#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void sub_081FCA44(void);
void SceneProjectile_OnPoolDestroy(void);
s32 SceneProjectile_InitPool(void *);
void *SceneProjectile_CreatePool(void)
{
    void *r = Task_Create(8, 0x11a0);
    if (r) {
        Task_SetCallbacks(r, sub_081FCA44, SceneProjectile_OnPoolDestroy);
        if (SceneProjectile_InitPool(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
