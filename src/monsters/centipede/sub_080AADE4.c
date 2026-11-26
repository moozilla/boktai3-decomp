#include "monsters/centipede/centipede_body.h"
extern const s16 gUnk_086149C4[];
void Centipede_ApplyPendingRadialVelocity(struct TentacleBody *body)
{
    s32 *state = &body->pendingImpulse;
    u32 stored = *state;
    if (stored != 0x7FFFFFFF) {
        struct TentacleAnimationSegment *segment = body->impulseSegment;
        u8 *anglep;
        u16 *radiusp;
        s16 *v;
        u32 angle;
        u32 radius;
        s32 value;
        s32 x;
        u32 zero;
        *state = 0x7FFFFFFF;
        anglep = &segment->impulseHeading;
        radiusp = &segment->bodyCollider.responseParameter;
        v = &segment->offset.x;
        angle = *anglep;
        radius = *radiusp;
        value = gUnk_086149C4[(angle + 0x40) & 0xFF] * radius;
        if (value >= 0) {
            x = value >> 12;
        } else {
            x = -((-value) >> 12);
        }
        zero = 0;
        v[0] = x;
        v[1] = zero;
        value = gUnk_086149C4[angle] * radius;
        if (value >= 0) {
            value >>= 12;
        } else {
            value = -((-value) >> 12);
        }
        v[2] = value;
    }
}
