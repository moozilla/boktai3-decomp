#include "monsters/enemy_particle.h"
s32 EnemyParticle_InitPool(void *);
void EnemyParticle_UpdatePool(void *);
void EnemyParticle_DestroyPool(void *);
void *EnemyParticle_CreatePool(void)
{
    void *p = Task_Create(8, sizeof(struct EnemyParticlePool));
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
