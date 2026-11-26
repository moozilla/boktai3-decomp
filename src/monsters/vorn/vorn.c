#include "global.h"
#include "scene/actor/scene_actor_reset.h"
#include "npc/manager/scene_actor_animation.h"
#include "scene/actor/scene_actor_reaction.h"
#include "player/player.h"
#include "scene/actor/scene_actor_chase.h"
#include "debug.h"
#include "scene/actor/scene_actor_displacement.h"
#include "scene/actor/scene_actor_chase_reaction.h"
#include "scene/actor/scene_actor_control_phase.h"
#include "effects/particles/spark_particle.h"
#include "monsters/vorn/vorn.h"
#include "scene/actor/scene_actor_terrain.h"
#include "engine/engine.h"
#include "scene/actor/scene_actor_contact_response.h"
#include "scene/actor/scene_actor_chase_shared.h"
#include "scene/actor/scene_actor_turn.h"
#include "scene/actor/scene_actor_init_actor.h"

inline static u8 blocked2048(u8 *c)
{
    u32 mask;
    u32 *p;
    u32 off;
    if (!c)
        goto zero;
    mask = 0x40;
    off = 0xB5;
    off <<= 2;
    p = (u32 *)(c + off);
    if ((*((u16 *)p)) & mask)
        goto one;
    mask = 0x80;
    mask <<= 10;
    off -= 4;
    p = (u32 *)(c + off);
    if ((*p) & mask)
        goto one;
    if ((*((s16 *)(c + 0x9C))) > 0)
        goto zero;
one:
    return 1;
zero:
    return 0;
}

void Vorn_OnContactEvent(u8 *unused, u8 *obj, u8 *ctx)
{
    int mask;
    u32 *flags;
    if ((blocked2048(ctx) == 0) && ((*((u16 *)(obj + 0xA))) & 0xF)) {
        *((u32 *)(ctx + 0x304)) = *((u16 *)(obj + 4));
        flags = (u32 *)(ctx + 0x2D0);
        mask = 0xFEFFFFFF;
        *flags &= mask;
    }
}

extern const u8 gUnk_0824E20C[];

s32 SceneActor_TryLaunchSelectedItem(u8 *);

void Vorn_BeginResponsePursuit(u8 *, u32, const u8 *, u32);

void Vorn_OnScriptActionRequest(u32 a, u32 b, u8 *p)
{
    u8 *q = *(u8 **)(p + 0x3d0);
    switch (SceneActor_TryLaunchSelectedItem(q)) {
        case 0:
            *(u16 *)(q + 0x682) = 1;
        case 1:
            Vorn_BeginResponsePursuit(p, 1, gUnk_0824E20C, 0x4a);
            break;
    }
}

struct P {
    u8 f[0x2dc];
    u32 flags;
};

void SceneActorReaction_HandleColliderContact(s32, s32, struct P *);

void Vorn_OnCollisionResponse(s32 a, s32 b, struct P *p)
{
    u32 f;
    SceneActorReaction_HandleColliderContact(a, b, p);
    f = p->flags;
    if (f & 0x20)
        p->flags = f | 0x80000;
}

void SceneActor_UpdateForwardCollider(struct ChaseActor *);

struct TargetedAttackContext {
    u8 unknown0[0x628];
    struct ContactCollider collider;
    u8 unknown67C[0x6D8 - 0x67C];
    struct ContactPosition target;
};

struct TargetedAttackSequence {
    u8 unknown0[0x14];
    u16 frame;
};

struct TargetedAttackTarget {
    u8 unknown0[0xC];
    struct ContactPosition position;
};

void SceneActor_UpdateTargetedAttack(struct ChaseActor *s)
{
    struct TargetedAttackContext *context = s->base.context;
    struct TargetedAttackSequence *sequence = (void *)((u8 *)s->base.drawable + 0x20);
    if (SceneActorReaction_TakePrepared(s)) {
        s->base.body.direction = (((s->base.tracking.heading + 0x20) & 0xFF) >> 6) << 6;
        SceneActorAnimation_SelectDirectionalEntry((void *)s, 5);
        context->target = ((struct TargetedAttackTarget *)s->base.tracking.target)->position;
        Sound_PlaySong(0x1D0);
    }
    if ((u16)(sequence->frame - 3) <= 1) {
        SceneActor_UpdateForwardCollider(s);
        Collider_Refresh(&context->collider);
    }
    if (s->base.restart)
        s->base.stepDone = 1;
}

void SceneActorAnimation_SelectDirectionalEntry(struct RoomActor *, s32);

void SceneActor_RunPositionScript(void *);

struct VornAnimation {
    u8 unknown0[0xC];
    u16 frame;
    u8 unknownE[6];
    u16 state;
};

static inline u8 TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return TRUE;
    }
    return FALSE;
}

void Vorn_UpdateScriptedAnimation(u8 *p)
{
    if (TakeFlag(p + 0x2B2))
        SceneActorAnimation_SelectDirectionalEntry(p, 0xF);
    {
        struct VornAnimation *anim = (struct VornAnimation *)(*(u8 **)(p + 4) + 0x20);
        if (anim->state == 3 && anim->frame == 0)
            SceneActor_RunPositionScript(*(void **)(p + 0x3D0));
    }
    if (*(u8 *)(p + 0x2B3))
        *(u8 *)(p + 0x2B0) = 1;
}

void Vorn_ResetStateEntry(u8 *p)
{
    if (p[0x2b2] != 0)
        p[0x2b2] = 0;
}

struct DelayedAnimationContext {
    u8 unknown0[0x694];
    u16 flags;
    u8 unknown696, delay;
};

void SceneActor_UpdateDelayedAnimation(struct ChaseActor *s)
{
    struct DelayedAnimationContext *context = s->base.context;
    if (SceneActorReaction_TakePrepared(s))
        SceneActorAnimation_SelectDirectionalEntry((void *)s, 0);
    if (context->delay) {
        if (--context->delay == 0) {
            if (!(context->flags & 1))
                SceneActorAnimation_SelectDirectionalEntry((void *)s, 14);
            else
                SceneActorAnimation_SelectDirectionalEntryFlags((void *)s, 14, 4);
        }
    } else if (s->base.restart) {
        SceneActorAnimation_SelectDirectionalEntry((void *)s, 0);
        s->base.stepDone = 1;
    }
}

