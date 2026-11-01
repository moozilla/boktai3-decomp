#include "global.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_reaction.h"
#include "scene_actor_delayed_response.h"
#include "scene_actor_control_phase.h"

u8 sub_0812F3CC(void *);
void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);

static inline void Skeleton_SnapDelayedRoute(struct ContactPosition *position,
                                             struct SceneActorRegistryRoute *route)
{
    position->x = route->point->x;
    position->z = route->point->z;
    position->y = SceneActorChase_HeightAt(SceneActorChase_CellAt(position), position);
}

static inline void Skeleton_BeginDelayedTurn(void *obj, u32 saved, const u16 *randomSamples)
{
    struct SceneActorRegistryResetView *actor = obj;
    struct SceneActorChaseGroup *group = &actor->group;
    if (!SceneActorChase_MotionFlags(actor, 0x400) &&
        !SceneActorChase_TestField((u16 *)&group->common.gate.flags, 4)) {
        group->turn.direction =
            SceneActorChase_RandomDirectionExcept(actor->body.direction, randomSamples);
        SceneActorChase_SaveControl(&group->saved, actor);
        group->turn.savedDirection = saved;
        SceneActorChase_SetControl(actor, group->turnControl, 0x1B);
        SceneActorChase_SetState(actor, 3, 1, 0);
        group->turn.turnsRemaining = 6;
    }
}

/* A successful cell-center reroute returns one. Other paths leave the
 * caller to try the separate partner-position route. */

static inline u8 Skeleton_ArriveAfterDelay(void *obj)
{
    struct ContactPosition target;
    struct SceneActorRegistryResetView *actor = obj;
    struct SceneActorChaseGroup *group = &actor->group;
    struct SceneActorChaseTracking *tracking = &actor->tracking;
    s32 cell, player;
    const u16 *randomSamples = gRandomTable;
    void **callback;
    Skeleton_SnapDelayedRoute(group->destination, &group->homeRoute);
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
        cell = SceneActorChase_CellAt(&t->position);
        player = 0;
    }
    if (cell != actor->terrainCell &&
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
        SceneActorChase_SetBehavior(actor, 12);
    } else {
        SceneActorChase_SetControl(actor, actor->entryTarget228, 1);
        SceneActorChase_SetBehavior(actor, 5);
        Skeleton_SnapDelayedRoute(group->destination, &group->homeRoute);
        (group->destination)->y = SceneActorChase_HeightAt(
            SceneActorChase_CellAt(group->destination), group->destination);
        (group->destination)->pad = 0;
    }
    {
        u8 saved = actor->body.direction;
        Skeleton_BeginDelayedTurn(obj, saved, randomSamples);
    }
    SceneActorTerrain_RequestRetreatRoute(obj);
    actor->field2C2 = 0x20;
    return 0;
reroute: {
    SceneActorChase_CellCenter(cell, &target);
    if (SceneActorChase_TryRoute(obj, group->route, &actor->body.position, &target))
        goto success;
}
    callback = &group->fallback;
invoke:
    Callback_InvokeActor(actor, *callback);
    return 0;
success:
    return 1;
}

void sub_080BFCC4(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    if (SceneActorControl_TakeStateEntry(actor))
        actor->body.direction = actor->tracking.heading;
    if ((u8)Callback_InvokeActor(actor, actor->contactQuery.opaque))
        return;
    if (sub_0812F3CC(actor))
        return;
    if ((s32)actor->counter2AC > 60) {
        if (SceneActorChase_QueryMotionFlags(actor, 0x80)) {
            SceneActorChase_SetState(actor, 0, 0, 12);
            SceneActorChase_SetControl(actor, group->common.gate.control, 0x1A);
            Skeleton_ArriveAfterDelay(actor);
        } else {
            SceneActorChase_SetState(actor, 3, 1, 0);
            SceneActorChase_SetControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
            SceneActorAttachment_ReplaceResponseSlot(actor, 0, 2, actor->category);
            SceneActorDelay_Prepare((void *)&group->saved, actor->entryTarget228, 7, 30);
            Skeleton_SnapDelayedRoute(group->destination, &group->homeRoute);
            if (!SceneActorChase_TryRoute(actor, group->route, &actor->body.position,
                                          group->destination)) {
                SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0);
                SceneActorChase_SetState(actor, 3, 0, 0);
            } else
                group->destination->pad = 1;
        }
    } else
        actor->counter2AC++;
}
