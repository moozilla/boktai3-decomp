#include "global.h"
#include "scene_actor_init_actor.h"
#include "render.h"
#include "engine_globals.h"
#include "display_sequence_inline.h"
#include "scene_actor_init_primitives.h"

void Octopus_InitProfileId(void *);
void Octopus_Update(void);
void Octopus_UpdateDefeat(void);
void SceneActorControl_BeginSoundTransition_080EBC38(void);
void SceneActorControl_UpdateMotionPhase_080EBD0C(void);
void SceneActorCollision_InitCompactBody(void *);
void Octopus_InitControlPresets(void *);
void Octopus_BeginIdleAnimation(void *);
void Octopus_UpdateInteraction(void);
void Octopus_Destroy(void);
extern const u8 gUnk_0860635C[];

#define SK_AT(T, off) (*(T *)((u8 *)arg0 + (off)))
void Memory_Clear(void *, u32);
void AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
void SceneActorAnimation_InitEnemySequence(void *, u32);
void ResourceHeader_Relocate(void *, void *);
void Widget_InitLinked(void *, void *, s32, s32, s32, s32, s32, void *);
void SceneActorReaction_OnEnemyDefault(void);
u8 sub_0807F81C(void *);
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

struct OctopusInit32Block {
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

struct OctopusInstanceState {
    u8 unknown0[0x624];
    u8 unknown624[8];
    u32 captureFlags;
    u8 unknown630[6];
    u16 field636;
    u8 unknown638;
    u8 field639;
    u8 field63A;
    u8 unknown63B;
};

static inline u8 OctopusInit_GetDrawableKind(struct SceneActorRegistryResetView *actor)
{
    return actor->drawableKind;
}
static inline u8 OctopusInit_GetCategory(void *object)
{
    struct SceneActorRegistryResetView *actor = object;
    return actor->category;
}

static inline u16 OctopusInit_GetInitialHealth(struct SceneActorRegistryResetView *actor)
{
    return actor->defaultHealth;
}

static inline u32 OctopusInit_GetPaletteBase(struct SceneActorRegistryResetView *actor)
{
    return actor->basePaletteBank;
}

static inline u32 OctopusInit_GetPaletteOffset(struct SceneActorRegistryResetView *actor)
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

static inline u32 OctopusInit_GetMotionFlags(struct SceneActorRegistryResetView *actor)
{
    return actor->flags2C0;
}
static inline u16 OctopusInit_GetHeightOffset(struct SceneActorRegistryResetView *actor)
{
    return actor->field2EA;
}
static inline u32 OctopusInit_GetBehaviorFlags(struct SceneActorRegistryResetView *actor)
{
    return actor->flags2CC;
}
static inline u32 OctopusInit_GetStatusFlags(struct SceneActorRegistryResetView *actor)
{
    return actor->flags2D0;
}
static inline u8 OctopusInit_GetState(struct SceneActorRegistryResetView *actor)
{
    return actor->state2A8;
}
static inline u8 OctopusInit_GetPhase(struct SceneActorRegistryResetView *actor)
{
    return actor->phase2A9;
}
static inline u8 OctopusInit_GetBehavior(struct SceneActorRegistryResetView *actor)
{
    return actor->behavior2AA;
}
static inline void *OctopusInit_GetCurrentControl(struct SceneActorRegistryResetView *actor)
{
    return actor->target;
}
static inline u8 OctopusInit_GetControlKind(struct SceneActorRegistryResetView *actor)
{
    return actor->field2B5;
}
static inline u8 OctopusInit_GetDirection(struct SceneActorRegistryResetView *actor)
{
    return actor->body.direction;
}
static inline u32 OctopusInit_GetTerrainCell(struct SceneActorRegistryResetView *actor)
{
    return actor->terrainCell;
}
static inline u32 OctopusInit_GetTargetingFlags(struct SceneActorChaseTracking *target)
{
    return target->flags;
}
static inline struct SceneActorDefaultControl *
OctopusInit_SavedControlOf(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorDefaultControl *saved = &actor->saved;
    return saved;
}

static inline void OctopusInit_SaveControl(struct SceneActorDefaultControl *saved,
                                           struct SceneActorRegistryResetView *actor)
{
    saved->motionFlags = OctopusInit_GetMotionFlags(actor);
    saved->heightOffset = OctopusInit_GetHeightOffset(actor);
    saved->behaviorFlags = OctopusInit_GetBehaviorFlags(actor);
    saved->statusFlags = OctopusInit_GetStatusFlags(actor);
    saved->direction = OctopusInit_GetDirection(actor);
    saved->state = OctopusInit_GetState(actor);
    saved->phase = OctopusInit_GetPhase(actor);
    saved->behavior = OctopusInit_GetBehavior(actor);
    saved->control = OctopusInit_GetCurrentControl(actor);
    saved->kind = OctopusInit_GetControlKind(actor);
}

static inline void OctopusInit_ClearColliderFlags(struct ContactCollider *collider, u32 mask)
{
    collider->flags = mask & collider->flags;
}
static inline s32 OctopusInit_Half(s32 value)
{
    if (value >= 0)
        return value >> 1;
    return -(-value >> 1);
}
static inline void OctopusInit_SetBehaviorFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    actor->flags2CC |= mask;
}

