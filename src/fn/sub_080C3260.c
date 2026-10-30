#include "global.h"
#include "render.h"
#include "engine_globals.h"
#include "display_sequence_inline.h"
#include "scene_actor_init_primitives.h"
#include "scene_actor_init_actor.h"

struct SkeletonResourceBlock {
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

struct SkeletonInstanceState {
    u8 unknown0[0x624];
    u8 unknown624[0xB8];
    u32 field6DC;
    u8 unknown6E0[0x38];
    u32 field718;
};

static inline u8 *SkeletonInit_Drawable(struct SceneActorRegistryResetView *actor)
{
    return actor->drawable;
}
static inline u8 SkeletonInit_GetDrawableKind(struct SceneActorRegistryResetView *actor)
{
    return actor->drawableKind;
}
static inline void SkeletonInit_SetCategory(struct SceneActorRegistryResetView *actor, u8 category)
{
    actor->category = category;
}
static inline u8 SkeletonInit_GetCategory(void *object)
{
    struct SceneActorRegistryResetView *actor = object;
    return actor->category;
}
static inline s32 SkeletonInit_GetHealth(struct SceneActorRegistryResetView *actor)
{
    return actor->health;
}
static inline u16 SkeletonInit_GetInitialHealth(struct SceneActorRegistryResetView *actor)
{
    return actor->defaultHealth;
}

static inline u32 SkeletonInit_GetPaletteBase(struct SceneActorRegistryResetView *actor)
{
    return actor->basePaletteBank;
}

static inline u32 SkeletonInit_GetPaletteOffset(struct SceneActorRegistryResetView *actor)
{
    return actor->paletteOffset;
}

void SceneActorBehavior_InitializeShadow(void *);
extern u8 *gUnknown_02000580;

void SceneActor_UpdateProfileForCategory(void *);
void SceneActorReaction_ParseOptions(void *);
void SceneActorScript_ReadPParameters(void *);
void SceneActorScript_ReadCallScriptConfig(void *);
void SceneActorControl_InitBehaviorColliders(void *);
void ProximityVolume_TranslateBounds(void *, void *);
void SceneActorMovement_ResolveRoutePosition(void *);
void SceneActorRegistry_RegisterCategoryReceiver(void *);
void Skeleton_InitVariantMotion(void *);
void Skeleton_SetMovementControlPreset(void *);
void Skeleton_SetAttackControlTable(void *);
void Skeleton_SetReactionControlTable(void *);
s32 Skeleton_Update(void *);
u16 SceneActorReaction_SelectTileParameter(void *);
void sub_080BAE04(void *);
void Skeleton_UpdateInactiveState(void *);
void Skeleton_UpdateIdleState(void *);
void Skeleton_ConvergeDisplacedPosition(void *);
void Skeleton_UpdateDefeat(void *);
u8 sub_0807F81C(void *);
void TentacleBody_UpdateReactionTransition(void *);
void SceneActorTarget_ResolvePendingObjectSkeleton(void);
void SceneActorPlacement_ResetStateThree(void);
void Skeleton_UpdateDownedState(void);
void SceneActorReaction_ReturnToRouteEntry(void);
void SceneActorReaction_BeginTargetRecovery(void);
void Skeleton_ResetRouteReaction(void);
void SceneActorReaction_UpdateGate(void);
void Skeleton_RestorePursuitRoute(void);
u8 Skeleton_ReactToContact(void *);
void SceneActorReaction_SinkThenDefeat(void);
void Skeleton_BeginKnockedFlight(void);
void SceneActorReaction_TickHiddenTimeout(void);
void Skeleton_UpdateBehaviorCallbacks(void);
void Skeleton_Destroy(void);
extern const u8 gUnk_086060FC[];

static inline u32 SkeletonInit_GetMotionFlags(struct SceneActorRegistryResetView *actor)
{
    return actor->flags2C0;
}
static inline u16 SkeletonInit_GetHeightOffset(struct SceneActorRegistryResetView *actor)
{
    return actor->field2EA;
}
static inline u32 SkeletonInit_GetBehaviorFlags(struct SceneActorRegistryResetView *actor)
{
    return actor->flags2CC;
}
static inline u32 SkeletonInit_GetStatusFlags(struct SceneActorRegistryResetView *actor)
{
    return actor->flags2D0;
}
static inline u8 SkeletonInit_GetState(struct SceneActorRegistryResetView *actor)
{
    return actor->state2A8;
}
static inline u8 SkeletonInit_GetPhase(struct SceneActorRegistryResetView *actor)
{
    return actor->phase2A9;
}
static inline u8 SkeletonInit_GetBehavior(struct SceneActorRegistryResetView *actor)
{
    return actor->behavior2AA;
}
static inline void *SkeletonInit_GetCurrentControl(struct SceneActorRegistryResetView *actor)
{
    return actor->target;
}
static inline u8 SkeletonInit_GetControlKind(struct SceneActorRegistryResetView *actor)
{
    return actor->field2B5;
}
static inline u8 SkeletonInit_GetDirection(struct SceneActorRegistryResetView *actor)
{
    return actor->body.direction;
}
static inline u32 SkeletonInit_GetTerrainCell(struct SceneActorRegistryResetView *actor)
{
    return actor->terrainCell;
}
static inline u8 SkeletonInit_GetTargetingFlags(struct SceneActorChaseTracking *target)
{
    return target->flags;
}
static inline struct SceneActorDefaultControl *
SkeletonInit_SavedControlOf(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorDefaultControl *saved = &actor->saved;
    return saved;
}
static inline struct ContactPosition *SkeletonInit_BodyPosition(void *object)
{
    struct SceneActorBody *body = object;
    struct ContactPosition *position = &body->position;
    return position;
}
static inline void SkeletonInit_SaveControl(struct SceneActorDefaultControl *saved,
                                            struct SceneActorRegistryResetView *actor)
{
    saved->motionFlags = SkeletonInit_GetMotionFlags(actor);
    saved->heightOffset = SkeletonInit_GetHeightOffset(actor);
    saved->behaviorFlags = SkeletonInit_GetBehaviorFlags(actor);
    saved->statusFlags = SkeletonInit_GetStatusFlags(actor);
    saved->direction = SkeletonInit_GetDirection(actor);
    saved->state = SkeletonInit_GetState(actor);
    saved->phase = SkeletonInit_GetPhase(actor);
    saved->behavior = SkeletonInit_GetBehavior(actor);
    saved->control = SkeletonInit_GetCurrentControl(actor);
    saved->kind = SkeletonInit_GetControlKind(actor);
}
static inline void SkeletonInit_ClearColliderFlags(struct ContactCollider *collider, u32 mask)
{
    collider->flags = mask & collider->flags;
}
static inline s32 SkeletonInit_Half(s32 value)
{
    if (value >= 0)
        return value >> 1;
    return -(-value >> 1);
}
static inline void SkeletonInit_SetBehaviorFlags(struct SceneActorRegistryResetView *actor,
                                                 u32 mask)
{
    actor->flags2CC |= mask;
}

static inline void SkeletonInit_SetStatusFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    actor->flags2D0 |= mask;
}

