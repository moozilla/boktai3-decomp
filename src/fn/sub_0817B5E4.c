#include "global.h"
#ifdef NONMATCHING
// fuzzy: 82.6%
// foundry draft (worker best 82.6%); what differs is mostly register allocation and instruction order
#define ActorEffect_StepTurn sub_0817B5E4
struct TurningEffectActor {
    u8 unknown0[0x51];
    u8 direction;
    u8 unknown52[0x56];
    u8 facing4, facing8, animation;
};
s32 sub_08249350(s32, s32);
static inline void update_facing(struct TurningEffectActor *actor)
{
    s32 direction = (actor->direction + 96) >> 6;
    u8 mask = 3;
    mask &= direction;
    actor->facing4 = mask;
    direction = (actor->direction + 112) >> 5;
    mask = 7;
    mask &= direction;
    actor->facing8 = mask;
}
s32 sub_0817B5E4(struct TurningEffectActor *actor, s32 frame, s32 start, s32 period, s32 mode)
{
    s32 elapsed = frame - start;
    s32 step = elapsed / period;
    s32 remainder;
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
            if (step == 0 || step == 3)
                actor->direction += 32;
            else
                goto subtract;
            break;
        case 1: {
            s32 quarter = period >> 2;
            if (remainder != 0 && remainder != quarter)
                return 0;
        }
            if (step == 4 && remainder == 0)
                return 1;
            if (step > 4)
                return 0;
            if (step == 0 || step == 3)
                actor->direction += 32;
            else
                goto subtract;
            break;
        default:
            goto animate;
    }
    goto facing;
subtract:
    actor->direction -= 32;
facing:
    update_facing(actor);
animate: {
    u32 animation = 1;
    actor->animation = animation;
}
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0817B5E4);
#endif
