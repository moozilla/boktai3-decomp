#include "global.h"
#include "bosses/ratatosk/ratatosk_particle.h"

struct RatatoskParticle *Ratatosk_FindFreeAnimatedParticle(struct RatatoskParticlePool *pool,
                                                           s32 *slot)
{
    struct RatatoskParticle *e = pool->particles;
    s32 i;
    for (i = 0; i < 32; i++, e++) {
        if (!(pool->mask & (1 << i))) {
            *slot = i;
            return e;
        }
    }
    return 0;
}
