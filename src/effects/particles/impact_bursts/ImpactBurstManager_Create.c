#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 ImpactBurstManager_Initialize(void *, void *);
void Task_Kill(void *);
void ImpactBurstManager_Update(void);
void ImpactBurstManager_Destroy(void);

void *ImpactBurstManager_Create(void *a)
{
    void *p = Task_Create(9, 0xf24);
    if (p != 0) {
        Task_SetCallbacks(p, ImpactBurstManager_Update, ImpactBurstManager_Destroy);
        if (ImpactBurstManager_Initialize(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
