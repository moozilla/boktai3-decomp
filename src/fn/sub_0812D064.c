#include "scene_indicator.h"
#include "engine.h"
#include "ram.h"
#include "heap.h"
#include "resource_registry.h"
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

// Tentative: emits the small particles used by the large fragment animation.

void LargeFragmentEffect_SpawnRandomParticle(struct Vec *position)
{
    struct Vec velocity;
    struct Vec *planar;
    u32 randomOrigin;
    u32 mask, sampleOrigin;
    s32 heading;
    s32 speed;
    s32 lifetime;

    /* The recovered bank origin cancels when converting a sample address to its table offset. */
    randomOrigin = (u32)&gHeapHead + RESOURCE_REGISTRY_ENTRIES_ADDR;
    mask = 255;
    heading = ParticleRandom_ReadDirectSample(gRandomTable) & mask;
    {
        s32 raw = ParticleRandom_ReadDirectSample(gRandomTable);
        s32 speedMask = 15;
        raw &= speedMask;
        planar = &velocity;
        speed = raw + 4;
    }
    {
        s32 x, rawX;
        rawX = gUnk_086149C4[(heading + 64) & mask] * speed;
        sampleOrigin = randomOrigin;
        x = Truncate(rawX);
        planar->x = x;
        planar->y = 0;
        planar->z = Truncate(gUnk_086149C4[heading] * speed);
    }
    {
        const u16 *physical = (const u16 *)RANDOM_TABLE_ADDR;
        velocity.y = (ParticleRandom_ReadRelativeSample(sampleOrigin, physical) & 31) + 12;
        lifetime = (ParticleRandom_ReadRelativeSample(sampleOrigin, physical) & 63) + 20;
    }
    SpriteEffectController_Create(position, &velocity, 0x1C1A, 0xD1B8, 3, 0, 0x31, 0, 2, 0, 0,
                                  lifetime, 4, 1, 0x200004, 30, 8, 8);
}