void Vorn_BeginIdle(u8 *p)
{
    if (p[0x2b2] != 0)
        p[0x2b2] = 0;
    SceneActorAnimation_SelectDirectionalEntry(p, 0);
}

u16 Route_GetCurrentPointFacing(void *);

void Anim_SetDurationScale(void *, u32);

void Anim_SetFrameDurationScale(void *, u32);


// confidence=low: aligns idle facing with the saved route direction after animation completion.

void Vorn_UpdateIdleFacing(struct ChaseActor *actor)
{
    u8 *group = (u8 *)actor + 0x50;
    void *route = &actor->group.homeRoute;
    u16 *scale;
    if (SceneActorReaction_TakePrepared(actor)) {
        s32 direction;
        s32 facing;
        actor->base.body.direction = (((actor->base.body.direction + 0x60) >> 6) & 3) * 64 - 64;
        direction = ((actor->base.body.direction + 0x60) >> 6) & 3;
        if (actor->base.behavior2AA != 20 && actor->base.behavior2AA != 6 &&
            actor->base.behavior2AA != 16 && !SceneActorContact_MotionFlags(actor, 0x4080)) {
            facing = (((u16)Route_GetCurrentPointFacing(route) + 0x70) >> 5) & 7;
            actor->base.nextDirection = facing;
        } else
            facing = actor->base.nextDirection;
        if (direction == (facing >> 1)) {
            actor->base.stepDone = 1;
            SceneActorAnimation_SelectDirectionalEntry(actor, 0);
            return;
        }
        SceneActorAnimation_SelectDirectionalEntry(actor, 0);
        actor->base.restart = 0;
    }
    if (SceneActorContact_MotionFlags(actor, 0xA02F)) {
        u16 *p = &actor->base.field2C2;
        *p = 64;
        scale = p;
    } else {
        u16 *p = &actor->base.field2C2;
        u16 value = *p;
        scale = p;
        if (!value)
            *scale = 64;
    }
    if (actor->base.drawableKind == 0)
        Anim_SetDurationScale((u8 *)actor->base.drawable + 0x48, *scale);
    else
        Anim_SetFrameDurationScale((u8 *)actor->base.drawable + 0x20, *scale);
    if (actor->base.restart) {
        group[9] = (actor->base.nextDirection + 5) << 5;
        actor->base.stepDone = 1;
        {
            u32 facing = (actor->base.body.direction + 0x60) >> 6;
            u32 quadrant = 3;
            quadrant &= facing;
            actor->base.body.direction = (quadrant << 6) - 64;
        }
        SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    }
}

void Vorn_OnReactionNoOp(void)
{
}

/* Vorn_OnUnusedNoOp: Empty neighboring callback with no known users; ownership is tentative */


void Vorn_OnUnusedNoOp(void)
{
}

#define SceneActorContext_RunPendingScript Vorn_RunPendingScript

struct SceneActorFacingTarget {
    u8 unknown0[0x30];
    s16 x;
    u8 unknown32[2];
    s16 z;
};

s32 Math_Atan2(s32, s32);

void Sound_PlaySong(u32);

void Vorn_BeginPursuitRoute(void *, u32);

void Vorn_RunPendingScript(void *);

void SceneActorReaction_UpdateFacingAttack(struct ChaseActor *actor, s32 frame)
{
    if (SceneActorReaction_TakePrepared(actor)) {
        if ((*(struct SceneActorFacingTarget **)gPlayers))
            actor->base.body.direction = Math_Atan2(
                (*(struct SceneActorFacingTarget **)gPlayers)->x - actor->base.body.position.x,
                (*(struct SceneActorFacingTarget **)gPlayers)->z - actor->base.body.position.z);
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        actor->base.heightOffset = 80;
        Sound_PlaySong(0x1CF);
    }
    {
        u32 mask = 4;
        actor->base.statusFlags |= mask;
    }
    if (frame == 14) {
        actor->base.heightOffset = 140;
        Vorn_RunPendingScript(actor->base.context);
    }
    if (frame > 60)
        Vorn_BeginPursuitRoute(actor, 1);
}

extern void ActorShadow_Disable(void *);

struct Obj;

void Vorn_RestoreSavedState(struct Obj *s);

static inline u8 take6030(u8 *s)
{
    if (s[0x2B2]) {
        s[0x2B2] = 0;
        return 1;
    }
    return 0;
}

struct Obj6030 {
    u32 flags;
    u8 pad[0x1C];
};

struct Part6030 {
    u8 pad[8];
    u32 flags;
};

void Vorn_UpdateHiddenRecovery(u8 *s, s32 arg, u32 unused)
{
    u32 queued = take6030(s);
    if (queued) {
        u32 mask = 1;
        if (s[8] == 0) {
            struct Obj6030 *obj = *(struct Obj6030 **)(s + 4);
            u32 value = obj->flags;
            obj->flags = value | mask;
        } else {
            struct Obj6030 *obj = *(struct Obj6030 **)(s + 4);
            struct Part6030 *part = (struct Part6030 *)((u8 *)obj + 0x20);
            u32 value = part->flags;
            part->flags = value | mask;
        }
        ActorShadow_Disable(s + 0x320);
    } else if (arg > 0x1A) {
        void (*fn)(void);
        u32 code;
        u32 *flags;
        u32 mask;
        SceneActorAnimation_SelectDirectionalEntry(s, 9);
        fn = Vorn_RestoreSavedState;
        code = 0xD;
        s[0x2B2] = 1;
        s[0x2B0] = queued;
        s[0x2B5] = code;
        *(void (**)(void))(s + 0x21C) = fn;
        *(u16 *)(s + 0x2C8) = queued;
        flags = (u32 *)(s + 0x114);
        mask = ~1;
        *flags &= mask;
        *(u32 *)(s + 0x104) = queued;
    } else {
        s[0x2B3] = queued;
    }
}

/* Observed in this callback: owner flags at +0 and inner +0x20/+8.
 * The snapshot fields below are copied and reset at the listed offsets. */

