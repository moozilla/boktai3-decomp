#include "global.h"
#include "contact_actor.h"
#define ActorEffect_SpawnFanParticles sub_0817AEC8
#define gEffectSineTable gUnk_086149C4
#define gEffectRandomTable gUnk_0203B400
#define gEffectRandomIndex gUnk_03005308
extern const s16 gUnk_086149C4[];
extern const u16 gUnk_0203B400[];
extern u32 gUnk_03005308;
struct SpriteEffectController;
struct SpriteEffectController *SpriteEffectController_Create(const struct ContactPosition *, const struct ContactPosition *, s32, u32, u32, u32, u32, u32, u32, u32, u32, u32, s32, s32, s32, s32, s32, s32);
static inline s32 random_value(void)
{
    u8 *table = (u8 *)gUnk_0203B400;
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    return *(u16 *)(table + (gUnk_03005308 << 1));
}
static inline s32 scale(s32 v)
{
    if (v >= 0) return v >> 12;
    return -((-v) >> 12);
}
void sub_0817AEC8(u8 *actor, s32 angle)
{
    s32 i;
    struct ContactPosition velocity;
    struct ContactPosition *v;
    for (i = 0; i < 6; i++) {
        s32 offset, radius, step, direction, speed, life;
        offset = (random_value() >> 4) & 63;
        v = &velocity;
        radius = (random_value() >> 4) & 31;
        step = i << 2;
        direction = offset + angle;
        direction -= 32;
        speed = radius + step;
        speed += 2;
        v->x = scale(gUnk_086149C4[(direction + 64) & 255] * speed);
        v->y = 0;
        v->z = scale(gUnk_086149C4[direction & 255] * speed);
        life = ((random_value() >> 4) & 15) + 16;
        SpriteEffectController_Create((struct ContactPosition *)(actor + 0x54), &velocity, 0x1C1E, 0xD43F, 0, 0, 17, 1, 1, 2, 0, life, 0, 0, 0, 0, -4, -4);
    }
}
