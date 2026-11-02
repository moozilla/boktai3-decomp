#ifndef GUARD_MONSTER_PARTICLE_RANDOM_H
#define GUARD_MONSTER_PARTICLE_RANDOM_H

#include "ram.h"
#include "engine_globals.h"

/* Several effects build this fixed RAM pointer a halfword at a time. */
union ParticleRandomTableSource {
    const u16 *table;
    u16 halves[2];
};
static inline const u16 *ParticleRandom_GetTable(const union ParticleRandomTableSource *source)
{
    return source->table;
}

static inline const u16 *ParticleRandom_SampleAddress(const u16 *table, u32 index)
{
    return (const u16 *)(index * sizeof(*table) + (u32)table);
}
static inline u32 ParticleRandom_NextSample(const u16 *table, u32 *seed, u32 mask)
{
    u32 index = (*seed + 1) & mask;
    const u16 *sample;
    *seed = index;
    sample = ParticleRandom_SampleAddress(table, *seed);
    return *sample;
}


/* The original draw code retains this numeric origin before rebasing to the sample table. */
#define PARTICLE_RANDOM_SAVE_ORIGIN(out_, origin_)                                                 \
    do {                                                                                           \
        (out_) = (origin_);                                                                        \
    } while (0)
#define PARTICLE_RANDOM_SAMPLE_ORIGIN 0x0403B714
#define PARTICLE_RANDOM_CAPTURE_ORIGIN(out_) \
    PARTICLE_RANDOM_SAVE_ORIGIN(out_, PARTICLE_RANDOM_SAMPLE_ORIGIN)

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

static inline u32 ParticleRandom_ReadSampleAddress(u32 address, u32 origin, const u16 *table)
{
    u32 offset = address - origin;
    return *(const u16 *)((const u8 *)table + offset);
}

static inline u32 ParticleRandom_ReadRelativeSample(u32 origin, const u16 *table)
{
    u32 address;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    address = origin + gRandomIndex * sizeof(*table);
    return ParticleRandom_ReadSampleAddress(address, origin, table);
}

#endif
