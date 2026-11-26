#include "engine/shared/global.h"
#include "player/player.h"
#include "actors/scene_actor_reaction/scene_actor_reaction.h"
#include "engine/shared/scene_actor_control_phase.h"
#include "engine/shared/scene_actor_chase_reaction.h"
#include "engine/shared/scene_actor_terrain.h"

struct SceneActorApproachCategoryContext {
    u8 unknown0[0x6F0];
    u16 timer;
    u8 unknown6F2[6];
    u16 duration;
};

void SceneActorReaction_TickQueuedContact(void);

void SceneActorReaction_TickContactAnimation(void);

void Vorn_SpawnHazard(struct ChaseActor *, s32);

void Skeleton_SelectDistanceReaction(struct ChaseActor *);

static inline u8 TakeReady(struct ChaseActor *actor)
{
    if (actor->base.field2B1) {
        actor->base.field2B1 = 0;
        return 1;
    }
    return 0;
}

static inline void SetCategoryControl(struct ChaseActor *actor, void *fn)
{
    u8 code = 3;
    actor->base.field2B2 = 1;
    actor->base.stepDone = 0;
    actor->base.controlKind = code;
    actor->base.target = fn;
    actor->base.field2C8 = 0;
    {
        u32 *flags = &actor->base.pendingControlFlags;
        s32 mask = -2;
        *flags &= mask;
    }
    actor->base.pendingControl = 0;
}

void SceneActorApproach_RunCategoryAction(struct ChaseActor *actor)
{
    struct SceneActorApproachCategoryContext *context = actor->base.context;
    if (TakeReady(actor)) {
        SceneActorReaction_EnableMotionFlags(0x100, &actor->base.motionFlags);
        {
            if (actor->base.category == MONSTER_CATEGORY_SKELETON_ARMED)
                SetCategoryControl(actor, SceneActorReaction_TickQueuedContact);
            else
                SetCategoryControl(actor, SceneActorReaction_TickContactAnimation);
        }
        Vorn_SpawnHazard(actor, 1);
        context->timer = context->duration;
    }
    if (actor->base.stepDone) {
        SceneActorPlacement_ClearFlags(&actor->base.motionFlags, ~0x100);
        Skeleton_SelectDistanceReaction(actor);
    } else
        actor->base.counter2AC++;
}

struct SceneActorApproachTimedContext {
    u8 unknown0[0x6F2];
    u16 timer;
    u8 unknown6F4[6];
    u16 duration;
};

void Skeleton_UpdateAttackWindup(void);

static inline void SetTimedControl(struct ChaseActor *actor, void *fn)
{
    u8 code = 0x27;
    actor->base.field2B2 = 1;
    actor->base.stepDone = 0;
    actor->base.controlKind = code;
    actor->base.target = fn;
    actor->base.field2C8 = 0;
    {
        u32 *flags = &actor->base.pendingControlFlags;
        s32 mask = -2;
        *flags &= mask;
    }
    actor->base.pendingControl = 0;
}

void SceneActorApproach_RunTimedAction(struct ChaseActor *actor)
{
    struct SceneActorApproachTimedContext *context = actor->base.context;
    if (TakeReady(actor)) {
        SceneActorReaction_EnableMotionFlags(0x100, &actor->base.motionFlags);
        SetTimedControl(actor, Skeleton_UpdateAttackWindup);
        context->timer = context->duration;
        Vorn_SpawnHazard(actor, 2);
    }
    if (actor->base.stepDone) {
        SceneActorPlacement_ClearFlags(&actor->base.motionFlags, ~0x100);
        Skeleton_SelectDistanceReaction(actor);
    } else
        actor->base.counter2AC++;
}

struct SceneActorApproachAlternateContext {
    u8 unknown0[0x6F4];
    u16 timer;
    u8 unknown6F6[6];
    u16 duration;
};

void SceneActorReaction_TickMultiHitAnimation(void);

static inline void SetAlternateControl(struct ChaseActor *actor, void *fn)
{
    u8 code = 0x2B;
    actor->base.field2B2 = 1;
    actor->base.stepDone = 0;
    actor->base.controlKind = code;
    actor->base.target = fn;
    actor->base.field2C8 = 0;
    {
        u32 *flags = &actor->base.pendingControlFlags;
        s32 mask = -2;
        *flags &= mask;
    }
    actor->base.pendingControl = 0;
}

void SceneActorApproach_RunAlternateTimedAction(struct ChaseActor *actor)
{
    struct SceneActorApproachAlternateContext *context = actor->base.context;
    if (TakeReady(actor)) {
        SceneActorReaction_EnableMotionFlags(0x100, &actor->base.motionFlags);
        SetAlternateControl(actor, SceneActorReaction_TickMultiHitAnimation);
        context->timer = context->duration;
        Vorn_SpawnHazard(actor, 3);
    }
    if (actor->base.stepDone) {
        SceneActorPlacement_ClearFlags(&actor->base.motionFlags, ~0x100);
        Skeleton_SelectDistanceReaction(actor);
    } else
        actor->base.counter2AC++;
}

