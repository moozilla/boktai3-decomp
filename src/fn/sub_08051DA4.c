#include "proximity_emitter.h"
#define ProximityEmitter_Tick sub_08051DA4
s32 ProximityEmitter_Tick(struct ProximityEmitter *emitter)
{
    s32 i = 0;
    do {
        if (emitter->particles[i].active != 0)
            sub_08051A3C(&emitter->particles[i], emitter->particleResource);
    } while (++i <= 7);
    sub_0824923C(emitter, emitter->update);
    return 0;
}
