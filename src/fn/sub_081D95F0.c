#include "task4e01_projectile.h"

u32 sub_08249498(u32, u32);
void Task4E01_BeginImpactAnimation(void *);
void Task4E01_SpawnImpactChildren(void *);

s32 Task4E01_UpdateExpandingImpact(struct Task4E01Projectile *p)
{
    struct Task4E01Params *params = &p->params;
    struct DisplaySequenceState *state;
    u8 ended;
    u32 tick;
    struct ContactSwarmSprite *sprite;
    if (--params->speed <= 3)
        params->speed = 4;
    else {
        u32 one = 1;
        p->flags |= one;
    }
    if (!(params->flags & 0x10) || (p->frame & 3) == ((params->flags & 0xF00) >> 8)) {
        u32 two = 2;
        p->flags |= two;
    }
    sprite = &p->sprite;
    ended = PlayerCue_StepSequenceEnded(sprite, &p->sequence);
    if (ended == 1 && !(params->flags & 0x10) && !p->restarted) {
        p->animation = 4;
        state = &p->sequence;
        Anim_RestartSequence(state, p->resource, 4, 0, 0);
        PlayerCue_StepSequenceEndedWithFirstFlipMask(&p->sprite, state, ended);
        p->restarted++;
    }
    tick = p->frame;
    if (sub_08249498(tick, 15) == 14 && tick <= 44) {
        s32 radius = p->radius += 16;
        Projectile_SetCollisionSize(params, radius, 80, radius);
        {
            struct Task4E01Collider *collider = &p->collider;
            collider->bounds = params->bounds;
        }
    }
    if (p->frame >= params->lifetime) {
        if (params->flags & 0x10) {
            Task4E01_BeginImpactAnimation(p);
        } else {
            params->flags |= 0x40;
            Projectile_SetUpdateCallback(params, Task4E01_SpawnImpactChildren);
        }
    }
    return 0;
}
