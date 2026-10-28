// MATCHING_CONCESSION: register-pinning
#include "scene_actor_reset.h"
#include "engine.h"
#include "ram.h"
extern const s16 gUnk_086149C4[];
u32 Slime_GetRandomVariantFlags(void *);
s32 Math_Atan2(s32, s32);
void sub_08127590(void *, void *, u32, u32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32);
static inline u16 Random(void)
{
    const u16 *table = gRandomTable;
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    index *= 2;
    return *(const u16 *)((u32)table + index);
}
static inline s32 Scale(s32 n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}
static inline void Polar(s32 angle, struct ContactPosition *v, s32 radius)
{
    v->x = Scale(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->y = 0;
    v->z = Scale(gUnk_086149C4[angle & 255] * radius);
}
static inline void Delta(struct ContactPosition *v, struct ContactPosition *from,
                         struct ContactPosition *to)
{
    v->x = to->x - from->x;
    v->y = to->y - from->y;
    v->z = to->z - from->z;
}
// confidence=low: launches three randomized reaction projectiles in independent random directions.
#ifdef NONMATCHING
// fuzzy: 80.4%
// Remaining differences: pointer spills/copies and random/polar-vector register lifetimes.
void Slime_EmitScatteredReactionBurst(struct SceneActorRegistryResetView *input)
{
    register struct SceneActorRegistryResetView *actor asm("sl") = input;
    struct ContactPosition velocity;
    u32 variant = Slime_GetRandomVariantFlags(actor);
    s32 jitter = Random() & 15;
    s32 angle;
    u16 *palette;
    u16 *offset;
    u16 *power;
    u16 *parameter;
    u32 *flags;
    u16 *value;
    struct ContactPosition *position;
    Delta(&velocity, &actor->position, (void *)(actor->fieldDC + 12));
    angle = Random() & 255;
    Polar(angle + jitter, &velocity, (Random() & 7) + 12);
    velocity.y = 48;
    {
        u32 sum, attack, response, bits, v;
        palette = &actor->basePaletteBank;
        offset = &actor->paletteOffset;
        sum = *palette + *offset;
        power = &actor->fieldB6;
        attack = *power;
        parameter = &actor->fieldBE;
        response = *parameter;
        flags = &actor->wordD0;
        bits = *flags | variant;
        value = &actor->fieldC6;
        v = *value;
        position = &actor->position;
        sub_08127590(position, &velocity, 0xB96C, 0xA929, 6, 24, 0, 0, sum, 0, 0, attack, response,
                     bits, v);
    }
    jitter = Random() & 15;
    angle = Random() & 255;
    Polar(angle + jitter, &velocity, (Random() & 7) + 12);
    velocity.y = 37;
    {
        u32 sum = *palette + *offset;
        u32 attack = *power;
        u32 response = *parameter;
        u32 bits = *flags | variant;
        u32 v = *value;
        sub_08127590(position, &velocity, 0xB96C, 0xA929, 7, 24, 0, 0, sum, 0, 0, attack, response,
                     bits, v);
    }
    jitter = Random() & 15;
    angle = Random() & 255;
    Polar(angle + jitter, &velocity, (Random() & 7) + 6);
    velocity.y = 32;
    {
        u32 sum = *palette + *offset;
        u32 attack = *power;
        u32 response = *parameter;
        u32 bits = *flags | variant;
        u32 v = *value;
        sub_08127590(position, &velocity, 0xB96C, 0xA929, 6, 24, 0, 0, sum, 1, 0, attack, response,
                     bits, v);
    }
}

#else
INCLUDE_ASM("asm/nonmatching", Slime_EmitScatteredReactionBurst);
#endif
