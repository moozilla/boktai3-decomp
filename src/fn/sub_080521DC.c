#include "stacked_actor.h"
static inline u16 StackedParticle_GetRaisedHeight(const struct StackedActorParticle *particle)
{
    return 768 + (u16)particle->position.y;
}
void StackedActor_InitRisingParticle(struct StackedActorParticle *particle,
                                     struct StackedActorPosition *position)
{
    particle->position = *position;
    {
        u16 *random = (u16 *)RANDOM_TABLE_ADDR;
        u32 *rng = &gRandomIndex;
        u32 index = *rng + 1;
        u32 mask = 1023;
        u32 mask31;
        u32 zero;
        u32 sample;
        u32 coordinate;
        u32 scratch;
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
        index += 1;
        index &= mask;
        sampleAddress = (u16 *)(index * 2 + (u32)random);
        particle->position.y = StackedParticle_GetRaisedHeight(particle) + *(u8 *)sampleAddress;
        index += 1;
        index &= mask;
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
        index += 1;
        index &= mask;
        {
            u32 *store = rng;
            *store = index;
        }
        {
            u32 sample = random[index];
            particle->velocityY = -((sample & mask31) + 32);
        }
        scratch = zero;
        particle->velocityZ = scratch;
        particle->flags &= ~1;
        particle->enabled = 1;
    }
}
