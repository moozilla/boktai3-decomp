#include "monsters/enemy_particle.h"
struct EnemyParticle *EnemyParticle_FindFreeSlot(struct EnemyParticlePool *a, s32 *out)
{
    struct EnemyParticle *e = a->slots;
    s32 i;
    for (i = 0; i < 16; i++, e++) {
        if (a->mask & (1u << i))
            continue;
        *out = i;
        return e;
    }
    return 0;
}
