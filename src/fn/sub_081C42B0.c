#include "global.h"
#include "engine.h"
#include "ram.h"
struct EffectVector {
    s16 x, y, z, pad;
};
struct SpriteEffectController;
extern const s16 gUnk_086149C4[];
struct SpriteEffectController *SpriteEffectController_Create(const struct EffectVector *,
                                                             const struct EffectVector *, s32, u32,
                                                             u32, u32, u32, u32, u32, u32, u32, u32,
                                                             s32, s32, s32, s32, s32, s32);
static inline s32 SpriteEffect_ScaleFixed(s32 v)
{
    return v >= 0 ? v >> 12 : -((-v) >> 12);
}
static inline void SpriteEffect_SetPolarVelocity(struct EffectVector *vel, s32 angle, s32 speed)
{
    vel->x = SpriteEffect_ScaleFixed(gUnk_086149C4[angle + 0x40] * speed);
    vel->y = SpriteEffect_ScaleFixed(gUnk_086149C4[angle] * speed);
    vel->z = 0;
}
// confidence=low: spawns a sprite effect at (x, y) flying off at a random angle and speed;
// bit 0 of flags picks the 0x200 attribute instead of 0x400.
void sub_081C42B0(s32 x, s32 y, u32 flags)
{
    struct EffectVector position, velocity;
    struct EffectVector *pos = &position;
    const u16 *table;
    u32 *seed;
    u16 index;
    u32 mask;
    s32 angle, random;
    u32 attribute;
    pos->x = x;
    pos->y = y;
    pos->z = 0;
    table = gRandomTable;
    seed = &gRandomIndex;
    index = *seed + 1;
    mask = 0x3FF;
    index &= mask;
    angle = table[index];
    angle >>= 4;
    angle = angle & 0x1F;
    index = (index + 1) & mask;
    *seed = index;
    random = table[index];
    random >>= 4;
    random &= 3;
    SpriteEffect_SetPolarVelocity(&velocity, angle + 0x10, random + 3);
    flags &= 1;
    attribute = 0x400;
    if (flags)
        attribute = 0x200;
    attribute |= 0x10;
    SpriteEffectController_Create(&position, &velocity, 0x8639, 0x75F6, 0, attribute, 9, 0x153, 2,
                                  0, 0, 30, 0, 0, 0, 0, 0, 0);
}
