#include "global.h"
#include "bosses/ratatosk/ratatosk_particle.h"

struct RatatoskParticle *Ratatosk_FindFreeAnimatedParticle(struct RatatoskParticlePool *a, s32 *out)
{
    struct RatatoskParticle *e = a->particles;
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
    } while (i <= 0x1F);
    return 0;
}
