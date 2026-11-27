#include "global.h"

void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 SolarBomb_Init(void *, s32, s32, s32);
void Task_Kill(void *);
void SolarBomb_OnTaskUpdate(void);
void SolarBomb_OnTaskDestroy(void);

void *SolarBomb_Create(s32 a, s32 b, s32 c)
{
    void *r = Task_Create(8, 0x1e0);
    if (r != 0) {
        Task_SetCallbacks(r, SolarBomb_OnTaskUpdate, SolarBomb_OnTaskDestroy);
        if (SolarBomb_Init(r, a, b, c) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
