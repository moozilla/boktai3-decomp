#include "global.h"
#include "bosses/ratatosk/ratatosk_small_particle.h"
#include "engine/ram.h"

void Sprite_Remove(void *);

void RatatoskParticle_Remove(struct ParticlePool *pool, struct Particle *e, u32 slot)
{
    pool->activeCount--;
    e->sprite.flags |= 1;
    Sprite_Remove(&e->sprite);
    pool->mask &= ~(1 << slot);
}
