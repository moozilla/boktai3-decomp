#include "golem.h"
#include "scene_actor_behavior.h"
#include "script_utility.h"
#include "engine.h"
#include "player.h"
#include "ram.h"
#include "scene_actor_part_burst.h"
#include "scene_actor_chase_reaction.h"
#include "global.h"
#include "scene_actor_control_phase.h"
#include "scene_actor_reaction.h"
#include "scene_actor_delayed_response.h"
#include "scene_actor_contact_response.h"
#include "scene_actor_turn.h"

void SceneActorReaction_PlayAnimationBUntilComplete(void);

void SceneActorAttachment_ReplaceResponseSlot(u8 *, u32, u32, u32);

void SceneActorBehavior_CallActionScript(u8 *);

void Golem_ResetIdleControl(u8 *);

void SceneActorEffects_AdvanceSteps(void *, u32);

void SceneActorTerrain_ClearCollisionTimers(struct RoomActor *);

void SceneActorBehavior_CallTerrainScript(void *);

void SceneActorPlacement_PlayCategoryEventSound(void *);

void *Bestiary_GetEntryForActor(u32, u32);

void Bestiary_UnlockEntry(void *);

void ActorShadow_Disable(void *);

void ActorParticles_SpawnWideBurst(void *);

void SceneActorControl_OnIdle(u8 *);

void Golem_OnUnusedState(void)
{
}

/* Releases the defeated golem, runs its script, and emits the defeat burst. */

void Golem_UpdateDefeat(struct ChaseActor *actor)
{
    s32 values[8];
    struct ContactPosition spread;
    struct ScriptUtilityArguments arguments;
    struct SceneActorRegistryControlGroup *group = &actor->group.common;
    u16 script;
    if (SceneActorControl_TakeFlag(&actor->base.field2B2)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 10);
        actor->base.restart = 0;
        SceneActorAttachment_ReplaceResponseSlot(actor, 0, 127, 0);
        SceneActorAttachment_ReplaceResponseSlot(actor, 1, 127, 0);
        SceneActorAttachment_ReplaceResponseSlot(actor, 3, 127, 0);
        SceneActorEffects_AdvanceSteps(actor, 4);
        {
            struct SceneActorRegistryControl *control = &group->control;
            SceneActorChase_ClearWordFlags(&actor->base.behaviorFlags, 0x141C0);
            control->counter = 0;
            control->field14 = 0;
            control->timer = 0;
        }
        Sound_PlaySong(0x13E);
        {
            u16 *flags = &actor->base.colliderA.flags;
            u32 value = *flags;
            u32 m = 4;
            m |= value;
            *flags = m;
        }
    }
    SceneActorTerrain_ClearCollisionTimers((void *)actor);
    if (!actor->base.restart)
        return;
    actor->base.stepDone = 1;
    SceneActorBehavior_CallTerrainScript(actor);
    script = (u16)actor->base.script1F8;
    if (script) {
        values[0] = actor->base.body.position.x;
        values[1] = actor->base.body.position.y;
        values[2] = actor->base.body.position.z;
        values[3] = actor->base.parameters1FC[0];
        values[4] = actor->base.parameters1FC[1];
        values[5] = actor->base.parameters1FC[2];
        values[6] = (s32)actor->base.hitOwners;
        values[7] = actor->base.body.id;
        arguments.count = 8;
        arguments.values = values;
        Script_Call(script, &arguments);
    }
    if (!SceneActorContact_MotionFlags(actor, 0x4000) && (u32)(actor->base.category - 11) > 2)
        Bestiary_UnlockEntry(
            Bestiary_GetEntryForActor(actor->base.category, actor->base.paletteOffset));
    if (!SceneActorTurn_ActorFlags(actor, 0x400)) {
        u8 category;
        spread.x = 80;
        spread.y = 60;
        spread.z = 60;
        {
            const u16 *table = gRandomTable;
            SCENE_ACTOR_SPAWN_DEFEAT_SPARK_RING(actor, &spread, table, 32);
            SCENE_ACTOR_SPAWN_DEFEAT_SPARK_RING(actor, &spread, table, 64);
            SCENE_ACTOR_SPAWN_DEFEAT_SPARK_RING(actor, &spread, table, 94);
            SCENE_ACTOR_SPAWN_DEFEAT_SPARK_RING(actor, &spread, table, 128);
            SCENE_ACTOR_SPAWN_DEFEAT_SPARK_RING(actor, &spread, table, 196);
            SCENE_ACTOR_SPAWN_DEFEAT_SPARK_RING(actor, &spread, table, 240);
        }
        category = actor->base.category;
        if (category == 4 || category == 1 || category == 5 || category == 7 || category == 6 ||
            category == 0x18 || category == 0x19)
            Sound_PlaySong(0xE5);
    }
    SceneActorChase_SetTypedControl(actor, SceneActorControl_OnIdle, 16);
    SceneActorChase_SelectState(actor, 1, 0, 2);
    SceneActorChase_ResetStateEntry(actor);
    ActorShadow_Disable(actor->base.shadow);
    if (!actor->base.drawableKind) {
        struct SceneActorRegistrySequenceDrawable *drawable = actor->base.drawable;
        drawable->flags |= 1;
    } else {
        struct SceneActorAnimationTileDrawable *drawable = actor->base.drawable;
        struct SceneInputWidget *tile = &drawable->tile;
        tile->flags |= 1;
    }
    ActorParticles_SpawnWideBurst(actor);
    SceneActorPlacement_PlayCategoryEventSound(actor);
}

