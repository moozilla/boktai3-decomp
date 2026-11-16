#include "global.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_reaction.h"
#include "scene_actor_delayed_response.h"
#include "display_sequence.h"

/* This class's own copies of the shared chase inlines it compiles differently. */
static inline struct SceneActorChaseGroup *Bat_GroupOf(struct ChaseActor *actor)
{
    return &actor->group;
}

static inline void Bat_BeginTurn(void *obj, u32 saved, const u16 *randomSamples)
{
    struct ChaseActor *actor = obj;
    struct SceneActorChaseGroup *group = Bat_GroupOf(obj);
    if (!SceneActorChase_MotionFlags(actor, 0x400) &&
        !SceneActorChase_TestField((u16 *)&group->common.gate.flags, 4)) {
        group->turn.direction =
            SceneActorChase_RandomDirectionExcept(actor->base.body.direction, randomSamples);
        SceneActorChase_SaveControl(&group->saved, actor);
        group->turn.savedDirection = saved;
        SceneActorChase_SetControl(actor, group->turnControl, 0x1B);
        SceneActorChase_SetState(actor, 3, 1, 0);
        group->turn.turnsRemaining = 6;
    }
}

static inline u8 Bat_Arrive(void *obj)
{
    struct ContactPosition target;
    struct ChaseActor *actor = obj;
    struct SceneActorChaseGroup *group = Bat_GroupOf(obj);
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
            goto invoke;
        }
        cell = ((struct SceneActorChasePlayer *)gUnknown_02000580[player])->cell->index;
    } else {
        struct SceneActorChaseTarget *t = tracking->target;
        cell = SceneActorChase_CellAt(&(t)->position);
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
        Bat_BeginTurn(obj, saved, randomSamples);
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

static inline u16 *Bat_PrepareLanding(struct ChaseActor *actor, s32 frame)
{
    struct DisplaySequenceState *animation;
    u16 *reactionFlags;
    if (SceneActorReaction_TakePrepared(actor))
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 3);
    SceneActorChase_SetReactionFlags(actor, 0x10);
    reactionFlags = &actor->base.reactionFlags;
    if (frame <= 17) {
        animation = ((struct SceneActorAnimationSequenceDrawable *)actor->base.drawable)->sequence;
        if (animation->index == 0 && animation->elapsed == (animation->duration != 0) &&
            animation->duration != 1)
            animation->elapsed = 0;
    }
    return reactionFlags;
}

/* Resume the suspended landing reaction, then queue pursuit or the home route. */
void sub_08087018(struct ChaseActor *actor, s32 frame)
{
    struct SceneActorChaseGroup *group = &actor->group;
    u16 *reactionFlags;
    reactionFlags = Bat_PrepareLanding(actor, frame);
    if (!actor->base.restart)
        return;
    actor->base.stepDone = 1;
    if (SceneActorChase_QueryMotionFlagComponent(actor, 2)) {
        SceneActorChase_SetState(actor, 3, 0, 0);
        SceneActorChase_SetControl(actor, group->contactControl, 7);
        SceneActorChase_ClearBits(reactionFlags, 0x983);
        return;
    }
    if (SceneActorChase_QueryMotionFlagComponent(actor, 0x80)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        SceneActorChase_SetState(actor, 3, 1, 0);
        SceneActorChase_SetControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
        SceneActorDelay_Prepare((void *)&group->saved, group->common.gate.control, 12, 30);
        Bat_Arrive(actor);
        return;
    }
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    SceneActorChase_SetState(actor, 3, 1, 0);
    SceneActorChase_SetControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
    SceneActorDelay_Prepare((void *)&group->saved, actor->base.onAlternateEntry, 7, 60);
    actor->base.field2C2 = 64;
    SceneActorChase_SetActorFlags(actor, 0x10);
    SceneActorChase_ClearMotionFlagComponent(actor, 0x20);
    SceneActorChase_SnapToRoute(group->destination, &group->homeRoute);
    if (!SceneActorChase_TryRoute(actor, group->route, &actor->base.body.position,
                                  group->destination)) {
        SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0);
        SceneActorChase_SetState(actor, 3, 0, 0);
        return;
    }
    group->destination->pad = 1;
}
