// MATCHING_CONCESSION: register-pinning
#include "proximity_emitter.h"
static inline void *resourceAt(void **resource)
{
    void **address = resource;
    return *address;
}
void ProximityEmitter_InitializeParticles(struct ProximityEmitter *input)
{
    struct ProximityEmitter *emitter = input;
    void **resource;
    s32 scale;
    u32 one;
    s32 i;
    {
        void *found = AnimData_FindEntry(0x1C1E);
        void **address = &emitter->particleResource;
        *address = found;
        i = 0;
        resource = address;
    }
    {
        s32 value = -4;
        scale = value;
    }
    {
        u32 value = 1;
        one = value;
    }
    do {
        u32 offset = i * sizeof(struct ProximityEmitterParticle);
        u32 particleOffset = offset + 0xC0;
        struct ProximityEmitterParticle *particle;
        u16 *active;
        particle = (void *)((u8 *)emitter + particleOffset);
        ParticleSprite_Initialize(particle, resourceAt(resource), 0);
        Effect_SetOrigin(particle, scale, scale);
        SpriteDrawable_SelectTileFrame(particle, resourceAt(resource), 2);
        Effect_SetPalette(particle, 1);
        {
            u32 value = one;
            ((u8 *)particle)[0xF] = value;
        }
        ((u8 *)particle)[0x10] = 20;
        particle->flags |= one;
        active = (u16 *)((u8 *)emitter + offset + 0xF0);
        {
            u32 zero = 0;
            DEBUG_NOP();
            *active = zero;
        }
    } while (++i <= 7);
    {
        u8 *address = &emitter->nextParticle;
        u32 zero = 0;
        *address = zero;
        {
            u16 *frameAddress = (u16 *)(address + 1);
            *frameAddress = zero;
        }
    }
}