void SceneActorRoute_CommitReachableDestination(void *);

/* This class's own copies of the shared chase inlines it compiles differently. */

static inline void Golem_SelectState(void *object, u32 state, u32 phase, u32 behavior)
{
    struct ChaseActor *actor = object;
    actor->base.state2A8 = state;
    actor->base.phase2A9 = phase;
    actor->base.behavior2AA = behavior;
}

static inline u8 Golem_CoordinateFraction(s16 coordinate)
{
    return coordinate;
}

static inline s32 Golem_CellAt(struct ContactPosition *position)
{
    return SceneActorChase_CellIndex(position->x >> 8, position->z >> 8);
}

static inline u32 Golem_HeightAt(s32 cell, struct ContactPosition *position)
{
    struct SceneActorChaseTerrainCell *data = SceneActorChase_CellProperties(cell);
    u32 kind, height;
    kind = data->attributes >> 4;
    height = (data->attributes & 15) << 8;
    switch (kind) {
        case 1:
            height -= Golem_CoordinateFraction(position->z);
            break;
        case 2:
            height -= Golem_CoordinateFraction(position->x);
            break;
    }
    return height;
}

static inline u8 Golem_TryRoute(struct ChaseActor *actor, void *route,
                                struct ContactPosition *position, struct ContactPosition *target)
{
    s32 result;
    void *routeOwner = route;
    struct ContactPosition *from = position;
    struct ContactPosition *to = target;
    s32 status = Nav_IsPositionInMesh(routeOwner, from, to);
    if (status)
        status = Terrain_ApplyBlocked(routeOwner, from, to);
    result = status;
    if (result) {
        if (result != 2)
            Memory_Clear(SceneActorChase_MotionOf(actor), 8);
        return 1;
    }
    SceneActorChase_ClearBits(SceneActorChase_MotionFlagsOf(actor), 0x200);
    return 0;
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
    return Golem_HeightAt(Golem_CellAt(position), position);
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
        SceneActorChase_SaveControl(&group->saved, actor);
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
        cell = ((struct SceneActorChasePlayer *)gPlayers[player])->cell->index;
    } else {
        struct SceneActorChaseTarget *t = tracking->target;
        cell = SceneActorChase_CellIndex((t)->position.x >> 8, (t)->position.z >> 8);
        player = 0;
    }
    if (cell != actor->base.terrainCell &&
        !SceneActorChase_Bits32(&gPlayers[player]->statusFlags, 0x20000))
        goto reroute;
    if (SceneActorChase_MotionFlags(actor, 0x400))
        return 0;
    SceneActorChase_ClearFlagComponent((void *)&group->common.gate.flags, 4);
    if (SceneActorChase_MotionFlags(actor, 0x80)) {
        SceneActorChase_SetControl(actor, group->common.gate.control, 0x1A);
        Golem_SetBehavior(actor, 12);
    } else {
        SceneActorChase_SetControl(actor, actor->base.onAlternateEntry, 1);
        Golem_SetBehavior(actor, 5);
        Golem_SnapToRoute(group->destination, &group->homeRoute);
        (group->destination)->y =
            Golem_HeightAt(Golem_CellAt(group->destination), group->destination);
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
    if (Golem_TryRoute(obj, group->route, &actor->base.body.position, &target))
        goto success;
}
    callback = &group->fallback;
