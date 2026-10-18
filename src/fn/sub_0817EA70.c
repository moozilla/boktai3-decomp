#include "global.h"
#define SceneEventActor_UpdateTimedAnimation sub_0817EA70
struct SceneEventActor {
    u8 unknown0[0x51];
    u8 angle;
    u8 unknown52[0x4A];
    u32 flags;
    u8 unknownA0[8];
    u8 facing, facing8;
    u8 unknownAA[4];
    u8 mode, finished;
    u8 unknownB0;
    u8 restart;
    u8 unknownB2[0x1A];
    u8 requestedAngle;
    u8 unknownCD[0x37];
    void *update;
};
void SceneActors_StartEightDirectionAnimation(void *, void *, void *, int, int, int, int);
s32 Callback_InvokeActor(void *, void *);
void ActorAttachment_PlayEffect(void *, s32);
void sub_0817EA70(struct SceneEventActor *actor, s32 tick)
{
    if (actor->restart) {
        actor->restart = 0;
        actor->angle = actor->requestedAngle;
        {
            int shifted; u32 value;
            shifted = (actor->angle + 0x60) >> 6;
            value = 3;
            value &= shifted;
            actor->facing = value;
            shifted = (actor->angle + 0x70) >> 5;
            value = 7;
            value &= shifted;
            actor->facing8 = value;
        }
        { u32 mask = 0x400004; actor->flags |= mask; }
    }
    SceneActors_StartEightDirectionAnimation(actor, (u8 *)actor + 0x2CC, (u8 *)actor + 0x2AC, 0xF4, 1, 1, 4);
    if (tick == 1) {
        if (actor->mode == 6)
            ActorAttachment_PlayEffect(actor, 3);
        else
            ActorAttachment_PlayEffect(actor, 1);
    }
    if (tick > 30)
        actor->finished = 1;
    if (!(u8)Callback_InvokeActor(actor, actor->update)) {
        u32 mask = 0x40;
        actor->flags |= mask;
    }
}