static inline void SkeletonInit_SetMotionFlags(struct SceneActorRegistryResetView *actor, u16 mask)
{
    actor->flags2C0 |= mask;
}
static inline u8 SkeletonInit_TestMotionFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    if (SkeletonInit_GetMotionFlags(actor) & mask)
        return 1;
    return 0;
}

static inline u8 SkeletonInit_TestSkeletonFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    if (((struct SkeletonInstanceState *)actor)->field6DC & mask)
        return 1;
    return 0;
}
static inline u8 SkeletonInit_TestTargetingFlags(struct SceneActorChaseTracking *target, u32 mask)
{
    if (SkeletonInit_GetTargetingFlags(target) & mask)
        return 1;
    return 0;
}
static inline void SkeletonInit_SetTargetingFlags(struct SceneActorChaseTracking *target, u8 mask)
{
    mask |= target->flags;
    target->flags = mask;
}
static inline void SkeletonInit_ClearControlFlags(struct SceneActorRegistryResetView *actor,
                                                  u32 mask)
{
    actor->flags114 &= mask;
}
static inline u8 SkeletonInit_IsInCurrentRegion(struct SceneActorRegistryResetView *actor)
{
    if (*(u32 *)(gGameState + 0x5A0) == actor->regionId)
        return 1;
    return 0;
}
static inline void *SkeletonInit_GetStandingControl(struct SceneActorRegistryResetView *actor)
{
    return actor->savedTarget;
}
static inline void SkeletonInit_SetControl(struct SceneActorRegistryResetView *actor,
                                           void *callback, u32 behavior)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = behavior;
    actor->target = callback;
    actor->field2C8 = 0;
    SkeletonInit_ClearControlFlags(actor, ~1);
    actor->word104 = 0;
}
static inline void SkeletonInit_SetState(struct SceneActorRegistryResetView *actor, u32 state,
                                         u32 phase, u32 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
static inline void SkeletonInit_ResetFlags(void *object)
{
    struct SkeletonInstanceState *actor = object;
    actor->field6DC = 0;
}
static inline void SkeletonInit_SetFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    ((struct SkeletonInstanceState *)actor)->field6DC |= mask;
}
static inline void SkeletonInit_InitializeGate(struct SceneActorControlGate *gate, u32 parameter)
{
    gate->flags = 0;
    gate->duration = parameter;
    gate->counter = 0;
    gate->field41A = 0;
    gate->control = 0;
}

