#include "global.h"
#include "ratatosk_particle.h"
#include "ram.h"

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
    return gRatatoskParticlePool = 0;
}
