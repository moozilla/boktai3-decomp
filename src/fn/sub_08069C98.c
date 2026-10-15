#include "burst_emitter.h"
#define BurstEmitter_TickActive sub_08069C98
void sub_0821FE40(struct ContactCollider *);
void BurstEmitter_TickActive(struct BurstEmitterPool *pool, struct BurstEmitter *emitter)
{
    sub_08069964(emitter);
    sub_08069B68(emitter);
    if ((u16)(emitter->frame - 10) <= 5)
        sub_0821FE40(&emitter->collider);
    if (emitter->frame > 40)
        emitter->state = 2;
    else
        emitter->frame++;
}