// foundry draft (worker best 94.9%); what differs is mostly register allocation and instruction order

struct SceneActorApproachCaptureContext {
    u8 unknown0[0x6F2];
    u16 timer;
    u8 unknown6F4[6];
    u16 duration;
    u8 unknown6FC[0x14];
    struct ContactPosition destination;
};

void Skeleton_BeginHomingBurst(void);

static inline void SetCaptureControl(struct ChaseActor *actor, void *fn)
{
    u8 code = 0x2C;
    actor->base.field2B2 = 1;
    actor->base.stepDone = 0;
    actor->base.controlKind = code;
    actor->base.target = fn;
    actor->base.field2C8 = 0;
    {
        u32 *flags = &actor->base.pendingControlFlags;
        s32 mask = -2;
        *flags &= mask;
    }
    actor->base.pendingControl = 0;
}

void SceneActorApproach_CaptureTargetAndRunAction(struct ChaseActor *actor)
{
    struct SceneActorApproachCaptureContext *context = actor->base.context;
    if (TakeReady(actor)) {
        SceneActorReaction_EnableMotionFlags(0x100, &actor->base.motionFlags);
        SetCaptureControl(actor, Skeleton_BeginHomingBurst);
        context->destination = actor->base.tracking.target->position;
        context->timer = context->duration;
        Vorn_SpawnHazard(actor, 2);
    }
    if (actor->base.stepDone) {
        SceneActorPlacement_ClearFlags(&actor->base.motionFlags, ~0x100);
        Skeleton_SelectDistanceReaction(actor);
    } else
        actor->base.counter2AC++;
}

void Skeleton_UpdateScriptWaitState(void);

s32 Callback_InvokeActor(void *, void *);

s32 Math_Atan2(s32, s32);

void Anim_SetDurationScale(void *, u32);

void Anim_SetFrameDurationScale(void *, u32);

struct SkeletonApproachProbe {
    u8 flags;
};

struct SkeletonChaseBody {
    u8 unknown0[0x6DC];
    u32 flags;
    u8 unknown6E0[0x28];
    u32 minimumDistance, maximumDistance;
};

static inline s32 Skeleton_ScaleApproachMotion(s32 n)
{
    return n >= 0 ? n >> 16 : -((-n) >> 16);
}

static inline void Skeleton_SetApproachState(void *object, u8 state, u8 phase, u32 behavior)
{
    struct ChaseActor *actor = object;
    actor->base.state2A8 = state;
    actor->base.phase2A9 = phase;
    actor->base.behavior2AA = behavior;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}

static inline void Skeleton_GetApproachCellCenter(s32 cell, struct ContactPosition *position)
{
    s32 x = Mod(cell, SceneActorChase_MapWidth(SceneActorChase_CurrentMap()));
    s32 z = Div(cell, SceneActorChase_MapWidth(SceneActorChase_CurrentMap()));
    position->x = ((u32)x << 8) + 0x80;
    {
        u32 attributes = SceneActorChase_CellProperties(cell)->attributes;
        u32 height = 15;
        height &= attributes;
        position->y = height << 8;
    }
    position->z = ((u32)z << 8) + 0x80;
}

static inline u8 Skeleton_ApproachTargetCell(void *obj)
{
    struct ContactPosition target;
    struct ChaseActor *actor = obj;
    struct SceneActorChaseGroup *group = SceneActorChase_GroupOf(obj);
    struct SceneActorChaseTracking *tracking = &actor->base.tracking;
    s32 cell, player;
    const u16 *randomSamples = gRandomTable;
    void **callback;
    SceneActorChase_SnapToRoute(group->destination, &group->homeRoute);
    if (SceneActorChase_Test(gEngineFlags, 0x4800)) {
        struct SceneActorChaseTarget *t = tracking->target;
        if (!t || (player = SceneActorChase_PlayerSlot(t)) < 0) {
            Callback_InvokeActor(actor, group->fallback);
            return 0;
        }
        cell = ((struct SceneActorChasePlayer *)gPlayers[player])->cell->index;
    } else {
        struct SceneActorChaseTarget *t = tracking->target;
        cell = SceneActorChase_CellAt(&t->position);
        player = 0;
    }
    if (cell != actor->base.terrainCell &&
        !SceneActorChase_Bits32(&gPlayers[player]->statusFlags,
                                0x20000))
        goto reroute;
    if (SceneActorChase_MotionFlags(actor, 0x400))
        return 0;
    {
        u32 bits = 4;
        group->common.gate.flags &= ~bits;
    }
    if (SceneActorChase_MotionFlags(actor, 0x80)) {
        SceneActorChase_SetControl(actor, group->common.gate.control, 0x1A);
        SceneActorChase_SetBehavior(actor, 12);
    } else {
        SceneActorChase_SetControl(actor, actor->base.onAlternateEntry, 1);
        SceneActorChase_SetBehavior(actor, 5);
        SceneActorChase_SnapToRoute(group->destination, &group->homeRoute);
        (group->destination)->y = SceneActorChase_HeightAt(
            SceneActorChase_CellAt(group->destination), group->destination);
        (group->destination)->pad = 0;
    }
    {
        u8 saved = actor->base.body.direction;
        SceneActorChase_BeginTurn(obj, saved, randomSamples);
    }
    SceneActorTerrain_RequestRetreatRoute(obj);
    actor->base.field2C2 = 0x20;
    return 0;
reroute: {
    Skeleton_GetApproachCellCenter(cell, &target);
    if (SceneActorChase_TryRoute(obj, group->route, &actor->base.body.position, &target))
        goto success;
}
    callback = &group->fallback;
    Callback_InvokeActor(actor, *callback);
    return 0;
success:
    return 1;
}

