#include "monsters/enemy_particle.h"
#include "engine/ram.h"
void EnemyParticle_ReleaseSlot(struct EnemyParticlePool *, struct EnemyParticle *, s32);
u32 EnemyParticle_DestroyPool(struct EnemyParticlePool *a)
{
    struct EnemyParticle *e = a->slots;
    s32 i = 0;
    do {
        if (a->mask & (1 << i))
            EnemyParticle_ReleaseSlot(a, e, i);
        i++;
        e++;
    } while (i <= 0xF);
    return gEnemyParticlePool = 0;
}
