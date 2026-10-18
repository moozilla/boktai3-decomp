#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 SolarBank_Init(void *);
void Task_Kill(void *);
void SolarBank_OnUpdate(void);
void SolarBank_OnDestroy(void);
void *SolarBank_Create(void)
{
    void *p = Task_Create(0xb, 0x734);
    if (p) {
        Task_SetCallbacks(p, SolarBank_OnUpdate, SolarBank_OnDestroy);
        if (SolarBank_Init(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
