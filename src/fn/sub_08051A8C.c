// MATCHING_CONCESSION: register-pinning
#include "proximity_emitter.h"
void ProximityEmitter_EmitParticle(struct ProximityEmitter *emitter, void *resource)
{
    u8 *next = &emitter->nextParticle;
    struct ProximityEmitterParticle *particle = &emitter->particles[*next];
    const u16 *random;
    register u32 *indexAddress asm("r9");
    register u32 index asm("r1"), mask;
    register u32 xzMask asm("r8");
    SpriteDrawable_SelectTileFrame(particle, resource, 2);
    particle->flags &= ~1;
    random = (const u16 *)RANDOM_TABLE_ADDR;
    {
        register u32 *address asm("r1") = &gRandomIndex;
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
    index = (index + 1) & mask;
    {
        register s32 sample asm("r0") = random[index];
        register u16 *address asm("r12");
        u32 yOffset = 0xAE;
        u16 *lowAddress;
        u32 yMask;
        register u32 base asm("r6");
        yOffset += (u32)emitter;
        lowAddress = (u16 *)yOffset;
        address = lowAddress;
        asm("" : "+r"(address));
        yMask = 63;
        sample &= yMask;
        {
            register u16 *loadAddress asm("r6") = address;
            base = *loadAddress;
        }
        particle->position.y = sample + base;
    }
    index = (index + 1) & mask;
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
    index = (index + 1) & mask;
    {
        register u32 *address asm("r2") = indexAddress;
        *address = index;
    }
    {
        u32 sample = random[index];
        sample &= 15;
        sample += 16;
        particle->velocityY = sample;
    }
    {
        register u32 zero asm("r6") = 0;
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
