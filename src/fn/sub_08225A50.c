#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
void Task_Kill(void *);
s32 SceneBody_InitRegistry(void *, u32, u32);
void SceneBody_UpdateRegistryTask(void);
void SceneBody_DestroyRegistryTask(void);
extern void *gUnk_030025F8;
void *SceneBody_CreateRegistryTask(u32 a, u32 b)
{
    void *p;
    if (gUnk_030025F8 != 0)
        return gUnk_030025F8;
    p = Task_Create(2, 0x20);
    if (p) {
        Task_SetCallbacks(p, SceneBody_UpdateRegistryTask, SceneBody_DestroyRegistryTask);
        if (SceneBody_InitRegistry(p, a, b) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
