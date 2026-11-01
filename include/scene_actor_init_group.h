#ifndef GUARD_SCENE_ACTOR_INIT_GROUP_H
#define GUARD_SCENE_ACTOR_INIT_GROUP_H

#include "scene_actor_init_actor.h"

void SceneBody_StartRoute(void *, void *, u32, u32, u32);
s32 Div(s32, s32);
s32 Path_GetPointPositionById(u16 *, u32, u32);
void ProximityVolume_TranslateBounds(void *, struct ContactPosition *);
void SceneActorReaction_BeginFadedState(struct SceneActorRegistryResetView *);
void SceneActorReaction_BeginDefeatBurst(struct SceneActorRegistryResetView *);
void SceneActorRecovery_ResumePursuit(struct SceneActorRegistryResetView *);
void SceneActorReaction_RestoreAfterTurns(struct SceneActorRegistryResetView *, s32);
void sub_0807FCA8(struct SceneActorRegistryResetView *, s32);
void SceneActorReaction_TurnBackToRoute(struct SceneActorRegistryResetView *);
s32 SceneActorTarget_TryStimulusRoute(struct SceneActorRegistryResetView *);
void SceneActorReaction_UpdateAnimationGate(struct SceneActorRegistryResetView *);
void sub_0808170C(struct SceneActorRegistryResetView *);
void SceneActorReaction_ResumeAfterAnimation(struct SceneActorRegistryResetView *, s32);
void SceneActorReaction_TryResumeRoute(struct SceneActorRegistryResetView *, s32);
void SceneActorReaction_BeginReactionSound(struct SceneActorRegistryResetView *);

static inline u32 MonsterInit_GetTargetingFlags(struct SceneActorChaseTracking *target)
{
    return target->flags;
}

static inline u8 MonsterInit_TestTargetingFlags(struct SceneActorChaseTracking *target, u32 mask)
{
    if (MonsterInit_GetTargetingFlags(target) & mask)
        return 1;
    return 0;
}

static inline void MonsterInit_SetTargetingFlags(struct SceneActorChaseTracking *target, u8 mask)
{
    mask |= target->flags;
    target->flags = mask;
}

static inline void MonsterInit_ReadTargeting(struct SceneActorChaseTracking *target)
{
    s32 nearRadius, flags, farRadius;
    if (Script_SeekToKeyword('e')) {
        nearRadius = MonsterInit_ReadWord(0x30);
        flags = Script_GetPc() ? (u8)Script_GetValue() : 0x40;
        farRadius = nearRadius;
    } else {
        nearRadius = 0x30;
        farRadius = 0x30;
        flags = 0x40;
    }
    target->target = 0;
    target->x = nearRadius;
    target->z = farRadius;
    target->limit = nearRadius * nearRadius;
    target->zSquared = farRadius * farRadius;
    target->fieldEA = flags;
    if (MonsterInit_TestTargetingFlags(target, 4))
        target->range = target->limit;
    else
        target->range = Div(target->limit * 8 - target->limit, 10);
}

static inline void MonsterInit_InitializeGate(struct SceneActorControlGate *gate, u16 parameter)
{
    gate->flags = 0;
    gate->duration = parameter;
    gate->counter = 0;
    gate->field41A = 0;
    gate->control = 0;
}

static inline u32 MonsterInit_RangeRadius(struct SceneActorRegistryControl *range)
{
    return range->radius;
}

static inline void MonsterInit_ReadRange(struct SceneActorRegistryControl *range)
{
    s32 radius;
    if (Script_SeekToKeyword('n')) {
        radius = MonsterInit_ReadWord(0);
        range->parameter = MonsterInit_ReadHalfword(0);
    } else {
        radius = 0;
        range->parameter = radius;
    }
    range->radius = radius;
    range->radiusSquared = MonsterInit_RangeRadius(range) * MonsterInit_RangeRadius(range);
    range->mode = 0;
}

static inline u32 MonsterInit_RoutePointXRaw(struct SceneActorRegistryRoute *route)
{
    return (u16)route->point->x;
}

static inline u32 MonsterInit_RoutePointZRaw(struct SceneActorRegistryRoute *route)
{
    return (u16)route->point->z;
}

static inline void MonsterInit_SnapToRoute(struct ContactPosition *position,
                                           struct SceneActorRegistryRoute *route)
{
    position->x = MonsterInit_RoutePointXRaw(route);
    position->z = MonsterInit_RoutePointZRaw(route);
    position->y = MonsterInit_TerrainHeight(MonsterInit_CellIndex(position), position);
}

static inline u8 MonsterInit_RouteId(struct SceneActorRegistryRoute *route)
{
    return route->routeId;
}

