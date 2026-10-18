#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void sub_081FBF2C(void);
void Ratatosk_OnAnimatedParticlePoolDestroy(void);
s32 Ratatosk_InitAnimatedParticlePool(void *);
void *Ratatosk_CreateAnimatedParticlePool(void)
{
    void *r = Task_Create(8, 0xf40);
    if (r) {
        Task_SetCallbacks(r, sub_081FBF2C, Ratatosk_OnAnimatedParticlePoolDestroy);
        if (Ratatosk_InitAnimatedParticlePool(r) < 0) {
            Task_Kill(r);
            return 0;
        }
        return r;
    }
}
