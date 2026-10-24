#include "global.h"
struct EffectVector {
    s16 x, y, z, pad;
};
struct SpriteEffectController;
extern const u16 gUnk_0203B400[];
extern u32 gUnk_03005308;
extern const s16 gUnk_086149C4[];
struct SpriteEffectController *SpriteEffectController_Create(const struct EffectVector *,
                                                             const struct EffectVector *, s32, u32,
                                                             u32, u32, u32, u32, u32, u32, u32, u32,
                                                             s32, s32, s32, s32, s32, s32);
static inline s32 Scale(s32 v)
{
    return v >= 0 ? v >> 12 : -((-v) >> 12);
}
// confidence=low: spawns a sprite effect at (x, y) flying off at a random angle and speed;
// bit 0 of flags picks the 0x200 attribute instead of 0x400.
#ifdef NONMATCHING
// fuzzy: 81.2%
// The ROM computes angle + 0x10 after the second random draw, and the velocity pointer late (it lives in r3, then r7).
void sub_081C42B0(s32 x, s32 y, u32 flags)
{
    struct EffectVector position, velocity;
    struct EffectVector *pos = &position, *vel;
    const u16 *table;
    u32 *seed;
    u16 index;
    u32 mask;
    s32 angle, speed, random;
    u32 attribute;
    pos->x = x;
    pos->y = y;
    pos->z = 0;
    table = gUnk_0203B400;
    seed = &gUnk_03005308;
    index = *seed + 1;
    mask = 0x3FF;
    index &= mask;
    angle = table[index];
    angle >>= 4;
    angle = (angle & 0x1F) + 0x10;
    index = (index + 1) & mask;
    *seed = index;
    random = table[index];
    random >>= 4;
    speed = (random & 3) + 3;
    vel = &velocity;
    vel->x = Scale(gUnk_086149C4[angle + 0x40] * speed);
    vel->y = Scale(gUnk_086149C4[angle] * speed);
    vel->z = 0;
    attribute = 0x400;
    if (flags & 1)
        attribute = 0x200;
    SpriteEffectController_Create(&position, vel, 0x8639, 0x75F6, 0, attribute | 0x10, 9, 0x153, 2,
                                  0, 0, 30, 0, 0, 0, 0, 0, 0);
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081C42B0);
#endif