typedef struct Inner {
    u8 pad0[8];
    u32 unk8;
} Inner;

typedef struct Owner {
    u32 unk0;
    u8 pad4[0x1c];
    Inner inner;
} Owner;

typedef struct Obj {
    u8 pad0[0x4];
    Owner *unk4;
    u8 unk8;
    u8 pad9[0x50];
    u8 unk59;
    u8 pad5A[0x42];
    u16 unk9C;
    u8 pad9E[0x12];
    s16 unkB0;
    u8 padB2[0x2A];
    u32 unkDC;
    u8 padE0[0x38];
    u32 unk118;
    u32 unk11C;
    u8 unk120;
    u8 unk121;
    u8 unk122;
    u8 unk123;
    u32 unk124;
    u8 unk128;
    u8 pad129[0x3];
    u16 unk12C;
    u16 unk12E;
    u8 pad130[0x48];
    u16 unk178;
    u8 pad17A[0xA2];
    u32 unk21C;
    u8 pad220[0x88];
    u8 unk2A8;
    u8 unk2A9;
    u8 unk2AA;
    u8 pad2AB[0x5];
    u8 unk2B0;
    u8 unk2B1;
    u8 unk2B2;
    u8 unk2B3;
    u8 pad2B4[0x1];
    u8 unk2B5;
    u8 unk2B6;
    u8 pad2B7[0x9];
    u16 unk2C0;
    u16 unk2C2;
    u8 pad2C4[0x8];
    u32 unk2CC;
    u32 unk2D0;
    u16 unk2D4;
    u16 unk2D6;
    u32 unk2D8;
    u32 unk2DC;
    u32 unk2E0;
    u8 pad2E4[0x2];
    u16 unk2E6;
    u8 pad2E8[0x2];
    u16 unk2EA;
    u8 pad2EC[0x18];
    u32 unk304;
    u8 pad308[0x18];
    u8 unk320[0xac];
    u8 unk3CC;
} Obj;

void ActorShadow_Enable(void *);

void SceneActor_ResetContextAnimation(Obj *);

void SceneActorPalette_ClearCompletedTransitions(void);

static inline u8 take_flag(Obj *s)
{
    if (s->unk2B2) {
        s->unk2B2 = 0;
        return 1;
    }
    return 0;
}

/* Keep this byte predicate local; its address is not retained by the body. */

static inline u8 peek_flag(Obj *s)
{
    return s->unk2B3;
}

void Vorn_RestoreSavedState(Obj *s)
{
    s32 n;
    if (take_flag(s)) {
        ActorShadow_Enable(s->unk320);
        if (!s->unk8) {
            u32 *q = &s->unk4->unk0;
            u32 mask = ~1;
            *q = mask & *q;
        } else {
            Inner *q = &s->unk4->inner;
            u32 mask = ~1;
            q->unk8 = mask & q->unk8;
        }
        {
            u16 one = 1;
            s->unk9C = one;
        }
        s->unk178 = 60;
    }
    if (!peek_flag(s))
        return;
    {
        SceneActor_ResetContextAnimation(s);
        s->unk2CC = s->unk118;
        s->unk2D0 = s->unk11C;
        s->unk59 = s->unk120;
        s->unk2A8 = s->unk121;
        s->unk2A9 = s->unk122;
        s->unk2AA = s->unk123;
        s->unk21C = s->unk124;
        s->unk2B5 = s->unk128;
        s->unk2C0 = s->unk12E;
        s->unk2EA = s->unk12C;
        n = s->unkB0;
        if (n <= 0)
            n = 0;
        s->unk9C = n;
        s->unk2D6 = 0;
        s->unk2D4 = 0;
        s->unk2B0 = 0;
        s->unk2B1 = 1;
        s->unk2B2 = 1;
        s->unk2B3 = 0;
        s->unk304 = 0;
        s->unk2B6 = 0;
        s->unk2C2 = 64;
        s->unk2D8 = 0;
        s->unk2DC = 0;
        s->unk2E0 = 0;
        s->unk2E6 = 0;
        s->unk3CC = 0;
        s->unkDC = 0;
        SceneActorPalette_ClearCompletedTransitions();
    }
}

struct SceneActorReactionContext {
    u8 unknown0[0x6D4];
    u16 flags;
};

struct SceneActorReactionRoutePoint {
    u8 unknown0[0x1C];
    s16 x;
    u8 unknown1E[2];
    s16 z;
};

void SceneActor_SetPositionXZ(void *, s32, s32);


static inline void Reaction_SetControl(struct ChaseActor *actor, u32 kind, void *control)
{
    actor->base.field2B2 = 1;
    actor->base.stepDone = 0;
    actor->base.controlKind = kind;
    actor->base.target = control;
    actor->base.field2C8 = 0;
    {
        u32 *flags = &actor->base.pendingControlFlags;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->base.pendingControl = 0;
}

void SceneActorReaction_ResumeSavedTarget(struct ChaseActor *actor, s32 frame)
{
    struct SceneActorReactionContext *context = actor->base.context;
    struct SceneActorRegistryControlGroup *group = &actor->group.common;
    if (SceneActorReaction_TakePrepared(actor)) {
        actor->base.restart = 1;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 12);
        {
            u16 *flags = &context->flags;
            s32 mask = ~12;
            *flags &= mask;
        }
    }
    if (frame > 60 && actor->base.restart) {
        u16 blocked;
        actor->base.stepDone = 1;
        blocked = SceneActorContact_MotionFlags(actor, 128);
        if (!blocked) {
            void *target;
            SceneActor_SetPositionXZ(
                &actor->base.body.id,
                (*(struct SceneActorReactionRoutePoint **)((u8 *)group + 0x1FC))->x,
                (*(struct SceneActorReactionRoutePoint **)((u8 *)group + 0x1FC))->z);
            SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
            target = actor->base.onAlternateEntry;
            Reaction_SetControl(actor, 1, target);
            {
                u32 behavior = 7;
                actor->base.state2A8 = 0;
                actor->base.phase2A9 = 0;
                actor->base.behavior2AA = behavior;
                actor->base.counter2AC = 0;
                actor->base.field2B1 = 1;
            }
            {
                u32 mask = 16;
                actor->base.behaviorFlags |= mask;
            }
        }
    }
}

