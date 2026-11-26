#include "monsters/enemy_particle.h"
struct EnemyParticle *EnemyAttackParticle_FindFreeSlot(struct EnemyParticlePool *a, s32 *out)
{
    struct EnemyParticle *e = a->slots;
    s32 i = 0;
    u32 m = 1;
    u32 w = a->mask;
    do {
        if ((m << i & w) != 0) {
            i++;
            e++;
            continue;
        }
        *out = i;
        return e;
    } while (i <= 0xF);
    return 0;
}
