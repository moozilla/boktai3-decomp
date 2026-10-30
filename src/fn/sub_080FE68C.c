#include "global.h"
#include "scene_actor_init_actor.h"
#include "render.h"
#include "engine_globals.h"
#include "display_sequence_inline.h"
#include "player.h"
#include "scene_actor_init_primitives.h"

void Axe_InitProfile(void *);
void Axe_InitAttackCooldowns(void *);
void ActorGroup_ReadScriptOptions(void *);
void Axe_InitEffectPalettes(void *);
void Axe_InitColliders(void *);
void Axe_OnUpdate(void);
s32 Axe_GetActorCategory(void);
void Axe_UpdateTurnAnimation(void);
void Axe_OnAnimationReset(void);
void sub_080F93A4(void);
void Axe_ConvergeRecoveryMotion(void);
void Axe_UpdateDefeat(void);
s32 SceneActorControl_CanAdvanceHover(void *);
void SceneActorTarget_ResolvePendingObject(void);
void SceneActorControl_AdvanceReactionCount(void);
void SceneActorControl_InitializeHoverState(void);
void Axe_OnReaction(void);
void Axe_ReleaseAndIdle(void);
void Axe_UpdateDamageSpin(void);
void SceneActorBehavior_InitializeShadow(void *);
void Axe_InitMotionTable(void *);
void Axe_InitActionCallbacks(void *);
void Axe_InitEventCallbacks(void *);
void SpriteGroup_InitAnimatedSprites(void *);
void Axe_InitRoutePointHistory(void *);
void ActorCollision_InitThreeAuxiliaryAttacks(void *);
void Axe_OnRender(void);
void Axe_ReleaseResources(void);

#define SK_AT(T, off) (*(T *)((u8 *)arg0 + (off)))
void Memory_Clear(void *, u32);
void AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
void SceneActorAnimation_InitEnemySequence(void *, u32);
void ResourceHeader_Relocate(void *, void *);
void Widget_InitLinked(void *, void *, s32, s32, s32, s32, s32, void *);
void SceneActorReaction_OnEnemyDefault(void);
u8 sub_0807F81C(void *);
void SceneActorTarget_ResolvePendingObject(void);
void SceneActorReaction_SinkAndRemove(void);
void sub_081363F4(void);
void SceneActorPlacement_ResetStateThree(void);
void SceneActorReaction_BeginTargetRecovery(void);
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
void SceneActorScript_OnCommand(void);
void SceneActorControl_OnIdle(void);
void sub_08080BE0(void);
void ProximityVolume_TranslateBounds(void *, void *);
void SceneActorMovement_ResolveRoutePosition(void *);
void SceneActorRegistry_RegisterCategoryReceiver(void *);
void SceneActorBehavior_InitializeShadow(void *);
s32 Div(s32, s32);
void SceneBody_Init(void *, s32, void *, s32, s32, void *);
void SceneBody_StartRoute(void *, void *, u32, u32, u32);
s32 Path_GetPointPositionById(u16 *, u32, u32);
u32 Route_GetCurrentPointFacing(void *);
s32 TargetRegistry_RegisterActor(void *, void *, void *);
void SceneBody_SetSprite(void *, void *);
void SceneBody_SetMovementProbe(void *, void *, s32, s32);
s32 SceneActorScript_GetManagerField62(void);
void SceneActorPlacement_HideActor(void *, s32);
void SceneActorAnimation_SelectDirectionalEntry(void *, s32);
void SceneActorBehavior_InstallCallbackTables(void *);
void SceneActorTerrain_ReadScriptParameters(void *);

#define SV(d) MonsterInit_ReadWord(d)