invoke:
    Callback_InvokeActor(actor, *callback);
    return 0;
success:
    return 1;
}

static inline void Golem_SetTypedState(struct ChaseActor *actor, u32 state, u32 phase, u32 behavior)
{
    Golem_SelectState(actor, state, phase, behavior);
    SceneActorChase_ResetStateEntry(actor);
}

/* Fields are established by 080C9C04 and the ordinary ReturnHome/ChargeOrReturnHome
 * relatives. Names for the response count and flags are tentative. */

static inline void GolemReaction_SetBodyFlags(struct Golem *body, u32 flags)
{
    body->flags628 |= flags;
}

static inline void GolemReaction_LatchHitReact(void *object)
{
    struct ChaseActor *actor = object;
    if (!actor->group.hitReactionLatch && (actor->base.field2E6 & 8)) {
        actor->base.hitLockTimer = 0x2D;
        actor->group.hitReactionLatch = 0x41;
        SceneActorAnimation_SelectDirectionalEntry(actor, 3);
    }
}

static inline s16 GolemReaction_CurrentHeight(struct ChaseActor *actor)
{
    return actor->base.attachmentRestart;
}

static inline void GolemReaction_SetChargeHeight(struct ChaseActor *actor, s32 height)
{
    actor->base.attachmentTimer = height;
}

static inline void GolemReaction_EnterCharge(struct ChaseActor *actor, s16 height)
{
    GolemReaction_SetChargeHeight(actor, height);
    SceneActorChase_SetControl(actor, actor->base.onReaction, 0x12);
    Golem_SetTypedState(actor, 3, 0, 0);
}

static inline void GolemReaction_Settle(struct ChaseActor *actor)
{
    SceneActorChase_SetTypedControl(actor, actor->base.onAlternateEntry, 1);
    Golem_SetTypedState(actor, 0, 0, 7);
    actor->base.field2C2 = 0x20;
}

/* Dispatch the hit-reaction state; return one when entering a state. */

static inline void Golem_InvokeRouteFallback(struct ChaseActor *actor,
                                             struct SceneActorChaseGroup *group)
{
    Callback_InvokeActor(actor, group->fallback);
}

