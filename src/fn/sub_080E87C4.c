#include "global.h"
#include "scene_actor_init_actor.h"
#include "render.h"
#include "engine_globals.h"
#include "display_sequence_inline.h"
#include "scene_actor_init_primitives.h"

void Mimic_InitProfile(void *);
void SceneActor_InitializeReactionColliders(void *);
void Mimic_UpdateBehavior(void);
void SceneActor_EnterFlaggedEntryPhase(void);
void Mimic_OnAnimationStateChanged(void);
void Mimic_ConvergeAndTurn(void);
void Mimic_UpdateDefeatBlink(void);
void SceneActor_AcquireCollisionTarget(void);
void Mimic_StartHitReaction(void);
void Mimic_OnBehaviorComplete(void);
void Mimic_ResetPursuitDrawable(void);
void SceneActor_PrepareStationaryReaction(void);
void SceneActor_RestoreSavedTargetOnCompletion(void);
void Mimic_InitBehaviorTable(void *);
void Mimic_InitActionTable(void *);
void Mimic_OnUpdate(void);
void Mimic_ReleaseResources(void);

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

struct MimicInit32Block {
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

struct MimicInstanceState {
    u8 unknown0[0x624];
    u8 unknown624[0x2C];
    struct ContactPosition origin650;
    u16 originCell658;
    u8 unknown65A[4];
    u8 parameter65E;
    u8 unknown65F;
    s32 halfHealth660;
};

static inline u8 MimicInit_GetDrawableKind(struct SceneActorRegistryResetView *actor)
{
    return actor->drawableKind;
}
static inline u8 MimicInit_GetCategory(void *object)
{
    struct SceneActorRegistryResetView *actor = object;
    return actor->category;
}
static inline s32 MimicInit_GetHealth(struct SceneActorRegistryResetView *actor)
{
    return actor->health;
}
static inline u16 MimicInit_GetInitialHealth(struct SceneActorRegistryResetView *actor)
{
    return actor->defaultHealth;
}

static inline u32 MimicInit_GetPaletteBase(struct SceneActorRegistryResetView *actor)
{
    return actor->basePaletteBank;
}

static inline u32 MimicInit_GetPaletteOffset(struct SceneActorRegistryResetView *actor)
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

static inline u16 MimicInit_GetMotionFlags(struct SceneActorRegistryResetView *actor)
{
    return actor->flags2C0;
}
static inline u16 MimicInit_GetHeightOffset(struct SceneActorRegistryResetView *actor)
{
    return actor->field2EA;
}
static inline u32 MimicInit_GetBehaviorFlags(struct SceneActorRegistryResetView *actor)
{
    return actor->flags2CC;
}
static inline u32 MimicInit_GetStatusFlags(struct SceneActorRegistryResetView *actor)
{
    return actor->flags2D0;
}
static inline u8 MimicInit_GetState(struct SceneActorRegistryResetView *actor)
{
    return actor->state2A8;
}
static inline u8 MimicInit_GetPhase(struct SceneActorRegistryResetView *actor)
{
    return actor->phase2A9;
}
static inline u8 MimicInit_GetBehavior(struct SceneActorRegistryResetView *actor)
{
    return actor->behavior2AA;
}
static inline void *MimicInit_GetCurrentControl(struct SceneActorRegistryResetView *actor)
{
    return actor->target;
}
static inline u8 MimicInit_GetControlKind(struct SceneActorRegistryResetView *actor)
{
    return actor->field2B5;
}
static inline u8 MimicInit_GetDirection(struct SceneActorRegistryResetView *actor)
{
    return actor->body.direction;
}
static inline u32 MimicInit_GetTerrainCell(struct SceneActorRegistryResetView *actor)
{
    return actor->terrainCell;
}
static inline u32 MimicInit_GetTargetingFlags(struct SceneActorChaseTracking *target)
{
    return target->flags;
}
static inline struct SceneActorDefaultControl *
MimicInit_SavedControlOf(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorDefaultControl *saved = &actor->saved;
    return saved;
}
static inline struct ContactPosition *MimicInit_BodyPosition(void *object)
{
    struct SceneActorBody *body = object;
    struct ContactPosition *position = &body->position;
    return position;
}
static inline void MimicInit_SaveControl(struct SceneActorDefaultControl *saved,
                                         struct SceneActorRegistryResetView *actor)
{
    saved->motionFlags = MimicInit_GetMotionFlags(actor);
    saved->heightOffset = MimicInit_GetHeightOffset(actor);
    saved->behaviorFlags = MimicInit_GetBehaviorFlags(actor);
    saved->statusFlags = MimicInit_GetStatusFlags(actor);
    saved->direction = MimicInit_GetDirection(actor);
    saved->state = MimicInit_GetState(actor);
    saved->phase = MimicInit_GetPhase(actor);
    saved->behavior = MimicInit_GetBehavior(actor);
    saved->control = MimicInit_GetCurrentControl(actor);
    saved->kind = MimicInit_GetControlKind(actor);
}

static inline void MimicInit_ClearColliderFlags(struct ContactCollider *collider, u32 mask)
{
    collider->flags = mask & collider->flags;
}
static inline s32 MimicInit_Half(s32 value)
{
    if (value >= 0)
        return value >> 1;
    return -(-value >> 1);
}
static inline void MimicInit_SetBehaviorFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    actor->flags2CC |= mask;
}

