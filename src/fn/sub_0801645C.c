#include "global.h"
extern u32 gUnk_0200006C;
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 FadingParticleManager_Initialize(void *, void *);
void Task_Kill(void *);
void sub_08016334(void);
void sub_0801636C(void);

void *sub_0801645C(void *a)
{
    void *p;
    if (gUnk_0200006C != 0)
        return (void *)gUnk_0200006C;
    p = Task_Create(10, 0xcfc);
    if (p != 0) {
        Task_SetCallbacks(p, sub_08016334, sub_0801636C);
        if (FadingParticleManager_Initialize(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
