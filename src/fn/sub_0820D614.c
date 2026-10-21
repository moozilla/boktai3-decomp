#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 EnemyParticle_InitPool(void *);
void Task_Kill(void *);
void EnemyParticle_UpdatePool(void);
void EnemyParticle_DestroyPool(void);
void *EnemyParticle_CreatePool(void)
{
    void *p = Task_Create(8, 0xB1C);
    if (p) {
        Task_SetCallbacks(p, EnemyParticle_UpdatePool, EnemyParticle_DestroyPool);
        if (EnemyParticle_InitPool(p) >= 0)
            goto ok;
        Task_Kill(p);
        return 0;
    }
ok:
    return p;
}
