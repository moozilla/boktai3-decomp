#include "burst_emitter.h"
void BurstEmitter_TickActive(struct BurstEmitterPool *pool, struct BurstEmitter *emitter)
{
    BurstEmitter_EmitFrame(emitter);
    BurstEmitter_PlayAmbientSound(emitter);
    if ((u16)(emitter->frame - 10) <= 5)
        Collider_Refresh(&emitter->collider);
    if (emitter->frame > 40)
        emitter->state = 2;
    else
        emitter->frame++;
}
