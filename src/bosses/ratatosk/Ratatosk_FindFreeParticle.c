#include "global.h"
#include "bosses/ratatosk/ratatosk_small_particle.h"
#include "engine/ram.h"

struct Particle *Ratatosk_FindFreeParticle(struct ParticlePool *pool, s32 *slot)
{
    struct Particle *e = pool->particles;
    s32 i;
    for (i = 0; i < 24; i++, e++) {
        if (!(pool->mask & (1 << i))) {
            *slot = i;
            return e;
        }
    }
    return 0;
}