s32 Golem_UpdateHitReaction(struct ChaseActor *actor)
{
    struct Golem *body = actor->base.context;
    struct SceneActorChaseGroup *group = &actor->group;
    void *object = actor;

    GolemReaction_SetBodyFlags(body, 0x800);
    actor->base.statusFlags &= ~0x1000000;
    SceneActorPlacement_PlayVariantEventSound(actor, 1);

    if (SceneActorChase_QueryMotionFlags(actor, 0x2000)) {
        actor->base.body.direction = actor->base.reactionDirection + 0x80;
        SceneActorAnimation_SelectDirectionalEntry(actor, 0);
        SceneActorChase_ClearMotionFlags(actor, 0x2020);
        SceneActorChase_SetActorFlags(actor, 0x10);
        group->destination->pad = 0;
        actor->base.tracking.target = &((struct SceneActorChasePlayer *)gPlayers[0])->target;
        if (!Golem_Arrive(object)) {
            if (!Golem_TryRoute(object, group->route, &actor->base.body.position,
                                &actor->base.tracking.target->position)) {
                Golem_InvokeRouteFallback(actor, group);
                return 0;
            }
        }
        GolemReaction_Settle(actor);
        return 1;
    }
    if (SceneActorChase_QueryMotionFlags(actor, 0x8000)) {
        SceneActorChase_ClearMotionFlags(actor, 0x20);
        return 0;
    }
    if (SceneActorChase_QueryMotionFlags(actor, 2)) {
        actor->base.reactionStart = actor->base.reactionEnd = actor->base.body.position;
        SceneActorChase_SetControl(actor, group->contactControl, 7);
        Golem_SetTypedState(actor, 3, 0, 0);
        return 1;
    }
    if (SceneActorChase_QueryMotionFlags(actor, 0x100)) {
        if (actor->base.hitAttributes & 0x400) {
            body->shakeStrength = 30;
            body->shakeTimer = 20;
            GolemReaction_EnterCharge(actor, GolemReaction_CurrentHeight(actor));
            actor->base.hitReactionFlags |= 0x80000;
            SceneActorChase_ClearWordFlags(&actor->base.behaviorFlags, 0x80);
            SceneActorChase_ClearWordFlags(&actor->base.statusFlags, 0x1000003);
            actor->base.field2C2 = 0x40;
            return 1;
        }
        if (actor->base.hitReactionFlags & 0xC0000) {
            GolemReaction_EnterCharge(actor, GolemReaction_CurrentHeight(actor));
            actor->base.statusFlags &= ~0x1000000;
            return 1;
        }
        if (actor->base.tracking.target) {
            SceneActorRoute_CommitReachableDestination(actor);
            body->routeDelay = 120;
        }
        SceneActorChase_ClearMotionFlags(actor, 0x20);
        return 0;
    }
    if (actor->base.hitReactionFlags & 0xC0000) {
        GolemReaction_EnterCharge(actor, GolemReaction_CurrentHeight(actor));
        actor->base.statusFlags &= ~0x1000000;
        return 1;
    }

    SceneActorChase_ClearMotionFlags(actor, 0x20);
    if (SceneActorChase_TrackingFlags(&actor->base.tracking, 0x10))
        return 0;
    if (SceneActorChase_QueryMotionFlags(actor, 0x80)) {
        GolemReaction_LatchHitReact(object);
        if (!actor->base.tracking.target)
            return 0;
        actor->base.tracking.target = &((struct SceneActorChasePlayer *)gPlayers[0])->target;
        SceneActorChase_SetActorFlags(actor, 0x80);
        SceneActorChase_GroupSetFlags(group, 4);
        if (!Golem_Arrive(object)) {
            if (!Golem_TryRoute(object, group->route, &actor->base.body.position,
                                &actor->base.tracking.target->position)) {
                Golem_InvokeRouteFallback(actor, group);
                return 0;
            }
        }
        SceneActorChase_SetTypedControl(actor, group->common.gate.control, 0x1A);
        Golem_SetTypedState(actor, 0, 0, 12);
        return 1;
    }
    if (!body->responseCount) {
        GolemReaction_LatchHitReact(object);
        if (!actor->base.tracking.target)
            return 0;
        actor->base.tracking.target = &((struct SceneActorChasePlayer *)gPlayers[0])->target;
        actor->base.body.direction = actor->base.tracking.heading;
        group->destination->pad = 1;
        if (!Golem_Arrive(object)) {
            if (!Golem_TryRoute(object, group->route, &actor->base.body.position,
                                &actor->base.tracking.target->position)) {
                Golem_InvokeRouteFallback(actor, group);
                return 0;
            }
        }
        actor->base.field2C2 = 0x40;
        if (body->responseCount <= 2)
            body->responseCount++;
        SceneActorChase_SetTypedControl(actor, SceneActorReaction_PlayAnimationBUntilComplete, 10);
        Golem_SetTypedState(actor, 0, 1, 1);
        SceneActorChase_SetActorFlags(actor, 0x10);
        return 1;
    }

    GolemReaction_LatchHitReact(object);
    if (!actor->base.tracking.target)
        return 0;
    actor->base.tracking.target = &((struct SceneActorChasePlayer *)gPlayers[0])->target;
    SceneActorChase_SetActorFlags(actor, 0x90);
    group->destination->pad = 0;
    if (!Golem_Arrive(object)) {
        if (!Golem_TryRoute(object, group->route, &actor->base.body.position,
                            &actor->base.tracking.target->position)) {
            Golem_InvokeRouteFallback(actor, group);
            return 0;
        }
    }
    GolemReaction_Settle(actor);
    return 1;
}

