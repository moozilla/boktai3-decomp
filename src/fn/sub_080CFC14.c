#include "global.h"
#include "render.h"
#include "engine_globals.h"
#include "display_sequence_inline.h"
#include "scene_actor_init_primitives.h"
#include "scene_actor_init_actor.h"

struct GolemResourceBlock {
    u32 w[8];
};

void *Heap_Alloc(u32);
void Memory_Clear(void *, u32);
void AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
void SceneActorAnimation_InitEnemySequence(void *, u32);
void *ScriptProc_Start(u32, u32);
void ResourceHeader_Relocate(void *, void *);
void Widget_InitLinked(void *, void *, s32, s32, s32, s32, s32, void *);
void SceneActorRegistry_SetObjectContext(void *, void *, s32, s32);
void sub_08080BE0(void);
void SceneActorReaction_SinkAndRemove(void);
void SceneActorReaction_BeginFadedState(void);
void SceneActorReaction_BeginDefeatBurst(void);
void sub_0807DB34(void);
void SceneActorReaction_RestoreAfterTurns(void);
void sub_0807FCA8(void);
void SceneActorReaction_TurnBackToRoute(void);
void sub_0807F130(void);
void SceneActorReaction_UpdateAnimationGate(void);
void sub_0808170C(void);
void SceneActorReaction_ResumeAfterAnimation(void);
void SceneActorReaction_TryResumeRoute(void);
void SceneActorReaction_BeginReactionSound(void);
u32 SceneActorReaction_OnEnemyDefault(void);
void sub_081363F4(void);
void SceneActorScript_OnCommand(void);
void SceneActorControl_OnIdle(void);
extern const u8 gUnk_086108B0[];
extern const u8 gUnk_08606400[];
s32 Div(s32, s32);
void SceneBody_Init(void *, s32, void *, s32, s32, void *);
void SceneBody_StartRoute(void *, void *, u32, u32, u32);
s32 Path_GetPointPositionById(u16 *, u32, u32);
u32 Route_GetCurrentPointFacing(void *);
s32 TargetRegistry_RegisterActor(void *, void *, void *);
void SceneBody_SetSprite(void *, void *);
void SceneBody_SetMovementProbe(void *, void *, s32, s32);
void AnimData_SelectPaletteBank(void *, s32);
s32 SceneActorScript_GetManagerField62(void);
void SceneActorPlacement_HideActor(void *, s32);
void SceneActorAnimation_SelectDirectionalEntry(void *, s32);
void Anim_Step(void *, void *);
void SceneActorBehavior_InstallCallbackTables(void *);
void SceneActorTerrain_ReadScriptParameters(void *);
u8 SceneActorRegistry_LinkActor(void *);
void *Region_FindContaining(void *);

extern u8 *gGameState;

struct GolemInitContext {
    u8 unknown0[0x62C];
    u32 rangeSquared62C;
};

struct GolemInstanceState {
    u8 unknown0[0x624];
    u8 field624;
    u8 unknown625[3];
    u32 field628;
    u8 unknown62C[0x54];
    u16 field680, field682, field684, field686;
    u16 unknown688;
    u16 field68A;
    u8 unknown68C;
    u8 field68D;
    u8 unknown68E[5];
    u8 field693;
};

static inline u8 *GolemInit_Drawable(struct SceneActorRegistryResetView *actor)
{
    return actor->drawable;
}
static inline u8 GolemInit_GetDrawableKind(struct SceneActorRegistryResetView *actor)
{
    return actor->drawableKind;
}
static inline void GolemInit_SetCategory(void *object, u8 category)
{
    struct SceneActorRegistryResetView *actor = object;
    actor->category = category;
}
static inline u8 GolemInit_GetCategory(void *object)
{
    struct SceneActorRegistryResetView *actor = object;
    return actor->category;
}

static inline u16 GolemInit_GetInitialHealth(struct SceneActorRegistryResetView *actor)
{
    return actor->defaultHealth;
}

static inline u32 GolemInit_GetPaletteBase(struct SceneActorRegistryResetView *actor)
{
    return actor->basePaletteBank;
}

static inline u32 GolemInit_GetPaletteOffset(struct SceneActorRegistryResetView *actor)
{
    return actor->paletteOffset;
}

void SceneActorBehavior_InitializeShadow(void *);
extern u8 *gUnknown_02000580;