struct AxeInit32Block {
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
void SceneActorReaction_OnEnemyDefault(void);
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

struct AxeInstanceState {
    u8 unknown0[0x624];
    u8 unknown624[0x824];
    s32 halfHealthE48;
    u32 savedFlagsE4C;
    u8 unknownE50[8];
    u16 groundHeightE58;
    u8 unknownE5A[0x5C];
    u16 fieldEB6;
    u8 unknownEB8[0x1F];
    u8 variantED7;
    u8 unknownED8[0x44];
    struct SceneVec playerPositionF1C;
};

static inline u8 AxeInit_GetDrawableKind(struct SceneActorRegistryResetView *actor)
{
    return actor->drawableKind;
}
static inline u8 AxeInit_GetCategory(void *object)
{
    struct SceneActorRegistryResetView *actor = object;
    return actor->category;
}
static inline s32 AxeInit_GetHealth(struct SceneActorRegistryResetView *actor)
{
    return actor->health;
}
static inline u16 AxeInit_GetInitialHealth(struct SceneActorRegistryResetView *actor)
{
    return actor->defaultHealth;
}

static inline u32 AxeInit_GetPaletteBase(struct SceneActorRegistryResetView *actor)
{
    return actor->basePaletteBank;
}

static inline u32 AxeInit_GetPaletteOffset(struct SceneActorRegistryResetView *actor)
{
    return actor->paletteOffset;
}

void SceneActorBehavior_InitializeShadow(void *);
extern u8 *gUnknown_02000580;

void ProximityVolume_TranslateBounds(void *, void *);
void SceneActorMovement_ResolveRoutePosition(void *);
void SceneActorRegistry_RegisterCategoryReceiver(void *);
u8 sub_0807F81C(void *);
void SceneActorPlacement_ResetStateThree(void);
void SceneActorReaction_BeginTargetRecovery(void);
extern const u8 gUnk_086060FC[];

static inline u16 AxeInit_GetMotionFlags(struct SceneActorRegistryResetView *actor)
{
    return actor->flags2C0;
}
static inline u16 AxeInit_GetHeightOffset(struct SceneActorRegistryResetView *actor)
{
    return actor->field2EA;
}
static inline u32 AxeInit_GetBehaviorFlags(struct SceneActorRegistryResetView *actor)
{
    return actor->flags2CC;
}
static inline u32 AxeInit_GetStatusFlags(struct SceneActorRegistryResetView *actor)
{
    return actor->flags2D0;
}
static inline u8 AxeInit_GetState(struct SceneActorRegistryResetView *actor)
{
    return actor->state2A8;
}
static inline u8 AxeInit_GetPhase(struct SceneActorRegistryResetView *actor)
{
    return actor->phase2A9;
}
static inline u8 AxeInit_GetBehavior(struct SceneActorRegistryResetView *actor)
{
    return actor->behavior2AA;
}
static inline void *AxeInit_GetCurrentControl(struct SceneActorRegistryResetView *actor)
{
    return actor->target;
}
static inline u8 AxeInit_GetControlKind(struct SceneActorRegistryResetView *actor)
{
    return actor->field2B5;
}
static inline u8 AxeInit_GetDirection(struct SceneActorRegistryResetView *actor)
{
    return actor->body.direction;
}
static inline u32 AxeInit_GetTerrainCell(struct SceneActorRegistryResetView *actor)
{
    return actor->terrainCell;
}
static inline u32 AxeInit_GetTargetingFlags(struct SceneActorChaseTracking *target)
{
    return target->flags;
}
static inline struct SceneActorDefaultControl *
AxeInit_SavedControlOf(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorDefaultControl *saved = &actor->saved;
    return saved;
}

static inline void AxeInit_SaveControl(struct SceneActorDefaultControl *saved,
                                       struct SceneActorRegistryResetView *actor)
{
    saved->motionFlags = AxeInit_GetMotionFlags(actor);
    saved->heightOffset = AxeInit_GetHeightOffset(actor);
    saved->behaviorFlags = AxeInit_GetBehaviorFlags(actor);
    saved->statusFlags = AxeInit_GetStatusFlags(actor);
    saved->direction = AxeInit_GetDirection(actor);
    saved->state = AxeInit_GetState(actor);
    saved->phase = AxeInit_GetPhase(actor);
    saved->behavior = AxeInit_GetBehavior(actor);
    saved->control = AxeInit_GetCurrentControl(actor);
    saved->kind = AxeInit_GetControlKind(actor);
}

static inline void AxeInit_ClearColliderFlags(struct ContactCollider *collider, u32 mask)
{
    collider->flags = mask & collider->flags;
}
static inline s32 AxeInit_Half(s32 value)
{
    if (value >= 0)
        return value >> 1;
    return -(-value >> 1);
}
static inline void AxeInit_SetBehaviorFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    actor->flags2CC |= mask;
}

