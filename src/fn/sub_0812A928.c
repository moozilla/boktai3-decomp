#include "global.h"
#include "engine.h"
#ifdef NONMATCHING
// fuzzy: 80.0%
// Open: loop-invariant order, sine-table reuse, and random-result allocation.
extern const s16 gUnk_086149C4[];
struct Vec {
    s16 x;
    s16 y;
    s16 z;
    s16 pad;
};
void SceneMovingParticle_Create(void *, void *, s32, s32);
inline static s32 Shift12(s32 x)
{
    if (x >= 0)
        return x >> 12;
    return -((-x) >> 12);
}

inline static u32 NextRandom(void)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return *((u16 *)((index << 1) + RANDOM_TABLE_ADDR));
}

void LinkedEffect_SetVariant(void *position, u8 variant)
{
    s32 count = 5;
    u16 randomHeight;
    struct Vec velocity;
    register struct Vec *v = &velocity;
    s32 angle = variant + 214;
    do {
        u32 heading = (u8)angle;
        u32 index;
        u32 speed;
        u32 lifetime;
        speed = (NextRandom() & 3) + 4;
        v->x = Shift12(gUnk_086149C4[(heading + 64) & 255] * speed);
        v->y = 0;
        v->z = Shift12(gUnk_086149C4[heading] * speed);
        index = (gRandomIndex + 1) & 1023;
        randomHeight = *((u16 *)(RANDOM_TABLE_ADDR + (index * 2)));
        v->y = randomHeight & 15;
        index = (index + 1) & 1023;
        lifetime = ((*((u16 *)(RANDOM_TABLE_ADDR + (index * 2)))) & 15) + 30;
        gRandomIndex = index;
        SceneMovingParticle_Create(position, &velocity, lifetime, 5);
        angle += 14;
    } while ((--count) >= 0);
}

#else
INCLUDE_ASM("asm/nonmatching", LinkedEffect_SetVariant);
#endif
