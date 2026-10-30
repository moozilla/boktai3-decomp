#ifndef GUARD_MONSTER_PARTICLE_RANDOM_H
#define GUARD_MONSTER_PARTICLE_RANDOM_H

#include "ram.h"

/* Capture a sample separately from advancing the shared random cursor. */
#define PARTICLE_RANDOM_READ_SAMPLE(out, table, index)                                             \
    do {                                                                                           \
        (out) = (table)[(index)];                                                                  \
    } while (0)

static inline u32 ParticleRandom_ReadSample(const u16 *table)
{
    u32 sample;
    gRandomIndex = (gRandomIndex + 1) & 0x3FF;
    PARTICLE_RANDOM_READ_SAMPLE(sample, table, gRandomIndex);
    return sample;
}

static inline u32 ParticleRandom_ReadDirectSample(const u16 *table)
{
    gRandomIndex = (gRandomIndex + 1) & 0x3FF;
    return table[gRandomIndex];
}

static inline u32 ParticleRandom_ReadBankSample(const u16 *bank)
{
    bank += 512;
    gRandomIndex = (gRandomIndex + 1) & 0x3FF;
    return bank[gRandomIndex];
}

#endif