static inline void OctopusInit_SetStatusFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    actor->flags2D0 |= mask;
}

static inline void OctopusInit_SetMotionFlags(struct SceneActorRegistryResetView *actor, u16 mask)
{
    actor->flags2C0 |= mask;
}
static inline u8 OctopusInit_TestMotionFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    if (OctopusInit_GetMotionFlags(actor) & mask)
        return 1;
    return 0;
}

static inline u8 OctopusInit_TestTargetingFlags(struct SceneActorChaseTracking *target, u32 mask)
{
    if (OctopusInit_GetTargetingFlags(target) & mask)
        return 1;
    return 0;
}

static inline void OctopusInit_ClearControlFlags(struct SceneActorRegistryResetView *actor,
                                                 u32 mask)
{
    actor->flags114 &= mask;
}
static inline u8 OctopusInit_IsInCurrentRegion(struct SceneActorRegistryResetView *actor)
{
    if (*(u32 *)(gGameState + 0x5A0) == actor->regionId)
        return 1;
    return 0;
}
static inline void *OctopusInit_GetStandingControl(struct SceneActorRegistryResetView *actor)
{
    return actor->savedTarget;
}

static inline void OctopusInit_SetControl(struct SceneActorRegistryResetView *actor, void *callback,
                                          u32 behavior)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = behavior;
    actor->target = callback;
    actor->field2C8 = 0;
    OctopusInit_ClearControlFlags(actor, ~1);
    actor->word104 = 0;
}
static inline void OctopusInit_SetState(struct SceneActorRegistryResetView *actor, u32 state,
                                        u32 phase, u32 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

static inline void OctopusInit_InitializeGate(struct SceneActorControlGate *gate, u32 parameter)
{
    gate->flags = 0;
    gate->duration = parameter;
    gate->counter = 0;
    gate->field41A = 0;
    gate->control = 0;
}

static inline void OctopusInit_ReadTargeting(struct SceneActorChaseTracking *target)
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
    if (OctopusInit_TestTargetingFlags(target, 4))
        target->range = target->limit;
    else
        target->range = Div(target->limit * 8 - target->limit, 10);
}

static inline u16 OctopusInit_RangeRadius(struct SceneActorRegistryControl *range)
{
    return range->radius;
}
static inline void OctopusInit_ReadRange(struct SceneActorRegistryControl *range)
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
    range->radiusSquared = OctopusInit_RangeRadius(range) * OctopusInit_RangeRadius(range);
    range->mode = 0;
}

static inline u32 OctopusInit_RoutePointX(struct SceneActorRegistryRoute *route)
{
    return (u16)route->point->x;
}

static inline u32 OctopusInit_RoutePointZ(struct SceneActorRegistryRoute *route)
{
    return (u16)route->point->z;
}

