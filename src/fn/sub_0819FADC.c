#include "global.h"
#ifdef NONMATCHING
// fuzzy: 86.5%
// foundry draft (worker best 86.5%); what differs is mostly register allocation and instruction order
#define Hresvelgr_UpdateMotionPhase sub_0819FADC
struct ActorReaction {
    u8 unknown0[0xAF];
    u8 finished;
    u8 unknownB0;
    u8 pending;
    u8 animationDone;
    u8 unknownB3[0xC1];
    u8 collider174[0x20], collider194[0x20];
};
void Hresvelgr_SetAnimation(struct ActorReaction *, void *, void *, u32, u32, u32);
void Anim_SetFrameDurationScale(void *, u32);
void sub_0819C8F8(struct ActorReaction *, u32, u32);
static inline s32 UpdateMotionAnimation(struct ActorReaction *p)
{
    if (p->pending) {
        p->pending = 0;
        Hresvelgr_SetAnimation(p, p->collider194, p->collider174, 17, 2, 6);
        Anim_SetFrameDurationScale(p->collider194, 48);
    }
    if (p->animationDone) {
        Anim_SetFrameDurationScale(p->collider194, 64);
        p->finished = 1;
        return 1;
    }
    return 0;
}
void sub_0819FADC(struct ActorReaction *actor, u32 frame)
{
    if (!UpdateMotionAnimation(actor))
        sub_0819C8F8(actor, 12, frame << 2);
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0819FADC);
#endif
