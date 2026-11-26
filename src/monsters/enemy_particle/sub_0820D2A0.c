#include "monsters/enemy_particle.h"
void EnemyParticle_ReleaseSlot(struct EnemyParticlePool *a, struct EnemyParticle *b, u32 n)
{
    b->sprite.flags |= 1;
    Sprite_Remove(&b->sprite);
    a->mask &= ~(1 << n);
}
