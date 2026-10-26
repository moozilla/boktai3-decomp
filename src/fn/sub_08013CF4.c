#include "global.h"
#include "ram.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 ShadowRegistry_InitTask(void *);
void Task_Kill(void *);
void ShadowRegistry_OnUpdate(void);
void ShadowRegistry_OnDestroy(void);

void *ShadowRegistry_CreateTask(void)
{
    void *p;
    if (gShadowRegistry != 0)
        return (void *)gShadowRegistry;
    p = Task_Create(9, 0x1c);
    if (p != 0) {
        Task_SetCallbacks(p, ShadowRegistry_OnUpdate, ShadowRegistry_OnDestroy);
        if (ShadowRegistry_InitTask(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