static inline void SkeletonInit_ReadTargeting(struct SceneActorChaseTracking *target)
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
    if (SkeletonInit_TestTargetingFlags(target, 4))
        target->range = target->limit;
    else
        target->range = Div(target->limit * 8 - target->limit, 10);
}

static inline u16 SkeletonInit_RangeRadius(struct SceneActorRegistryControl *range)
{
    return range->radius;
}
static inline void SkeletonInit_ReadRange(struct SceneActorRegistryControl *range)
{
    s32 radius;
    if (Script_SeekToKeyword(0x6E)) {
        radius = MonsterInit_ReadWord(0);
        range->parameter = MonsterInit_ReadWord(0);
    } else {
        radius = 0;
        range->parameter = radius;
    }
    range->radius = radius;
    range->radiusSquared = SkeletonInit_RangeRadius(range) * SkeletonInit_RangeRadius(range);
    range->mode = 0;
}

static inline u32 SkeletonInit_RoutePointX(struct SceneActorRegistryRoute *route)
{
    return (u16)route->point->x;
}
static inline u32 SkeletonInit_RoutePointZ(struct SceneActorRegistryRoute *route)
{
    return (u16)route->point->z;
}
static inline void SkeletonInit_SnapToRoute(struct ContactPosition *position,
                                            struct SceneActorRegistryRoute *route)
{
    position->x = SkeletonInit_RoutePointX(route);
    position->z = SkeletonInit_RoutePointZ(route);
    position->y = MonsterInit_TerrainHeight(MonsterInit_CellIndex(position), position);
}
static inline void SkeletonInit_DrawablePosition(struct SceneActorRegistryResetView *actor)
{
    if (SkeletonInit_GetDrawableKind(actor) == 0)
        *(struct ContactPosition *)(actor->drawable + 0x1C) = actor->body.position;
    else
        *(struct ContactPosition *)(actor->drawable + 0x40) = actor->body.position;
}
static inline void SkeletonInit_SetFirstAttack(struct SceneActorRegistryResetView *actor, s32 index,
                                               s32 value)
{
    actor->attacks.first[index] = value;
}

