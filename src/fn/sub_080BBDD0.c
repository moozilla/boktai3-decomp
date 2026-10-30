#include "global.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_reaction.h"

struct SkeletonRecoveryContext {
    struct SceneActorRegistryResetView base;
    u8 unknown6EC[4];
    u16 timer;
};

static inline void Skeleton_SnapRecoveryRoute(struct ContactPosition *position,
                                          struct SceneActorRegistryRoute *route)
{
    position->x = route->point->x;
    position->z = route->point->z;
    position->y = SceneActorChase_HeightAt(SceneActorChase_CellAt(position), position);
}

static inline void Skeleton_BeginRecoveryTurn(void *obj, u32 saved, const u16 *randomSamples)
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

static inline u8 Skeleton_ArriveAfterRecovery(void *obj)
{
    struct ContactPosition target;
    struct SceneActorRegistryResetView *actor = obj;
    struct SceneActorChaseGroup *group = &actor->group;
    struct SceneActorChaseTracking *tracking = &actor->tracking;
    s32 cell, player;
    const u16 *randomSamples = gRandomTable;
    void **callback;
    Skeleton_SnapRecoveryRoute(group->destination, &group->homeRoute);
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
        Skeleton_SnapRecoveryRoute(group->destination, &group->homeRoute);
        (group->destination)->y = SceneActorChase_HeightAt(
            SceneActorChase_CellAt(group->destination), group->destination);
        (group->destination)->pad = 0;
    }
    {
        u8 saved = actor->body.direction;
        Skeleton_BeginRecoveryTurn(obj, saved, randomSamples);
    }
    sub_0807E334(obj);
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

static inline s32 Skeleton_QuarterDisplacement(s32 d)
{
    s32 result;
    if (d >= 0)
        result = d >> 2;
    else
        result = -(-d >> 2);
    return result;
}

static inline u8 Skeleton_DisplacementStopped(s32 dx, s32 dz)
{
    if (dx == 0 && dz == 0)
        return 1;
    return 0;
}

static inline s32 Skeleton_ReadDisplacementAxis(s32 desired, s16 *current, s16 **field)
{
    s32 difference = desired - *current;
    *field = current;
    return Skeleton_QuarterDisplacement(difference);
}

static inline u8 Skeleton_Drift(struct SceneActorRegistryResetView *actor)
{
    s32 dx, dz;
    s16 *x, *z;
    if (SceneActorReaction_TakePrepared(actor))
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 3);
    SceneActorChase_SetActorFlags(actor, 0x2000);
    {
        u32 mask = 0x10000;
        actor->flags2D0 |= mask;
    }
    dx = Skeleton_ReadDisplacementAxis(actor->reactionStart.x, &actor->reactionEnd.x, &x);
    dz = Skeleton_ReadDisplacementAxis(actor->reactionStart.z, &actor->reactionEnd.z, &z);
    if (dx > 200)
        dx = 200;
    else if (dx < -200)
        dx = -200;
    if (dz > 200)
        dz = 200;
    else if (dz < -200)
        dz = -200;
    actor->body.motion.x += dx;
    actor->body.motion.z += dz;
    *x += dx;
    *z += dz;
    return Skeleton_DisplacementStopped(dx, dz);
}

static inline void Skeleton_Settle(struct SceneActorRegistryResetView *actor,
                                    struct SceneActorChaseGroup *group,
                                    struct SkeletonRecoveryContext *context)
{
    SceneActorChase_ClearMotionFlagComponent(actor, 0x120);
    SceneActorChase_ClearActorFlags(actor, 0x80);
    if (actor->field2DC & 0x80000) {
        SceneActorChase_SetReactionFlags(actor, 2);
        return;
    }
    if (SceneActorChase_QueryMotionFlagComponent(actor, 0x80)) {
        SceneActorChase_SetActorFlags(actor, 0x80);
        SceneActorChase_SetFlagComponent((struct SceneActorChaseFlags16 *)&group->common.gate.flags,
                                         4);
        SceneActorChase_TrackPlayer(actor);
        if (!Skeleton_ArriveAfterRecovery(actor)) {
            if (!SceneActorChase_TryRoute(actor, group->route, &actor->body.position,
                                          &actor->tracking.target->position)) {
                Callback_InvokeActor(actor, group->fallback);
                return;
            }
        }
        SceneActorChase_SetControl(actor, group->common.gate.control, 0x1A);
        SceneActorChase_SetState(actor, 0, 0, 12);
    } else {
        SceneActorChase_TrackPlayer(actor);
        SceneActorChase_SetActorFlags(actor, 0x10);
        group->destination->pad = 0;
        if (!Skeleton_ArriveAfterRecovery(actor)) {
            context->timer = 0;
            if (!SceneActorChase_TryRoute(actor, group->route, &actor->body.position,
                                          &actor->tracking.target->position)) {
                Callback_InvokeActor(actor, group->fallback);
                return;
            }
        }
        SceneActorChase_SetControl(actor, actor->entryTarget228, 1);
        SceneActorChase_SetState(actor, 0, 0, 7);
        actor->field2C2 = 0x20;
    }
}

void Skeleton_ConvergeDisplacedPosition(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    if (Skeleton_Drift(actor)) {
        struct SkeletonRecoveryContext *context = actor->context;
        actor->field2B0 = 1;
        Skeleton_Settle(actor, group, context);
    }
}
