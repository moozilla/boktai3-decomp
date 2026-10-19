#include "global.h"
/* Tentative family name; visual identity is not yet established. */
#include "contact_actor.h"
#define ActorEffect_EmitParticleFan sub_0812A928
#define Particle_EmitWithVelocity   sub_08125EC4
#define gRandomIndex                gUnk_03005308
#define gRandomTable                gUnk_0203B400
#define gSineTable                  gUnk_086149C4
extern const u16 gUnk_0203B400[];
extern u32 gUnk_03005308;
extern const s16 gUnk_086149C4[];
void sub_08125EC4(struct ContactPosition *, struct ContactPosition *, s32, s32);
static inline s32 truncateProduct(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
void sub_0812A928(struct ContactPosition *position, u8 direction)
{
    const u16 *random = gUnk_0203B400;
    struct ContactPosition velocity;
    struct ContactPosition *v = &velocity;
    s32 angle = direction + 214;
    s32 count = 5;
    do {
        u8 currentAngle = angle;
        s32 magnitude;
        u32 index;
        gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
        magnitude = (random[gUnk_03005308] & 3) + 4;
        v->x = truncateProduct(gUnk_086149C4[(currentAngle + 64) & 255] * magnitude);
        v->y = 0;
        v->z = truncateProduct(gUnk_086149C4[currentAngle] * magnitude);
        index = (gUnk_03005308 + 1) & 1023;
        v->y = random[index] & 15;
        index = (index + 1) & 1023;
        gUnk_03005308 = index;
        sub_08125EC4(position, &velocity, (random[index] & 15) + 30, 5);
        angle += 14;
    } while (--count >= 0);
}