static inline void MimicInit_SetStatusFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    actor->flags2D0 |= mask;
}

static inline void MimicInit_SetMotionFlags(struct SceneActorRegistryResetView *actor, u16 mask)
{
    actor->flags2C0 |= mask;
}
static inline u8 MimicInit_TestMotionFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    if (MimicInit_GetMotionFlags(actor) & mask)
        return 1;
    return 0;
}

static inline u8 MimicInit_TestTargetingFlags(struct SceneActorChaseTracking *target, u32 mask)
{
    if (MimicInit_GetTargetingFlags(target) & mask)
        return 1;
    return 0;
}

static inline void MimicInit_ClearControlFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    actor->flags114 &= mask;
}
static inline u8 MimicInit_IsInCurrentRegion(struct SceneActorRegistryResetView *actor)
{
    if (*(u32 *)(gGameState + 0x5A0) == actor->regionId)
        return 1;
    return 0;
}
static inline void *MimicInit_GetStandingControl(struct SceneActorRegistryResetView *actor)
{
    return actor->savedTarget;
}

static inline void MimicInit_SetControl(struct SceneActorRegistryResetView *actor, void *callback,
                                        u32 behavior)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = behavior;
    actor->target = callback;
    actor->field2C8 = 0;
    MimicInit_ClearControlFlags(actor, ~1);
    actor->word104 = 0;
}
static inline void MimicInit_SetState(struct SceneActorRegistryResetView *actor, u32 state,
                                      u32 phase, u32 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

static inline void MimicInit_InitializeGate(struct SceneActorControlGate *gate, u32 parameter)
{
    gate->flags = 0;
    gate->duration = parameter;
    gate->counter = 0;
    gate->field41A = 0;
    gate->control = 0;
}

static inline void MimicInit_ReadTargeting(struct SceneActorChaseTracking *target)
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
    if (MimicInit_TestTargetingFlags(target, 4))
        target->range = target->limit;
    else
        target->range = Div(target->limit * 8 - target->limit, 10);
}

static inline u32 MimicInit_RangeRadius(struct SceneActorRegistryControl *range)
{
    return range->radius;
}
static inline void MimicInit_ReadRange(struct SceneActorRegistryControl *range)
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
    range->radiusSquared = MimicInit_RangeRadius(range) * MimicInit_RangeRadius(range);
    range->mode = 0;
}

static inline u32 MimicInit_RoutePointX(struct SceneActorRegistryRoute *route)
{
    return (u16)route->point->x;
}

static inline u32 MimicInit_RoutePointZ(struct SceneActorRegistryRoute *route)
{
    return (u16)route->point->z;
}

static inline void MimicInit_SnapToRoute(struct ContactPosition *position,
                                         struct SceneActorRegistryRoute *route)
{
    position->x = MimicInit_RoutePointX(route);
    position->z = MimicInit_RoutePointZ(route);
    position->y = MonsterInit_TerrainHeight(MonsterInit_CellIndex(position), position);
}
static inline void MimicInit_DrawablePosition(struct SceneActorRegistryResetView *actor)
{
    if (MimicInit_GetDrawableKind(actor) == 0)
        *(struct ContactPosition *)(actor->drawable + 0x1C) = actor->body.position;
    else
        *(struct ContactPosition *)(actor->drawable + 0x40) = actor->body.position;
}

