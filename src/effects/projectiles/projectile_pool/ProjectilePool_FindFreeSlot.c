#include "effects/projectiles/projectile_pool.h"

struct ProjectileSlot *ProjectilePool_FindFreeSlot(struct TimedProjectilePool *pool, s32 *out)
{
    struct ProjectileSlot *slot = pool->slots;
    s32 i;

    for (i = 0; i < 16; i++, slot++) {
        if (!(pool->mask & (1 << i))) {
            *out = i;
            return slot;
        }
    }
    return NULL;
}
