#include "global.h"
#include "scene_actor_control_phase.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_reaction.h"
#include "scene_actor_delayed_response.h"

/* This class's own copies of the shared chase inlines it compiles differently. */
static inline void Golem_SaveControl(struct SceneActorChaseSavedControl *packet, void *object)
{
    struct ChaseActor *actor = object;
    packet->control = actor->base.target;
    packet->kind = actor->base.controlKind;
    packet->state = actor->base.state2A8;
    packet->phase = actor->base.phase2A9;
    packet->behavior = actor->base.behavior2AA;
}

static inline void Golem_SelectState(void *object, u32 state, u32 phase, u32 behavior)
{
    struct ChaseActor *actor = object;
    actor->base.state2A8 = state;
    actor->base.phase2A9 = phase;
    actor->base.behavior2AA = behavior;
}

static inline s32 Golem_CellAt(struct ContactPosition *position)
{
    return SceneActorChase_CellIndex(position->x >> 8, position->z >> 8);
}

static inline void Golem_SetState(void *object, u32 state, u32 phase, u32 behavior)
{
    struct ChaseActor *actor = object;
    Golem_SelectState(actor, state, phase, behavior);
    SceneActorChase_ResetStateEntry(actor);
}

static inline void Golem_SetPhase(void *object, u32 phase, u32 behavior)
{
    struct ChaseActor *actor = object;
    Golem_SelectState(actor, 0, phase, behavior);
    SceneActorChase_ResetStateEntry(actor);
}

static inline void Golem_SetBehavior(void *object, u32 behavior)
{
    struct ChaseActor *actor = object;
    Golem_SetPhase(actor, 0, behavior);
}

static inline u32 Golem_Height(struct ContactPosition *position)
{
    return SceneActorChase_HeightAt(Golem_CellAt(position), position);
}

static inline void Golem_SnapToRoute(struct ContactPosition *position,
                                     struct SceneActorRegistryRoute *route)
{
    position->x = route->point->x;
    position->z = route->point->z;
    position->y = Golem_Height(position);
}

static inline void Golem_BeginTurn(void *obj, u32 saved, const u16 *randomSamples)
{
    struct ChaseActor *actor = obj;
    struct SceneActorChaseGroup *group = SceneActorChase_GroupOf(obj);
    if (!SceneActorChase_MotionFlags(actor, 0x400) &&
        !SceneActorChase_TestField((u16 *)&group->common.gate.flags, 4)) {
        group->turn.direction =
            SceneActorChase_RandomDirectionExcept(actor->base.body.direction, randomSamples);
        Golem_SaveControl(&group->saved, actor);
        group->turn.savedDirection = saved;
        SceneActorChase_SetControl(actor, group->turnControl, 0x1B);
        Golem_SetState(actor, 3, 1, 0);
        group->turn.turnsRemaining = 6;
    }
}

static inline u8 Golem_Arrive(void *obj)
{
    struct ContactPosition target;
    struct ChaseActor *actor = obj;
    struct SceneActorChaseGroup *group = SceneActorChase_GroupOf(obj);
    struct SceneActorChaseTracking *tracking = &actor->base.tracking;
    s32 cell, player;
    const u16 *randomSamples = gRandomTable;
    void **callback;
    Golem_SnapToRoute(group->destination, &group->homeRoute);
    if (SceneActorChase_Test(gEngineFlags, 0x4800)) {
        struct SceneActorChaseTarget *t = tracking->target;
        if (!t)
            goto invalid_target;
        player = SceneActorChase_PlayerSlot(t);
        if (player < 0) {
        invalid_target:
            callback = &group->fallback;
            goto invoke;
        }
        cell = ((struct SceneActorChasePlayer *)gUnknown_02000580[player])->cell->index;
    } else {
        struct SceneActorChaseTarget *t = tracking->target;
        cell = SceneActorChase_CellIndex((t)->position.x >> 8, (t)->position.z >> 8);
        player = 0;
    }
    if (cell != actor->base.terrainCell &&
        !SceneActorChase_Bits32(&((struct SceneActorChasePlayer *)gUnknown_02000580[player])->flags,
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
        Golem_SetBehavior(actor, 12);
    } else {
        SceneActorChase_SetControl(actor, actor->base.onAlternateEntry, 1);
        Golem_SetBehavior(actor, 5);
        Golem_SnapToRoute(group->destination, &group->homeRoute);
        (group->destination)->y =
            SceneActorChase_HeightAt(Golem_CellAt(group->destination), group->destination);
        (group->destination)->pad = 0;
    }
    {
        u8 saved = actor->base.body.direction;
        Golem_BeginTurn(obj, saved, randomSamples);
    }
    SceneActorTerrain_RequestRetreatRoute(obj);
    actor->base.field2C2 = 0x20;
    return 0;
reroute: {
    SceneActorChase_CellCenter(cell, &target);
    if (SceneActorChase_TryRoute(obj, group->route, &actor->base.body.position, &target))
        goto success;
}
    callback = &group->fallback;
invoke:
    Callback_InvokeActor(actor, *callback);
    return 0;
success:
    return 1;
}

/* Queue pursuit or a return-home route after the recovery delay. */
void Golem_ResumeDelayedRoute(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    u32 zero;
    SceneActorControl_TakeStateEntry(actor);
    if ((u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque))
        return;
    zero = (u8)sub_0812F3CC(actor);
    if (zero)
        return;
    if ((s32)actor->base.counter2AC <= 60)
        goto waiting;
    SceneActorAttachment_ReplaceResponseSlot((u8 *)actor, 0, 2, actor->base.category);
    Golem_SetState(actor, 3, 1, zero);
    SceneActorChase_SetControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
    if (SceneActorChase_MotionFlags(actor, 0x80)) {
        SceneActorDelay_Prepare((void *)&group->saved, group->common.gate.control, 12, 30);
        Golem_Arrive(actor);
    } else {
        SceneActorChase_SetActorFlags(actor, 0x10);
        SceneActorDelay_Prepare((void *)&group->saved, actor->base.onAlternateEntry, 7, 30);
        Golem_SnapToRoute(group->destination, &group->homeRoute);
        if (!SceneActorChase_TryRoute(actor, group->route, &actor->base.body.position,
                                      group->destination)) {
            SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0);
            Golem_SetState(actor, 3, 0, 0);
        } else
            group->destination->pad = 1;
    }
    return;
waiting:
    actor->base.counter2AC++;
}
