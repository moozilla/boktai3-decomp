#include "proximity_emitter.h"
#define ProximityEmitter_Destroy sub_08051DE4
u32 ProximityEmitter_Destroy(struct ProximityEmitter *emitter)
{
    struct ProximityEmitterParticle *particle;
    s32 i;
    Sprite_Remove(&emitter->primary.sprite);
    Sprite_Remove(&emitter->secondary.sprite);
    particle = emitter->particles;
    i = 7;
    do {
        Effect_Remove(particle);
        particle++;
    } while (--i >= 0);
    return 0;
}