static inline void AxeInit_SetStatusFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    actor->flags2D0 |= mask;
}

static inline void AxeInit_SetMotionFlags(struct SceneActorRegistryResetView *actor, u16 mask)
{
    actor->flags2C0 |= mask;
}
static inline u8 AxeInit_TestMotionFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    if (AxeInit_GetMotionFlags(actor) & mask)
        return 1;
    return 0;
}

static inline u8 AxeInit_TestTargetingFlags(struct SceneActorChaseTracking *target, u32 mask)
{
    if (AxeInit_GetTargetingFlags(target) & mask)
        return 1;
    return 0;
}

static inline void AxeInit_ClearControlFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    actor->flags114 &= mask;
}
static inline u8 AxeInit_IsInCurrentRegion(struct SceneActorRegistryResetView *actor)
{
    if (*(u32 *)(gGameState + 0x5A0) == actor->regionId)
        return 1;
    return 0;
}
static inline void *AxeInit_GetStandingControl(struct SceneActorRegistryResetView *actor)
{
    return actor->savedTarget;
}

static inline void AxeInit_SetControl(struct SceneActorRegistryResetView *actor, void *callback,
                                      u32 behavior)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = behavior;
    actor->target = callback;
    actor->field2C8 = 0;
    AxeInit_ClearControlFlags(actor, ~1);
    actor->word104 = 0;
}
static inline void AxeInit_SetState(struct SceneActorRegistryResetView *actor, u32 state, u32 phase,
                                    u32 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

static inline void AxeInit_InitializeGate(struct SceneActorControlGate *gate, u32 parameter)
{
    gate->flags = 0;
    gate->duration = parameter;
    gate->counter = 0;
    gate->field41A = 0;
    gate->control = 0;
}

static inline void AxeInit_ReadTargeting(struct SceneActorChaseTracking *target)
{
    s32 r4v, r2v, r1v;
    if (Script_SeekToKeyword(0x65)) {
        r4v = SV(0x30);
        r2v = Script_GetPc() ? (u8)Script_GetValue() : 0x40;
        r1v = r4v;
    } else {
        r4v = 0x30;
        r1v = 0x30;
        r2v = 0x40;
    }
    target->target = 0;
    target->x = r4v;
    target->z = r1v;
    target->limit = r4v * r4v;
    target->zSquared = r1v * r1v;
    target->fieldEA = r2v;
    if (AxeInit_TestTargetingFlags(target, 4))
        target->range = target->limit;
    else
        target->range = Div(target->limit * 8 - target->limit, 10);
}

static inline u16 AxeInit_RangeRadius(struct SceneActorRegistryControl *range)
{
    return range->radius;
}
static inline void AxeInit_ReadRange(struct SceneActorRegistryControl *range)
{
    s32 r4v;
    if (Script_SeekToKeyword(0x6E)) {
        r4v = SV(0);
        range->parameter = SV(0);
    } else {
        r4v = 0;
        range->parameter = r4v;
    }
    range->radius = r4v;
    range->radiusSquared = AxeInit_RangeRadius(range) * AxeInit_RangeRadius(range);
    range->mode = 0;
}

static inline u32 AxeInit_RoutePointX(struct SceneActorRegistryRoute *route)
{
    return (u16)route->point->x;
}

static inline u32 AxeInit_RoutePointZ(struct SceneActorRegistryRoute *route)
{
    return (u16)route->point->z;
}

static inline void AxeInit_SnapToRoute(struct ContactPosition *position,
                                       struct SceneActorRegistryRoute *route)
{
    position->x = AxeInit_RoutePointX(route);
    position->z = AxeInit_RoutePointZ(route);
    position->y = MonsterInit_TerrainHeight(MonsterInit_CellIndex(position), position);
}
static inline void AxeInit_DrawablePosition(struct SceneActorRegistryResetView *actor)
{
    if (AxeInit_GetDrawableKind(actor) == 0)
        *(struct ContactPosition *)(actor->drawable + 0x1C) = actor->body.position;
    else
        *(struct ContactPosition *)(actor->drawable + 0x40) = actor->body.position;
}

static inline u8 AxeInit_RouteId(struct SceneActorRegistryRoute *route)
{
    return route->routeId;
}
static inline u8 AxeInit_RouteParameter(struct SceneActorRegistryRoute *route)
{
    return route->parameterB;
}
static inline u8 AxeInit_RoutePointId(struct SceneActorRegistryRoute *route)
{
    return route->pointId;
}
static inline u8 AxeInit_RouteActiveBranch(struct SceneActorRegistryRoute *route)
{
    return route->activeBranch;
}

static inline s32 AxeInit_ClampHealth(u16 raw)
{
    s32 health = (s16)raw;
    if (health <= 0)
        health = 0;
    return health;
}

static inline void AxeInit_InstallIdle(struct SceneActorRegistryResetView *actor, void *idle)
{
    actor->target = idle;
}

static inline void AxeInit_InitializeHealth(struct SceneActorRegistryResetView *arg0,
                                            u16 initialHealth, s32 firstLimit, s32 secondLimit)
{
    arg0->flags2D4 = 0;
    AxeInit_InstallIdle(arg0, SceneActorControl_OnIdle);
    arg0->field2B2 = 1;
    arg0->field2B0 = 0;
    arg0->health = AxeInit_ClampHealth(initialHealth);
    arg0->fieldAA = 0;
    if (secondLimit <= 0)
        secondLimit = firstLimit;
    arg0->attachmentLimit = firstLimit;
    arg0->fieldA8 = secondLimit;
    arg0->regionId = 0;
}

static inline void AxeInit_SetInitialHealth(struct SceneActorRegistryResetView *actor, u16 health)
{
    actor->defaultHealth = health;
}

static inline u32 AxeInit_RouteAlternateBranch(struct SceneActorRegistryRoute *route)
{
    return 1 ^ route->activeBranch;
}
static inline void AxeInit_BeginRoute(struct SceneActorBody *body,
                                      struct SceneActorRegistryRoute *route)
{
    SceneBody_StartRoute(body, route, AxeInit_RouteId(route), AxeInit_RouteParameter(route),
                         AxeInit_RoutePointId(route));
}

static inline void AxeInit_SetFirstAttack(struct SceneActorRegistryResetView *actor, s32 index,
                                          s32 value)
{
    actor->attacks.first[index] = value;
}

static inline u32 AxeInit_ReadRoute(struct SceneActorRegistryResetView *arg0,
                                    struct SceneActorChaseGroup *group,
                                    struct SceneActorRegistryRoute *rt)
{
    struct ContactPosition point;
    s32 r1v;
    rt->activeBranch = 0;

    if (Script_SeekToKeyword(0x72)) {
        rt->routeId = SV(0);
        rt->parameterB = 0;
        r1v = SV(0);
        rt->pointId = r1v;
        rt->currentPoint = r1v;
        rt->branches[AxeInit_RouteActiveBranch(rt)] = rt->routeId;
        rt->branches[AxeInit_RouteAlternateBranch(rt)] =
            Script_GetPc() ? Script_GetValue() : AxeInit_RouteId(rt);
        Path_GetPointPositionById((u16 *)&point, AxeInit_RouteId(rt), AxeInit_RoutePointId(rt));
        arg0->terrainCell = MonsterInit_CellIndex(&point);
        point.y = arg0->groundHeight2BE =
            MonsterInit_TerrainHeight(AxeInit_GetTerrainCell(arg0), &point);
        AxeInit_BeginRoute(&arg0->body, rt);
        return 1;
    } else {
        rt->routeId = 0;
        rt->parameterB = 0;
        rt->pointId = 0;
        rt->currentPoint = 0;
        group->homeRoute.branches[AxeInit_RouteActiveBranch(rt)] = 0;
        group->homeRoute.branches[AxeInit_RouteAlternateBranch(rt)] = 0;
    }

    return 0;
}

static inline void AxeInit_InitializeBody(void *object, u32 *fill)
{
    struct SceneActorRegistryResetView *arg0 = object;
    struct SceneActorChaseGroup *group = &arg0->group;
    struct SceneActorBody *body = &arg0->body;
    struct ContactPosition origin;
    struct ContactPosition point;
    s32 mode, i, r4v, r6v, r5v, r2v, r1v;
    u16 initialHealth;
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
            AxeInit_SetFirstAttack(arg0, i, MonsterInit_ReadHalfword(0x18));
            arg0->attacks.second[i] = MonsterInit_ReadHalfword(0x18);
            arg0->attacks.flags[i] = SV(0);
            arg0->attacks.parameter[i] = MonsterInit_ReadHalfword(0);
            i++;
            if (i > 3)
                break;
        }
    }

    if (Script_SeekToKeyword(0x6C)) {
        r5v = SV(0x12C);
        r6v = SV(0x12C);
        r2v = SV(0x12C);
    } else {
        r5v = 0;
        r6v = 0;
        r2v = 0;
    }
    AxeInit_InitializeGate(&group->common.gate, r2v);
    AxeInit_ReadRange(&group->common.control);
    AxeInit_ReadTargeting(&arg0->tracking);
    if (Script_GetKeywordValue(0x62, 0))
        AxeInit_SetStatusFlags(arg0, 0x200);
    arg0->field1F0 = MonsterInit_ReadKeywordHalfword(0x46, 0);
    arg0->field1EC = MonsterInit_ReadKeywordHalfword(0x52, 0);

    if (Script_SeekToKeyword(0x41)) {
        AxeInit_SetInitialHealth(arg0, MonsterInit_ReadHalfword(0));
        group->halfHealth = AxeInit_Half(AxeInit_GetInitialHealth(arg0));
        arg0->field9E = MonsterInit_ReadByte(0);
        arg0->field9F = MonsterInit_ReadByte(0);
        arg0->fieldA0 = MonsterInit_ReadByte(0);
        arg0->fieldA2 = MonsterInit_ReadHalfword(0);
        arg0->fieldA4 = MonsterInit_ReadHalfword(0);
    }

    AxeInit_InitializeHealth(arg0, AxeInit_GetInitialHealth(arg0), r5v, r6v);

    if (Script_SeekToKeyword(0x58)) {
        arg0->fieldAC = SV(0);
        arg0->fieldA1 = MonsterInit_ReadByte(0);
    }
    arg0->fieldB2 = Script_GetKeywordValue(0x4B, 2);
    r1v = (s16)Script_GetKeywordValue(0x56, -1);
    if (r1v < 0)
        r1v = 0xFFFF;
    arg0->field9A = r1v;
    if (Script_GetKeywordValue(0x48, 0))
        AxeInit_SetStatusFlags(arg0, 0x800);
    if (Script_GetKeywordValue(0x4C, 0))
        AxeInit_SetStatusFlags(arg0, 0x8000);

    SceneBody_Init(body, mode, &origin, 0, 4, arg0);
    rt = &group->homeRoute;
    if (AxeInit_ReadRoute(arg0, group, rt)) {
        AxeInit_SnapToRoute(&arg0->body.position, rt);
        AxeInit_DrawablePosition(arg0);
        arg0->body.direction = Route_GetCurrentPointFacing(rt);
    }

    if (AxeInit_TestMotionFlags(arg0, 0x200))
        TargetRegistry_RegisterActor((u8 *)group + 0x30, body, sub_08080BE0);
    SceneBody_SetSprite(body, AxeInit_GetDrawableKind(arg0) == 0 ? arg0->drawable : 0);
    if (AxeInit_GetCategory(arg0) == 0x15)
        SceneBody_SetMovementProbe(body, (u8 *)arg0 + 0x40, 0x3C, 0x3C);
    else
        SceneBody_SetMovementProbe(body, (u8 *)arg0 + 0x40, 0x40, 0x40);

    group->common.bodyId = arg0->body.id;
    group->common.bodyPosition = &arg0->body.position;
}