static inline u8 MonsterInit_RouteParameter(struct SceneActorRegistryRoute *route)
{
    return route->parameterB;
}

static inline u8 MonsterInit_RoutePointId(struct SceneActorRegistryRoute *route)
{
    return route->pointId;
}

static inline u8 MonsterInit_RouteActiveBranch(struct SceneActorRegistryRoute *route)
{
    return route->activeBranch;
}

static inline u32 MonsterInit_RouteAlternateBranch(struct SceneActorRegistryRoute *route)
{
    return 1 ^ route->activeBranch;
}

static inline void MonsterInit_BeginRoute(struct SceneActorBody *body,
                                          struct SceneActorRegistryRoute *route)
{
    SceneBody_StartRoute(body, route, MonsterInit_RouteId(route), MonsterInit_RouteParameter(route),
                         MonsterInit_RoutePointId(route));
}

static inline u32 MonsterInit_ReadRoute(struct SceneActorRegistryResetView *arg0,
                                        struct SceneActorChaseGroup *group,
                                        struct SceneActorRegistryRoute *rt)
{
    struct ContactPosition point;
    s32 pointId;
    rt->activeBranch = 0;

    if (Script_SeekToKeyword('r')) {
        rt->routeId = MonsterInit_ReadByte(0);
        rt->parameterB = 0;
        pointId = MonsterInit_ReadWord(0);
        rt->pointId = pointId;
        rt->currentPoint = pointId;
        rt->branches[MonsterInit_RouteActiveBranch(rt)] = rt->routeId;
        rt->branches[MonsterInit_RouteAlternateBranch(rt)] =
            Script_GetPc() ? Script_GetValue() : MonsterInit_RouteId(rt);
        Path_GetPointPositionById((u16 *)&point, MonsterInit_RouteId(rt),
                                  MonsterInit_RoutePointId(rt));
        arg0->terrainCell = MonsterInit_CellIndex(&point);
        point.y = arg0->groundHeight2BE =
            MonsterInit_TerrainHeight(MonsterInit_GetTerrainCell(arg0), &point);
        MonsterInit_BeginRoute(&arg0->body, rt);
        return 1;
    } else {
        rt->routeId = 0;
        rt->parameterB = 0;
        rt->pointId = 0;
        rt->currentPoint = 0;
        group->homeRoute.branches[MonsterInit_RouteActiveBranch(rt)] = 0;
        group->homeRoute.branches[MonsterInit_RouteAlternateBranch(rt)] = 0;
    }

    return 0;
}

static inline void MonsterInit_InitializeGroup(struct SceneActorChaseGroup *group,
                                               void *firstBounds, void *secondBounds, void *downed,
                                               void *returnToRoute, void *recover, void *animate,
                                               u8 active)
{
    group->route = firstBounds;
    group->destination = secondBounds;
    group->routePhaseCallback = downed;
    group->fallback = recover;
    group->field5FC = SceneActorReaction_BeginFadedState;
    group->field600 = SceneActorReaction_BeginDefeatBurst;
    group->common.field43C = returnToRoute;
    group->common.field440 = 0;
    group->common.control.active = active;
    group->interactionCallback = SceneActorRecovery_ResumePursuit;
    group->field608 = 0;
    group->responseCallback = 0;
    group->field620 = 0;
    group->turnControl = SceneActorReaction_RestoreAfterTurns;
    group->field60C = sub_0807FCA8;
    group->field604 = (u32)animate;
    group->field614 = SceneActorReaction_TurnBackToRoute;
    group->routeChoiceControl = 0;
    group->common.fallback = SceneActorTarget_TryStimulusRoute;
    group->common.gate.control = SceneActorReaction_UpdateAnimationGate;
    group->common.gate.field41A = 0x28;
    group->contactControl = sub_0808170C;
    group->field5E0 = SceneActorReaction_ResumeAfterAnimation;
    group->field5E4 = SceneActorReaction_TryResumeRoute;
    group->field610 = SceneActorReaction_BeginReactionSound;
}

static inline void MonsterInit_InitializeGroupWithBounds(struct SceneActorChaseGroup *group,
                                                         void *firstBounds, void *secondBounds,
                                                         void *downed, void *returnToRoute,
                                                         void *recover, void *animate, u8 active,
                                                         struct SceneActorBody *body)
{
    MonsterInit_InitializeGroup(group, firstBounds, secondBounds, downed, returnToRoute, recover,
                                animate, active);
    ProximityVolume_TranslateBounds(firstBounds, MonsterInit_BodyPosition(body));
}

#endif
