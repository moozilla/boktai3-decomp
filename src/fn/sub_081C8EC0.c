#include "state_actor.h"
// MATCHING_CONCESSION: register-pinning
#ifdef NONMATCHING
// fuzzy: 83.9%
extern const s16 gUnk_086149C4[];
extern const u16 gUnk_0203B400[];
extern u32 gUnk_03005308;
void *SpriteEffectController_Create(void *, void *, u32, u32, u32, u32, u32, u32, u32, u32, u32,
                                    u32, u32, u32, u32, u32, s32, s32);
static inline s32 Scale(s32 value)
{
    return value >= 0 ? value >> 12 : -(-value >> 12);
}
static inline s32 Random(void)
{
    const u16 *table = gUnk_0203B400;
    u32 *cursor = &gUnk_03005308;
    *cursor = (*cursor + 1) & 1023;
    return table[*cursor];
}
// confidence=low: emits a radial effect around the actor tag's position.
#define StateActor_SpawnRadialEffect sub_081C8EC0
void StateActor_SpawnRadialEffect(struct StateActor *actor)
{
    struct StateActorPos velocity;
    struct StateActorTag *tag = &actor->tag;
    register u32 unused asm("r5") = 0x0403B714;
    s32 angle;
    s32 radius, frames;
    struct StateActorPos *v;
    asm("" ::"r"(unused));
    angle = (u8)Random();
    radius = ((Random() >> 4) & 63) + 32;
    v = &velocity;
    v->x = Scale(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->y = 0;
    v->z = Scale(gUnk_086149C4[angle] * radius);
    frames = ((Random() >> 4) & 15) + 7;
    SpriteEffectController_Create((u8 *)tag + 8, &velocity, 0x1C1E, 0xD43F, 0, 0, 17, 0, 2, 6, 0,
                                  frames, 0, 0, 0, 0, -4, -4);
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081C8EC0);
#endif
