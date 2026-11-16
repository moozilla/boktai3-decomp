#include "global.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_control_phase.h"
#include "scene_actor_timed_behavior.h"

s32 SceneActorTerrain_FindOpenNeighborDirection(struct ChaseActor *, s32);

static inline u32 Skeleton_RandomSample(const u16 *samples)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return samples[index];
}

static inline s32 Skeleton_ScaleMotion(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}

static inline void Skeleton_SetPolarMotion(struct ContactPosition *motion, s32 angle, s32 speed)
{
    motion->x = Skeleton_ScaleMotion(gUnk_086149C4[(angle + 64) & 255] * speed);
    motion->y = 0;
    motion->z = Skeleton_ScaleMotion(gUnk_086149C4[angle & 255] * speed);
}

static inline struct SceneActorChaseFlags16 *Skeleton_GateFlags(struct ChaseActor *actor)
{
    return (struct SceneActorChaseFlags16 *)&actor->group.common.gate.flags;
}

static inline void Skeleton_EnableMotionFlags(struct ChaseActor *actor, u16 bits)
{
    SceneActorChase_SetFlagComponent((struct SceneActorChaseFlags16 *)&actor->base.flags2D4, bits);
}

static inline void Skeleton_SetContextFlags(struct SceneActorTimedBehaviorContext *context,
                                            u32 bits)
{
    context->flags6DC |= bits;
}

static inline u32 Skeleton_MovementFlags(struct ChaseActor *actor, u32 mask)
{
    return SceneActorChase_Bits32(&actor->base.flags2D0, mask);
}

static inline void Skeleton_UpdateArrival(void *obj)
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
        if (!t)
            goto invalid_target;
        player = SceneActorChase_PlayerSlot(t);
        if (player < 0) {
        invalid_target:
            callback = &group->fallback;
            Callback_InvokeActor(actor, *callback);
            return;
        }
        cell = ((struct SceneActorChasePlayer *)gUnknown_02000580[player])->cell->index;
    } else {
        struct SceneActorChaseTarget *t = tracking->target;
        cell = SceneActorChase_CellAt(&t->position);
        player = 0;
    }
    if (cell != actor->base.terrainCell &&
        !SceneActorChase_Bits32(&((struct SceneActorChasePlayer *)gUnknown_02000580[player])->flags,
                                0x20000))
        goto reroute;
    if (SceneActorChase_MotionFlags(actor, 0x400))
        return;
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
    return;
reroute: {
    SceneActorChase_CellCenter(cell, &target);
    if (SceneActorChase_TryRoute(obj, group->route, &actor->base.body.position, &target))
        goto success;
}
    callback = &group->fallback;
    Callback_InvokeActor(actor, *callback);
    return;
success:
    return;
}

static inline void Skeleton_StartHop(struct ChaseActor *actor, s32 direction, s32 speed)
{
    Skeleton_SetPolarMotion(&actor->base.secondaryMotion, direction << 6, speed);
    actor->base.body.direction = ((direction + 2) & 3) << 6;
    actor->base.secondaryMotion.y =
        (Skeleton_RandomSample((const u16 *)RANDOM_TABLE_ADDR) & 15) + 12;
    SceneActorAnimation_SelectDirectionalEntry(&actor->base, 14);
}

static inline void Skeleton_RestartGate(struct ChaseActor *actor)
{
    actor->group.common.gate.counter = actor->group.common.gate.duration;
}

void sub_080B9158(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    if (SceneActorControl_TakeFlag(&actor->base.field2B2)) {
        s32 speed = (Skeleton_RandomSample((const u16 *)RANDOM_TABLE_ADDR) & 3) + 10;
        s32 direction =
            SceneActorTerrain_FindOpenNeighborDirection(actor, actor->base.reactionDirection);
        if (direction < 0) {
            struct SceneActorTimedBehaviorContext *context;
            SceneActorAnimation_SelectDirectionalEntry(&actor->base, 15);
            SceneActorChase_SetFlagComponent(Skeleton_GateFlags(actor), 4);
            Skeleton_EnableMotionFlags(actor, 0x80);
            Skeleton_RestartGate(actor);
            SceneActorChase_SelectState(actor, 0, 0, 12);
            SceneActorChase_ResetStateEntry(actor);
            SceneActorChase_SetTypedControl(actor, group->common.gate.control, 0x1A);
            Skeleton_UpdateArrival(actor);
            context = actor->base.context;
            Skeleton_SetContextFlags(context, 0x80);
            return;
        }
        Skeleton_StartHop(actor, direction, speed);
    }
    if (Skeleton_MovementFlags(actor, 0x10)) {
        SceneActorAnimation_SelectDirectionalEntry(&actor->base, 15);
        SceneActorChase_SelectState(actor, 0, 0, 12);
        SceneActorChase_ResetStateEntry(actor);
        SceneActorChase_SetTypedControl(actor, group->common.gate.control, 0x1A);
        Skeleton_UpdateArrival(actor);
    }
}
