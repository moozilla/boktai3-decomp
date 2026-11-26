#include "global.h"

struct O {
    u8 pad[0x54];
    u8 a;
    u8 pad2[2];
    u8 c;
    u16 h;
};

u8 Modifier_IsFirstSourceReady(struct O *o)
{
    if (o->a != 0 && o->h == 0 && o->c != 1)
        return 1;
    return 0;
}

struct CallbackList {
    u8 pad0[0x58];
    s16 value;
};

struct CallbackReduction {
    u8 pad0[8];
    u16 amount;
};

s32 CallbackList_RunMatchingState(struct CallbackList *, s32, s32);

void CallbackList_SubtractClampedValue(struct CallbackList *list,
                                       struct CallbackReduction *reduction, s32 amount)
{
    if (amount <= 0) {
        amount = reduction->amount;
        amount = CallbackList_RunMatchingState(list, amount, 9);
    }
    list->value -= amount;
    if (list->value < 0)
        list->value = 0;
}

#define ActorEmitter_CanEmit Modifier_IsFirstSourceReady

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
    u8 profile[0xC];
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

u8 Modifier_IsFirstSourceReady();


s32 NativeEffect_CreateFromProfile(struct EmitterVector *, s32, struct EmitterVector *, void *, s32,
                                   s32, s32, s32);

void CallbackList_SubtractClampedValue();

s32 ActorEmitter_TryEmit(struct ActorEmitter *emitter)
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
    height = actor->height - 0x16000;
    if (NativeEffect_CreateFromProfile(&owner->position, height, &velocity, owner->profile,
                                       owner->kind + 1, 0, 1, 0) >= 0)
        goto success;
fail:
    return 0;
success:
    CallbackList_SubtractClampedValue(actor, emitter, cost);
    emitter->timer = emitter->duration;
    return 1;
}

