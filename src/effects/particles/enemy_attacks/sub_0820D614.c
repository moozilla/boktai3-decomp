#include "monsters/enemy_particle.h"
s32 EnemyAttackParticle_InitPool(void *);
void EnemyAttackParticle_UpdatePool(void *);
void EnemyAttackParticle_DestroyPool(void *);
void *EnemyAttackParticle_CreatePool(void)
{
    void *p = Task_Create(8, sizeof(struct EnemyParticlePool));
    if (p) {
        Task_SetCallbacks(p, EnemyAttackParticle_UpdatePool, EnemyAttackParticle_DestroyPool);
        if (EnemyAttackParticle_InitPool(p) >= 0)
            goto ok;
        Task_Kill(p);
        return 0;
    }
ok:
    return p;
}