static inline void OctopusInit_SnapToRoute(struct ContactPosition *position,
                                           struct SceneActorRegistryRoute *route)
{
    position->x = OctopusInit_RoutePointX(route);
    position->z = OctopusInit_RoutePointZ(route);
    position->y = MonsterInit_TerrainHeight(MonsterInit_CellIndex(position), position);
}
static inline void OctopusInit_DrawablePosition(struct SceneActorRegistryResetView *actor)
{
    if (OctopusInit_GetDrawableKind(actor) == 0)
        *(struct ContactPosition *)(actor->drawable + 0x1C) = actor->body.position;
    else
        *(struct ContactPosition *)(actor->drawable + 0x40) = actor->body.position;
}

static inline u8 OctopusInit_RouteId(struct SceneActorRegistryRoute *route)
{
    return route->routeId;
}
static inline u8 OctopusInit_RouteParameter(struct SceneActorRegistryRoute *route)
{
    return route->parameterB;
}
static inline u8 OctopusInit_RoutePointId(struct SceneActorRegistryRoute *route)
{
    return route->pointId;
}
static inline u8 OctopusInit_RouteActiveBranch(struct SceneActorRegistryRoute *route)
{
    return route->activeBranch;
}

static inline s32 OctopusInit_ClampHealth(u16 raw)
{
    s32 health = (s16)raw;
    if (health <= 0)
        health = 0;
    return health;
}

static inline void OctopusInit_InstallIdle(struct SceneActorRegistryResetView *actor, void *idle)
{
    actor->target = idle;
}

static inline void OctopusInit_InitializeHealth(struct SceneActorRegistryResetView *arg0,
                                                u16 initialHealth, s32 firstLimit, s32 secondLimit)
{
    arg0->flags2D4 = 0;
    OctopusInit_InstallIdle(arg0, SceneActorControl_OnIdle);
    arg0->field2B2 = 1;
    arg0->field2B0 = 0;
    arg0->health = OctopusInit_ClampHealth(initialHealth);
    arg0->fieldAA = 0;
    if (secondLimit <= 0)
        secondLimit = firstLimit;
    arg0->attachmentLimit = firstLimit;
    arg0->fieldA8 = secondLimit;
    arg0->regionId = 0;
}

static inline void OctopusInit_SetInitialHealth(struct SceneActorRegistryResetView *actor,
                                                u16 health)
{
    actor->defaultHealth = health;
}

static inline u32 OctopusInit_RouteAlternateBranch(struct SceneActorRegistryRoute *route)
{
    return 1 ^ route->activeBranch;
}
static inline void OctopusInit_BeginRoute(struct SceneActorBody *body,
                                          struct SceneActorRegistryRoute *route)
{
    SceneBody_StartRoute(body, route, OctopusInit_RouteId(route), OctopusInit_RouteParameter(route),
                         OctopusInit_RoutePointId(route));
}

static inline void OctopusInit_SetFirstAttack(struct SceneActorRegistryResetView *actor, s32 index,
                                              s32 value)
{
    actor->attacks.first[index] = value;
}

static inline u32 OctopusInit_ReadRoute(struct SceneActorRegistryResetView *arg0,
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
        rt->branches[OctopusInit_RouteActiveBranch(rt)] = rt->routeId;
        rt->branches[OctopusInit_RouteAlternateBranch(rt)] =
            Script_GetPc() ? Script_GetValue() : OctopusInit_RouteId(rt);
        Path_GetPointPositionById((u16 *)&point, OctopusInit_RouteId(rt),
                                  OctopusInit_RoutePointId(rt));
        arg0->terrainCell = MonsterInit_CellIndex(&point);
        point.y = arg0->groundHeight2BE =
            MonsterInit_TerrainHeight(OctopusInit_GetTerrainCell(arg0), &point);
        OctopusInit_BeginRoute(&arg0->body, rt);
        return 1;
    } else {
        rt->routeId = 0;
        rt->parameterB = 0;
        rt->pointId = 0;
        rt->currentPoint = 0;
        group->homeRoute.branches[OctopusInit_RouteActiveBranch(rt)] = 0;
        group->homeRoute.branches[OctopusInit_RouteAlternateBranch(rt)] = 0;
    }

    return 0;
}

