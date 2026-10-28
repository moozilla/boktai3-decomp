// MATCHING_CONCESSION: register-pinning
#include "scene_indicator.h"
#include "engine.h"
#include "ram.h"

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
    /* The inlined random helper retains this unused address. */
    register u32 opaque asm("r12") = 0x0403B714;
    const u16 *table = gRandomTable;
    struct Vec velocity;
    struct Vec *v;
    u32 *cursor;
    u16 index;
    u32 mask;
    register s32 angle asm("r6");
    s32 radius, lifetime;
    const u16 *source;
    const u8 *sample;
    asm volatile("" : "+r"(opaque));
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
        register u32 scratch asm("r5") = opaque;
        asm volatile("" : : "r"(scratch));
        v->x = Truncate(product);
    }
    v->y = 0;
    {
        s32 product = gUnk_086149C4[angle] * radius;
        /* Keep the angle live through both horizontal component calculations. */
        asm("" : : "r"(angle));
        v->z = Truncate(product);
    }
    source = (const u16 *)0x0203B400;
    velocity.y = (Random(source) & 31) + 12;
    lifetime = (Random(source) & 63) + 20;
    SpriteEffectController_Create(position, &velocity, 0x1C1A, 0xD1B8, 3, 0, 0x31, 0, 2, 0, 0,
                                  lifetime, 4, 1, 0x200004, 30, 8, 8);
}
