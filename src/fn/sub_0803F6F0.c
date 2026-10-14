#include "global.h"

void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 GroundEffect_InitializeManager(void *, void *);
void Task_Kill(void *);
void GroundEffect_UpdateManager(void);
void GroundEffect_DestroyManager(void);

void *GroundEffect_CreateManager(void *a)
{
    void *p = Task_Create(9, 0x20);
    if (p != 0) {
        Task_SetCallbacks(p, GroundEffect_UpdateManager, GroundEffect_DestroyManager);
        if (GroundEffect_InitializeManager(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