u8 Skeleton_RerouteToTarget(void *);

u8 SceneActor_DispatchContextAction(void *, u32);

// confidence=low: after the action delay, restores a reaction or reroutes toward the player.


static inline u8 ContextFlags(u8 *context, u32 mask)
{
    if (*(u16 *)(context + 0x6D4) & mask)
        return 1;
    return 0;
}

static inline u16 ReactionFlagMask(u16 flag)
{
    return flag;
}

struct VornRouteGroup {
    u8 pad[0x1FC];
    void *mesh;
    struct ContactPosition *destination;
    u8 pad204[4];
    void *control;
    u8 pad20C[0x18];
    void *failure;
};

void Vorn_ResumeRouteAfterDelay(struct ChaseActor *actor, s32 frame)
{
    struct VornRouteGroup *group = (void *)&actor->group.common;
    u8 *context = actor->base.context;
    if (SceneActorReaction_TakePrepared(actor)) {
        DEBUG_PRINT();
    }
    {
        u32 mask = ReactionFlagMask(16);
        actor->base.reactionFlags = mask | actor->base.reactionFlags;
    }
    if (frame <= 18)
        return;
    actor->base.stepDone = 1;
    if (SceneActorContact_MotionFlags(actor, 2)) {
        u32 mask;
        Chase_SetState(actor, 3, 0, 0);
        Chase_SetCtl(actor, group->control, 7);
        mask = ~0x983;
        actor->base.reactionFlags = mask & actor->base.reactionFlags;
    } else if (ContextFlags(context, 0x1D))
        SceneActor_DispatchContextAction(context, 1);
    else {
        actor->base.tracking.target = (u32)gPlayers[0] + 0x24;
        {
            u32 mask = 16;
            actor->base.behaviorFlags |= mask;
        }
        group->destination->pad = 0;
        if (!Skeleton_RerouteToTarget(actor) &&
            !Chase_TryRoute(actor, group->mesh, &actor->base.body.position,
                            &actor->base.tracking.target->position)) {
            Callback_InvokeActor(actor, group->failure);
            return;
        }
        Chase_SetCtl(actor, actor->base.onAlternateEntry, 1);
        Chase_SetBehavior(actor, 7);
        actor->base.field2C2 = 32;
    }
}

void Vorn_ResumeRouteAfterDelay(struct ChaseActor *, s32);

struct VornAnimationState {
    u8 unknown0[12];
    u16 delay;
};

struct VornDrawable {
    u8 unknown0[0x20];
    struct VornAnimationState animation;
};

static inline u8 Vorn_DisplacementStopped(s32 x, s32 z)
{
    if (!x && !z)
        return 1;
    return 0;
}

static inline u8 Vorn_ContextFlags(struct SceneActorAnimationLink *context, u32 mask)
{
    if (context->flags & mask)
        return 1;
    return 0;
}



void Vorn_UpdateStaggerRecovery(struct ChaseActor *actor, s32 frame)
{
    struct SceneActorAnimationLink *context = actor->base.context;
    struct VornAnimationState *animation =
        &((struct VornDrawable *)actor->base.drawable)->animation;
    struct SceneActorChaseGroup *group = SceneActorChase_GroupOf(actor);
    struct ContactPackedVector range;
    s16 *x, *z;
    s32 dx, dz;
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectDirectionalEntry(&actor->base, 3);
        if (SceneActorChase_QueryMotionFlagComponent(actor, 8)) {
            Sound_PlaySong(0x1CB);
            Sound_PlaySong(0x12A);
        }
        SceneActorChase_ClearMotionFlagComponent(actor, 1);
        actor->base.restart = 0;
    }
    SceneActorChase_SetActorFlags(actor, 0x2000);
    SceneActorChase_SetStatusFlags(actor, 0x10000);
    if (frame <= 17)
        animation->delay = 0;
    if (Mod(frame, 12) == 0 && SceneActorChase_QueryMotionFlagComponent(actor, 8)) {
        u32 index, sample;
        u8 count;
        range.x = 60;
        range.y = 60;
        range.z = 40;
        index = (gRandomIndex + 1) & 1023;
        gRandomIndex = index;
        index *= 2;
        sample = *(const u16 *)(RANDOM_TABLE_ADDR + index);
        sample &= 3;
        count = sample + 1;
        SparkParticle_Spawn(&actor->base.body.position, count,
                            (const struct ContactPosition *)&range,
                            actor->base.body.direction + 128, 10, 6, 2, 2);
    }
    dx = SceneActorDisplacement_ReadAxis(actor->base.reactionStart.x, &actor->base.reactionEnd.x,
                                         &x);
    dz = SceneActorDisplacement_ReadAxis(actor->base.reactionStart.z, &actor->base.reactionEnd.z,
                                         &z);
    if (dx > 200)
        dx = 200;
    else if (dx < -200)
        dx = -200;
    if (dz > 200)
        dz = 200;
    else if (dz < -200)
        dz = -200;
    actor->base.body.motion.x += dx;
    actor->base.body.motion.z += dz;
    *x += dx;
    *z += dz;
    if (!Vorn_DisplacementStopped(dx, dz))
        return;
    if (!actor->base.restart)
        return;
    actor->base.stepDone = 1;
    SceneActorChase_ClearMotionFlagComponent(actor, 0x120);
    SceneActorChase_ClearActorFlags(actor, 0x80);
    if (actor->base.hitReactionFlags & 0x80000) {
        SceneActorChase_SetReactionFlags(actor, 2);
        return;
    }
    if (SceneActorChase_QueryMotionFlagComponent(actor, 8)) {
        SceneActorChase_SetState(actor, 3, 0, 0);
        SceneActorChase_SetControl(actor, Vorn_ResumeRouteAfterDelay, 20);
        SceneActorChase_ClearMotionFlagComponent(actor, 8);
        actor->base.field2C2 = 64;
    } else if (Vorn_ContextFlags(context, 0x1D))
        SceneActor_DispatchContextAction(context, 1);
    else {
        SceneActorChase_TrackPlayer(actor);
        SceneActorChase_SetActorFlags(actor, 16);
        group->destination->pad = 0;
        if (!Skeleton_RerouteToTarget(actor) &&
            !SceneActorChase_TryRoute(actor, group->route, &actor->base.body.position,
                                      &actor->base.tracking.target->position)) {
            Callback_InvokeActor(actor, group->fallback);
            return;
        }
        SceneActorChase_SetControl(actor, actor->base.onAlternateEntry, 1);
        SceneActorChase_SetBehavior(actor, 7);
        actor->base.field2C2 = 32;
    }
}