static inline u8 MimicInit_RouteId(struct SceneActorRegistryRoute *route)
{
    return route->routeId;
}
static inline u8 MimicInit_RouteParameter(struct SceneActorRegistryRoute *route)
{
    return route->parameterB;
}
static inline u8 MimicInit_RoutePointId(struct SceneActorRegistryRoute *route)
{
    return route->pointId;
}
static inline u8 MimicInit_RouteActiveBranch(struct SceneActorRegistryRoute *route)
{
    return route->activeBranch;
}

static inline s32 MimicInit_ClampHealth(u16 raw)
{
    s32 health = (s16)raw;
    if (health <= 0)
        health = 0;
    return health;
}

static inline void MimicInit_InstallIdle(struct SceneActorRegistryResetView *actor, void *idle)
{
    actor->target = idle;
}

static inline void MimicInit_InitializeHealth(struct SceneActorRegistryResetView *arg0,
                                              u16 initialHealth, s32 firstLimit, s32 secondLimit)
{
    arg0->flags2D4 = 0;
    MimicInit_InstallIdle(arg0, SceneActorControl_OnIdle);
    arg0->field2B2 = 1;
    arg0->field2B0 = 0;
    arg0->health = MimicInit_ClampHealth(initialHealth);
    arg0->fieldAA = 0;
    if (secondLimit <= 0)
        secondLimit = firstLimit;
    arg0->attachmentLimit = firstLimit;
    arg0->fieldA8 = secondLimit;
    arg0->regionId = 0;
}

static inline void MimicInit_SetInitialHealth(struct SceneActorRegistryResetView *actor, u16 health)
{
    actor->defaultHealth = health;
}

static inline u32 MimicInit_RouteAlternateBranch(struct SceneActorRegistryRoute *route)
{
    return 1 ^ route->activeBranch;
}
static inline void MimicInit_BeginRoute(struct SceneActorBody *body,
                                        struct SceneActorRegistryRoute *route)
{
    SceneBody_StartRoute(body, route, MimicInit_RouteId(route), MimicInit_RouteParameter(route),
                         MimicInit_RoutePointId(route));
}

static inline void MimicInit_SetFirstAttack(struct SceneActorRegistryResetView *actor, s32 index,
                                            s32 value)
{
    actor->attacks.first[index] = value;
}

static inline u32 MimicInit_ReadRoute(struct SceneActorRegistryResetView *arg0,
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
        rt->branches[MimicInit_RouteActiveBranch(rt)] = rt->routeId;
        rt->branches[MimicInit_RouteAlternateBranch(rt)] =
            Script_GetPc() ? Script_GetValue() : MimicInit_RouteId(rt);
        Path_GetPointPositionById((u16 *)&point, MimicInit_RouteId(rt), MimicInit_RoutePointId(rt));
        arg0->terrainCell = MonsterInit_CellIndex(&point);
        point.y = arg0->groundHeight2BE =
            MonsterInit_TerrainHeight(MimicInit_GetTerrainCell(arg0), &point);
        MimicInit_BeginRoute(&arg0->body, rt);
        return 1;
    } else {
        rt->routeId = 0;
        rt->parameterB = 0;
        rt->pointId = 0;
        rt->currentPoint = 0;
        group->homeRoute.branches[MimicInit_RouteActiveBranch(rt)] = 0;
        group->homeRoute.branches[MimicInit_RouteAlternateBranch(rt)] = 0;
    }

    return 0;
}

