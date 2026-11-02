#include "projectile_pool.h"
#include "ram.h"
u32 ProjectilePool_Destroy(struct TimedProjectilePool *a)
{
    struct ProjectileSlot *e = a->slots;
    s32 i = 0;
    do {
        if (a->mask & (1 << i))
            ProjectilePool_ReleaseSlot(a, e, i);
        i++;
        e++;
    } while (i <= 0xf);
    gProjectilePool = 0;
    return 0;
}
