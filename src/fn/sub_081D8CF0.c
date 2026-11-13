#include "task4e01_projectile.h"

s32 Task4E01_UpdateGrowingImpact(struct Task4E01Projectile *p)
{
    struct Task4E01Params *params = &p->params;
    u8 ended;
    u32 draw;
    if (p->frame <= 11) {
        p->runtime.radius += 10;
        Projectile_SetCollisionSize(params, p->runtime.radius, 80, p->runtime.radius);
        {
            struct ContactCollider *collider = &p->colliders[0];
            collider->bounds = p->params.bounds;
        }
    }
    ended = PlayerCue_StepSequenceEnded(&p->sprite, &p->runtime.sequence);
    if (ended == 1 && !p->runtime.restarted) {
        Anim_RestartSequence(&p->runtime.sequence, p->runtime.resource, 8, 0, 0);
        PlayerCue_StepSequenceEndedWithFirstFlipMask(&p->sprite, &p->runtime.sequence, ended);
        p->runtime.restarted++;
    }
    draw = 2;
    p->flags |= draw;
    if (p->frame >= params->lifetime)
        return draw;
    return 0;
}
