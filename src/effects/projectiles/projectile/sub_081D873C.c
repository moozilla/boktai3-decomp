#include "effects/projectiles/projectile_task.h"
s32 ProjectileTask_StepTimedSequence(struct Task4E01Projectile *p)
{
    struct Task4E01Params *config = &p->params;
    u32 bit = 2;
    p->flags |= bit;
    PlayerCue_StepSequence(&p->sprite, &p->runtime.sequence);
    if (p->frame >= config->lifetime)
        return 2;
    return 0;
}