static inline void OctopusInit_InitializeBody(void *object, u32 *fill)
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
            OctopusInit_SetFirstAttack(arg0, i, MonsterInit_ReadHalfword(0x18));
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
    OctopusInit_InitializeGate(&group->common.gate, r2v);
    OctopusInit_ReadRange(&group->common.control);
    OctopusInit_ReadTargeting(&arg0->tracking);
    if (Script_GetKeywordValue(0x62, 0))
        OctopusInit_SetStatusFlags(arg0, 0x200);
    arg0->field1F0 = MonsterInit_ReadKeywordHalfword(0x46, 0);
    arg0->field1EC = MonsterInit_ReadKeywordHalfword(0x52, 0);

    if (Script_SeekToKeyword(0x41)) {
        OctopusInit_SetInitialHealth(arg0, MonsterInit_ReadHalfword(0));
        group->halfHealth = OctopusInit_Half(OctopusInit_GetInitialHealth(arg0));
        arg0->field9E = MonsterInit_ReadByte(0);
        arg0->field9F = MonsterInit_ReadByte(0);
        arg0->fieldA0 = MonsterInit_ReadByte(0);
        arg0->fieldA2 = MonsterInit_ReadHalfword(0);
        arg0->fieldA4 = MonsterInit_ReadHalfword(0);
    }

    OctopusInit_InitializeHealth(arg0, OctopusInit_GetInitialHealth(arg0), r5v, r6v);

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
        OctopusInit_SetStatusFlags(arg0, 0x800);
    if (Script_GetKeywordValue(0x4C, 0))
        OctopusInit_SetStatusFlags(arg0, 0x8000);

    SceneBody_Init(body, mode, &origin, 0, 4, arg0);
    rt = &group->homeRoute;
    if (OctopusInit_ReadRoute(arg0, group, rt)) {
        OctopusInit_SnapToRoute(&arg0->body.position, rt);
        OctopusInit_DrawablePosition(arg0);
        arg0->body.direction = Route_GetCurrentPointFacing(rt);
    }

    if (OctopusInit_TestMotionFlags(arg0, 0x200))
        TargetRegistry_RegisterActor((u8 *)group + 0x30, body, sub_08080BE0);
    SceneBody_SetSprite(body, OctopusInit_GetDrawableKind(arg0) == 0 ? arg0->drawable : 0);
    if (OctopusInit_GetCategory(arg0) == 0x15)
        SceneBody_SetMovementProbe(body, (u8 *)arg0 + 0x40, 0x3C, 0x3C);
    else
        SceneBody_SetMovementProbe(body, (u8 *)arg0 + 0x40, 0x40, 0x40);

    group->common.bodyId = arg0->body.id;
    group->common.bodyPosition = &arg0->body.position;
}

struct OctopusInitWidget {
    u8 unknown0[0x3A];
    u16 paletteBank;
};
static inline void OctopusInit_SetWidgetScriptPalette(struct OctopusInitWidget *widget,
                                                      s32 scriptPalette)
{
    widget->paletteBank = scriptPalette - 0x5FFF;
}
static inline void OctopusInit_SetMotionControl(struct SceneActorRegistryResetView *actor,
                                                void *callback, u32 kind)
{
    actor->word104 = (u32)callback;
    actor->word108 = 0;
    actor->word10C = 0;
    actor->word110 = kind;
    actor->flags114 = 1;
}
static inline void OctopusInit_ClearMotion(void *object)
{
    struct SceneActorRegistryResetView *actor = object;
    u32 zero = 0;
    OctopusInit_SetMotionControl(actor, 0, 0);
    actor->flags114 = zero;
}
static inline void OctopusInit_ReadPalette(struct SceneActorRegistryResetView *actor, u32 base,
                                           u32 alternate)
{
    s32 r1v;
    actor->paletteOffset = Script_GetKeywordValue(0x43, 0);
    actor->basePaletteBank = base;
    actor->paletteAlternateBank = alternate;
    actor->alternateOffset = OctopusInit_GetPaletteOffset(actor) + base;
    r1v = OctopusInit_GetPaletteBase(actor) + OctopusInit_GetPaletteOffset(actor);
    if (OctopusInit_GetDrawableKind(actor) == 0)
        AnimData_SelectPaletteBank(actor->drawable + 0x2C, r1v);
    else
        *(u8 **)(actor->drawable + 0x68) = gPaletteBankData + (r1v << 5);
    if (OctopusInit_GetDrawableKind(actor) == 1)
        OctopusInit_SetWidgetScriptPalette((void *)(actor->drawable + 0x20),
                                           SceneActorScript_GetManagerField62());
}

