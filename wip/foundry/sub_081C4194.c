#define SpriteEffects_EmitFanBurst sub_081C4194
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
void sub_081C4194(s32 x, s32 y, s32 variant)
{
    s32 i;
    struct ContactPosition position;
    struct ContactPosition velocity;
    struct ContactPosition *v = &velocity;
    struct ContactPosition *p = &position;
    s32 zero = 0;
    s32 variantCopy = variant;
    for (i = 0; i <= 15; i++) {
        s32 nibble, jitter, angle, pow2, magnitude;
        u32 index = gUnk_03005308;
        index = (index + 1) & 1023;
        {
            s32 sample = gUnk_0203B400[index];
            nibble = (sample >> 4) & 15;
        }
        index = (index + 1) & 1023;
        gUnk_03005308 = index;
        {
            s32 sample = gUnk_0203B400[index];
            jitter = (sample >> 4) & 7;
        }
        pow2 = 1 << (i >> 2);
        angle = nibble + 24;
        magnitude = jitter + pow2 + 1;
        v->x = Truncate(magnitude * gUnk_086149C4[angle + 64]);
        v->y = Truncate(magnitude * gUnk_086149C4[angle]);
        v->z = zero;
        p->x = x;
        p->y = y;
        p->z = zero;
        SpriteEffectController_Create(&position, &velocity, 0x1c1e, 0xd43f, 0, 0x10, 0x11, 1, 2,
                                      variantCopy, 0, 0x5a, 0, 0, 0, 0, -4, -4);
    }
}
