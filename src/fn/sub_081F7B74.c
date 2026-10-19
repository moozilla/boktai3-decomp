#include "global.h"
#ifdef NONMATCHING
// fuzzy: 86.5%
// foundry draft (worker best 86.5%); what differs is mostly register allocation and instruction order
#define PreviewActor_UpdateHeading sub_081F7B74
struct PreviewActor {
    u8 pad0[0x51];
    u8 heading;
    u8 pad52[0xa8 - 0x52];
    u8 direction4, direction8, animation;
    u8 padAB[4];
    u8 inputMode, active;
    u8 padB1[0xc4 - 0xb1];
    u32 timer;
    u8 padC8[4];
    u8 requestedHeading;
    u8 padCD[0x184 - 0xcd];
    u32 animationFlag;
    u8 pad188[0xe58 - 0x188];
    s16 variant;
};
static inline s32 ConsumeActivation(struct PreviewActor *p)
{
    if (p->active) {
        p->active = 0;
        p->inputMode = 0;
        return 1;
    }
    return 0;
}
void sub_081F7B74(struct PreviewActor *p)
{
    struct PreviewActor *q = p;
    p->heading = p->requestedHeading;
    {
        s32 angle = (p->heading + 0x60) >> 6;
        u16 mask = 3;
        mask &= angle;
        p->direction4 = mask;
    }
    {
        s32 angle = (p->heading + 0x70) >> 5;
        u16 mask = 7;
        mask &= angle;
        p->direction8 = mask;
    }
    if (ConsumeActivation(p) == 1) {
        if (q->variant == 0)
            p->animation = 1;
        else {
            u32 animation = 2;
            p->animation = animation;
        }
    }
    if (p->inputMode)
        q->animationFlag = 1;
    p->timer++;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081F7B74);
#endif
