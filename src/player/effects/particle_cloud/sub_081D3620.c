#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "engine/math/monster_particle_random.h"
#include "player/effects/player_particle_cloud.h"

// confidence=low: scatters particle anchors horizontally and raises them by sixteen units.
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
        u32 sampleIndex;
        sampleIndex = *indexPtr;
        sampleIndex++;
        sampleIndex &= mask;
        *indexPtr = sampleIndex;
        {
            const u16 *sample = ParticleRandom_SampleAddress(table, sampleIndex);
            s32 coordinate;
            u32 random = *sample;
            coordinate = (u16)particle->origin.x;
            coordinate += bias;
            random &= mask;
            coordinate += random;
            {
                u32 raw = (u16)particle->origin.x;
                if ((s32)raw < 0)
                    coordinate = 0;
            }
            particle->origin.x = coordinate;
        }
        sampleIndex++;
        sampleIndex &= mask;
        *indexPtr = sampleIndex;
        {
            const u16 *sample;
            s32 coordinate;
            u32 random;
            sampleIndex *= 2;
            sampleIndex += (u32)table;
            sample = (const u16 *)sampleIndex;
            random = *sample;
            coordinate = (u16)particle->origin.z;
            coordinate += bias;
            random &= mask;
            coordinate += random;
            {
                u32 raw = (u16)particle->origin.z;
                if ((s32)raw < 0)
                    coordinate = 0;
            }
            particle->origin.z = coordinate;
        }
        particle->origin.y += 4096;
        particle->anchor = &particle->origin;
    }

    return 0;
}
