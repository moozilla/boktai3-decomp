#include "effects/projectiles/projectile_task.h"

void TimedEffect_UpdateDeceleration(void);

static inline void ClearFlags(u32 *flags, u32 mask)
{
    *flags = mask & *flags;
}

s32 Projectile_InitArcFlight(struct Task4E01Projectile *projectile)
{
    Projectile_SetUpdateCallback(&projectile->params.flags, TimedEffect_UpdateDeceleration);
    {
        struct ContactSwarmSprite *sprite = &projectile->sprite;
        sprite->frame = 25;
    }
    ClearFlags(&projectile->flags, ~1);
    projectile->runtime.arcStartHeight = projectile->sprite.position.y;
    return 0;
}
