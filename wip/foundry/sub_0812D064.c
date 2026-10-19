#define SpriteEffects_EmitRandomRadialParticle sub_0812D064
#define gRandomSamples                         gUnk_0203B400
#define gRandomCursor                          gUnk_03005308
#define gSineTable                             gUnk_086149C4
#include "global.h"
#include "sprite_effect_controller.h"
extern const u16 gUnk_0203B400[];
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
void sub_0812D064(struct ContactPosition *position)
{
    const u16 *table = gUnk_0203B400;
    struct ContactPosition velocity;
    struct ContactPosition *v;
    s32 angle, magnitude;
    u32 *cursor;
    u32 mask;
    {
        u32 index = ((cursor = &gUnk_03005308), *cursor + 1) & 1023;
        const u8 *sample = (void *)(index * 2 + (u32)table);
        mask = 255;
        angle = *sample;
        index = (index + 1) & 1023;
        *cursor = index;
        {
            s32 raw = *(const u16 *)(index * 2 + (u32)table);
            s32 bits = 15;
            raw &= bits;
            magnitude = raw;
        }
    }
    v = &velocity;
    magnitude += 4;
    {
        const s16 *sine = gUnk_086149C4;
        v->x = Truncate(sine[(angle + 64) & mask] * magnitude);
        v->y = 0;
        v->z = Truncate(sine[angle] * magnitude);
    }
    {
        const u16 *source;
        u32 index = (*cursor + 1) & 1023;
        s32 lifetime;
        source = (const u16 *)RANDOM_TABLE_ADDR;
        velocity.y = (*(const u16 *)(index * 2 + (u32)source) & 31) + 12;
        index = (index + 1) & 1023;
        *cursor = index;
        lifetime = (*(const u16 *)(index * 2 + (u32)source) & 63) + 20;
        SpriteEffectController_Create(position, &velocity, 0x1c1a, 0xd1b8, 3, 0, 0x31, 0, 2, 0, 0,
                                      lifetime, 4, 1, 0x200004, 30, 8, 8);
    }
}
