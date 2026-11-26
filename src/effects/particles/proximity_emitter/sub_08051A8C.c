#include "effects/particles/proximity_emitter.h"
void ProximityEmitter_EmitParticle(struct ProximityEmitter *emitter, void *resource)
{
    u8 *next = &emitter->nextParticle;
    struct ProximityEmitterParticle *particle = &emitter->particles[*next];
    const u16 *random;
    u32 *indexAddress;
    u32 index;
    SpriteDrawable_SelectTileFrame(particle, resource, 2);
    particle->flags &= ~1;
    random = (const u16 *)RANDOM_TABLE_ADDR;
    {
        u32 *address = &gRandomIndex;
        indexAddress = address;
        index = *address + 1;
    }
    index &= 1023;
    {
        s32 sample = random[index];
        u16 *address = (u16 *)&emitter->position.x;
        particle->position.x = *address + ((sample >> 4) & 255) - 127;
    }
    index += 1;
    index &= 1023;
    {
        s32 sample = random[index];
        u16 *address;
        u32 base;
        address = (u16 *)&emitter->position.y;
        sample &= 63;
        {
            u16 *loadAddress = address;
            base = *loadAddress;
        }
        particle->position.y = sample + base;
    }
    index += 1;
    index &= 1023;
    {
        s32 sample = random[index];
        u16 *address = (u16 *)&emitter->position.z;
        particle->position.z = *address + ((sample >> 4) & 255) - 127;
    }
    {
        particle->velocityX = 0;
    }
    index += 1;
    index &= 1023;
    {
        u32 *address = indexAddress;
        *address = index;
    }
    {
        u32 sample = random[index];
        sample &= 15;
        sample += 16;
        particle->velocityY = sample;
    }
    {
        particle->velocityZ = 0;
        particle->frame = 0;
    }
    particle->active = 1;
    {
        u8 *address = next;
        u32 updated = *address + 1;
        *address = updated;
        if ((u8)updated > 7)
            *address = 0;
    }
}