void Vorn_SelectRouteAwayFromTarget(void *);

static inline u8 Pending(u8 *context, u16 **out)
{
    u32 mask = 2;
    u16 *flags = (void *)(context + 0x6D4);
    u32 value = *flags & mask;
    *out = flags;
    if (value)
        return 1;
    return 0;
}

struct VornWaitTimer {
    u16 value : 16;
};

// confidence=low: waits for pursuit distance or timeout before restoring behavior 15.


void Vorn_UpdatePursuitWait(struct ChaseActor *input)
{
    struct ChaseActor *a = input;
    u8 *context = a->base.context;
    u16 *flags;
    if (SceneActorControl_TakeFlag(&a->base.field2B2)) {
        *(u16 *)(context + 0x69C) = 0;
    } else if (Pending(context, &flags)) {
        if (a->base.tracking.distance <= 0x18FFF) {
            Vorn_SelectRouteAwayFromTarget(a);
            {
                s32 mask = ~2;
                ((struct ChaseFlags40 *)flags)->bits &= mask;
            }
            Chase_SetState(a, 0, 0, 15);
        } else {
            struct VornWaitTimer *timer;
            a->base.body.direction = a->base.tracking.heading;
            SceneActorAnimation_SelectDirectionalEntry((void *)a, 0);
            timer = (void *)(context + 0x69C);
            ++timer->value;
            if (timer->value > 120) {
                Vorn_SelectRouteAwayFromTarget(a);
                {
                    s32 mask = ~2;
                    ((struct ChaseFlags40 *)flags)->bits &= mask;
                }
                Chase_SetState(a, 0, 0, 15);
                timer->value = 0;
            }
        }
    } else
        SceneActorAnimation_SelectDirectionalEntry((void *)a, 1);
}

// confidence=low: tentative Vorn behavior inferred from context and neighboring callbacks.

// confidence=low: tentative Vorn behavior role.

// confidence=low: target distance controls approach, idle animation, and a delayed action.

void Vorn_BroadcastPosition(void *);

static inline u8 Vorn_TestNear(struct Vorn *p, u32 mask, u16 **out)
{
    u16 *field = (u16 *)&p->flags6D4;
    u32 value = *field & mask;
    *out = field;
    if (value)
        return 1;
    return 0;
}

void Vorn_UpdatePursuitDistance(struct ChaseActor *actor)
{
    struct Vorn *context = actor->base.context;
    u8 *group = (u8 *)context + 0x3D4;
    struct ContactPosition center;
    u16 *flags;
    u8 near;
    if (SceneActorReaction_TakePrepared(actor))
        return;
    Vorn_BroadcastPosition(actor);
    if (actor->base.tracking.distance < 0x38400) {
        near = Vorn_TestNear(context, 2, &flags);
        if (near) {
            if (context->nearTimer > 40) {
                Vorn_SelectRouteAwayFromTarget(actor);
                Chase_SetBehavior(actor, 15);
                {
                    s32 mask = ~2;
                    mask &= *flags;
                    *flags = mask;
                }
                SceneActorAnimation_SelectDirectionalEntry((void *)actor, 1);
                context->nearTimer = 0;
            } else {
                context->nearTimer++;
                SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
                actor->base.body.direction = actor->base.tracking.heading;
            }
        } else {
            if (actor->base.tracking.distance < 0x19000) {
                if (context->nearTimer > 30) {
                    Vorn_SelectRouteAwayFromTarget(actor);
                    Chase_SetBehavior(actor, 15);
                    {
                        s32 mask = ~2;
                        mask &= *flags;
                        *flags = mask;
                    }
                    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 1);
                    context->nearTimer = 0;
                    return;
                }
                context->nearTimer++;
            }
            SceneActorAnimation_SelectDirectionalEntry((void *)actor, 1);
        }
    } else if (actor->base.tracking.distance > 0x90000) {
        near = Vorn_TestNear(context, 2, &flags);
        if (!near) {
            SceneActorAnimation_SelectDirectionalEntry((void *)actor, 1);
            context->farTimer = 0;
        } else {
            if (context->farTimer <= 59) {
                actor->base.body.direction = actor->base.tracking.heading;
                SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
                context->farTimer++;
            } else {
                if (!actor->base.tracking.target)
                    return;
                {
                    struct ChaseTarget *target = (void *)actor->base.tracking.target;
                    SceneActorTerrain_GetCenter(
                        Chase_CellIndex(target->position.x >> 8, target->position.z >> 8), &center);
                }
                if (!Chase_TryRoute(actor, *(void **)(group + 0x1FC), &actor->base.body.position,
                                    &center)) {
                    Callback_InvokeActor(actor, *(void **)(group + 0x224));
                    return;
                }
                SceneActorAnimation_SelectDirectionalEntry((void *)actor, 1);
                Chase_SetBehavior(actor, 15);
                {
                    s32 mask = ~2;
                    mask &= *flags;
                    *flags = mask;
                }
                context->farTimer = 0;
            }
        }
    } else {
        near = Vorn_TestNear(context, 2, &flags);
        if (near) {
            actor->base.body.direction = actor->base.tracking.heading;
            SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
            if (context->farTimer > 60) {
                if (!actor->base.tracking.target)
                    return;
                {
                    struct ChaseTarget *target = (void *)actor->base.tracking.target;
                    SceneActorTerrain_GetCenter(
                        Chase_CellIndex(target->position.x >> 8, target->position.z >> 8), &center);
                }
                if (!Chase_TryRoute(actor, *(void **)(group + 0x1FC), &actor->base.body.position,
                                    &center)) {
                    Callback_InvokeActor(actor, *(void **)(group + 0x224));
                    return;
                }
                SceneActorAnimation_SelectDirectionalEntry((void *)actor, 1);
                Chase_SetBehavior(actor, 15);
                {
                    s32 mask = ~2;
                    mask &= *flags;
                    *flags = mask;
                }
                context->farTimer = 0;
            } else
                context->farTimer++;
        } else
            SceneActorAnimation_SelectDirectionalEntry((void *)actor, 1);
    }
}