void Golem_UpdateScriptedAction(struct ChaseActor *obj)
{
    if (SceneActorControl_TakeFlag(((u8 *)obj) + 0x2B1)) {
        u16 *h;
        u16 value;
        s32 off;
        SceneActorChase_SetControl(obj, SceneActorReaction_PlayAnimationBUntilComplete, 10);
        {
            u32 byte = obj->base.tracking.heading;
            obj->base.body.direction = byte;
        }
        SceneActorAttachment_ReplaceResponseSlot((u8 *)obj, 0, 0, obj->base.category);
        SceneActorBehavior_CallActionScript((u8 *)obj);
        h = (u16 *)(((u8 *)obj) + 0x414);
        {
            u32 flag = 4;
            flag |= *h;
            *h = flag;
        }
        {
            u32 flag = 0x80;
            u16 *flags = (u16 *)(((u8 *)obj) + 0x2D4);
            flag |= *flags;
            *flags = flag;
        }
        off = 0x416;
        value = *((u16 *)(((u8 *)obj) + off));
        off += 2;
        *((u16 *)(((u8 *)obj) - (-off))) = value;
    }
    if (obj->base.stepDone) {
        Golem_ResetIdleControl((u8 *)obj);
    } else {
        ++(*((u32 *)(((u8 *)obj) + 0x2AC)));
    }
}

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

static inline u32 Golem_Height_ResumeDelayedRoute(struct ContactPosition *position)
{
    return SceneActorChase_HeightAt(Golem_CellAt(position), position);
}

static inline void Golem_SnapToRoute_ResumeDelayedRoute(struct ContactPosition *position,
                                                        struct SceneActorRegistryRoute *route)
{
    position->x = route->point->x;
    position->z = route->point->z;
    position->y = Golem_Height_ResumeDelayedRoute(position);
}

static inline void Golem_BeginTurn_ResumeDelayedRoute(void *obj, u32 saved,
                                                      const u16 *randomSamples)
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

