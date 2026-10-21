#include "proximity_emitter.h"
void ProximityEmitter_TickParticle(struct ProximityEmitterParticle *particle, void *resource)
{
    u32 previous = particle->frame;
    u32 frame;
    int tmp;
    tmp = previous + 1;
    frame = tmp;
    particle->frame = frame;
    if ((u16)frame > 15) {
        particle->flags |= 1;
        particle->active = 0;
    } else {
        particle->position.x += particle->velocityX;
        particle->position.y += particle->velocityY;
        tmp = 2;
        particle->position.z += particle->velocityZ;
        SpriteDrawable_SelectTileFrame(particle, resource, (((u16)frame >> tmp) & 1) + tmp);
    }
}