struct AxeInitWidget {
    u8 unknown0[0x3A];
    u16 paletteBank;
};
static inline void AxeInit_SetWidgetScriptPalette(struct AxeInitWidget *widget, s32 scriptPalette)
{
    widget->paletteBank = scriptPalette - 0x5FFF;
}
static inline void AxeInit_SetMotionControl(struct SceneActorRegistryResetView *actor,
                                            void *callback, u32 kind)
{
    actor->word104 = (u32)callback;
    actor->word108 = 0;
    actor->word10C = 0;
    actor->word110 = kind;
    actor->flags114 = 1;
}
static inline void AxeInit_ClearMotion(void *object)
{
    struct SceneActorRegistryResetView *actor = object;
    u32 zero = 0;
    AxeInit_SetMotionControl(actor, 0, 0);
    actor->flags114 = zero;
}
static inline void AxeInit_ReadPalette(struct SceneActorRegistryResetView *actor, u32 base,
                                       u32 alternate)
{
    s32 r1v;
    actor->paletteOffset = Script_GetKeywordValue(0x43, 0);
    actor->basePaletteBank = base;
    actor->paletteAlternateBank = alternate;
    actor->alternateOffset = AxeInit_GetPaletteOffset(actor) + base;
    r1v = AxeInit_GetPaletteBase(actor) + AxeInit_GetPaletteOffset(actor);
    if (AxeInit_GetDrawableKind(actor) == 0)
        AnimData_SelectPaletteBank(actor->drawable + 0x2C, r1v);
    else
        *(u8 **)(actor->drawable + 0x68) = gPaletteBankData + (r1v << 5);
    if (AxeInit_GetDrawableKind(actor) == 1)
        AxeInit_SetWidgetScriptPalette((void *)(actor->drawable + 0x20),
                                       SceneActorScript_GetManagerField62());
}