static inline void MimicInit_InitializeBody(void *object, u32 *fill)
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
            MimicInit_SetFirstAttack(arg0, i, MonsterInit_ReadHalfword(0x18));
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
    MimicInit_InitializeGate(&group->common.gate, r2v);
    MimicInit_ReadRange(&group->common.control);
    MimicInit_ReadTargeting(&arg0->tracking);
    if (Script_GetKeywordValue(0x62, 0))
        MimicInit_SetStatusFlags(arg0, 0x200);
    arg0->field1F0 = MonsterInit_ReadKeywordHalfword(0x46, 0);
    arg0->field1EC = MonsterInit_ReadKeywordHalfword(0x52, 0);

    if (Script_SeekToKeyword(0x41)) {
        MimicInit_SetInitialHealth(arg0, MonsterInit_ReadHalfword(0));
        group->halfHealth = MimicInit_Half(MimicInit_GetInitialHealth(arg0));
        arg0->field9E = MonsterInit_ReadByte(0);
        arg0->field9F = MonsterInit_ReadByte(0);
        arg0->fieldA0 = MonsterInit_ReadByte(0);
        arg0->fieldA2 = MonsterInit_ReadHalfword(0);
        arg0->fieldA4 = MonsterInit_ReadHalfword(0);
    }

    MimicInit_InitializeHealth(arg0, MimicInit_GetInitialHealth(arg0), r5v, r6v);

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
        MimicInit_SetStatusFlags(arg0, 0x800);
    if (Script_GetKeywordValue(0x4C, 0))
        MimicInit_SetStatusFlags(arg0, 0x8000);

    SceneBody_Init(body, mode, &origin, 0, 4, arg0);
    rt = &group->homeRoute;
    if (MimicInit_ReadRoute(arg0, group, rt)) {
        MimicInit_SnapToRoute(&arg0->body.position, rt);
        MimicInit_DrawablePosition(arg0);
        arg0->body.direction = Route_GetCurrentPointFacing(rt);
    }

    if (MimicInit_TestMotionFlags(arg0, 0x200))
        TargetRegistry_RegisterActor((u8 *)group + 0x30, body, sub_08080BE0);
    SceneBody_SetSprite(body, MimicInit_GetDrawableKind(arg0) == 0 ? arg0->drawable : 0);
    if (MimicInit_GetCategory(arg0) == 0x15)
        SceneBody_SetMovementProbe(body, (u8 *)arg0 + 0x40, 0x3C, 0x3C);
    else
        SceneBody_SetMovementProbe(body, (u8 *)arg0 + 0x40, 0x40, 0x40);

    group->common.bodyId = arg0->body.id;
    group->common.bodyPosition = &arg0->body.position;
}

struct MimicInitWidget {
    u8 unknown0[0x3A];
    u16 paletteBank;
};
static inline void MimicInit_SetWidgetScriptPalette(struct MimicInitWidget *widget,
                                                    s32 scriptPalette)
{
    widget->paletteBank = scriptPalette - 0x5FFF;
}
static inline void MimicInit_SetMotionControl(struct SceneActorRegistryResetView *actor,
                                              void *callback, u32 kind)
{
    actor->word104 = (u32)callback;
    actor->word108 = 0;
    actor->word10C = 0;
    actor->word110 = kind;
    actor->flags114 = 1;
}
static inline void MimicInit_ClearMotion(void *object)
{
    struct SceneActorRegistryResetView *actor = object;
    u32 zero = 0;
    MimicInit_SetMotionControl(actor, 0, 0);
    actor->flags114 = zero;
}
static inline void MimicInit_ReadPalette(struct SceneActorRegistryResetView *actor, u32 base,
                                         u32 alternate)
{
    s32 r1v;
    actor->paletteOffset = Script_GetKeywordValue(0x43, 0);
    actor->basePaletteBank = base;
    actor->paletteAlternateBank = alternate;
    actor->alternateOffset = MimicInit_GetPaletteOffset(actor) + base;
    r1v = MimicInit_GetPaletteBase(actor) + MimicInit_GetPaletteOffset(actor);
    if (MimicInit_GetDrawableKind(actor) == 0)
        AnimData_SelectPaletteBank(actor->drawable + 0x2C, r1v);
    else
        *(u8 **)(actor->drawable + 0x68) = gPaletteBankData + (r1v << 5);
    if (MimicInit_GetDrawableKind(actor) == 1)
        MimicInit_SetWidgetScriptPalette((void *)(actor->drawable + 0x20),
                                         SceneActorScript_GetManagerField62());
}

