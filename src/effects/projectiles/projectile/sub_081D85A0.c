#include "effects/projectiles/projectile_task.h"

u32 ProjectileTask_OnDefaultTimedAnimation(struct Task4E01Projectile *projectile)
{
    u32 two = 2;
    projectile->flags |= two;
    if (projectile->frame >= projectile->params.lifetime) {
        return 2;
    }
    return 0;
}