static inline void AxeInit_InstallPrimary(struct SceneActorRegistryResetView *arg0, void *cb0,
                                          s32 (*cb1)(void), void *cb2, void *cb3, void *cb4,
                                          void *hidden, void *cb5, void *cb6, void *defaultReaction,
                                          s32 (*cb270)(void *), void *targetQuery, void *damage,
                                          void *contact)
{
    AxeInit_SetBehaviorFlags(arg0, 0x2000);
    arg0->contextCallback = cb0;
    {
        s32 (**slot)(void) = (void *)&arg0->animationQuery.opaque;
        *slot = cb1;
    }
    arg0->entryTarget224 = cb2;
    arg0->entryTarget228 = cb3;
    arg0->savedTarget = cb4;
    arg0->reactionTarget230 = cb5;
    arg0->deathControl = cb6;
    arg0->field220 = hidden;
    arg0->defaultReaction = defaultReaction;
    {
        s32 (**slot)(void *) = (void *)&arg0->contactQuery.opaque;
        *slot = cb270;
    }
    MonsterInit_WriteDataPointer(&arg0->commandTable.opaque, gUnk_086108B0);
    arg0->callback248 = 0;
    arg0->update26C = targetQuery;
    arg0->field238 = SceneActorReaction_SinkAndRemove;
    arg0->damageCallback = damage;
    arg0->contactCallback = contact;
    arg0->field240 = 0;
    AxeInit_ClearMotion(arg0);
}

