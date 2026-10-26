// MATCHING_CONCESSION: register-pinning
#include "stacked_actor.h"
void StackedActor_InitFallingParticle(struct StackedActorParticle *particle,
                                      struct StackedActorPosition *position)
{
    particle->position = *position;
    {
        u16 *random = (u16 *)RANDOM_TABLE_ADDR;
        register u32 *rng asm("r9") = &gRandomIndex;
        register u32 index asm("r3") = *rng + 1;
        u32 mask = 1023;
        u32 mask31;
        register u32 zero asm("r8");
        u32 sample;
        register u32 coordinate asm("r1");
        u16 *sampleAddress;
        index &= mask;
        sample = random[index];
        coordinate = (u16)particle->position.x;
        coordinate -= 15;
        mask31 = 31;
        sample &= mask31;
        coordinate += sample;
        zero = 0;
        particle->position.x = coordinate;
        index = (index + 1) & mask;
        sampleAddress = (u16 *)(index * 2 + (u32)random);
        coordinate = (u16)particle->position.y;
        coordinate += *(u8 *)sampleAddress;
        particle->position.y = coordinate;
        index = (index + 1) & mask;
        sample = random[index];
        coordinate = (u16)particle->position.z;
        coordinate -= 15;
        sample &= mask31;
        coordinate += sample;
        particle->position.z = coordinate;
        particle->life = 24;
        {
            u32 zeroX = zero;
            particle->velocityX = zeroX;
        }
        index = (index + 1) & mask;
        {
            register u32 *store asm("r2") = rng;
            *store = index;
        }
        {
            u32 sample = random[index];
            particle->velocityY = (sample & mask31) + 24;
        }
        {
            u32 zeroZ = zero;
            particle->velocityZ = zeroZ;
        }
        particle->flags &= ~1;
        particle->enabled = 1;
    }
}
