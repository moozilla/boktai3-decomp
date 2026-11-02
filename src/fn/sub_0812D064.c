// MATCHING_CONCESSION: register-pinning
#include "scene_indicator.h"
#include "engine.h"
#include "ram.h"
#include "monster_particle_random.h"

extern const s16 gUnk_086149C4[];
void SpriteEffectController_Create(void *, const void *, s32, u32, u32, u32, u32, u32, u32, u32,
                                   u32, u32, s32, s32, s32, s32, s32, s32);

static inline s32 Truncate(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}

static inline u32 Random(const u16 *table)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return table[gRandomIndex];
}

// Tentative: emits the small particles used by the large fragment animation.

void LargeFragmentEffect_SpawnRandomParticle(struct Vec *position)
{
    u32 savedOrigin;
    register u32 origin asm("r12");
    const u16 *table;
    struct Vec velocity;
    struct Vec *v;
    u32 *cursor;
    u16 index;
    u32 mask;
    register s32 angle asm("r6");
    s32 radius, lifetime;
    const u16 *source;
    const u8 *sample;
    origin = PARTICLE_RANDOM_SAMPLE_ORIGIN;
    asm volatile("" : "+r"(origin));
    table = gRandomTable;
    index = ((cursor = &gRandomIndex), *cursor + 1) & 1023;
    sample = (const u8 *)(index * 2 + (u32)table);
    mask = 255;
    angle = *sample;
    index = (index + 1) & 1023;
    *cursor = index;
    {
        s32 raw = table[index];
        s32 bitmask = 15;
        raw &= bitmask;
        v = &velocity;
        radius = raw + 4;
    }
    {
        s32 product = gUnk_086149C4[(angle + 64) & mask] * radius;
        PARTICLE_RANDOM_SAVE_ORIGIN(savedOrigin, origin);
        v->x = Truncate(product);
    }
    v->y = 0;
    {
        s32 product = gUnk_086149C4[angle] * radius;
        /* Keep the angle live through both horizontal component calculations. */
        asm("" : : "r"(angle));
        v->z = Truncate(product);
    }
    source = (const u16 *)RANDOM_TABLE_ADDR;
    velocity.y = (ParticleRandom_ReadRelativeSample(savedOrigin, source) & 31) + 12;
    lifetime = (ParticleRandom_ReadRelativeSample(savedOrigin, source) & 63) + 20;
    SpriteEffectController_Create(position, &velocity, 0x1C1A, 0xD1B8, 3, 0, 0x31, 0, 2, 0, 0,
                                  lifetime, 4, 1, 0x200004, 30, 8, 8);
}