static inline void AxeInit_InitializeGroup(struct SceneActorChaseGroup *group, void *firstBounds,
                                           void *secondBounds, void *downed, void *returnToRoute,
                                           void *recover, void *animate)
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
}

static inline void AxeInit_InitializeRegion(struct SceneActorRegistryResetView *arg0)
{
    if (AxeInit_TestMotionFlags(arg0, 2))
        arg0->regionId = *(u32 *)(gGameState + 0x5A0);
    else
        arg0->regionId = (u16)(u32)Region_FindContaining(&arg0->body.position);
}

static inline void AxeInit_InstallCompletionCallbacks(struct SceneActorRegistryResetView *arg0)
{
    arg0->messageCallback = SceneActorScript_OnCommand;
    arg0->updateCallback = Axe_OnRender;
    arg0->destroyCallback = Axe_ReleaseResources;
}

static inline u8 *AxeInit_Drawable(struct SceneActorRegistryResetView *actor)
{
    return actor->drawable;
}

static inline void AxeInit_SetCategory(struct SceneActorRegistryResetView *actor, u8 category)
{
    actor->category = category;
}

static inline void AxeInit_InitializeSprite(u8 *drawable, u16 resource, u16 sequence)
{
    AnimData_Load(drawable + 0x2C, resource);
    Sprite_Init(drawable, drawable + 0x2C, 0);
    SceneActorAnimation_InitEnemySequence(drawable, sequence);
}

