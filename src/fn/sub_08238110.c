// MATCHING_CONCESSION: register-pinning
#include "global.h"
extern const s16 gUnk_086149C4[];
void PositionedSprite_Init(void *, void *, s32, s32);
void SpritePulseEffect_InitCollider(void *, u32, s32, s32, s32, u32);
static inline s32 scale(s32 n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}
// confidence=low: heading-based velocity and impact variant initialize the moving particle task.
#define PlayerOrbitEffect_InitMovingParticle sub_08238110
s32 PlayerOrbitEffect_InitMovingParticle(u8 *p, void *position, u32 angle, s32 speed, s32 a, s32 b,
                                         s32 c, s32 d, s32 e, u32 f)
{
    PositionedSprite_Init(p, position, a, b);
    SpritePulseEffect_InitCollider(p, angle, c, d, e, f);
    {
        s32 value = scale(speed * gUnk_086149C4[(angle + 64) & 255]);
        register u16 *velocity asm("r0") = (u16 *)(p + 0xB4);
        u32 zero = 0;
        velocity[0] = value;
        ++velocity;
        *velocity = zero;
    }
    {
        s32 value = scale(speed * gUnk_086149C4[angle & 255]);
        u16 *velocity = (u16 *)(p + 0xB8);
        u32 zero = 0;
        s32 direction;
        *velocity = value;
        direction = (((u16)angle + 0x70) >> 5) & 7;
        *(u16 *)(p + 0xC0) = direction;
        *(u16 *)(p + 0xC2) = zero;
    }
    return 0;
}