static inline void MimicInit_InstallPrimary(struct SceneActorRegistryResetView *arg0, void *cb0,
                                            u32 (*cb1)(void *), void *cb2, void *cb3, void *cb4,
                                            void *hidden, void *cb5, void *cb6,
                                            void *defaultReaction, void *cb270, void *targetQuery,
                                            void *damage, void *contact)
{
    MimicInit_SetBehaviorFlags(arg0, 0x2000);
    arg0->contextCallback = cb0;
    MonsterInit_WriteAnimationQuery(&arg0->animationQuery.word, cb1);
    arg0->entryTarget224 = cb2;
    arg0->entryTarget228 = cb3;
    arg0->savedTarget = cb4;
    arg0->reactionTarget230 = cb5;
    arg0->deathControl = cb6;
    arg0->field220 = hidden;
    arg0->defaultReaction = defaultReaction;
    MonsterInit_WriteCallback(&arg0->contactQuery.opaque, cb270);
    MonsterInit_WriteDataPointer(&arg0->commandTable.opaque, gUnk_086108B0);
    arg0->callback248 = 0;
    arg0->update26C = targetQuery;
    arg0->field238 = SceneActorReaction_SinkAndRemove;
    arg0->damageCallback = damage;
    arg0->contactCallback = contact;
    arg0->field240 = 0;
    MimicInit_ClearMotion(arg0);
}

static inline void MimicInit_InitializeGroup(struct SceneActorChaseGroup *group, void *firstBounds,
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
    ProximityVolume_TranslateBounds(firstBounds, MimicInit_BodyPosition(body));
}

static inline void MimicInit_InitializeRegion(struct SceneActorRegistryResetView *arg0)
{
    if (MimicInit_TestMotionFlags(arg0, 2))
        arg0->regionId = *(u32 *)(gGameState + 0x5A0);
    else
        arg0->regionId = (u16)(u32)Region_FindContaining(&arg0->body.position);
}

static inline void MimicInit_InstallCompletionCallbacks(struct SceneActorRegistryResetView *arg0)
{
    arg0->messageCallback = SceneActorScript_OnCommand;
    arg0->updateCallback = Mimic_OnUpdate;
    arg0->destroyCallback = Mimic_ReleaseResources;
}

static inline u8 *MimicInit_Drawable(struct SceneActorRegistryResetView *actor)
{
    return actor->drawable;
}

static inline void MimicInit_SetCategory(struct SceneActorRegistryResetView *actor, u8 category)
{
    actor->category = category;
}

static inline void MimicInit_InitializeSprite(u8 *drawable, u16 resource, u16 sequence)
{
    AnimData_Load(drawable + 0x2C, resource);
    Sprite_Init(drawable, drawable + 0x2C, 0);
    SceneActorAnimation_InitEnemySequence(drawable, sequence);
}

static inline void MimicInit_SetPaletteBank(struct SceneActorRegistryResetView *actor, u16 palette)
{
    actor->paletteBank = palette;
}
static inline void MimicInit_InstallRoutes(struct SceneActorRegistryResetView *actor)
{
    actor->callback248 = Mimic_ResetPursuitDrawable;
    actor->field23C = SceneActor_PrepareStationaryReaction;
}
static inline void MimicInit_ClearReceiver(struct SceneActorRegistryResetView *actor)
{
    actor->field304 = 0;
}
static inline void MimicInit_InitializeTerrainCell(struct SceneActorRegistryResetView *actor,
                                                   s32 cell)
{
    actor->terrainCell = cell;
    actor->previousTerrainCell = cell;
}

static inline void MimicInit_InitializeWidget(u8 *drawable, u16 script, u16 resource,
                                              struct ContactPosition *position)
{
    void *source = ScriptProc_Start(script, resource);
    *(struct MimicInit32Block *)drawable = *(struct MimicInit32Block *)source;
    ResourceHeader_Relocate(drawable, source);
    Widget_InitLinked(drawable + 0x20, drawable, 0, 0, 2, 2, 0x3C, position);
}