struct SceneActorSideStepContext {
    u8 unknown0[0x6B9];
    u8 side;
    u16 duration;
};

static inline void SideStep_SetFlags(struct SceneActorReactionMotion *motion, u8 mask)
{
    motion->flags |= mask;
}

void SceneActorReaction_BeginSideStep(struct ChaseActor *actor, s32 frame)
{
    struct SceneActorSideStepContext *context = actor->base.context;
    if (SceneActorReaction_TakePrepared(actor)) {
        u32 flags;
        if (!context->side) {
            flags = 1;
            actor->base.body.direction = 64;
            actor->base.tracking.destination = actor->base.body.position;
            actor->base.tracking.destination.x -= 160;
        } else {
            flags = 0;
            actor->base.body.direction = 64;
            actor->base.tracking.destination = actor->base.body.position;
            actor->base.tracking.destination.x += 160;
        }
        SceneActorAnimation_SelectEntryFlags((void *)actor, 16, flags);
    }
    SideStep_SetFlags((void *)&actor->base.tracking.target, 32);
    if (frame > context->duration)
        actor->base.stepDone = 1;
}

void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);

void Vorn_SendMessage(void *);

u8 Vorn_HasActiveResponse(void *);

void SceneBody_StartRoute(void *, void *, u32, u32, u32);

static inline s32 Vorn_DivideDirection(s32 n)
{
    return n >= 0 ? n >> 5 : -((-n) >> 5);
}




static inline void Vorn_ClearTrackingFlags(struct SceneActorChaseTracking *tracking, u32 bits)
{
    u32 mask = ~bits;
    tracking->flags &= mask;
}

static inline void Vorn_SetTrackingFlags(struct SceneActorChaseTracking *tracking, u8 bits)
{
    tracking->flags = bits | tracking->flags;
}

static inline void Vorn_AdjustMotionFlags(struct SceneActorChaseFlags16 *field, u16 set, u32 clear)
{
    u16 value = set;
    u32 result;
    value |= field->value;
    result = value;
    result &= ~clear;
    field->value = result;
}

static inline void Vorn_SetPaletteTimer(struct ChaseActor *actor, u32 value)
{
    actor->base.paletteTimer = value;
}

static inline void Vorn_ClearRoute(struct ChaseActor *actor,
                                   struct SceneActorRegistryControl *control)
{
    SceneActorChase_ClearActorFlags(actor, 0x141C0);
    control->counter = 0;
    control->field14 = 0;
    control->timer = 0;
}

static inline void Vorn_StartRoute(struct ChaseActor *actor, struct SceneActorChaseGroup *group)
{
    SceneBody_StartRoute(&actor->base.body.id, &group->homeRoute, group->homeRoute.routeId,
                         group->homeRoute.parameterB, group->homeRoute.pointId);
}

static inline u8 Vorn_ActorCollisionFlags(struct ChaseActor *actor, u32 bits)
{
    return SceneActorChase_TestField(&actor->base.placementFlags, bits);
}

static inline void Vorn_ClearContextFlags(u16 *flags, u32 bits)
{
    u32 mask = ~bits;
    *flags &= mask;
}

void Vorn_UpdateKnockbackRecovery(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *group = SceneActorChase_GroupOf(actor);
    s32 timer;
    s16 *x, *z;
    s32 dx, dz;
    if (SceneActorReaction_TakePrepared(actor)) {
        struct SceneActorChaseFlags16 *motion;
        actor->base.body.direction = (((actor->base.body.direction + 32) >> 6) & 3) << 6;
        SceneActorAnimation_SelectDirectionalEntry(&actor->base, 6);
        if (!SceneActorChase_StatusFlags(actor, 2)) {
            SceneActorAttachment_ReplaceResponseSlot(actor, 3, 0, 25);
            SceneActorChase_SetStatusFlags(actor, 2);
            Sound_PlaySong(0x1CC);
        }
        motion = (struct SceneActorChaseFlags16 *)&actor->base.motionFlags;
        Vorn_AdjustMotionFlags(motion, 2, 0x17A9);
        {
            struct SceneActorRegistryControl *control;
            actor->base.field2C2 = 64;
            actor->group.common.gate.flags = 0;
            actor->group.common.gate.counter = 0;
            SceneActorChase_ClearFlagComponent(motion, 0x380);
            Vorn_ClearTrackingFlags(&actor->base.tracking, 0x18);
            control = &actor->group.common.control;
            if (SceneActorTurn_ActorFlags(actor, 0x200000)) {
                SceneActorChase_ClearStatusFlags(actor, 0x1000000);
                SceneActorChase_ClearFlagComponent(motion, 0x200);
                control->mode = 0;
                control->counter = 0;
                control->timer = 0;
            }
            SceneActorChase_ClearFlagComponent(motion, 0x400);
            actor->group.turn.turnsRemaining = -1;
        }
        SceneActorChase_ClearActorFlags(actor, 0x80);
        actor->base.field2C2 = 64;
        Vorn_ClearTrackingFlags(&actor->base.tracking, 0x18);
        SceneActorChase_ClearStatusFlags(actor, 0x1000000);
        Vorn_ClearRoute(actor, &group->common.control);
        SceneActorChase_SetActorFlags(actor, 0x2000000);
        Vorn_SendMessage(actor);
    }
    Vorn_SetPaletteTimer(actor, 65);
    timer = (s16)actor->base.attachmentTimer;
    if ((u32)(timer - 30) <= 89) {
        s32 frame = Mod(timer, 30);
        s32 threshold = 15 - Div(timer, 10);
        if (frame < threshold)
            SceneActorChase_SetReactionFlags(actor, 32);
    }
    dx = SceneActorDisplacement_ReadAxis(actor->base.reactionStart.x, &actor->base.reactionEnd.x,
                                         &x);
    dz = SceneActorDisplacement_ReadAxis(actor->base.reactionStart.z, &actor->base.reactionEnd.z,
                                         &z);
    if (dx > 200)
        dx = 200;
    else if (dx < -200)
        dx = -200;
    if (dz > 200)
        dz = 200;
    else if (dz < -200)
        dz = -200;
    actor->base.body.motion.x += dx;
    actor->base.body.motion.z += dz;
    *x += dx;
    *z += dz;
    if (Vorn_DisplacementStopped(dx, dz) && timer <= 0) {
        struct ContactCollider *collider;
        actor->base.attachmentTimer = 0;
        actor->base.reactionDirection = (Vorn_DivideDirection(actor->base.body.direction) + 7) & 7;
        SceneActorChase_SetState(actor, 3, 1, 0);
        SceneActorChase_SetControl(actor, group->animationEndControl, 8);
        SceneActorChase_ClearWordFlags(&actor->base.statusFlags, 2);
        SceneActorChase_ClearMotionFlagComponent(actor, 0x3A);
        SceneActorChase_SetState(actor, 3, 1, 0);
        actor->base.body.direction = ((actor->base.body.direction + 32) >> 6) << 6;
        Vorn_StartRoute(actor, group);
        collider = &actor->base.colliderA;
        if (Vorn_ActorCollisionFlags(actor, 64))
            collider->flags &= 0xFEFF;
        else if (SceneActorChase_QueryMotionFlagComponent(actor, 0x2016))
            collider->flags &= 0xFEFF;
        else
            collider->flags |= 0x100;
        Vorn_ClearContextFlags(&((struct SceneActorAnimationLink *)actor->base.context)->flags,
                               0x1E);
        if (Vorn_HasActiveResponse(actor->base.context)) {
            Vorn_SetTrackingFlags(&actor->base.tracking, 8);
            SceneActorChase_SetActorFlags(actor, 0x80);
        }
    }
}