static inline void OctopusInit_InstallPrimary(struct SceneActorRegistryResetView *arg0, void *cb0,
                                              u32 (*cb1)(void *), void *cb2, void *cb3, void *cb4,
                                              void *hidden, void *cb5, void *cb6,
                                              void *defaultReaction, void *cb270, void *targetQuery,
                                              void *damage, void *contact)
{
    OctopusInit_SetBehaviorFlags(arg0, 0x2000);
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
    OctopusInit_ClearMotion(arg0);
}

static inline void OctopusInit_InitializeGroup(struct SceneActorChaseGroup *group,
                                               void *firstBounds, void *secondBounds, void *downed,
                                               void *returnToRoute, void *recover, void *animate)
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

static inline void OctopusInit_InitializeRegion(struct SceneActorRegistryResetView *arg0)
{
    if (OctopusInit_TestMotionFlags(arg0, 2))
        arg0->regionId = *(u32 *)(gGameState + 0x5A0);
    else
        arg0->regionId = (u16)(u32)Region_FindContaining(&arg0->body.position);
}

static inline void OctopusInit_InstallCompletionCallbacks(struct SceneActorRegistryResetView *arg0)
{
    arg0->messageCallback = SceneActorScript_OnCommand;
    arg0->updateCallback = Octopus_UpdateInteraction;
    arg0->destroyCallback = Octopus_Destroy;
}

static inline u8 *OctopusInit_Drawable(struct SceneActorRegistryResetView *actor)
{
    return actor->drawable;
}

static inline void OctopusInit_SetCategory(void *object, u8 category)
{
    struct SceneActorRegistryResetView *actor = object;
    actor->category = category;
}

static inline void OctopusInit_InitializeSprite(u8 *drawable, u16 resource, u16 sequence)
{
    AnimData_Load(drawable + 0x2C, resource);
    Sprite_Init(drawable, drawable + 0x2C, 0);
    SceneActorAnimation_InitEnemySequence(drawable, sequence);
}

static inline void OctopusInit_InstallRoutes(struct SceneActorRegistryResetView *actor)
{
    actor->callback248 = SceneActorControl_UpdateMotionPhase_080EBD0C;
    MonsterInit_WriteDataPointer(&actor->commandTable.opaque, gUnk_0860635C);
}
static inline void OctopusInit_ClearReceiver(struct SceneActorRegistryResetView *actor)
{
    actor->field304 = 0;
}

static inline void OctopusInit_InitializeWidget(u8 *drawable, u16 script, u16 resource,
                                                struct ContactPosition *position)
{
    void *source = ScriptProc_Start(script, resource);
    *(struct OctopusInit32Block *)drawable = *(struct OctopusInit32Block *)source;
    ResourceHeader_Relocate(drawable, source);
    Widget_InitLinked(drawable + 0x20, drawable, 0, 0, 2, 2, 0x3C, position);
}

static inline void OctopusInit_ResetCapture(void *object)
{
    struct SceneActorRegistryResetView *actor = object;
    ((struct OctopusInstanceState *)actor)->captureFlags = 0;
}

