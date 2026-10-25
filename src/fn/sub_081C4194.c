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
// Tentative: emits the transition's sixteen-particle burst.
void SceneTransition_SpawnParticleBurst(s32 x, s32 y, s32 flags)
{
    struct EffectVector position, velocity;
    s32 i;
    for (i = 0; i < 16; i++) {
        const u16 *table;
        u32 *seed;
        u16 index;
        u32 mask;
        s32 angle, random;
        struct EffectVector *pos;
        table = gUnk_0203B400;
        seed = &gUnk_03005308;
        index = *seed + 1;
        mask = 0x3FF;
        index &= mask;
        angle = table[index];
        angle >>= 4;
        angle &= 0xF;
        index = (index + 1) & mask;
        *seed = index;
        random = table[index];
        random >>= 4;
        random &= 7;
        SpriteEffect_SetPolarVelocity(&velocity, angle + 0x18, random + (1 << (i >> 2)) + 1);
        pos = &position;
        pos->x = x;
        pos->y = y;
        pos->z = 0;
        SpriteEffectController_Create(&position, &velocity, 0x1C1E, 0xD43F, 0, 0x10, 0x11, 1, 2,
                                      flags, 0, 90, 0, 0, 0, 0, -4, -4);
        table = gUnk_0203B400;
    }
}
