// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "engine.h"
#include "ram.h"
#include "monster_particle_random.h"
#include "player_particle_cloud.h"

// confidence=low: scatters particle anchors horizontally and raises them by sixteen units.
#define PlayerParticleCloud_ScatterAnchors sub_081D3620
s32 PlayerParticleCloud_ScatterAnchors(void)
{
    struct CloudTask *p = gCloudTask;
    struct CloudParticle *particle;
    const u16 *table;
    union ParticleRandomTableSource source;
    s32 i;
    s32 bias;
    u32 *indexPtr;
    u32 mask;
    if (!p)
        return -1;
    particle = p->particles;
    source.halves[0] = (u16)RANDOM_TABLE_ADDR;
    source.halves[1] = RANDOM_TABLE_ADDR >> 16;
    table = ParticleRandom_GetTable(&source);
    indexPtr = &gRandomIndex;
    mask = 1023;
    bias = -512;
    for (i = 39; i >= 0; i--, particle++) {
        u32 xIndex, zIndex;
        register u32 random asm("r0");
        xIndex = (*indexPtr + 1) & mask;
        *indexPtr = xIndex;
        {
            const u16 *sample = ParticleRandom_SampleAddress(table, xIndex);
            s32 coordinate;
            random = *sample;
            coordinate = (u16)particle->origin.x;
            coordinate += bias;
            random &= mask;
            coordinate += random;
            particle->origin.x = coordinate;
        }
        zIndex = (xIndex + 1) & mask;
        *indexPtr = zIndex;
        {
            const u16 *sample = ParticleRandom_SampleAddress(table, zIndex);
            s32 coordinate;
            random = *sample;
            coordinate = (u16)particle->origin.z;
            coordinate += bias;
            random &= mask;
            coordinate += random;
            particle->origin.z = coordinate;
        }
        particle->origin.y += 4096;
        particle->anchor = &particle->origin;
    }

    return 0;
}