s32 Octopus_Init(void *object)
{
    struct SceneActorRegistryResetView *arg0 = object;
    struct SceneActorRegistryResetView *actor = object;
    struct SceneActorChaseGroup *group = &arg0->group;
    u32 zero;
    struct ContactPosition tmp;
    u32 flag;

    OctopusInit_ResetCapture(arg0);
    SceneActorRegistry_SetObjectContext(actor, actor, 0, 1);
    OctopusInit_SetCategory(actor, 14);
    Octopus_InitProfileId(actor);
    if (OctopusInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->drawable, 0x5C);
        heap = OctopusInit_Drawable(actor);
        OctopusInit_InitializeSprite(heap, 0xD291, 0xDCEB);
        if (OctopusInit_GetCategory(actor) == 0x11 || OctopusInit_GetCategory(actor) == 0x17)
            heap[7] = 1;
        actor->body.direction = 0;
    } else {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->drawable, 0x80);
        heap = OctopusInit_Drawable(actor);
        MonsterInit_ClearPosition(&zero, &tmp);
        OctopusInit_InitializeWidget(heap, 0xCB05, 0xD291, &tmp);
        actor->animationIndex = 0xFF;
    }
    OctopusInit_InitializeBody(actor, &zero);
    if (Script_SeekToKeyword('P')) {
        ((struct OctopusInstanceState *)arg0)->field636 = MonsterInit_ReadHalfword(0);
        ((struct OctopusInstanceState *)arg0)->field63A = MonsterInit_ReadByte(0);
        ((struct OctopusInstanceState *)arg0)->field639 = MonsterInit_ReadByte(0);
    }
    actor->body.position.x += 0x20;
    actor->body.position.z += 0x20;
    OctopusInit_DrawablePosition(actor);
    OctopusInit_ReadPalette(actor, 0x248, 0x192);
    SceneActorCollision_InitCompactBody(actor);
    OctopusInit_InstallPrimary(actor, Octopus_Update, 0, 0, 0, 0, 0, 0, Octopus_UpdateDefeat,
                               SceneActorReaction_OnEnemyDefault, 0, 0, sub_081363F4,
                               SceneActorControl_BeginSoundTransition_080EBC38);
    OctopusInit_InitializeGroup(group, 0, 0, 0, 0, 0, 0);
    OctopusInit_InstallRoutes(actor);
    OctopusInit_SetBehaviorFlags(actor, 0x200);
    OctopusInit_SetStatusFlags(actor, 0x408);
    OctopusInit_SetMotionFlags(actor, 0x40);
    if (Script_GetKeywordValue('E', 0))
        OctopusInit_SetStatusFlags(actor, 0x2000000);
    SceneActorTerrain_ReadScriptParameters(actor);
    flag = 0;
    if (OctopusInit_TestMotionFlags(actor, 4)) {
        OctopusInit_SetStatusFlags(actor, 0x2000);
        flag = 1;
    }
    if (flag != 1) {
        OctopusInit_SetState(actor, 0, 0, 0);
        OctopusInit_SetControl(actor, OctopusInit_GetStandingControl(actor), 0);
    }
    OctopusInit_InitializeRegion(actor);
    if (OctopusInit_TestMotionFlags(actor, 0x40))
        OctopusInit_ClearColliderFlags((void *)((u8 *)actor + 0x134), ~0x100);
    if (Script_GetKeywordValue('S', 0) != 0 || !OctopusInit_IsInCurrentRegion(actor))
        SceneActorPlacement_HideActor(actor, 0);
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    if (OctopusInit_GetDrawableKind(actor) == 0)
        DisplaySequence_Step(actor->drawable);
    else
        Anim_Step(actor->drawable + 0x20, actor->drawable);
    OctopusInit_ClearReceiver(actor);
    SceneActorBehavior_InstallCallbackTables(actor);
    Octopus_InitControlPresets(actor);
    Octopus_BeginIdleAnimation(actor);
    OctopusInit_SaveControl(OctopusInit_SavedControlOf(actor), actor);
    OctopusInit_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}
