#include "effects/projectiles/projectile_task.h"
s32 ProjectileTask_UpdateTimedSprite(struct Task4E01Projectile *p)
{
    struct Task4E01Params *config = &p->params;
    struct ContactSwarmSprite *sprite = &p->sprite;
    s32 t;
    sprite->flags &= ~1;
    if (p->frame & 1) {
        t = p->runtime.trailFrame;
        sprite->frame = t + 0x19;
    } else if (p->runtime.trailFrame == 0) {
        sprite->flags |= 1;
    } else {
        t = p->runtime.trailFrame;
        sprite->frame = t + 0x18;
    }
    {
        u32 two = 2;
        p->flags |= two;
    }
    if (p->frame >= config->lifetime)
        return 2;
    return 0;
}
