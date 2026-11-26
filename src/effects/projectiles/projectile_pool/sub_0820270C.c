#include "effects/projectiles/projectile_pool.h"

void Sprite_Remove(void *);
void ProjectilePool_ReleaseSlot(struct TimedProjectilePool *pool, struct ProjectileSlot *slot,
                                u32 n)
{
    u32 t;
    u32 *q = &slot->sprite.flags;
    t = *q;
    *q = t | 1;
    Sprite_Remove(q);
    pool->mask &= ~(1 << n);
}