static inline void Resume_SetControl(struct ChaseActor *actor, u32 kind, void *control)
{
    actor->base.field2B2 = 1;
    actor->base.stepDone = 0;
    actor->base.controlKind = kind;
    actor->base.target = control;
    actor->base.field2C8 = 0;
    {
        u32 *flags = &actor->base.pendingControlFlags;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->base.pendingControl = 0;
}

void SceneActorReaction_ResumeOrRunScript(struct ChaseActor *actor)
{
    u8 *group = (u8 *)&actor->group.common;
    if (SceneActorReaction_TakePrepared(actor))
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 7);
    if (actor->base.restart) {
        actor->base.stepDone = 1;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        if (Vorn_HasActiveResponse(actor->base.context)) {
            struct Vorn *context = actor->base.context;
            u16 *flags = &context->flags6D4;
            s32 mask = ~3;
            *flags &= mask;
            Vorn_BeginResponsePursuit(actor, 1, gUnk_0824E20C, 0x367);
        } else {
            {
                u32 mask = 16;
                actor->base.behaviorFlags |= mask;
            }
            MonsterInit_SetState(actor, 3, 1, 0);
            {
                void *control = *(void **)(group + 0x210);
                Resume_SetControl(actor, 15, control);
            }
        }
    }
}

void Vorn_ResumeReactionRoute(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *group = SceneActorChase_GroupOf(actor);
    struct SceneActorAnimationLink *context = actor->base.context;
    u32 *status;
    struct SceneActorChaseFlags16 *motion;
    u16 *contextFlags;
    if (SceneActorReaction_TakePrepared(actor)) {
        actor->base.body.direction = (((actor->base.body.direction + 128) & 255) >> 6) << 6;
        SceneActorAnimation_SelectDirectionalEntry(&actor->base, 0);
    }
    status = &actor->base.statusFlags;
    {
        u32 value = *status;
        if (!(value & 16))
            return;
        value &= ~4;
        *status = value;
    }
    Vorn_SendMessage(actor);
    SceneActorAnimation_SelectDirectionalEntry(&actor->base, 0);
    motion = (struct SceneActorChaseFlags16 *)&actor->base.motionFlags;
    SceneActorChase_ClearFlagComponent(motion, 0x38);
    Vorn_StartRoute(actor, group);
    contextFlags = &context->flags;
    Vorn_ClearContextFlags(contextFlags, 10);
    if (Vorn_HasActiveResponse(context)) {
        Vorn_SetTrackingFlags(&actor->base.tracking, 8);
        SceneActorChase_SetActorFlags(actor, 0x80);
        Vorn_ClearContextFlags(&((struct SceneActorAnimationLink *)actor->base.context)->flags, 3);
        Vorn_BeginResponsePursuit(actor, 1, gUnk_0824E20C, 0x39F);
    } else if (!SceneActor_DispatchContextAction(context, 1)) {
        struct SceneActorRegistryControl *control;
        Vorn_ClearContextFlags(contextFlags, 0x1E);
        actor->base.field2C2 = 64;
        actor->group.common.gate.flags = 0;
        actor->group.common.gate.counter = 0;
        SceneActorChase_ClearFlagComponent(motion, 0x380);
        Vorn_ClearTrackingFlags(&actor->base.tracking, 0x18);
        control = &actor->group.common.control;
        if (SceneActorTurn_ActorFlags(actor, 0x200000)) {
            SceneActorChase_ClearWordFlags(status, 0x1000000);
            SceneActorChase_ClearFlagComponent(motion, 0x200);
            control->mode = 0;
            control->counter = 0;
            control->timer = 0;
        }
        SceneActorChase_ClearFlagComponent(motion, 0x400);
        actor->group.turn.turnsRemaining = -1;
        SceneActorChase_SetActorFlags(actor, 16);
        SceneActorChase_SetState(actor, 3, 1, 0);
        SceneActorChase_SetControl(actor, group->routeResumeControl, 15);
    }
    actor->base.field2C2 = 64;
}

/* Vorn_UpdateApproachRange: tentative */

u8 SceneActor_IsFacingNearbyTarget(struct ChaseActor *);

