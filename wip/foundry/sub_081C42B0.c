#define SpriteEffects_EmitFanParticle sub_081C42B0
#define gRandomSamples gUnk_0203B400
#define gRandomCursor gUnk_03005308
#define gSineTable gUnk_086149C4
#include "global.h"
#include "sprite_effect_controller.h"
extern u32 gUnk_03005308;
extern const s16 gUnk_086149C4[];
struct SpriteEffectController *SpriteEffectController_Create(const struct ContactPosition *,
                                                             const struct ContactPosition *, s32,
                                                             u32, u32, u32, u32, u32, u32, u32, u32,
                                                             u32, s32, s32, s32, s32, s32, s32);
static inline s32 Truncate(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
void sub_081C42B0(s32 x, s32 y, s32 flags)
{
    struct ContactPosition position;
    struct ContactPosition velocity;
    struct ContactPosition *p = &position;
    s32 zero = 0;
    s32 angle, magnitude;
    u32 index;
    p->x = x;
    p->y = y;
    p->z = zero;
    index = gUnk_03005308;
    index = (index + 1) & 1023;
    {
        s32 sample = gUnk_0203B400[index];
        angle = ((sample >> 4) & 31) + 16;
    }
    index = (index + 1) & 1023;
    gUnk_03005308 = index;
    {
        s32 sample = gUnk_0203B400[index];
        magnitude = ((sample >> 4) & 3) + 3;
    }
    {
        struct ContactPosition *v = &velocity;
        v->x = Truncate(magnitude * gUnk_086149C4[angle + 64]);
        v->y = Truncate(magnitude * gUnk_086149C4[angle]);
        v->z = zero;
    }
    {
        u32 drawableFlags = (flags & 1) ? 0x200 : 0x400;
        drawableFlags |= 0x10;
        SpriteEffectController_Create(&position, &velocity, 0x8639, 0x75f6, 0, drawableFlags, 9,
                                      0x153, 2, 0, 0, 30, 0, 0, 0, 0, 0, 0);
    }
}
