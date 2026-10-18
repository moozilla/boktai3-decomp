#define HomingEffect_TurnTowardTarget sub_081B6BC4
#include "global.h"
struct EffectTargetPosition {
    s16 x;
    u16 pad2;
    s16 y;
};
struct HomingEffect {
    u8 pad0[9];
    u8 direction;
    u16 targetAngle;
    u16 angle;
    u16 turnSpeed;
    u8 pad10[4];
    s16 x;
    u16 pad16;
    s16 y;
    u8 pad1A[0x54 - 0x1A];
    struct EffectTargetPosition *target;
};
s32 Math_Atan2(s32, s32);
void sub_081B6BC4(struct HomingEffect *effect)
{
    u16 delta;
    s16 limit;
    effect->targetAngle = Math_Atan2(effect->target->x - effect->x, effect->target->y - effect->y)
                          << 8;
    limit = effect->turnSpeed * 12;
    delta = effect->targetAngle - effect->angle;
    if ((s16)delta < -limit)
        delta = -limit;
    else if ((s16)delta > limit)
        delta = limit;
    {
        u32 angle = effect->angle;
        angle += delta;
        effect->angle = angle;
    }
    effect->direction = effect->angle >> 8;
}