struct LowEmitterOwner {
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

struct LowActorEmitter {
    u8 pad0[8];
    u16 id;
    u8 padA[6];
    u16 duration;
    u8 pad12[0x46];
    u16 timer;
    u8 pad5A[6];
    struct LowEmitterOwner *owner;
};

u8 Modifier_IsFirstSourceReady();

void CallbackList_SubtractClampedValue();

s32 ActorEmitter_TryEmitLow(struct LowActorEmitter *emitter)
{
    struct LowEmitterOwner *owner;
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
    if (NativeEffect_CreateFromProfile(&owner->position, height, &velocity, owner->profile,
                                       owner->kind + 1, 0, 1, 0) >= 0)
        goto success;
fail:
    return 0;
success:
    CallbackList_SubtractClampedValue(actor, emitter, cost);
    owner->emissionTimer = 0;
    emitter->timer = emitter->duration;
    return 1;
}

struct AnimatedEmitter {
    u8 pad0[8];
    u16 id;
    u8 padA[6];
    u16 duration;
    u8 pad12[2];
    u8 collision[0x40];
    u8 unknown54, entered;
    u8 pad56[2];
    u16 timer;
    u8 pad5A[2];
    u16 animation;
    u8 pad5E[2];
    struct EmitterOwner *owner;
};

u8 Modifier_IsFirstSourceReady();

void CallbackList_SubtractClampedValue();

void Anim_SetKeyframes(void *, void *, u32, u32, u32);

s32 Anim_Step(void *, void *);

u8 LinkParticipant_IsLocallySelected(void *);

void Sound_PlaySong(u32);

void Character_GetHeightAdjustedPosition(void *, s16 *);

void SpatialIndex_SetPosition(void *, s32, s32, s32);

void SpatialIndex_InsertEntry(void *);

s32 ActorEmitter_UpdateAnimation(struct AnimatedEmitter *emitter)
{
    struct EmitterOwner *owner;
    struct EmitterActor *actor;
    s32 cost;
    s16 position[3];
    s16 *pos;
    if (!Modifier_IsFirstSourceReady(emitter))
        goto fail;
    owner = emitter->owner;
    actor = (struct EmitterActor *)((u8 *)owner + 0x18);
    cost = CallbackList_RunMatchingState(actor, emitter->id, 9);
    if (owner->energy >= cost)
        goto active;
fail:
    return 0;
active:
    if (emitter->entered) {
        Anim_SetKeyframes((u8 *)owner + 0x670, (u8 *)owner + 0x7F0, emitter->animation, 1, 0);
        if (LinkParticipant_IsLocallySelected(owner))
            Sound_PlaySong(0x93);
        emitter->entered = 0;
    }
    CallbackList_SubtractClampedValue(actor, emitter, cost);
    Anim_Step((u8 *)owner + 0x670, (u8 *)owner + 0x7F0);
    pos = position;
    Character_GetHeightAdjustedPosition(actor, pos);
    emitter = (struct AnimatedEmitter *)emitter->collision;
    SpatialIndex_SetPosition(emitter, position[0], pos[1], pos[2]);
    SpatialIndex_InsertEntry(emitter);
    return 1;
}

struct PairEmitterActor {
    u8 pad0[0x10];
    struct EmitterVector position;
    u8 pad20[0x18];
    s32 height;
    u8 pad3C[0x278];
    u8 profile[0xC];
};

struct PairEmitter {
    u8 pad0[5];
    u8 kind;
    u8 pad6[2];
    u16 id;
    u8 padA[6];
    u16 duration;
    u8 pad12[0x46];
    u16 timer;
    u8 pad5A[6];
    struct EmitterOwner *owner;
};

u8 Modifier_IsFirstSourceReady();

void CallbackList_SubtractClampedValue();

s32 ActorEmitter_TryEmitPair(struct PairEmitter *emitter)
{
    struct EmitterOwner *owner;
    struct PairEmitterActor *actor;
    s32 cost, height;
    struct EmitterVector velocity;
    if (!Modifier_IsFirstSourceReady(emitter))
        goto fail;
    owner = emitter->owner;
    actor = (struct PairEmitterActor *)((u8 *)owner + 0x18);
    cost = CallbackList_RunMatchingState(actor, emitter->id, 9);
    if (owner->energy < cost)
        goto fail;
    velocity.x = owner->velocity;
    velocity.y = 0;
    velocity.z = 0;
    if (emitter->kind == 4)
        height = actor->height - 0x8000;
    else
        height = actor->height - 0x16000;
    if (NativeEffect_CreateFromProfile(&actor->position, height, &velocity, actor->profile,
                                       owner->kind + 1, 1, 2, 0) >= 0)
        goto success;
fail:
    return 0;
success:
    NativeEffect_CreateFromProfile(&actor->position, height, &velocity, actor->profile,
                                   owner->kind + 1, 0, 1, 1);
    CallbackList_SubtractClampedValue(actor, emitter, cost);
    emitter->timer = emitter->duration;
    return 1;
}

struct AnimatedPairActor {
    u8 pad0[0x10];
    struct EmitterVector position;
    u8 pad20[0x18];
    s32 height;
    u8 pad3C[0x278];
    u8 profile[0xC];
};

struct AnimatedPairEmitter {
    u8 pad0[5];
    u8 kind;
    u8 pad6[2];
    u16 id;
    u8 padA[6];
    u16 duration;
    u8 pad12[0x46];
    u16 timer;
    u8 pad5A[2];
    u16 animation;
    u8 pad5E[2];
    struct EmitterOwner *owner;
};

u8 Modifier_IsFirstSourceReady();

void CallbackList_SubtractClampedValue();

s32 ActorEmitter_TryEmitAnimatedPair(struct AnimatedPairEmitter *emitter)
{
    struct EmitterOwner *owner;
    struct AnimatedPairActor *actor;
    s32 height, cost;
    struct EmitterVector velocity;
    if (!Modifier_IsFirstSourceReady(emitter))
        goto fail;
    owner = emitter->owner;
    actor = (struct AnimatedPairActor *)((u8 *)owner + 0x18);
    cost = CallbackList_RunMatchingState(actor, emitter->id, 9);
    if (owner->energy < cost)
        goto fail;
    velocity.x = owner->velocity;
    velocity.y = 0;
    velocity.z = 0;
    height = actor->height - 0x8000;
    if (NativeEffect_CreateFromProfile(&owner->position, height, &velocity, owner->profile,
                                       owner->kind + 1, 1, 2, 0) >= 0)
        goto success;
fail:
    return 0;
success:
    NativeEffect_CreateFromProfile(&owner->position, height, &velocity, owner->profile,
                                   owner->kind + 1, 0, 1, 1);
    Anim_SetKeyframes((u8 *)owner + 0x670, (u8 *)owner + 0x7F0, emitter->animation, 2, 0);
    CallbackList_SubtractClampedValue(actor, emitter, cost);
    emitter->timer = emitter->duration;
    return 1;
}

// Tentative descriptive names; inferred from static field accesses and callees.

struct EffectHalfPair {
    u32 lo : 16;
    s32 hi : 16;
};

struct EffectExtent {
    struct EffectHalfPair x, y;
};

struct EffectSource {
    u8 pad0[0xa];
    u16 id;
    u8 padC[8];
    u8 origin[1];
};

void SpatialIndex_InitEntry(void *, void *, struct EffectExtent *, struct EffectExtent *, u32, u32,
                            u32, u32, u32, u32, u32);

void ActorEffect_EmitOffsetPair(void *owner, void *unused, struct EffectSource *source, u32 arg,
                                u32 kind)
{
    s16 position[4];
    struct EffectExtent extent, offset;
    Character_GetHeightAdjustedPosition(owner, position);
    extent.x.lo = 24;
    extent.x.hi = 2;
    extent.y.lo = 12;
    offset.x.lo = 32;
    offset.x.hi = -extent.x.hi;
    offset.y.lo = -extent.y.lo;
    SpatialIndex_InitEntry(source->origin, position, &extent, &offset, 12, kind, 5, source->id, 30,
                           arg, (u32)owner);
}

// Tentative descriptive names; inferred from static field accesses and callees.

struct ActorAction {
    void (*callback)(void);
    u8 unknown4, mode;
    u8 parameters[0x10 - 2];
    u8 unknown14[0x40];
    u8 active, ready;
    u8 unknown56[6];
    u16 duration, animation;
    void *owner;
};

struct ActorActionHost {
    u8 unknown0[0x55];
    u8 kind;
    u8 unknown56[0x2b0 - 0x56];
    struct ActorAction action;
};

void Memory_Clear(void *, u32);

void ScriptRecord_FindExtendedModifier(void *, s32);

s32 ActorEmitter_TryEmit(), ActorEmitter_TryEmitPair(), ActorEmitter_TryEmitLow(),
    ActorEmitter_TryEmitAnimatedPair(), ActorEmitter_UpdateAnimation();

void ActorAction_InitializeMode(struct ActorActionHost *host, void *owner)
{
    u8 *source = (u8 *)host + 0x48;
    struct ActorAction *action = &host->action;
    u16 *params;
    action->callback = 0;
    host->action.active = 0;
    action->owner = owner;
    params = (u16 *)((u8 *)host + 0x2b4);
    Memory_Clear(params, 16);
    ScriptRecord_FindExtendedModifier(params, source[13]);
    switch (action->mode) {
        case 0:
            action->callback = ActorEmitter_TryEmit;
            action->ready = 1;
            action->animation = 6;
            action->duration = 0xffff;
            action->active = 1;
            break;
        case 1:
            action->callback = ActorEmitter_TryEmitPair;
            action->ready = 1;
            action->animation = 7;
            action->duration = 0xffff;
            action->active = 1;
            break;
        case 2:
            action->callback = ActorEmitter_TryEmitLow;
            action->ready = 1;
            action->animation = 8;
            action->duration = 0xffff;
            action->active = 1;
            break;
        case 3:
            action->callback = ActorEmitter_TryEmitAnimatedPair;
            action->ready = 1;
            action->animation = 9;
            action->duration = 30;
            action->active = 1;
            break;
        case 4:
            action->callback = ActorEmitter_TryEmitPair;
            action->ready = 1;
            action->animation = 10;
            action->duration = 0xffff;
            action->active = 1;
            break;
        case 5:
            action->callback = ActorEmitter_UpdateAnimation;
            action->ready = 1;
            action->animation = 11;
            action->duration = 31;
            action->active = 1;
            break;
    }
}
