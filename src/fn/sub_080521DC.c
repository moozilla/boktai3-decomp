#include "stacked_actor.h"
#include "ram.h"
static inline u16 StackedParticle_GetRaisedHeight(const struct StackedActorParticle *particle)
{
    return 768 + (u16)particle->position.y;
}
void StackedActor_InitRisingParticle(struct StackedActorParticle *particle,
                                     struct StackedActorPosition *position)
{
    particle->position = *position;
    {
        const u16 *random = gRandomTable;
        u32 *rng = &gRandomIndex;
        u32 index = *rng + 1;
        u32 mask31;
        u32 sample;
        u32 coordinate;
        const u16 *sampleAddress;
        index &= 1023;
        sample = random[index];
        coordinate = (u16)particle->position.x;
        coordinate -= 15;
        mask31 = 31;
        sample &= mask31;
        coordinate += sample;
        particle->position.x = coordinate;
        index += 1;
        index &= 1023;
        sampleAddress = &random[index];
        particle->position.y =
            StackedParticle_GetRaisedHeight(particle) + *(const u8 *)sampleAddress;
        index += 1;
        index &= 1023;
        sample = random[index];
        coordinate = (u16)particle->position.z;
        coordinate -= 15;
        sample &= mask31;
        coordinate += sample;
        particle->position.z = coordinate;
        particle->life = 24;
        particle->velocityX = 0;
        index += 1;
        index &= 1023;
        {
            u32 *store = rng;
            *store = index;
        }
        {
            u32 sample = random[index];
            particle->velocityY = -((sample & mask31) + 32);
        }
        particle->velocityZ = 0;
        particle->flags &= ~1;
        particle->enabled = 1;
    }
}
