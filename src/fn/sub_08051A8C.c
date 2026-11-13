// MATCHING_CONCESSION: register-pinning
#include "proximity_emitter.h"
void ProximityEmitter_EmitParticle(struct ProximityEmitter *emitter, void *resource)
{
    u8 *next = &emitter->nextParticle;
    struct ProximityEmitterParticle *particle = &emitter->particles[*next];
    const u16 *random;
    u32 *indexAddress;
    u32 index, mask;
    register u32 xzMask asm("r8");
    SpriteDrawable_SelectTileFrame(particle, resource, 2);
    particle->flags &= ~1;
    random = (const u16 *)RANDOM_TABLE_ADDR;
    {
        u32 *address = &gRandomIndex;
        indexAddress = address;
        index = *address + 1;
    }
    mask = 1023;
    index &= mask;
    {
        s32 sample = random[index];
        u16 *address = (u16 *)&emitter->position.x;
        u32 applied;
        sample >>= 4;
        applied = 255;
        xzMask = applied;
        asm("" : "+r"(xzMask));
        applied = xzMask;
        sample &= applied;
        particle->position.x = *address + sample - 127;
    }
    index += 1;
    index &= mask;
    {
        s32 sample = random[index];
        u16 *address;
        u16 *lowAddress;
        u32 yMask;
        u32 base;
        lowAddress = (u16 *)&emitter->position.y;
        address = lowAddress;
        asm("" : "+r"(address));
        yMask = 63;
        sample &= yMask;
        {
            u16 *loadAddress = address;
            base = *loadAddress;
        }
        particle->position.y = sample + base;
    }
    index += 1;
    index &= mask;
    {
        s32 sample = random[index];
        u16 *address = (u16 *)&emitter->position.z;
        u32 applied;
        sample >>= 4;
        applied = xzMask;
        sample &= applied;
        particle->position.z = *address + sample - 127;
    }
    {
        u32 zero = 0;
        particle->velocityX = zero;
    }
    index += 1;
    index &= mask;
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
        u32 zero = 0;
        particle->velocityZ = zero;
        particle->frame = zero;
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
