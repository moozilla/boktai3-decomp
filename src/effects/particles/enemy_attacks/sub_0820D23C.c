#include "monsters/enemy_particle.h"
u32 EnemyAttackParticle_UpdateLifetime(struct EnemyParticle *p)
{
    u32 m = 2;
    p->flags |= m;
    if (p->timer > p->params.lifetime)
        return 1;
    return 0;
}
