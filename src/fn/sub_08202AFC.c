#include "projectile_pool.h"

struct ProjectileSlot *ProjectilePool_FindFreeSlot(struct TimedProjectilePool *pool, s32 *out)
{
    struct ProjectileSlot *e = pool->slots;
    s32 i = 0;
    u32 m = 1;
    u32 w = pool->mask;
    do {
        if ((m << i & w) != 0) {
            i++;
            e++;
            continue;
        }
        *out = i;
        return e;
    } while (i <= 0xf);
    return 0;
}