static inline u8 SkeletonInit_RouteId(struct SceneActorRegistryRoute *route)
{
    return route->routeId;
}
static inline u8 SkeletonInit_RouteParameter(struct SceneActorRegistryRoute *route)
{
    return route->parameterB;
}
static inline u8 SkeletonInit_RoutePointId(struct SceneActorRegistryRoute *route)
{
    return route->pointId;
}
static inline u8 SkeletonInit_RouteActiveBranch(struct SceneActorRegistryRoute *route)
{
    return route->activeBranch;
}
static inline u32 SkeletonInit_RouteAlternateBranch(struct SceneActorRegistryRoute *route)
{
    return 1 ^ route->activeBranch;
}
static inline void SkeletonInit_BeginRoute(struct SceneActorBody *body,
                                           struct SceneActorRegistryRoute *route)
{
    SceneBody_StartRoute(body, route, SkeletonInit_RouteId(route),
                         SkeletonInit_RouteParameter(route), SkeletonInit_RoutePointId(route));
}
static inline s32 SkeletonInit_ClampHealth(u16 raw)
{
    s32 health = (s16)raw;
    if (health <= 0)
        health = 0;
    return health;
}

static inline void SkeletonInit_InstallIdle(struct SceneActorRegistryResetView *actor, void *idle)
{
    actor->target = idle;
}

static inline void SkeletonInit_InitializeHealth(struct SceneActorRegistryResetView *arg0,
                                                 u16 initialHealth, s32 firstLimit, s32 secondLimit)
{
    arg0->flags2D4 = 0;
    SkeletonInit_InstallIdle(arg0, SceneActorControl_OnIdle);
    arg0->field2B2 = 1;
    arg0->field2B0 = 0;
    arg0->health = SkeletonInit_ClampHealth(initialHealth);
    arg0->fieldAA = 0;
    if (secondLimit <= 0)
        secondLimit = firstLimit;
    arg0->attachmentLimit = firstLimit;
    arg0->fieldA8 = secondLimit;
    arg0->regionId = 0;
}

static inline void SkeletonInit_SetInitialHealth(struct SceneActorRegistryResetView *actor,
                                                 u16 health)
{
    actor->defaultHealth = health;
}

static inline u32 SkeletonInit_ReadRoute(struct SceneActorRegistryResetView *arg0,
                                         struct SceneActorChaseGroup *group,
                                         struct SceneActorRegistryRoute *rt)
{
    struct ContactPosition point;
    s32 pointId;
    rt->activeBranch = 0;

    if (Script_SeekToKeyword(0x72)) {
        rt->routeId = MonsterInit_ReadWord(0);
        rt->parameterB = 0;
        pointId = MonsterInit_ReadWord(0);
        rt->pointId = pointId;
        rt->currentPoint = pointId;
        rt->branches[SkeletonInit_RouteActiveBranch(rt)] = rt->routeId;
        rt->branches[SkeletonInit_RouteAlternateBranch(rt)] =
            Script_GetPc() ? Script_GetValue() : SkeletonInit_RouteId(rt);
        Path_GetPointPositionById((u16 *)&point, SkeletonInit_RouteId(rt),
                                  SkeletonInit_RoutePointId(rt));
        arg0->terrainCell = MonsterInit_CellIndex(&point);
        point.y = arg0->groundHeight2BE =
            MonsterInit_TerrainHeight(SkeletonInit_GetTerrainCell(arg0), &point);
        SkeletonInit_BeginRoute(&arg0->body, rt);
        return 1;
    } else {
        rt->routeId = 0;
        rt->parameterB = 0;
        rt->pointId = 0;
        rt->currentPoint = 0;
        group->homeRoute.branches[SkeletonInit_RouteActiveBranch(rt)] = 0;
        group->homeRoute.branches[SkeletonInit_RouteAlternateBranch(rt)] = 0;
    }

