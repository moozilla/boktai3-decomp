#include "effects/particles/enemy_particle.h"
#include "engine/ram.h"
void EnemyAttackParticle_ReleaseSlot(struct EnemyParticlePool *, struct EnemyParticle *, s32);
u32 EnemyAttackParticle_DestroyPool(struct EnemyParticlePool *a)
{
    struct EnemyParticle *e = a->slots;
    s32 i = 0;
    do {
        if (a->mask & (1 << i))
            EnemyAttackParticle_ReleaseSlot(a, e, i);
        i++;
        e++;
    } while (i <= 0xF);
    return gEnemyParticlePool = 0;
}
