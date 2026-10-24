#include "state_actor.h"
// MATCHING_CONCESSION: register-pinning
extern const s16 gUnk_086149C4[];
extern const u16 gUnk_0203B400[];
extern u32 gUnk_03005308;
void *SpriteEffectController_Create(void *, void *, u32, u32, u32, u32, u32, u32, u32, u32, u32,
                                    u32, u32, u32, u32, u32, s32, s32);
static inline s32 Scale(s32 value)
{
    return value >= 0 ? value >> 12 : -(-value >> 12);
}
static inline s32 Random(const u16 *table)
{
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
    register u32 unusedAddress asm("r5") = 0x0403B714;
    s32 angle, radiusBits, scaledZ;
    s32 radius, frames;
    struct StateActorPos *v;
    angle = (u8)Random(gUnk_0203B400);
    radiusBits = (Random(gUnk_0203B400) >> 4) & 63;
    v = &velocity;
    radius = radiusBits + 32;
    v->x = Scale(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->y = 0;
    scaledZ = Scale(gUnk_086149C4[angle] * radius);
    asm("" ::"r"(unusedAddress));
    v->z = scaledZ;
    frames = ((Random((const u16 *)0x0203B400) >> 4) & 15) + 7;
    SpriteEffectController_Create((u8 *)tag + 8, &velocity, 0x1C1E, 0xD43F, 0, 0, 17, 0, 2, 6, 0,
                                  frames, 0, 0, 0, 0, -4, -4);
}
