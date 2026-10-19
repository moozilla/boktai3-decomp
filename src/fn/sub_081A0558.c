#include "global.h"
struct ActorActionSequence {
    u8 unknown0[0x51];
    u8 angle;
    u8 unknown52[0x9C - 0x52];
    u32 flags;
    u8 unknownA0[3];
    u8 animationCompleted;
    u32 timer;
    u8 direction4, direction8;
    u8 unknownAA[5];
    u8 done;
    u8 unknownB0;
    u8 pending;
    u8 repeat;
    u8 unknownB3[0xCC - 0xB3];
    u8 requestedAngle;
    u8 unknownCD[0x104 - 0xCD];
    void *callback;
    u8 unknown108[0x174 - 0x108];
    u8 animationResource[0x20];
    u8 animation[0x3a4 - 0x194];
    u32 motionState[2];
};
void Hresvelgr_SetAnimation(void *, void *, void *, s32, s32, s32);
void Sound_PlaySong(u32);
s32 Callback_InvokeActor(void *, void *);
void sub_0819C6AC(struct ActorActionSequence *);
void sub_0819CD60(struct ActorActionSequence *);
s32 SceneMover_IsNearTarget(void *, s32);
static inline void Clear(void *dest, u32 *zero)
{
    *zero = 0;
    CpuSet(zero, dest, 0x5000002);
}
void Hresvelgr_UpdateTargetMotion(struct ActorActionSequence *p)
{
    {
        u32 mask = 0x400040;
        p->flags |= mask;
    }
    if (p->pending) {
        u32 zero;
        p->pending = 0;
        Clear(p->motionState, &zero);
        p->angle = p->requestedAngle;
        {
            s32 angle = (p->angle + 0x60) >> 6;
            u32 direction = 3;
            direction &= angle;
            p->direction4 = direction;
        }
        {
            s32 angle = (p->angle + 0x70) >> 5;
            u32 direction = 7;
            direction &= angle;
            p->direction8 = direction;
        }
        Hresvelgr_SetAnimation(p, p->animation, p->animationResource, 0, 1, 0);
        sub_0819C6AC(p);
    }
    if ((u8)Callback_InvokeActor(p, p->callback))
        return;
    if (p->repeat)
        Sound_PlaySong(0x3d9);
    if (!SceneMover_IsNearTarget(p, 4)) {
        sub_0819CD60(p);
        if (++p->timer > 0x40) {
            p->timer = 1;
            sub_0819C6AC(p);
        }
    } else
        sub_0819C6AC(p);
}
