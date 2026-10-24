#include "global.h"
struct TurningEffectActor {
    u8 unknown0[0x51];
    u8 direction;
    u8 unknown52[0x56];
    u8 facing4, facing8, animation;
};
s32 sub_08249350(s32, s32);
static inline void update_facing(struct TurningEffectActor *actor, u8 *facingDirection)
{
    s32 direction = (*facingDirection + 96) >> 6;
    u8 mask = 3;
    do {
        mask &= direction;
    } while (0);
    actor->facing4 = mask;
    direction = (*facingDirection + 112) >> 5;
    mask = 7;
    mask &= direction;
    actor->facing8 = mask;
}

s32 ActorEffect_StepTurn(struct TurningEffectActor *actor, s32 frame, s32 start, s32 period,
                         s32 mode)
{
    s32 elapsed = frame - start;
    s32 step = elapsed / period;
    s32 remainder;
    u8 *direction;
    if (elapsed < 0)
        return 0;
    remainder = sub_08249350(elapsed, period);
    switch (mode) {
        case 0:
            if (remainder != 0)
                return 0;
            if (step == 4)
                return 1;
            if (step > 4)
                return 0;
            if ((step == 0) || (step == 3)) {
                direction = &actor->direction;
                *direction += 32;
            } else
                goto subtract;
            break;

        case 1: {
            s32 quarter = period >> 2;
            if ((remainder != 0) && (remainder != quarter))
                return 0;
        }
            if ((step == 4) && (remainder == 0))
                return 1;
            if (step > 4)
                return 0;
            if ((step == 0) || (step == 3)) {
                direction = &actor->direction;
                *direction += 32;
            } else
                goto subtract;
            break;

        default:
            goto animate;
    }

    goto facing;
subtract:
    direction = &actor->direction;
    *direction -= 32;

facing:
    update_facing(actor, direction);

animate: {
    u32 animation = 1;
    actor->animation = animation;
}

    return 0;
}