    return 0;
}
static inline void SkeletonInit_InitializeBody(void *object, u32 *fill)
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
            SkeletonInit_SetFirstAttack(arg0, i, MonsterInit_ReadHalfword(0x18));
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
    SkeletonInit_InitializeGate(&group->common.gate, gateDuration);
    SkeletonInit_ReadRange(&group->common.control);
    SkeletonInit_ReadTargeting(&arg0->tracking);
    if (Script_GetKeywordValue(0x62, 0))
        SkeletonInit_SetStatusFlags(arg0, 0x200);
    arg0->field1F0 = MonsterInit_ReadKeywordHalfword(0x46, 0);
    arg0->field1EC = MonsterInit_ReadKeywordHalfword(0x52, 0);

    if (Script_SeekToKeyword(0x41)) {
        SkeletonInit_SetInitialHealth(arg0, MonsterInit_ReadHalfword(0));
        group->halfHealth = SkeletonInit_Half(SkeletonInit_GetInitialHealth(arg0));
        arg0->field9E = MonsterInit_ReadByte(0);
        arg0->field9F = MonsterInit_ReadByte(0);
        arg0->fieldA0 = MonsterInit_ReadByte(0);
        arg0->fieldA2 = MonsterInit_ReadHalfword(0);
        arg0->fieldA4 = MonsterInit_ReadHalfword(0);
    }

    SkeletonInit_InitializeHealth(arg0, SkeletonInit_GetInitialHealth(arg0), firstLimit,
                                  secondLimit);

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
        SkeletonInit_SetStatusFlags(arg0, 0x800);
    if (Script_GetKeywordValue(0x4C, 0))
        SkeletonInit_SetStatusFlags(arg0, 0x8000);

    SceneBody_Init(body, mode, &origin, 0, 4, arg0);
    rt = &group->homeRoute;
    if (SkeletonInit_ReadRoute(arg0, group, rt)) {
        SkeletonInit_SnapToRoute(&arg0->body.position, rt);
        SkeletonInit_DrawablePosition(arg0);
        arg0->body.direction = Route_GetCurrentPointFacing(rt);
    }

    if (SkeletonInit_TestMotionFlags(arg0, 0x200))
        TargetRegistry_RegisterActor((u8 *)group + 0x30, body, sub_08080BE0);
    SceneBody_SetSprite(body, SkeletonInit_GetDrawableKind(arg0) == 0 ? arg0->drawable : 0);
    if (SkeletonInit_GetCategory(arg0) == 0x15)
        SceneBody_SetMovementProbe(body, (u8 *)arg0 + 0x40, 0x3C, 0x3C);
    else
        SceneBody_SetMovementProbe(body, (u8 *)arg0 + 0x40, 0x40, 0x40);

    group->common.bodyId = arg0->body.id;
    group->common.bodyPosition = &arg0->body.position;
}

struct SkelWidget {
    u8 unknown0[0x3A];
    u16 paletteBank;
};
static inline void SkeletonInit_SetWidgetScriptPalette(struct SkelWidget *widget, s32 palette)
{
    widget->paletteBank = palette - 0x5FFF;
}
static inline void SkeletonInit_ReadWidgetRelativePalette(struct SkelWidget *widget)
{
    widget->paletteBank = Script_GetPc() ? Script_GetValue() + 0x12B : 0x12B;
}
static inline void SkeletonInit_SetMotionControl(struct SceneActorRegistryResetView *actor,
                                                 void *callback, u32 kind)
{
    actor->word104 = (u32)callback;
    actor->word108 = 0;
    actor->word10C = 0;
    actor->word110 = kind;
    actor->flags114 = 1;
}
static inline void SkeletonInit_ClearMotion(void *object)
{
    struct SceneActorRegistryResetView *actor = object;
    SkeletonInit_SetMotionControl(actor, 0, 0);
    actor->flags114 = 0;
}
static inline void SkeletonInit_ReadPalette(struct SceneActorRegistryResetView *actor, u32 base,
                                            u32 alternate)
{
    s32 palette;
    actor->paletteOffset = Script_GetKeywordValue(0x43, 0);
    actor->basePaletteBank = base;
    actor->paletteAlternateBank = alternate;
    actor->alternateOffset = SkeletonInit_GetPaletteOffset(actor) + base;
    palette = SkeletonInit_GetPaletteBase(actor) + SkeletonInit_GetPaletteOffset(actor);
    if (SkeletonInit_GetDrawableKind(actor) == 0)
        AnimData_SelectPaletteBank(actor->drawable + 0x2C, palette);
    else
        *(u8 **)(actor->drawable + 0x68) = gPaletteBankData + (palette << 5);
    if (SkeletonInit_GetDrawableKind(actor) == 1)
        SkeletonInit_SetWidgetScriptPalette((void *)(actor->drawable + 0x20),
                                            SceneActorScript_GetManagerField62());
}

