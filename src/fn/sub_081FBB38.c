#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void sub_081FB8A0(void);
void RatatoskParticle_Destroy(void);
s32 RatatoskParticle_Init(void *);
void *Ratatosk_CreateParticlePool(void)
{
    void *r = Task_Create(8, 0xb20);
    if (r) {
        Task_SetCallbacks(r, sub_081FB8A0, RatatoskParticle_Destroy);
        if (RatatoskParticle_Init(r) < 0) {
            Task_Kill(r);
            return 0;
        }
        return r;
    }
}