s32 Mimic_Init(void *object)
{
    struct SceneActorRegistryResetView *arg0 = object;
    struct SceneActorRegistryResetView *actor = object;
    struct SceneActorChaseGroup *group = &arg0->group;
    struct SceneActorBody *body = &arg0->body;
    u32 zero;
    struct ContactPosition tmp;
    u32 flag;
    s32 lift;

    SceneActorRegistry_SetObjectContext(actor, actor, 0, 1);
    MimicInit_SetCategory(actor, 10);
    Mimic_InitProfile(actor);
    if (MimicInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->drawable, 0x5C);
        heap = MimicInit_Drawable(actor);
        MimicInit_InitializeSprite(heap, 0x3876, 0x9AF2);
        if (MimicInit_GetCategory(actor) == 0x11 || MimicInit_GetCategory(actor) == 0x17)
            heap[7] = 1;
        actor->body.direction = 0;
    } else {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->drawable, 0x80);
        heap = MimicInit_Drawable(actor);
        MonsterInit_ClearPosition(&zero, &tmp);
        MimicInit_InitializeWidget(heap, 0xCB05, 0x3876, &tmp);
        actor->animationIndex = 0xFF;
    }
    MimicInit_InitializeBody(arg0, &zero);
    ((struct MimicInstanceState *)arg0)->halfHealth660 = MimicInit_GetHealth(actor) >> 1;
    actor->body.direction = Script_GetKeywordValue('d', 0);
    ((struct MimicInstanceState *)arg0)->parameter65E = Script_GetKeywordValue('s', 10);
    if (Script_SeekToKeyword('p')) {
        s32 cell;
        actor->body.position.x = MonsterInit_ReadHalfword(0);
        actor->body.position.y = MonsterInit_ReadHalfword(0);
        actor->body.position.z = MonsterInit_ReadHalfword(0);
        MimicInit_InitializeTerrainCell(actor, MonsterInit_CellIndex(&actor->body.position));
        actor->groundHeight2BE =
            MonsterInit_TerrainHeight(MimicInit_GetTerrainCell(actor), &actor->body.position);
        actor->field2EA = 0;
        actor->body.position.y = actor->groundHeight2BE;
        MimicInit_DrawablePosition(actor);
    }
    ((struct MimicInstanceState *)arg0)->origin650 = actor->body.position;
    ((struct MimicInstanceState *)arg0)->originCell658 = actor->terrainCell;
    MimicInit_ReadPalette(actor, 0x304, 0x192);
    MimicInit_SetPaletteBank(actor, 0x306);
    SceneActor_InitializeReactionColliders(actor);
    MimicInit_InstallPrimary(
        actor, Mimic_UpdateBehavior, 0, 0, SceneActor_EnterFlaggedEntryPhase,
        Mimic_OnAnimationStateChanged, 0, Mimic_ConvergeAndTurn, Mimic_UpdateDefeatBlink,
        SceneActorReaction_OnEnemyDefault, SceneActor_AcquireCollisionTarget,
        SceneActorTarget_ResolvePendingObject, sub_081363F4, Mimic_StartHitReaction);
    MimicInit_InitializeGroup(group, (u8 *)arg0 + 0x624, (u8 *)arg0 + 0x648, 0, 0,
                              Mimic_OnBehaviorComplete, 0, body);
    MimicInit_InstallRoutes(actor);
    MimicInit_SetBehaviorFlags(actor, 0x400200);
    MimicInit_SetStatusFlags(actor, 8);
    MimicInit_SetMotionFlags(actor, 0x40);
    SceneActorBehavior_InitializeShadow(actor);
    SceneActorTerrain_ReadScriptParameters(actor);
    flag = 0;
    if (MimicInit_TestMotionFlags(actor, 4)) {
        MimicInit_SetStatusFlags(actor, 0x2000);
        flag = 1;
    }
    if (flag != 1) {
        MimicInit_SetState(actor, 0, 0, 0);
        MimicInit_SetControl(actor, MimicInit_GetStandingControl(actor), 0);
    }
    MimicInit_InitializeRegion(actor);
    if (MimicInit_TestMotionFlags(actor, 0x40))
        MimicInit_ClearColliderFlags((void *)((u8 *)actor + 0x134), ~0x100);
    if (Script_GetKeywordValue(0x53, 0) != 0 || !MimicInit_IsInCurrentRegion(actor))
        SceneActorPlacement_HideActor(actor, 0);
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    if (MimicInit_GetDrawableKind(actor) == 0)
        DisplaySequence_Step(actor->drawable);
    else
        Anim_Step(actor->drawable + 0x20, actor->drawable);
    MimicInit_ClearReceiver(actor);
    lift = Script_GetKeywordValue('f', 0);
    if (lift) {
        actor->field2EA = lift;
        MimicInit_SetControl(actor, SceneActor_RestoreSavedTargetOnCompletion, 0x26);
        MimicInit_SetState(actor, 3, 0, 0);
    }
    SceneActorBehavior_InstallCallbackTables(actor);
    Mimic_InitBehaviorTable(actor);
    Mimic_InitActionTable(actor);
    actor->field2E8 = 0;
    MimicInit_SaveControl(MimicInit_SavedControlOf(actor), actor);
    MimicInit_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}