static inline void AxeInit_InstallRoutes(struct SceneActorRegistryResetView *actor)
{
    actor->callback248 = SceneActorControl_InitializeHoverState;
    actor->field23C = Axe_OnReaction;
}
static inline void AxeInit_ClearReceiver(struct SceneActorRegistryResetView *actor)
{
    actor->field304 = 0;
}

static inline void AxeInit_InitializeWidget(u8 *drawable, u16 script, u16 resource,
                                            struct ContactPosition *position)
{
    void *source = ScriptProc_Start(script, resource);
    *(struct AxeInit32Block *)drawable = *(struct AxeInit32Block *)source;
    ResourceHeader_Relocate(drawable, source);
    Widget_InitLinked(drawable + 0x20, drawable, 0, 0, 2, 2, 0x3C, position);
}

static inline u8 AxeInit_BoundedVariant(s32 variant)
{
    if (variant > 1)
        return 1;
    return variant;
}

static inline void AxeInit_SetVariant(struct SceneActorRegistryResetView *actor, u8 variant)
{
    ((struct AxeInstanceState *)actor)->variantED7 = variant;
}

static inline void AxeInit_InitializePaletteRange(struct SceneActorRegistryResetView *actor,
                                                  u32 alternate, u32 current)
{
    actor->alternatePalette = AxeInit_GetPaletteBase(actor) + alternate;
    actor->paletteBank = AxeInit_GetPaletteBase(actor) + current;
}

static inline void AxeInit_SetCurrentPalette(struct SceneActorRegistryResetView *actor, u16 palette)
{
    actor->alternateOffset = palette;
}

