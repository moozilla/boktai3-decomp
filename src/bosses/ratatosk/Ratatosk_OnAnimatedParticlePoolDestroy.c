#include "global.h"
#include "bosses/ratatosk/ratatosk_particle.h"
#include "engine/ram.h"

u32 Ratatosk_OnAnimatedParticlePoolDestroy(struct RatatoskParticlePool *pool)
{
    struct RatatoskParticle *e = pool->particles;
    s32 i;
    for (i = 0; i < 32; i++, e++) {
        if (pool->mask & (1 << i))
            Ratatosk_ReleaseAnimatedParticle(pool, e, i);
    }
    gRatatoskParticlePool = 0;
    return 0;
}
