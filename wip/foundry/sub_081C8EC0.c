#include "global.h"
#define StateActor_SpawnTriggerSparkleParticle sub_081C8EC0
extern const u16 gUnk_0203B400[];
extern u32 gUnk_03005308;
extern const s16 gUnk_086149C4[];
struct ContactPosition {
    s16 x, y, z, pad;
} __attribute__((aligned(4)));
struct SpriteEffectController *SpriteEffectController_Create(const struct ContactPosition *,
                                                             const struct ContactPosition *, s32,
                                                             u32, u32, u32, u32, u32, u32, u32, u32,
                                                             u32, s32, s32, s32, s32, s32, s32);
static inline s32 Truncate(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
void sub_081C8EC0(u8 *b)
{
    u8 *base20 = b + 0x20;
    u32 *counter = &gUnk_03005308;
    u8 angle;
    u32 magnitude;
    struct ContactPosition velocity;
    u32 lifetime;
    {
        const u16 *table = gUnk_0203B400;
        u32 index = (*counter + 1) & 1023;
        const u8 *sample = (const u8 *)table + index * 2;
        s32 t;
        angle = *sample;
        index = (index + 1) & 1023;
        *counter = index;
        t = table[index];
        t = t >> 4;
        magnitude = (t & 0x3f) + 0x20;
    }
    {
        struct ContactPosition *v = &velocity;
        v->x = Truncate(gUnk_086149C4[(angle + 0x40) & 0xff] * magnitude);
        v->y = 0;
        v->z = Truncate(gUnk_086149C4[angle] * magnitude);
    }
    {
        u32 v = (*counter + 1) & 1023;
        s32 t;
        *counter = v;
        t = gUnk_0203B400[v];
        t = t >> 4;
        lifetime = (t & 0xf) + 7;
    }
    SpriteEffectController_Create((struct ContactPosition *)(base20 + 8), &velocity, 0x1c1e, 0xd43f,
                                  0, 0, 0x11, 0, 2, 6, 0, lifetime, 0, 0, 0, 0, -4, -4);
}
