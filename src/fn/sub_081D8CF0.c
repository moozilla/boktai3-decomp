#include "task4e01_projectile.h"

s32 Task4E01_UpdateGrowingImpact(struct Task4E01Projectile *p)
{
    struct Task4E01Params *params = &p->params;
    u8 ended;
    u32 draw;
    if (p->frame <= 11) {
        p->radius += 10;
        Projectile_SetCollisionSize(params, p->radius, 80, p->radius);
        {
            struct Task4E01Collider *collider = &p->collider;
            collider->bounds = p->params.bounds;
        }
    }
    ended = PlayerCue_StepSequenceEnded(&p->sprite, &p->sequence);
    if (ended == 1 && !p->restarted) {
        Anim_RestartSequence(&p->sequence, p->resource, 8, 0, 0);
        PlayerCue_StepSequenceEndedWithFirstFlipMask(&p->sprite, &p->sequence, ended);
        p->restarted++;
    }
    draw = 2;
    p->flags |= draw;
    if (p->frame >= params->lifetime)
        return draw;
    return 0;
}
