#include "global.h"
#include "ratatosk_particle.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
struct RatatoskParticlePool *Ratatosk_CreateAnimatedParticlePool(void)
{
    void *r = Task_Create(8, sizeof(struct RatatoskParticlePool));
    if (r) {
        Task_SetCallbacks(r, sub_081FBF2C, Ratatosk_OnAnimatedParticlePoolDestroy);
        if (Ratatosk_InitAnimatedParticlePool(r) < 0) {
            Task_Kill(r);
            return 0;
        }
        return r;
    }
}