static inline void SkeletonInit_InstallPrimary(struct SceneActorRegistryResetView *arg0,
                                               s32 (*cb0)(void *), u16 (*cb1)(void *),
                                               void (*cb2)(void *), void (*cb3)(void *),
                                               void (*cb4)(void *), void (*hidden)(void *),
                                               void (*cb5)(void *), void (*cb6)(void *),
                                               u32 (*defaultReaction)(void), u8 (*cb270)(void *),
                                               void *targetQuery, void *damage, void *contact)
{
    SkeletonInit_SetBehaviorFlags(arg0, 0x2000);
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
    SkeletonInit_ClearMotion(arg0);
}

static inline void SkeletonInit_InitializeGroup(struct SceneActorChaseGroup *group,
                                                void *firstBounds, void *secondBounds, void *downed,
                                                void *returnToRoute, void *recover, void *animate,
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
    ProximityVolume_TranslateBounds(firstBounds, SkeletonInit_BodyPosition(body));
}

static inline void SkeletonInit_SetRouteReset(struct SceneActorRegistryResetView *actor,
                                              void *callback)
{
    actor->callback248 = callback;
}
static inline void SkeletonInit_SetReactionGate(struct SceneActorRegistryResetView *actor,
                                                void *callback)
{
    actor->field23C = callback;
}

static inline void SkeletonInit_InstallContactDefaults(struct SceneActorRegistryResetView *arg0)
{
    arg0->contactCallback = Skeleton_ReactToContact;
    MonsterInit_WriteDataPointer(&arg0->commandTable.opaque, gUnk_086060FC);
    arg0->field238 = SceneActorReaction_SinkThenDefeat;
}

static inline void SkeletonInit_EnableInitialFlags(struct SceneActorRegistryResetView *arg0)
{
    SkeletonInit_SetTargetingFlags(&arg0->tracking, 1);
    SkeletonInit_SetBehaviorFlags(arg0, 0x1600000);
    SkeletonInit_SetStatusFlags(arg0, 0x8000008);
    SkeletonInit_SetMotionFlags(arg0, 0x80);
}

static inline void SkeletonInit_ReadInitialMode(struct SceneActorRegistryResetView *arg0,
                                                struct SceneActorChaseGroup *group)
{
    s32 i;
    i = Script_GetKeywordValue(0x75, 0);
    arg0->group.effect = 0x21;
    if (i != 0) {
        SkeletonInit_SetBehaviorFlags(arg0, 0x40000);
        if (i == 2)
            SkeletonInit_SetBehaviorFlags(arg0, 0x80000);
    }
}

static inline void SkeletonInit_InitializeRegion(struct SceneActorRegistryResetView *arg0)
{
    if (SkeletonInit_TestMotionFlags(arg0, 2))
        arg0->regionId = *(u32 *)(gGameState + 0x5A0);
    else
        arg0->regionId = (u16)(u32)Region_FindContaining(&arg0->body.position);
}

static inline void SkeletonInit_InitializeReceiver(struct SceneActorRegistryResetView *arg0)
{
    arg0->field304 = 0;
    SceneActorRegistry_RegisterCategoryReceiver(arg0);
}

static inline void SkeletonInit_InstallCompletionCallbacks(struct SceneActorRegistryResetView *arg0)
{
    arg0->messageCallback = SceneActorScript_OnCommand;
    arg0->updateCallback = Skeleton_UpdateBehaviorCallbacks;
    arg0->destroyCallback = Skeleton_Destroy;
}

