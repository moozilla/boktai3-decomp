// MATCHING_CONCESSION: register-pinning
#include "proximity_emitter.h"
#define ProximityEmitter_TickParticle sub_08051A3C
void ProximityEmitter_TickParticle(struct ProximityEmitterParticle *particle, void *resource)
{
    register u32 previous asm("r0") = particle->frame;
    register u32 frame asm("r2");
    frame = previous + 1;
    particle->frame = frame;
    if ((u16)frame > 15) {
        particle->flags |= 1;
        particle->active = 0;
    } else {
        particle->position.x += particle->velocityX;
        particle->position.y += particle->velocityY;
        particle->position.z += particle->velocityZ;
        sub_08217EEC(particle, resource, (((u16)frame >> 2) & 1) + 2);
    }
}
