#include "monsters/centipede/centipede_body.h"
#include "monsters/scene_route_sample.h"
extern const s16 gUnk_086149C4[];
void Centipede_ApplyPendingRadialVelocity(struct TentacleBody *body)
{
    if (body->pendingImpulse != 0x7FFFFFFF) {
        struct TentacleAnimationSegment *segment = body->impulseSegment;
        u8 *anglep;
        u16 *radiusp;
        s16 *v;
        u32 angle;
        u32 radius;
        s32 value;
        s32 x;
        u32 zero;
        body->pendingImpulse = 0x7FFFFFFF;
        anglep = &segment->impulseHeading;
        radiusp = &segment->bodyCollider.responseParameter;
        v = &segment->offset.x;
        angle = *anglep;
        radius = *radiusp;
        value = gUnk_086149C4[(angle + 0x40) & 0xFF] * radius;
        x = SceneRoute_ScaleComponent(value);
        zero = 0;
        v[0] = x;
        v[1] = zero;
        value = gUnk_086149C4[angle] * radius;
        value = SceneRoute_ScaleComponent(value);
        v[2] = value;
    }
}
