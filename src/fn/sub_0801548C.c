#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 AnimatedParticleManager_Initialize(void *, void *);
void Task_Kill(void *);
void AnimatedParticleManager_Update(void);
void sub_0801537C(void);

void *sub_0801548C(void *a)
{
    void *p = Task_Create(9, 0xf24);
    if (p != 0) {
        Task_SetCallbacks(p, AnimatedParticleManager_Update, sub_0801537C);
        if (AnimatedParticleManager_Initialize(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
