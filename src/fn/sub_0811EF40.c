#define ActorEmitter_CanEmit Modifier_IsFirstSourceReady
#include "global.h"
struct EmitterVector {
    s32 x, y, z;
    u32 : 32;
};
struct EmitterActor {
    u8 pad0[0x38];
    s32 height;
};
struct EmitterOwner {
    u8 pad0[0x28];
    struct EmitterVector position;
    u8 pad38[0x18];
    s32 height;
    u8 pad54[0x1C];
    s16 energy;
    u8 pad72[0x25A];
    u8 profile[0xA];
    u16 emissionTimer;
    u8 pad2D8[0x610];
    s32 velocity;
    u8 pad8EC[0xC];
    s8 kind;
};
struct ActorEmitter {
    u8 pad0[8];
    u16 id;
    u8 padA[6];
    u16 duration;
    u8 pad12[0x46];
    u16 timer;
    u8 pad5A[6];
    struct EmitterOwner *owner;
};
u8 Modifier_IsFirstSourceReady(struct ActorEmitter *);
s32 CallbackList_RunMatchingState(void *, u32, s32);
s32 sub_08114928(struct EmitterVector *, s32, struct EmitterVector *, void *, s32, s32, s32, s32);
void CallbackList_SubtractClampedValue(void *, struct ActorEmitter *, s32);
s32 ActorEmitter_TryEmitLow(struct ActorEmitter *emitter)
{
    struct EmitterOwner *owner;
    struct EmitterActor *actor;
    s32 cost;
    struct EmitterVector velocity;
    s32 height;
    if (!Modifier_IsFirstSourceReady(emitter))
        goto fail;
    owner = emitter->owner;
    actor = (struct EmitterActor *)((u8 *)owner + 0x18);
    cost = CallbackList_RunMatchingState(actor, emitter->id, 9);
    if (owner->energy < cost)
        goto fail;
    velocity.x = owner->velocity;
    velocity.y = 0;
    velocity.z = 0;
    height = actor->height - 0x12000;
    if (sub_08114928(&owner->position, height, &velocity, owner->profile, owner->kind + 1, 0, 1,
                     0) >= 0)
        goto success;
fail:
    return 0;
success:
    CallbackList_SubtractClampedValue(actor, emitter, cost);
    owner->emissionTimer = 0;
    emitter->timer = emitter->duration;
    return 1;
}
