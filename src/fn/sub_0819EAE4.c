#include "global.h"
#define Callback_InvokeActor _call_via_r1
struct ActorActionSequence {
    u8 unknown0[0x51];
    u8 angle;
    u8 unknown52[0x9C - 0x52];
    u32 flags;
    u8 unknownA0[3];
    u8 animationCompleted;
    u8 unknownA4[4];
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
    u8 animation[1];
};
void Hresvelgr_SetAnimation(void *, void *, void *, s32, s32, s32);
void Sound_PlaySong(u32);
s32 Callback_InvokeActor(void *, void *);
void ActorAction_UpdateDirectionalAnimation(struct ActorActionSequence *p)
{
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
    {
        u32 mask = 0x200040;
        p->flags |= mask;
    }
    if (p->pending) {
        p->pending = 0;
        Hresvelgr_SetAnimation(p, p->animation, p->animationResource, 0x10, 1, 4);
        Sound_PlaySong(0x4D0);
    }
    if (p->repeat && p->animationCompleted > 5)
        p->done = 1;
    Callback_InvokeActor(p, p->callback);
}
