#include "global.h"
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
void Hresvelgr_UpdateMotionPhase(struct ActorReaction *actor, u32 frame)
{
    struct ActorReaction *a2;
    struct ActorReaction *q;
    q = actor;
    a2 = *(&actor);
    if (!UpdateMotionAnimation(q))
        sub_0819C8F8(a2, 12, frame << 2);
}
