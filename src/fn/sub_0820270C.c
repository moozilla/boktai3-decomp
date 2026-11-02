#include "projectile_pool.h"

void Sprite_Remove(void *);
void ProjectilePool_ReleaseSlot(struct TimedProjectilePool *pool, struct ProjectileSlot *slot,
                                u32 n)
{
    u32 m, t;
    u32 *q = &slot->sprite.flags;
    t = *q;
    m = 1;
    *q = t | m;
    Sprite_Remove(q);
    pool->mask &= ~(m << n);
}
