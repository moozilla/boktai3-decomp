#include "global.h"
#include "bosses/ratatosk/ratatosk_small_particle.h"
#include "engine/ram.h"

void RatatoskParticle_Remove(struct ParticlePool *, struct Particle *, s32);

u32 RatatoskParticle_Destroy(struct ParticlePool *pool)
{
    struct Particle *e = pool->particles;
    s32 i;
    for (i = 0; i < 24; i++, e++) {
        if (pool->mask & (1 << i))
            RatatoskParticle_Remove(pool, e, i);
    }
    gParticlePool = 0;
    return 0;
}