void Vorn_SelectRetreatRoute(struct RoomActor *, u32);

s32 Callback_InvokeActor(void *, void *);

void Vorn_UpdateApproachRange(struct ChaseActor *actor)
{
    struct Vorn *context = actor->base.context;
    u8 *group = (u8 *)&actor->group.common;
    if (SceneActorReaction_TakePrepared(actor))
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    else if (actor->base.tracking.distance > context->maximumDistance ||
             actor->base.tracking.distance < context->minimumDistance) {
        Vorn_SelectRetreatRoute((void *)actor, 1);
    } else if (SceneActor_IsFacingNearbyTarget(actor)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        if (!Chase_TryRoute(actor, *(void **)(group + 0x1FC), &actor->base.body.position,
                            &((struct ChaseTarget *)actor->base.tracking.target)->position))
            Callback_InvokeActor(actor, *(void **)(group + 0x224));
        else {
            {
                struct ChaseFlags40 *flags = (void *)&context->flags6D4;
                s32 mask = ~0x40;
                flags->bits &= mask;
            }
            Chase_SetBehavior(actor, 21);
        }
    }
}

static inline u8 ConsumeFlag(struct RoomActor *p)
{
    if (p->field2B2) {
        p->field2B2 = 0;
        return 1;
    }
    return 0;
}

static inline u8 LinkActive(struct SceneActorAnimationLink *p)
{
    u32 mask = 0x40;
    if (p->flags & mask)
        return 1;
    return 0;
}

static inline s32 AngleIndex(s32 angle)
{
    if (angle >= 0)
        return angle >> 5;
    return -((-angle) >> 5);
}

static inline s32 RandomByte(void)
{
    u32 offset;
    u32 *index = &gRandomIndex;
    u32 next = *index + 1;
    *index = next & 1023;
    offset = *index * 2;
    return *(const u8 *)(offset + RANDOM_TABLE_ADDR);
}

void SceneActorPhase_SelectLinkedAnimation(struct RoomActor *p)
{
    struct SceneActorAnimationLink *link = p->context;
    if (!ConsumeFlag(p)) {
        if (LinkActive(link)) {
            if (p->tracking.distance > link->phaseTimer) {
                u32 state = 0x17;
                p->state2A8 = 0;
                p->phase2A9 = 0;
                p->behavior2AA = state;
                p->counter2AC = 0;
                p->field2B1 = 1;
                p->nextDirection = (AngleIndex(p->tracking.heading) + 3) & 7;
            } else {
                if (RandomByte() <= 0x1f)
                    Vorn_SelectRetreatRoute(p, 1);
                else
                    SceneActorAnimation_SelectDirectionalEntry(p, 0);
            }
        } else
            SceneActorAnimation_SelectDirectionalEntry(p, 1);
    }
}

// confidence=low: restores a saved patrol destination and delayed command.

// confidence=low: tentative Vorn behavior role recovered from the surrounding callbacks.

struct VornPatrolRoute {
    u8 unknown0[8];
    const u16 *points;
};


static inline u8 Vorn_GetRestorableMotion(struct SceneActorReactionMotion *m, u32 mask,
                                          struct SceneActorReactionMotion **saved)
{
    u32 bits = m->flags & mask;
    *saved = m;
    if (bits)
        return 1;
    return 0;
}


s32 Vorn_TryRestorePatrolRoute(struct ChaseActor *actor)
{
    u8 *group = (u8 *)(&actor->group.common);
    u8 *context = actor->base.context;
    struct SceneActorReactionMotion *savedMotion;
    struct ContactPosition *destination;
    if (((!SceneActorTurn_ActorFlags(actor, 0x400000)) || (!SceneActorTurn_ActorFlags(actor, 0x200000))) ||
        (!Vorn_GetRestorableMotion((void *)(&actor->base.tracking.target), 1, &savedMotion)))
        return 0;
    MonsterInit_SetBehaviorFlags(actor, 0x100);
    actor->base.field2C2 = 64;
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    Chase_SetState(actor, 3, 1, 0);
    Chase_SetCtl(actor, SceneActorReaction_RestoreAfterDelay, 14);
    MonsterInit_SetBehaviorFlags(actor, 16);
    {
        struct SceneActorReactionCommand *command = (void *)(group + 0x8C);
        void *payload = actor->base.onStanding;
        u32 kind = 9;
        command->duration = 30;
        command->payload = payload;
        command->first = 0;
        command->second = 0;
        command->kind = kind;
    }
    destination = *((struct ContactPosition **)(group + 0x200));
    {
        struct VornPatrolRoute *route = (void *)(group + 0x74);
        destination->x = ((u16 *)route->points)[0];
        destination->z = ((u16 *)route->points)[1];
    }
    destination->y = Chase_Height(destination);
    (*((struct ContactPosition **)(group + 0x200)))->pad = 1;
    actor->base.field2C2 = 64;
    actor->group.common.gate.flags = 0;
    actor->group.common.gate.counter = 0;
    Chase_ClearBits(&actor->base.motionFlags, 0x380);
    {
        struct SceneActorReactionMotion *m = savedMotion;
        s32 mask = ~0x18;
        m->flags &= mask;
    }
    {
        struct SceneActorRegistryControl *control = &actor->group.common.control;
        u32 restoreBits = SceneActorTurn_ActorFlags(actor, 0x200000);
        u32 *flags = &actor->base.statusFlags;
        if (restoreBits) {
            {
                u32 k2 = 0x1000000;
                *flags &= ~k2;
            }
            Chase_ClearBits(&actor->base.motionFlags, 0x200);
            control->mode = 0;
            control->counter = 0;
            control->timer = 0;
        }
        Chase_ClearBits(&actor->base.motionFlags, 0x400);
        actor->group.turn.turnsRemaining = 0xFF;
        Chase_ClearBits(&actor->base.motionFlags, 0x1000);
        *((u16 *)(context + 0x6D4)) = 0;
        *((u16 *)(context + 0x69E)) = 0;
        *((u16 *)(context + 0x6AC)) = 0;
        {
            s32 mask = ~4;
            *flags &= mask;
        }
    }
    Vorn_SendMessage(actor);
    return 1;
}