static inline u8 Golem_Arrive_ResumeDelayedRoute(void *obj)
{
    struct ContactPosition target;
    struct ChaseActor *actor = obj;
    struct SceneActorChaseGroup *group = SceneActorChase_GroupOf(obj);
    struct SceneActorChaseTracking *tracking = &actor->base.tracking;
    s32 cell, player;
    const u16 *randomSamples = gRandomTable;
    void **callback;
    Golem_SnapToRoute_ResumeDelayedRoute(group->destination, &group->homeRoute);
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
        cell = ((struct SceneActorChasePlayer *)gPlayers[player])->cell->index;
    } else {
        struct SceneActorChaseTarget *t = tracking->target;
        cell = SceneActorChase_CellIndex((t)->position.x >> 8, (t)->position.z >> 8);
        player = 0;
    }
    if (cell != actor->base.terrainCell &&
        !SceneActorChase_Bits32(&gPlayers[player]->statusFlags, 0x20000))
        goto reroute;
    if (SceneActorChase_MotionFlags(actor, 0x400))
        return 0;
    SceneActorChase_ClearFlagComponent((void *)&group->common.gate.flags, 4);
    if (SceneActorChase_MotionFlags(actor, 0x80)) {
        SceneActorChase_SetControl(actor, group->common.gate.control, 0x1A);
        Golem_SetBehavior(actor, 12);
    } else {
        SceneActorChase_SetControl(actor, actor->base.onAlternateEntry, 1);
        Golem_SetBehavior(actor, 5);
        Golem_SnapToRoute_ResumeDelayedRoute(group->destination, &group->homeRoute);
        (group->destination)->y =
            SceneActorChase_HeightAt(Golem_CellAt(group->destination), group->destination);
        (group->destination)->pad = 0;
    }
    {
        u8 saved = actor->base.body.direction;
        Golem_BeginTurn_ResumeDelayedRoute(obj, saved, randomSamples);
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
    SceneActorControl_TakeStateEntry(actor);
    if ((u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque))
        return;
    if ((u8)sub_0812F3CC(actor))
        return;
    if ((s32)actor->base.counter2AC <= 60)
        goto waiting;
    SceneActorAttachment_ReplaceResponseSlot((u8 *)actor, 0, 2, actor->base.category);
    Golem_SetState(actor, 3, 1, 0);
    SceneActorChase_SetControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
    if (SceneActorChase_MotionFlags(actor, 0x80)) {
        SceneActorDelay_Prepare((void *)&group->saved, group->common.gate.control, 12, 30);
        Golem_Arrive_ResumeDelayedRoute(actor);
    } else {
        SceneActorChase_SetActorFlags(actor, 0x10);
        SceneActorDelay_Prepare((void *)&group->saved, actor->base.onAlternateEntry, 7, 30);
        Golem_SnapToRoute_ResumeDelayedRoute(group->destination, &group->homeRoute);
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

extern const u8 gUnk_08606134[];

void Camera_RequestShake(u32);

void Skeleton_TryTriggerAction(void *);

void Skeleton_ClearPendingFlag(void *);

void Golem_UpdateAttackProperties(void *);

static inline u8 Unavailable(struct Golem *a)
{
    if (a) {
        if (SceneActorContact_MotionFlags(&a->chase, 0x40) || SceneActorChase_StatusFlags(&a->chase, 0x20000) || a->chase.base.health <= 0)
            return 1;
    }
    return 0;
}

void Golem_InitActionHandlers(struct Golem *actor)
{
    actor->chase.base.alternateBehaviorCallbacks = (void **)gUnk_08606134;
}

s32 Golem_TickFacingQuadrant(struct Golem *a)
{
    struct SceneActorBody *body = &a->chase.base.body;
    struct Golem *state = a->chase.base.context;
    if (!Unavailable(a)) {
        if (state->countdown626)
            state->countdown626--;
    }
    {
        u32 quadrant = (body->direction + 0x60) >> 6;
        u32 mask = 3;
        mask &= quadrant;
        a->field68C = mask;
    }
    a->savedDirection690 = a->direction68E;
    return 1;
}

s32 Golem_UpdateEffectsAndContacts(struct Golem *actor)
{
    struct Golem *state = actor->chase.base.context;
    if (!(SceneActorChase_QueryMotionFlags(&actor->chase, 0x40))) {
        if (state->shakeTimer != 0) {
            Camera_RequestShake(state->shakeStrength);
            state->shakeTimer--;
        }
    }
    Skeleton_TryTriggerAction(actor);
    Skeleton_ClearPendingFlag(actor);
    Golem_UpdateAttackProperties(actor);
    return 1;
}
