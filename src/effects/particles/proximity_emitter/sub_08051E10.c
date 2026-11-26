#include "effects/particles/proximity_emitter.h"
#include "engine/video/sprite_oam.h"
void ProximityEmitter_InitializeParticles(struct ProximityEmitter *input)
{
    struct ProximityEmitter *emitter = input;
    void **resource;
    s32 i;
    {
        void *found = AnimData_FindEntry(0x1C1E);
        void **address = &emitter->particleResource;
        *address = found;
        i = 0;
        resource = address;
    }
    do {
        struct ProximityEmitterParticle *particle = &emitter->particles[i];
        struct SpriteDrawable *sprite;
        ParticleSprite_Initialize(particle, *resource, 0);
        Effect_SetOrigin(particle, -4, -4);
        SpriteDrawable_SelectTileFrame(particle, *resource, 2);
        Effect_SetPalette(particle, 1);
        sprite = (void *)particle;
        sprite->priority = 1;
        sprite->depthOffset = 20;
        sprite->flags |= 1;
        emitter->particles[i].active = 0;
    } while (++i <= 7);
    emitter->nextParticle = 0;
    emitter->particleFrame = 0;
}