static inline void Skeleton_UpdateApproachAnimation(struct ChaseActor *actor)
{
    u32 mask = 0xA02F;
    u16 *scale;
    if (actor->base.motionFlags & mask) {
        actor->base.field2C2 = 0x40;
        scale = &actor->base.field2C2;
    } else {
        u16 t = actor->base.field2C2;
        scale = &actor->base.field2C2;
        if (t == 0)
            *scale = 0x40;
    }
    if (actor->base.drawableKind == 0)
        Anim_SetDurationScale(
            &((struct SceneActorAnimationSequenceDrawable *)actor->base.drawable)->sequence, *scale);
    else
        Anim_SetFrameDurationScale(
            &((struct SceneActorAnimationTileDrawable *)actor->base.drawable)->tile, *scale);
}

static inline void Skeleton_ApplyApproachMotion(struct ChaseActor *actor,
                                                struct SkeletonChaseBody *body, u32 angle,
                                                s32 speed)
{
    Skeleton_UpdateApproachAnimation(actor);
    actor->base.body.motion.x +=
        Skeleton_ScaleApproachMotion(gUnk_086149C4[(angle + 0x40) & 0xFF] * speed);
    actor->base.body.motion.z += Skeleton_ScaleApproachMotion(gUnk_086149C4[angle & 0xFF] * speed);
    {
        u32 mask = 0x2000;
        actor->base.behaviorFlags |= mask;
    }
    if (actor->base.tracking.distance > body->minimumDistance &&
        actor->base.tracking.distance < body->maximumDistance) {
        u16 mask = 0x80;
        actor->base.reactionFlags = mask | actor->base.reactionFlags;
    } else
        actor->base.counter2AC++;
}

void Skeleton_UpdateTargetApproach(struct ChaseActor *actor)
{
    struct SkeletonChaseBody *body = actor->base.context;
    struct SceneActorChaseGroup *group = &actor->group;
    if (SceneActorControl_TakeStateEntry(actor)) {
        void *target = Skeleton_UpdateScriptWaitState;
        SceneActorChase_SetControl(actor, target, 0x26);
    }
    if (((struct SkeletonApproachProbe *)actor->base.body.movementProbe)->flags & 1) {
        Skeleton_SetApproachState(actor, 0, 0, 12);
        SceneActorChase_SetControl(actor, group->common.gate.control, 0x1A);
        Skeleton_ApproachTargetCell(actor);
        {
            struct SkeletonChaseBody *b = actor->base.context;
            u32 mask = 0x80;
            b->flags |= mask;
        }
    } else {
        struct ContactPosition diff;
        u32 angle;
        s32 speed;
        {
            struct ContactPosition *out = &diff;
            struct ContactPosition *from = &actor->base.body.position;
            struct ContactPosition *to = &actor->base.tracking.target->position;
            SceneActorTerrain_Subtract(out, to, from);
        }
        angle = Math_Atan2(diff.x, diff.z);
        if (actor->base.tracking.distance < body->minimumDistance) {
            angle = (angle + 0x80) & 0xFF;
            actor->base.field2C2 = 0x20;
        } else if (actor->base.tracking.distance > body->maximumDistance) {
            actor->base.field2C2 = 0x20;
            actor->base.body.direction = angle;
        }
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 1);
        speed = Callback_InvokeActor(actor, actor->base.animationQuery.opaque);
        Skeleton_ApplyApproachMotion(actor, body, angle, speed);
    }
}

extern const u32 gUnk_086060A0[];

void Skeleton_SetReactionControlTable(u8 *p)
{
    *(const u32 **)(p + 0x28c) = gUnk_086060A0;
}