void ProximityVolume_TranslateBounds(void *, void *);
void SceneActorMovement_ResolveRoutePosition(void *);
u8 sub_0807F81C(void *);
void SceneActorPlacement_ResetStateThree(void);
void SceneActorReaction_BeginTargetRecovery(void);
extern const u8 gUnk_086060FC[];

static inline u32 GolemInit_GetMotionFlags(struct SceneActorRegistryResetView *actor)
{
    return actor->flags2C0;
}
static inline u16 GolemInit_GetHeightOffset(struct SceneActorRegistryResetView *actor)
{
    return actor->field2EA;
}
static inline u32 GolemInit_GetBehaviorFlags(struct SceneActorRegistryResetView *actor)
{
    return actor->flags2CC;
}
static inline u32 GolemInit_GetStatusFlags(struct SceneActorRegistryResetView *actor)
{
    return actor->flags2D0;
}
static inline u8 GolemInit_GetState(struct SceneActorRegistryResetView *actor)
{
    return actor->state2A8;
}
static inline u8 GolemInit_GetPhase(struct SceneActorRegistryResetView *actor)
{
    return actor->phase2A9;
}
static inline u8 GolemInit_GetBehavior(struct SceneActorRegistryResetView *actor)
{
    return actor->behavior2AA;
}
static inline void *GolemInit_GetCurrentControl(struct SceneActorRegistryResetView *actor)
{
    return actor->target;
}
static inline u8 GolemInit_GetControlKind(struct SceneActorRegistryResetView *actor)
{
    return actor->field2B5;
}
static inline u8 GolemInit_GetDirection(struct SceneActorRegistryResetView *actor)
{
    return actor->body.direction;
}
static inline u32 GolemInit_GetTerrainCell(struct SceneActorRegistryResetView *actor)
{
    return actor->terrainCell;
}
static inline u8 GolemInit_GetTargetingFlags(struct SceneActorChaseTracking *target)
{
    return target->flags;
}
static inline struct SceneActorDefaultControl *
GolemInit_SavedControlOf(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorDefaultControl *saved = &actor->saved;
    return saved;
}
static inline struct ContactPosition *GolemInit_BodyPosition(void *object)
{
    struct SceneActorBody *body = object;
    struct ContactPosition *position = &body->position;
    return position;
}
static inline void GolemInit_SaveControl(struct SceneActorDefaultControl *saved,
                                         struct SceneActorRegistryResetView *actor)
{
    saved->motionFlags = GolemInit_GetMotionFlags(actor);
    saved->heightOffset = GolemInit_GetHeightOffset(actor);
    saved->behaviorFlags = GolemInit_GetBehaviorFlags(actor);
    saved->statusFlags = GolemInit_GetStatusFlags(actor);
    saved->direction = GolemInit_GetDirection(actor);
    saved->state = GolemInit_GetState(actor);
    saved->phase = GolemInit_GetPhase(actor);
    saved->behavior = GolemInit_GetBehavior(actor);
    saved->control = GolemInit_GetCurrentControl(actor);
    saved->kind = GolemInit_GetControlKind(actor);
}
static inline void GolemInit_ClearColliderFlags(struct ContactCollider *collider, u32 mask)
{
    collider->flags = mask & collider->flags;
}
static inline s32 GolemInit_Half(s32 value)
{
    if (value >= 0)
        return value >> 1;
    return -(-value >> 1);
}
static inline void GolemInit_SetBehaviorFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    actor->flags2CC |= mask;
}

static inline void GolemInit_SetStatusFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    actor->flags2D0 |= mask;
}

static inline u8 GolemInit_TestMotionFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    if (GolemInit_GetMotionFlags(actor) & mask)
        return 1;
    return 0;
}

