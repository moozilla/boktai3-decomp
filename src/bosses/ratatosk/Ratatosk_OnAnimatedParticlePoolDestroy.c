#include "global.h"
#include "bosses/ratatosk/ratatosk_particle.h"
#include "engine/ram.h"

u32 Ratatosk_OnAnimatedParticlePoolDestroy(struct RatatoskParticlePool *a)
{
    struct RatatoskParticle *e = a->particles;
    s32 i = 0;
    do {
        if (a->mask & (1 << i))
            Ratatosk_ReleaseAnimatedParticle(a, e, i);
        i++;
        e++;
    } while (i <= 0x1F);
    gRatatoskParticlePool = 0;
    return 0;
}
