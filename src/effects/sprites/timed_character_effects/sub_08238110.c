#include "global.h"
extern const s16 gUnk_086149C4[];
void PositionedSprite_Init(void *, void *, s32, s32);
void SpritePulseEffect_InitCollider(void *, u32, s32, s32, s32, u32);
static inline s32 scale(s32 n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}
struct MovingParticle {
    u8 unknown0[0xb4];
    struct ParticleVelocity {
        s16 x, y, z, pad;
    } velocity;
    u8 unknownBC[4];
    u16 direction, phase;
};
// confidence=low: heading-based velocity and impact variant initialize the moving particle task.
#define PlayerOrbitEffect_InitMovingParticle sub_08238110
s32 PlayerOrbitEffect_InitMovingParticle(struct MovingParticle *p, void *position, u32 angle,
                                         s32 speed, s32 spriteType, s32 spriteVariant, s32 c, s32 d,
                                         s32 e, u32 f)
{
    struct ParticleVelocity *velocity;
    s32 value;
    PositionedSprite_Init(p, position, spriteType, spriteVariant);
    SpritePulseEffect_InitCollider(p, angle, c, d, e, f);
    value = scale(speed * gUnk_086149C4[(angle + 64) & 255]);
    velocity = &p->velocity;
    velocity->x = value;
    p->velocity.y = 0;
    value = scale(speed * gUnk_086149C4[angle & 255]);
    p->velocity.z = value;
    p->direction = (((u16)angle + 0x70) >> 5) & 7;
    p->phase = 0;
    return 0;
}