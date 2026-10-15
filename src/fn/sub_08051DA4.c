#include "proximity_emitter.h"
s32 ProximityEmitter_Tick(struct ProximityEmitter *emitter)
{
    s32 i = 0;
    do {
        if (emitter->particles[i].active != 0)
            ProximityEmitter_TickParticle(&emitter->particles[i], emitter->particleResource);
    } while (++i <= 7);
    sub_0824923C(emitter, emitter->update);
    return 0;
}