static inline u8 GolemInit_TestTargetingFlags(struct SceneActorChaseTracking *target, u32 mask)
{
    if (GolemInit_GetTargetingFlags(target) & mask)
        return 1;
    return 0;
}
static inline void GolemInit_SetTargetingFlags(struct SceneActorChaseTracking *target, u8 mask)
{
    mask |= target->flags;
    target->flags = mask;
}
static inline void GolemInit_ClearControlFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    actor->flags114 &= mask;
}
static inline u8 GolemInit_IsInCurrentRegion(struct SceneActorRegistryResetView *actor)
{
    if (*(u32 *)(gGameState + 0x5A0) == actor->regionId)
        return 1;
    return 0;
}
static inline void *GolemInit_GetStandingControl(struct SceneActorRegistryResetView *actor)
{
    return actor->savedTarget;
}
static inline void GolemInit_SetControl(struct SceneActorRegistryResetView *actor, void *callback,
                                        u32 behavior)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = behavior;
    actor->target = callback;
    actor->field2C8 = 0;
    GolemInit_ClearControlFlags(actor, ~1);
    actor->word104 = 0;
}
static inline void GolemInit_SetState(struct SceneActorRegistryResetView *actor, u32 state,
                                      u32 phase, u32 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
static inline void GolemInit_ResetFlags(void *object)
{
    struct GolemInstanceState *actor = object;
    actor->field628 = 0;
}

static inline void GolemInit_InitializeGate(struct SceneActorControlGate *gate, u16 parameter)
{
    gate->flags = 0;
    gate->duration = parameter;
    gate->counter = 0;
    gate->field41A = 0;
    gate->control = 0;
}

static inline void GolemInit_ReadTargeting(struct SceneActorChaseTracking *target)
{
    s32 nearRadius, flags, farRadius;
    if (Script_SeekToKeyword(0x65)) {
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
    if (GolemInit_TestTargetingFlags(target, 4))
        target->range = target->limit;
    else
        target->range = Div(target->limit * 8 - target->limit, 10);
}

static inline u16 GolemInit_RangeRadius(struct SceneActorRegistryControl *range)
{
    return range->radius;
}
static inline void GolemInit_ReadRange(struct SceneActorRegistryControl *range)
{
    s32 radius;
    if (Script_SeekToKeyword(0x6E)) {
        radius = MonsterInit_ReadWord(0);
        range->parameter = MonsterInit_ReadHalfword(0);
    } else {
        radius = 0;
        range->parameter = radius;
    }
    range->radius = radius;
    range->radiusSquared = GolemInit_RangeRadius(range) * GolemInit_RangeRadius(range);
    range->mode = 0;
}

static inline u32 GolemInit_RoutePointX(struct SceneActorRegistryRoute *route)
{
    return (u16)route->point->x;
}
static inline u32 GolemInit_RoutePointZ(struct SceneActorRegistryRoute *route)
{
    return (u16)route->point->z;
}
static inline void GolemInit_SnapToRoute(struct ContactPosition *position,
                                         struct SceneActorRegistryRoute *route)
{
    position->x = GolemInit_RoutePointX(route);
    position->z = GolemInit_RoutePointZ(route);
    position->y = MonsterInit_TerrainHeight(MonsterInit_CellIndex(position), position);
}
static inline void GolemInit_DrawablePosition(struct SceneActorRegistryResetView *actor)
{
    if (GolemInit_GetDrawableKind(actor) == 0)
        *(struct ContactPosition *)(actor->drawable + 0x1C) = actor->body.position;
    else
        *(struct ContactPosition *)(actor->drawable + 0x40) = actor->body.position;
}
static inline void GolemInit_SetFirstAttack(struct SceneActorRegistryResetView *actor, s32 index,
                                            s32 value)
{
    actor->attacks.first[index] = value;
}

static inline u8 GolemInit_RouteId(struct SceneActorRegistryRoute *route)
{
    return route->routeId;
}
static inline u8 GolemInit_RouteParameter(struct SceneActorRegistryRoute *route)
{
    return route->parameterB;
}
static inline u8 GolemInit_RoutePointId(struct SceneActorRegistryRoute *route)
{
    return route->pointId;
}
static inline u8 GolemInit_RouteActiveBranch(struct SceneActorRegistryRoute *route)
{
    return route->activeBranch;
}
static inline u32 GolemInit_RouteAlternateBranch(struct SceneActorRegistryRoute *route)
{
    return 1 ^ route->activeBranch;
}
static inline void GolemInit_BeginRoute(struct SceneActorBody *body,
                                        struct SceneActorRegistryRoute *route)
{
    SceneBody_StartRoute(body, route, GolemInit_RouteId(route), GolemInit_RouteParameter(route),
                         GolemInit_RoutePointId(route));
}
static inline s32 GolemInit_ClampHealth(u16 raw)
{
    s32 health = (s16)raw;
    if (health <= 0)
        health = 0;
    return health;
}

static inline void GolemInit_InstallIdle(struct SceneActorRegistryResetView *actor, void *idle)
{
    actor->target = idle;
}

static inline void GolemInit_InitializeHealth(struct SceneActorRegistryResetView *arg0,
                                              u16 initialHealth, s32 firstLimit, s32 secondLimit)
{
    arg0->flags2D4 = 0;
    GolemInit_InstallIdle(arg0, SceneActorControl_OnIdle);
    arg0->field2B2 = 1;
    arg0->field2B0 = 0;
    arg0->health = GolemInit_ClampHealth(initialHealth);
    arg0->fieldAA = 0;
    if (secondLimit <= 0)
        secondLimit = firstLimit;
    arg0->attachmentLimit = firstLimit;
    arg0->fieldA8 = secondLimit;
    arg0->regionId = 0;
}

static inline void GolemInit_SetInitialHealth(struct SceneActorRegistryResetView *actor, u16 health)
{
    actor->defaultHealth = health;
}

static inline u32 GolemInit_ReadRoute(struct SceneActorRegistryResetView *arg0,
                                      struct SceneActorChaseGroup *group,
                                      struct SceneActorRegistryRoute *rt)
{
    struct ContactPosition point;
    s32 pointId;
    rt->activeBranch = 0;

    if (Script_SeekToKeyword(0x72)) {
        rt->routeId = MonsterInit_ReadByte(0);
        rt->parameterB = 0;
        pointId = MonsterInit_ReadWord(0);
        rt->pointId = pointId;
        rt->currentPoint = pointId;
        rt->branches[GolemInit_RouteActiveBranch(rt)] = rt->routeId;
        rt->branches[GolemInit_RouteAlternateBranch(rt)] =
            Script_GetPc() ? Script_GetValue() : GolemInit_RouteId(rt);
        Path_GetPointPositionById((u16 *)&point, GolemInit_RouteId(rt), GolemInit_RoutePointId(rt));
        arg0->terrainCell = MonsterInit_CellIndex(&point);
        point.y = arg0->groundHeight2BE =
            MonsterInit_TerrainHeight(GolemInit_GetTerrainCell(arg0), &point);
        GolemInit_BeginRoute(&arg0->body, rt);
        return 1;
    } else {
        rt->routeId = 0;
        rt->parameterB = 0;
        rt->pointId = 0;
        rt->currentPoint = 0;
        group->homeRoute.branches[GolemInit_RouteActiveBranch(rt)] = 0;
        group->homeRoute.branches[GolemInit_RouteAlternateBranch(rt)] = 0;
    }

    return 0;
}
static inline void GolemInit_InitializeBody(void *object, u32 *fill)
{
    struct SceneActorRegistryResetView *arg0 = object;
    struct SceneActorChaseGroup *group = &arg0->group;
    struct SceneActorBody *body = &arg0->body;
    struct ContactPosition origin;
    s32 mode, i, secondLimit, firstLimit, gateDuration, identity;
    struct SceneActorRegistryRoute *rt;
    arg0->flags2CC = 0;
    arg0->flags2D0 = 0;
    arg0->enabled = 0;
    arg0->field2E8 = 8;
    arg0->flags2C0 = Script_GetKeywordValue(0x4D, 0);
    arg0->field2C2 = 0x40;
    MonsterInit_ClearPosition(fill, &origin);
    mode = Script_GetKeywordValue(0x4E, 0);

    if (Script_SeekToKeyword(0x61)) {
        i = 0;
        while (1) {
            GolemInit_SetFirstAttack(arg0, i, MonsterInit_ReadHalfword(0x18));
            arg0->attacks.second[i] = MonsterInit_ReadHalfword(0x18);
            arg0->attacks.flags[i] = MonsterInit_ReadWord(0);
            arg0->attacks.parameter[i] = MonsterInit_ReadHalfword(0);
            i++;
            if (i > 3)
                break;
        }
    }

    if (Script_SeekToKeyword(0x6C)) {
        firstLimit = MonsterInit_ReadWord(0x12C);
        secondLimit = MonsterInit_ReadWord(0x12C);
        gateDuration = MonsterInit_ReadWord(0x12C);
    } else {
        firstLimit = 0;
        secondLimit = 0;
        gateDuration = 0;
    }
    GolemInit_InitializeGate(&group->common.gate, gateDuration);
    GolemInit_ReadRange(&group->common.control);
    GolemInit_ReadTargeting(&arg0->tracking);
    if (Script_GetKeywordValue(0x62, 0))
        GolemInit_SetStatusFlags(arg0, 0x200);
    arg0->field1F0 = MonsterInit_ReadKeywordHalfword(0x46, 0);
    arg0->field1EC = MonsterInit_ReadKeywordHalfword(0x52, 0);

    if (Script_SeekToKeyword(0x41)) {
        GolemInit_SetInitialHealth(arg0, MonsterInit_ReadHalfword(0));
        group->halfHealth = GolemInit_Half(GolemInit_GetInitialHealth(arg0));
        arg0->field9E = MonsterInit_ReadByte(0);
        arg0->field9F = MonsterInit_ReadByte(0);
        arg0->fieldA0 = MonsterInit_ReadByte(0);
        arg0->fieldA2 = MonsterInit_ReadHalfword(0);
        arg0->fieldA4 = MonsterInit_ReadHalfword(0);
    }

    GolemInit_InitializeHealth(arg0, GolemInit_GetInitialHealth(arg0), firstLimit, secondLimit);

    if (Script_SeekToKeyword(0x58)) {
        arg0->fieldAC = MonsterInit_ReadWord(0);
        arg0->fieldA1 = MonsterInit_ReadByte(0);
    }
    arg0->fieldB2 = Script_GetKeywordValue(0x4B, 2);
    identity = (s16)Script_GetKeywordValue(0x56, -1);
    if (identity < 0)
        identity = 0xFFFF;
    arg0->field9A = identity;
    if (Script_GetKeywordValue(0x48, 0))
        GolemInit_SetStatusFlags(arg0, 0x800);
    if (Script_GetKeywordValue(0x4C, 0))
        GolemInit_SetStatusFlags(arg0, 0x8000);

    SceneBody_Init(body, mode, &origin, 0, 4, arg0);
    rt = &group->homeRoute;
    if (GolemInit_ReadRoute(arg0, group, rt)) {
        GolemInit_SnapToRoute(&arg0->body.position, rt);
        GolemInit_DrawablePosition(arg0);
        arg0->body.direction = Route_GetCurrentPointFacing(rt);
    }

    if (GolemInit_TestMotionFlags(arg0, 0x200))
        TargetRegistry_RegisterActor((u8 *)group + 0x30, body, sub_08080BE0);
    SceneBody_SetSprite(body, GolemInit_GetDrawableKind(arg0) == 0 ? arg0->drawable : 0);
    if (GolemInit_GetCategory(arg0) == 0x15)
        SceneBody_SetMovementProbe(body, (u8 *)arg0 + 0x40, 0x3C, 0x3C);
    else
        SceneBody_SetMovementProbe(body, (u8 *)arg0 + 0x40, 0x40, 0x40);

    group->common.bodyId = arg0->body.id;
    group->common.bodyPosition = &arg0->body.position;
}

struct GolemInitWidget {
    u8 unknown0[0x3A];
    u16 paletteBank;
};
static inline void GolemInit_SetWidgetScriptPalette(struct GolemInitWidget *widget, s32 palette)
{
    widget->paletteBank = palette - 0x5FFF;
}

static inline void GolemInit_SetMotionControl(struct SceneActorRegistryResetView *actor,
                                              void *callback, u32 kind)
{
    actor->word104 = (u32)callback;
    actor->word108 = 0;
    actor->word10C = 0;
    actor->word110 = kind;
    actor->flags114 = 1;
}
static inline void GolemInit_ClearMotion(void *object)
{
    struct SceneActorRegistryResetView *actor = object;
    GolemInit_SetMotionControl(actor, 0, 0);
    actor->flags114 = 0;
}
static inline void GolemInit_ReadPalette(struct SceneActorRegistryResetView *actor, u32 base,
                                         u32 alternate)
{
    s32 palette;
    actor->paletteOffset = Script_GetKeywordValue(0x43, 0);
    actor->basePaletteBank = base;
    actor->paletteAlternateBank = alternate;
    actor->alternateOffset = GolemInit_GetPaletteOffset(actor) + base;
    palette = GolemInit_GetPaletteBase(actor) + GolemInit_GetPaletteOffset(actor);
    if (GolemInit_GetDrawableKind(actor) == 0)
        AnimData_SelectPaletteBank(actor->drawable + 0x2C, palette);
    else
        *(u8 **)(actor->drawable + 0x68) = gPaletteBankData + (palette << 5);
    if (GolemInit_GetDrawableKind(actor) == 1)
        GolemInit_SetWidgetScriptPalette((void *)(actor->drawable + 0x20),
                                         SceneActorScript_GetManagerField62());
}

static inline void GolemInit_InstallPrimary(struct SceneActorRegistryResetView *arg0,
                                            s32 (*cb0)(void *), u16 (*cb1)(void *),
                                            void (*cb2)(void *), void (*cb3)(void *),
                                            void (*cb4)(void *), void (*hidden)(void *),
                                            void (*cb5)(void *), void (*cb6)(void *),
                                            u32 (*defaultReaction)(void), u8 (*cb270)(void *),
                                            void *targetQuery, void *damage, void *contact)
{
    GolemInit_SetBehaviorFlags(arg0, 0x2000);
    MonsterInit_WriteContextUpdate(&arg0->contextCallback, cb0);
    MonsterInit_WriteSequenceQuery(&arg0->animationQuery.sequence, cb1);
    MonsterInit_WriteActorAction(&arg0->entryTarget224, cb2);
    MonsterInit_WriteActorAction(&arg0->entryTarget228, cb3);
    MonsterInit_WriteActorAction(&arg0->savedTarget, cb4);
    MonsterInit_WriteActorAction(&arg0->reactionTarget230, cb5);
    MonsterInit_WriteActorAction(&arg0->deathControl, cb6);
    MonsterInit_WriteActorAction(&arg0->field220, hidden);
    MonsterInit_WriteDefaultReaction(&arg0->defaultReaction, defaultReaction);
    MonsterInit_WriteContactPredicate(&arg0->contactQuery.predicate, cb270);
    MonsterInit_WriteDataPointer(&arg0->commandTable.opaque, gUnk_086108B0);
    arg0->callback248 = 0;
    arg0->update26C = targetQuery;
    arg0->field238 = SceneActorReaction_SinkAndRemove;
    arg0->damageCallback = damage;
    arg0->contactCallback = contact;
    arg0->field240 = 0;
    GolemInit_ClearMotion(arg0);
}

static inline void GolemInit_InitializeGroup(struct SceneActorChaseGroup *group, void *firstBounds,
                                             void *secondBounds, void *downed, void *returnToRoute,
                                             void *recover, void *animate,
                                             struct SceneActorBody *body)
{
    group->route = firstBounds;
    group->destination = secondBounds;
    group->routePhaseCallback = downed;
    group->fallback = recover;
    group->field5FC = SceneActorReaction_BeginFadedState;
    group->field600 = SceneActorReaction_BeginDefeatBurst;
    group->common.field43C = returnToRoute;
    group->common.field440 = 0;
    group->common.control.active = 1;
    group->interactionCallback = sub_0807DB34;
    group->field608 = 0;
    group->responseCallback = 0;
    group->field620 = 0;
    group->turnControl = SceneActorReaction_RestoreAfterTurns;
    group->field60C = sub_0807FCA8;
    group->field604 = (u32)animate;
    group->field614 = SceneActorReaction_TurnBackToRoute;
    group->routeChoiceControl = 0;
    group->common.fallback = sub_0807F130;
    group->common.gate.control = SceneActorReaction_UpdateAnimationGate;
    group->common.gate.field41A = 0x28;
    group->contactControl = sub_0808170C;
    group->field5E0 = SceneActorReaction_ResumeAfterAnimation;
    group->field5E4 = SceneActorReaction_TryResumeRoute;
    group->field610 = SceneActorReaction_BeginReactionSound;
    ProximityVolume_TranslateBounds(firstBounds, GolemInit_BodyPosition(body));
}

static inline void GolemInit_SetRouteReset(struct SceneActorRegistryResetView *actor,
                                           void *callback)
{
    actor->callback248 = callback;
}
static inline void GolemInit_SetReactionGate(struct SceneActorRegistryResetView *actor,
                                             void *callback)
{
    actor->field23C = callback;
}

void Golem_InitAnimationProfile(void *);
void Golem_InitColliders(void *);
void Golem_InitActionHandlers(void *);
void Golem_InitSpecialActionHandlers(void *);
s32 Golem_OnUpdate(void *);
u16 Skeleton_LookupAnimationEntry(void *);
void sub_080C5930(void *);
void Golem_UpdateIdleState(void *);
void Golem_UpdateIdle(void *);
void Golem_ResumeBehaviorAfterDisplacement(void *);
void Golem_UpdateDefeat(void *);
void Golem_UpdateRevivalReturn(void *);
void SceneActorTarget_ResolvePendingObject(void);
void Golem_UpdateDying(void);
void Golem_ResumeSavedTarget(void);
void Golem_OnUnusedState(void);
void Golem_ResetCombatState(void);
void SceneActorControl_RefreshMotionRoute(void);
u8 Golem_UpdateHitReaction(void *);
void Golem_ClearControlFlag(void);
void Golem_RestoreRoute(void);
void Golem_OnSharedActorUpdate(void);
void Golem_OnDestroy(void);

static inline void GolemInit_SetContactCallback(struct SceneActorRegistryResetView *actor,
                                                void *callback)
{
    actor->contactCallback = callback;
}

static inline void GolemInit_EnableInitialFlags(struct SceneActorRegistryResetView *actor)
{
    GolemInit_SetTargetingFlags(&actor->tracking, 1);
    GolemInit_SetBehaviorFlags(actor, 0x1600000);
    GolemInit_SetStatusFlags(actor, 0x8000008);
}

static inline void GolemInit_ReadInitialMode(struct SceneActorRegistryResetView *arg0,
                                             struct SceneActorChaseGroup *group)
{
    s32 i;
    i = Script_GetKeywordValue(0x75, 0);
    arg0->group.effect = 0x21;
    if (i != 0) {
        GolemInit_SetBehaviorFlags(arg0, 0x40000);
        if (i == 2)
            GolemInit_SetBehaviorFlags(arg0, 0x80000);
    }
}

static inline void GolemInit_InitializeRegion(struct SceneActorRegistryResetView *arg0)
{
    if (GolemInit_TestMotionFlags(arg0, 2))
        arg0->regionId = *(u32 *)(gGameState + 0x5A0);
    else
        arg0->regionId = (u16)(u32)Region_FindContaining(&arg0->body.position);
}

static inline struct GolemInitContext *GolemInit_Context(struct SceneActorRegistryResetView *actor)
{
    return actor->context;
}
static inline void GolemInit_InitializeReceiver(struct SceneActorRegistryResetView *actor)
{
    actor->field304 = 0;
    GolemInit_Context(actor)->rangeSquared62C = actor->tracking.range;
}

static inline void GolemInit_InstallCompletionCallbacks(struct SceneActorRegistryResetView *arg0)
{
    arg0->messageCallback = SceneActorScript_OnCommand;
    arg0->updateCallback = Golem_OnSharedActorUpdate;
    arg0->destroyCallback = Golem_OnDestroy;
}

static inline void GolemInit_InitializeSprite(u8 *drawable, u16 resource, u16 sequence)
{
    AnimData_Load(drawable + 0x2C, resource);
    Sprite_Init(drawable, drawable + 0x2C, 0);
    SceneActorAnimation_InitEnemySequence(drawable, sequence);
}

static inline void GolemInit_InitializeWidget(u8 *drawable, u16 script, u16 resource,
                                              struct ContactPosition *position)
{
    void *source = ScriptProc_Start(script, resource);
    *(struct GolemResourceBlock *)drawable = *(struct GolemResourceBlock *)source;
    ResourceHeader_Relocate(drawable, source);
    Widget_InitLinked(drawable + 0x20, drawable, 0, 0, 2, 2, 0x3C, position);
}

static inline void GolemInit_InitializePaletteRange(struct SceneActorRegistryResetView *actor,
                                                    u32 alternate, u32 current)
{
    actor->alternatePalette = GolemInit_GetPaletteBase(actor) + alternate;
    actor->paletteBank = GolemInit_GetPaletteBase(actor) + current;
}

s32 Golem_Init(void *object)
{
    struct SceneActorRegistryResetView *arg0 = object;
    struct SceneActorRegistryResetView *actor = object;
    struct SceneActorChaseGroup *group = &arg0->group;
    u32 zero;
    struct ContactPosition tmp;
    u32 flag;

    GolemInit_ResetFlags(arg0);
    SceneActorRegistry_SetObjectContext(actor, actor, 0, 1);
    GolemInit_SetCategory(actor, 3);
    Golem_InitAnimationProfile(actor);
    if (GolemInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->drawable, 0x5C);
        heap = GolemInit_Drawable(actor);
        GolemInit_InitializeSprite(heap, 0x5290, 0x1E38);
        if (GolemInit_GetCategory(actor) == 0x11 || GolemInit_GetCategory(actor) == 0x17)
            heap[7] = 1;
        actor->body.direction = 0;
    } else {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->drawable, 0x80);
        heap = GolemInit_Drawable(actor);
        MonsterInit_ClearPosition(&zero, &tmp);
        GolemInit_InitializeWidget(heap, 0xCB05, 0x5290, &tmp);
        actor->animationIndex = 0xFF;
    }
    GolemInit_InitializeBody(arg0, &zero);
    GolemInit_ReadPalette(actor, 0x27D, 0x192);
    GolemInit_InitializePaletteRange(actor, 2, 0);
    Golem_InitColliders(actor);
    GolemInit_InstallPrimary(
        actor, Golem_OnUpdate, Skeleton_LookupAnimationEntry, sub_080C5930, Golem_UpdateIdleState,
        Golem_UpdateIdle, Golem_UpdateRevivalReturn, Golem_ResumeBehaviorAfterDisplacement,
        Golem_UpdateDefeat, SceneActorReaction_OnEnemyDefault, sub_0807F81C,
        SceneActorTarget_ResolvePendingObject, sub_081363F4, SceneActorPlacement_ResetStateThree);
    GolemInit_InitializeGroup(group, (u8 *)arg0 + 0x638, (u8 *)arg0 + 0x65C, Golem_UpdateDying,
                              Golem_ResumeSavedTarget, SceneActorReaction_BeginTargetRecovery,
                              Golem_OnUnusedState, &actor->body);
    SceneActorMovement_ResolveRoutePosition(actor);
    GolemInit_SetRouteReset(actor, Golem_ResetCombatState);
    GolemInit_SetReactionGate(actor, SceneActorControl_RefreshMotionRoute);
    GolemInit_SetContactCallback(actor, Golem_UpdateHitReaction);
    group->responseCallback = (u32)Golem_ClearControlFlag;
    group->field5EC = Golem_RestoreRoute;
    actor->field2E8 = 0;
    GolemInit_EnableInitialFlags(actor);
    GolemInit_ClearColliderFlags((void *)((u8 *)actor + 0x134), ~1);
    SceneActorBehavior_InitializeShadow(actor);
    SceneActorTerrain_ReadScriptParameters(actor);
    GolemInit_ReadInitialMode(actor, group);
    flag = 0;
    if (GolemInit_TestMotionFlags(actor, 4)) {
        GolemInit_SetStatusFlags(actor, 0x2000);
        flag = 1;
    }
    if (flag != 1) {
        GolemInit_SetState(actor, 0, 0, 0);
        GolemInit_SetControl(actor, GolemInit_GetStandingControl(actor), 0);
    }
    GolemInit_InitializeRegion(actor);
    if (GolemInit_TestMotionFlags(actor, 0x40))
        GolemInit_ClearColliderFlags((void *)((u8 *)actor + 0x134), ~0x100);
    if (Script_GetKeywordValue(0x53, 0) != 0 || !GolemInit_IsInCurrentRegion(actor))
        SceneActorPlacement_HideActor(actor, 0);
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    if (GolemInit_GetDrawableKind(actor) == 0)
        DisplaySequence_Step(actor->drawable);
    else
        Anim_Step(actor->drawable + 0x20, actor->drawable);
    GolemInit_InitializeReceiver(actor);
    SceneActorBehavior_InstallCallbackTables(actor);
    Golem_InitActionHandlers(actor);
    Golem_InitSpecialActionHandlers(actor);

    {
        struct GolemInstanceState *instance = (void *)arg0;
        instance->field68D = 0;
        instance->field680 = 0x78;
        instance->field686 = 0;
        instance->field684 = 2;
        instance->field682 = 0;
        instance->field680 = 0xA0;
        instance->field68A = 0;
        instance->field624 = 0;
        instance->field693 = 0;
    }

    GolemInit_SaveControl(GolemInit_SavedControlOf(actor), actor);
    GolemInit_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}