s32 Axe_Init(void *object)
{
    struct SceneActorRegistryResetView *arg0 = object;
    struct SceneActorRegistryResetView *actor = object;
    struct SceneActorChaseGroup *group = &arg0->group;
    u32 zero;
    struct ContactPosition tmp;
    u32 flag;
    s32 variant;

    ((struct AxeInstanceState *)arg0)->savedFlagsE4C = 0;
    SceneActorRegistry_SetObjectContext(actor, actor, 0, 1);
    ((struct AxeInstanceState *)actor)->variantED7 = Script_GetKeywordValue('t', 0);
    variant = AxeInit_BoundedVariant(((struct AxeInstanceState *)actor)->variantED7);
    AxeInit_SetVariant(arg0, variant);
    if (variant == 0)
        AxeInit_SetCategory(actor, 12);
    else
        AxeInit_SetCategory(actor, 13);
    Axe_InitProfile(actor);
    if (AxeInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->drawable, 0x5C);
        heap = AxeInit_Drawable(actor);
        AxeInit_InitializeSprite(heap, 0x5291, 0x3F72);
        if (AxeInit_GetCategory(actor) == 0x11 || AxeInit_GetCategory(actor) == 0x17)
            heap[7] = 1;
        actor->body.direction = 0;
    } else {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->drawable, 0x80);
        heap = AxeInit_Drawable(actor);
        MonsterInit_ClearPosition(&zero, &tmp);
        AxeInit_InitializeWidget(heap, 0xCB05, 0x5291, &tmp);
        actor->animationIndex = 0xFF;
    }
    AxeInit_InitializeBody(arg0, &zero);
    ((struct AxeInstanceState *)arg0)->halfHealthE48 = AxeInit_GetHealth(actor) >> 1;
    Axe_InitAttackCooldowns(actor);
    if (Script_SeekToKeyword('p')) {
        u32 height;
        actor->body.position.x = MonsterInit_ReadHalfword(0);
        actor->body.position.y = MonsterInit_ReadHalfword(0);
        actor->body.position.z = MonsterInit_ReadHalfword(0);
        actor->terrainCell = MonsterInit_CellIndex(&actor->body.position);
        height = MonsterInit_TerrainHeight(AxeInit_GetTerrainCell(actor), &actor->body.position);
        actor->groundHeight2BE = height;
        ((struct AxeInstanceState *)arg0)->groundHeightE58 = height;
    } else {
        ((struct AxeInstanceState *)arg0)->groundHeightE58 = actor->groundHeight2BE;
    }
    ((struct AxeInstanceState *)arg0)->fieldEB6 = Script_GetKeywordValue('w', -1);
    ActorGroup_ReadScriptOptions(actor);
    ((struct AxeInstanceState *)arg0)->playerPositionF1C = gPlayers[0]->position;
    AxeInit_ReadPalette(actor, 0x2BD, 0x192);
    AxeInit_InitializePaletteRange(actor, 0, 2);
    Axe_InitEffectPalettes(actor);
    AxeInit_SetCurrentPalette(actor, 0x1EE);
    Axe_InitColliders(actor);
    AxeInit_InstallPrimary(actor, Axe_OnUpdate, Axe_GetActorCategory, Axe_UpdateTurnAnimation,
                           Axe_OnAnimationReset, sub_080F93A4, 0, Axe_ConvergeRecoveryMotion,
                           Axe_UpdateDefeat, SceneActorReaction_OnEnemyDefault,
                           SceneActorControl_CanAdvanceHover, SceneActorTarget_ResolvePendingObject,
                           sub_081363F4, SceneActorControl_AdvanceReactionCount);
    AxeInit_InitializeGroup(group, (u8 *)arg0 + 0xED8, (u8 *)arg0 + 0xEFC, 0, 0, 0, 0);
    AxeInit_InstallRoutes(actor);
    group->responseCallback = 0;
    group->field5EC = 0;
    group->field614 = Axe_ReleaseAndIdle;
    group->field610 = Axe_UpdateDamageSpin;
    AxeInit_SetMotionFlags(actor, 0x60);
    AxeInit_SetBehaviorFlags(actor, 0x200);
    AxeInit_SetStatusFlags(actor, 0x02100408);
    SceneActorBehavior_InitializeShadow(actor);
    SceneActorTerrain_ReadScriptParameters(actor);
    flag = 0;
    if (AxeInit_TestMotionFlags(actor, 4)) {
        AxeInit_SetStatusFlags(actor, 0x2000);
        flag = 1;
    }
    if (flag != 1) {
        AxeInit_SetState(actor, 0, 0, 0);
        AxeInit_SetControl(actor, AxeInit_GetStandingControl(actor), 0);
    }
    AxeInit_InitializeRegion(actor);
    if (AxeInit_TestMotionFlags(actor, 0x40))
        AxeInit_ClearColliderFlags((void *)((u8 *)actor + 0x134), ~0x100);
    if (Script_GetKeywordValue('S', 0) != 0 || !AxeInit_IsInCurrentRegion(actor))
        SceneActorPlacement_HideActor(actor, 0);
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    if (AxeInit_GetDrawableKind(actor) == 0)
        DisplaySequence_Step(actor->drawable);
    else
        Anim_Step(actor->drawable + 0x20, actor->drawable);
    AxeInit_ClearReceiver(actor);
    SceneActorBehavior_InstallCallbackTables(actor);
    Axe_InitMotionTable(actor);
    Axe_InitActionCallbacks(actor);
    Axe_InitEventCallbacks(actor);
    AxeInit_Drawable(actor)[7] = 1;
    actor->field2E8 = 0;
    SpriteGroup_InitAnimatedSprites(actor);
    Axe_InitRoutePointHistory(actor);
    ActorCollision_InitThreeAuxiliaryAttacks(actor);
    AxeInit_SaveControl(AxeInit_SavedControlOf(actor), actor);
    actor->saved.privateFlags = ((struct AxeInstanceState *)arg0)->savedFlagsE4C;
    AxeInit_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}
