#include "effects/particles/proximity_emitter.h"
s32 TetherRing_Tick(struct ProximityEmitter *emitter)
{
    s32 i = 0;
    do {
        if (emitter->particles[i].active != 0)
            TetherRing_TickParticle(&emitter->particles[i], emitter->particleResource);
    } while (++i <= 7);
    Callback_InvokeActor(emitter, emitter->update);
    return 0;
}