static inline void SkeletonInit_InitializeSprite(u8 *drawable, u16 resource, u16 sequence)
{
    AnimData_Load(drawable + 0x2C, resource);
    Sprite_Init(drawable, drawable + 0x2C, 0);
    SceneActorAnimation_InitEnemySequence(drawable, sequence);
}

static inline void SkeletonInit_InitializeWidget(u8 *drawable, u16 script, u16 resource,
                                                 struct ContactPosition *position)
{
    void *source = ScriptProc_Start(script, resource);
    *(struct SkeletonResourceBlock *)drawable = *(struct SkeletonResourceBlock *)source;
    ResourceHeader_Relocate(drawable, source);
    Widget_InitLinked(drawable + 0x20, drawable, 0, 0, 2, 2, 0x3C, position);
}
static inline void SkeletonInit_SetFlagWord(u32 mask, u32 *field)
{
    *field |= mask;
}

s32 Skeleton_Init(void *object)
{
    struct SceneActorRegistryResetView *arg0 = object;
    struct SceneActorRegistryResetView *actor = object;
    struct SceneActorChaseGroup *group = &arg0->group;
    struct SceneActorBody *body = &arg0->body;
    u32 zero;
    struct ContactPosition tmp;
    u32 flag;

    SkeletonInit_ResetFlags(arg0);
    SceneActorRegistry_SetObjectContext(actor, actor, 1, 1);
    {
        SkeletonInit_SetCategory(actor, Script_GetKeywordValue(0x6B, 5));
        if (SkeletonInit_GetCategory(actor) == 7)
            SkeletonInit_SetFlagWord(0x40, &((struct SkeletonInstanceState *)arg0)->field6DC);
    }
    SceneActor_UpdateProfileForCategory(actor);

    if (SkeletonInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->drawable, 0x5C);
        heap = SkeletonInit_Drawable(actor);
        SkeletonInit_InitializeSprite(heap, 0xAE52, 0);
        if (SkeletonInit_GetCategory(actor) == 0x11 || SkeletonInit_GetCategory(actor) == 0x17)
            heap[7] = 1;
        actor->body.direction = 0;
    } else {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->drawable, 0x80);
        heap = SkeletonInit_Drawable(actor);
        MonsterInit_ClearPosition(&zero, &tmp);
        SkeletonInit_InitializeWidget(heap, 0xCB05, 0xAE52, &tmp);
        actor->animationIndex = 0xFF;
    }

    SkeletonInit_InitializeBody(arg0, &zero);
    SceneActorReaction_ParseOptions(actor);
    SceneActorScript_ReadPParameters(actor);
    SceneActorScript_ReadCallScriptConfig(actor);

    SkeletonInit_ReadPalette(actor, 0x13C, 0x146);
    SceneActorControl_InitBehaviorColliders(actor);

    SkeletonInit_InstallPrimary(actor, Skeleton_Update, SceneActorReaction_SelectTileParameter,
                                sub_080BAE04, Skeleton_UpdateInactiveState,
                                Skeleton_UpdateIdleState, TentacleBody_UpdateReactionTransition,
                                Skeleton_ConvergeDisplacedPosition, Skeleton_UpdateDefeat,
                                SceneActorReaction_OnEnemyDefault, sub_0807F81C,
                                SceneActorTarget_ResolvePendingObjectSkeleton, sub_081363F4,
                                SceneActorPlacement_ResetStateThree);
    SkeletonInit_InitializeGroup(group, (u8 *)arg0 + 0x71C, (u8 *)arg0 + 0x740,
                                 Skeleton_UpdateDownedState, SceneActorReaction_ReturnToRouteEntry,
                                 SceneActorReaction_BeginTargetRecovery, 0, body);
    SceneActorMovement_ResolveRoutePosition(arg0);
    SkeletonInit_SetRouteReset(actor, Skeleton_ResetRouteReaction);
    group->responseCallback = 0;
    SkeletonInit_SetReactionGate(actor, SceneActorReaction_UpdateGate);
    group->field5EC = Skeleton_RestorePursuitRoute;
    SkeletonInit_InstallContactDefaults(actor);
    actor->field2E8 = 0;
    ((struct SkeletonInstanceState *)arg0)->field718 = (s16)SkeletonInit_GetHealth(actor) >> 1;
    SkeletonInit_EnableInitialFlags(actor);
    SkeletonInit_ClearColliderFlags((void *)((u8 *)actor + 0x134), ~1);
    SceneActorBehavior_InitializeShadow(actor);
    SceneActorTerrain_ReadScriptParameters(actor);
    SkeletonInit_ReadInitialMode(actor, group);
    flag = 0;
    if (SkeletonInit_TestMotionFlags(actor, 4)) {
        SkeletonInit_SetStatusFlags(actor, 0x2000);
        flag = 1;
    }

    if (flag != 1) {
        SkeletonInit_SetState(actor, 0, 0, 0);
        SkeletonInit_SetControl(actor, SkeletonInit_GetStandingControl(actor), 0);
    }

    SkeletonInit_InitializeRegion(actor);
    if (SkeletonInit_TestMotionFlags(actor, 0x40))
        SkeletonInit_ClearColliderFlags((void *)((u8 *)actor + 0x134), ~0x100);
    if (Script_GetKeywordValue(0x53, 0) != 0 || !SkeletonInit_IsInCurrentRegion(actor))
        SceneActorPlacement_HideActor(actor, 0);

    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    if (SkeletonInit_GetDrawableKind(actor) == 0)
        DisplaySequence_Step(actor->drawable);
    else
        Anim_Step(actor->drawable + 0x20, actor->drawable);

    SkeletonInit_InitializeReceiver(actor);
    Skeleton_InitVariantMotion(actor);
    SceneActorBehavior_InstallCallbackTables(actor);
    Skeleton_SetMovementControlPreset(actor);
    Skeleton_SetAttackControlTable(actor);
    Skeleton_SetReactionControlTable(actor);

    SkeletonInit_SaveControl(SkeletonInit_SavedControlOf(actor), actor);
    actor->saved.privateFlags = ((struct SkeletonInstanceState *)arg0)->field6DC;

    if (SkeletonInit_TestSkeletonFlags(arg0, 0x800)) {
        SkeletonInit_SetControl(actor, Skeleton_BeginKnockedFlight, 0x3D);
        SkeletonInit_SetState(actor, 3, 0, 0);
        SkeletonInit_SetFlags(arg0, 0x10);
        SkeletonInit_SetMotionFlags(actor, 0x1800);
        if (Script_SeekToKeyword(0x70)) {
            actor->body.position.x = MonsterInit_ReadHalfword(0);
            actor->body.position.y = MonsterInit_ReadHalfword(0);
            actor->body.position.z = MonsterInit_ReadHalfword(0);
            actor->body.direction = MonsterInit_ReadByte(0);
            SkeletonInit_ReadWidgetRelativePalette((void *)(actor->drawable + 0x20));
            actor->terrainCell = MonsterInit_CellIndex(SkeletonInit_BodyPosition(&actor->body));
            actor->groundHeight2BE = MonsterInit_TerrainHeight(
                SkeletonInit_GetTerrainCell(actor), SkeletonInit_BodyPosition(&actor->body));
            actor->field2EA = actor->body.position.y - actor->groundHeight2BE;
            actor->body.position.y = actor->groundHeight2BE;
        }
    } else if (SkeletonInit_TestSkeletonFlags(arg0, 8)) {
        SkeletonInit_SetControl(actor, SceneActorReaction_TickHiddenTimeout, 0x39);
        SkeletonInit_SetState(actor, 3, 0, 0);
        SkeletonInit_SetMotionFlags(actor, 0x1800);
        SkeletonInit_SetFlags(arg0, 0x10);
    }
    if (SkeletonInit_GetCategory(actor) == 8)
        SkeletonInit_SetMotionFlags(actor, 0x800);

    SkeletonInit_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}
