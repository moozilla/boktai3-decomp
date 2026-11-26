#include "engine/shared/global.h"
#include "bosses/ratatosk/ratatosk_particle.h"

void Sprite_Remove(void *);
void Ratatosk_ReleaseAnimatedParticle(struct RatatoskParticlePool *a, struct RatatoskParticle *b,
                                      u32 n)
{
    a->activeCount--;
    b->sprite.flags |= 1;
    Sprite_Remove(&b->sprite);
    a->mask &= ~(1 << n);
}
