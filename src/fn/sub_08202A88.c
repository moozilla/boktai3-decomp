#include "projectile_pool.h"
#include "ram.h"
u32 ProjectilePool_Init(struct TimedProjectilePool *a)
{
    struct ProjectileSlot *e;
    s32 i;
    gProjectilePool = a;
    a->mask = 0;
    e = a->slots;
    i = 0;
    do {
        TimedSpriteEffect_InitSlot(a, e, i);
        i++;
        e++;
    } while (i <= 0xf);
    return 0;
}
