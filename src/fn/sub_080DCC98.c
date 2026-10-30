#include "global.h"
#include "render.h"
#include "engine_globals.h"
#include "display_sequence_inline.h"
#include "scene_actor_init_primitives.h"
#include "scene_actor_init_actor.h"

struct MummyResourceBlock {
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

struct MummyInitContext {
    u8 unknown0[0x6CC];
    u16 field6CC, field6CE;
    u8 unknown6D0[8];
    u32 field6D8, field6DC;
};

struct MummyInstanceState {
    u8 unknown0[0x624];
    u8 unknown624[0xC4];
    u16 field6E8;
    u8 unknown6EA[8];
    u16 field6F2;
};

static inline u8 *MummyInit_Drawable(struct SceneActorRegistryResetView *actor)
{
    return actor->drawable;
}
static inline u8 MummyInit_GetDrawableKind(struct SceneActorRegistryResetView *actor)
{
    return actor->drawableKind;
}
static inline void MummyInit_SetCategory(void *object, u8 category)
{
    struct SceneActorRegistryResetView *actor = object;
    actor->category = category;
}
static inline u8 MummyInit_GetCategory(void *object)
{
    struct SceneActorRegistryResetView *actor = object;
    return actor->category;
}

static inline u16 MummyInit_GetInitialHealth(struct SceneActorRegistryResetView *actor)
{
    return actor->defaultHealth;
}

static inline u32 MummyInit_GetPaletteBase(struct SceneActorRegistryResetView *actor)
{
    return actor->basePaletteBank;
}

static inline u32 MummyInit_GetPaletteOffset(struct SceneActorRegistryResetView *actor)
{
    return actor->paletteOffset;
}

void SceneActorBehavior_InitializeShadow(void *);
extern u8 *gUnknown_02000580;

void ProximityVolume_TranslateBounds(void *, void *);
void SceneActorMovement_ResolveRoutePosition(void *);
u8 sub_080D7568(void *);
void SceneActorPlacement_ResetStateThree(void);
void SceneActorReaction_BeginTargetRecovery(void);
extern const u8 gUnk_086060FC[];

static inline u32 MummyInit_GetMotionFlags(struct SceneActorRegistryResetView *actor)
{
    return actor->flags2C0;
}
static inline u16 MummyInit_GetHeightOffset(struct SceneActorRegistryResetView *actor)
{
    return actor->field2EA;
}
static inline u32 MummyInit_GetBehaviorFlags(struct SceneActorRegistryResetView *actor)
{
    return actor->flags2CC;
}
static inline u32 MummyInit_GetStatusFlags(struct SceneActorRegistryResetView *actor)
{
    return actor->flags2D0;
}
static inline u8 MummyInit_GetState(struct SceneActorRegistryResetView *actor)
{
    return actor->state2A8;
}
static inline u8 MummyInit_GetPhase(struct SceneActorRegistryResetView *actor)
{
    return actor->phase2A9;
}
static inline u8 MummyInit_GetBehavior(struct SceneActorRegistryResetView *actor)
{
    return actor->behavior2AA;
}
static inline void *MummyInit_GetCurrentControl(struct SceneActorRegistryResetView *actor)
{
    return actor->target;
}
static inline u8 MummyInit_GetControlKind(struct SceneActorRegistryResetView *actor)
{
    return actor->field2B5;
}
static inline u8 MummyInit_GetDirection(struct SceneActorRegistryResetView *actor)
{
    return actor->body.direction;
}
static inline u32 MummyInit_GetTerrainCell(struct SceneActorRegistryResetView *actor)
{
    return actor->terrainCell;
}
static inline u8 MummyInit_GetTargetingFlags(struct SceneActorChaseTracking *target)
{
    return target->flags;
}
static inline struct SceneActorDefaultControl *
MummyInit_SavedControlOf(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorDefaultControl *saved = &actor->saved;
    return saved;
}
static inline struct ContactPosition *MummyInit_BodyPosition(void *object)
{
    struct SceneActorBody *body = object;
    struct ContactPosition *position = &body->position;
    return position;
}
static inline void MummyInit_SaveControl(struct SceneActorDefaultControl *saved,
                                         struct SceneActorRegistryResetView *actor)
{
    saved->motionFlags = MummyInit_GetMotionFlags(actor);
    saved->heightOffset = MummyInit_GetHeightOffset(actor);
    saved->behaviorFlags = MummyInit_GetBehaviorFlags(actor);
    saved->statusFlags = MummyInit_GetStatusFlags(actor);
    saved->direction = MummyInit_GetDirection(actor);
    saved->state = MummyInit_GetState(actor);
    saved->phase = MummyInit_GetPhase(actor);
    saved->behavior = MummyInit_GetBehavior(actor);
    saved->control = MummyInit_GetCurrentControl(actor);
    saved->kind = MummyInit_GetControlKind(actor);
}
static inline void MummyInit_ClearColliderFlags(struct ContactCollider *collider, u32 mask)
{
    collider->flags = mask & collider->flags;
}
static inline s32 MummyInit_Half(s32 value)
{
    if (value >= 0)
        return value >> 1;
    return -(-value >> 1);
}
static inline void MummyInit_SetBehaviorFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    actor->flags2CC |= mask;
}

static inline void MummyInit_SetStatusFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    actor->flags2D0 |= mask;
}

static inline u8 MummyInit_TestMotionFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    if (MummyInit_GetMotionFlags(actor) & mask)
        return 1;
    return 0;
}

static inline u8 MummyInit_TestTargetingFlags(struct SceneActorChaseTracking *target, u32 mask)
{
    if (MummyInit_GetTargetingFlags(target) & mask)
        return 1;
    return 0;
}
static inline void MummyInit_SetTargetingFlags(struct SceneActorChaseTracking *target, u8 mask)
{
    mask |= target->flags;
    target->flags = mask;
}
static inline void MummyInit_ClearControlFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    actor->flags114 &= mask;
}
static inline u8 MummyInit_IsInCurrentRegion(struct SceneActorRegistryResetView *actor)
{
    if (*(u32 *)(gGameState + 0x5A0) == actor->regionId)
        return 1;
    return 0;
}
static inline void *MummyInit_GetStandingControl(struct SceneActorRegistryResetView *actor)
{
    return actor->savedTarget;
}
static inline void MummyInit_SetControl(struct SceneActorRegistryResetView *actor, void *callback,
                                        u32 behavior)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = behavior;
    actor->target = callback;
    actor->field2C8 = 0;
    MummyInit_ClearControlFlags(actor, ~1);
    actor->word104 = 0;
}
static inline void MummyInit_SetState(struct SceneActorRegistryResetView *actor, u32 state,
                                      u32 phase, u32 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
static inline void MummyInit_ResetFlags(void *object)
{
    struct MummyInstanceState *actor = object;
    actor->field6F2 = 0;
}

static inline void MummyInit_InitializeGate(struct SceneActorControlGate *gate, u16 parameter)
{
    gate->flags = 0;
    gate->duration = parameter;
    gate->counter = 0;
    gate->field41A = 0;
    gate->control = 0;
}

static inline void MummyInit_ReadTargeting(struct SceneActorChaseTracking *target)
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
    if (MummyInit_TestTargetingFlags(target, 4))
        target->range = target->limit;
    else
        target->range = Div(target->limit * 8 - target->limit, 10);
}

static inline u16 MummyInit_RangeRadius(struct SceneActorRegistryControl *range)
{
    return range->radius;
}
static inline void MummyInit_ReadRange(struct SceneActorRegistryControl *range)
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
    range->radiusSquared = MummyInit_RangeRadius(range) * MummyInit_RangeRadius(range);
    range->mode = 0;
}

static inline u32 MummyInit_RoutePointX(struct SceneActorRegistryRoute *route)
{
    return (u16)route->point->x;
}
static inline u32 MummyInit_RoutePointZ(struct SceneActorRegistryRoute *route)
{
    return (u16)route->point->z;
}
static inline void MummyInit_SnapToRoute(struct ContactPosition *position,
                                         struct SceneActorRegistryRoute *route)
{
    position->x = MummyInit_RoutePointX(route);
    position->z = MummyInit_RoutePointZ(route);
    position->y = MonsterInit_TerrainHeight(MonsterInit_CellIndex(position), position);
}
static inline void MummyInit_DrawablePosition(struct SceneActorRegistryResetView *actor)
{
    if (MummyInit_GetDrawableKind(actor) == 0)
        *(struct ContactPosition *)(actor->drawable + 0x1C) = actor->body.position;
    else
        *(struct ContactPosition *)(actor->drawable + 0x40) = actor->body.position;
}
static inline void MummyInit_SetFirstAttack(struct SceneActorRegistryResetView *actor, s32 index,
                                            s32 value)
{
    actor->attacks.first[index] = value;
}

static inline u8 MummyInit_RouteId(struct SceneActorRegistryRoute *route)
{
    return route->routeId;
}
static inline u8 MummyInit_RouteParameter(struct SceneActorRegistryRoute *route)
{
    return route->parameterB;
}
static inline u8 MummyInit_RoutePointId(struct SceneActorRegistryRoute *route)
{
    return route->pointId;
}
static inline u8 MummyInit_RouteActiveBranch(struct SceneActorRegistryRoute *route)
{
    return route->activeBranch;
}
static inline u32 MummyInit_RouteAlternateBranch(struct SceneActorRegistryRoute *route)
{
    return 1 ^ route->activeBranch;
}
static inline void MummyInit_BeginRoute(struct SceneActorBody *body,
                                        struct SceneActorRegistryRoute *route)
{
    SceneBody_StartRoute(body, route, MummyInit_RouteId(route), MummyInit_RouteParameter(route),
                         MummyInit_RoutePointId(route));
}
static inline s32 MummyInit_ClampHealth(u16 raw)
{
    s32 health = (s16)raw;
    if (health <= 0)
        health = 0;
    return health;
}

static inline void MummyInit_InstallIdle(struct SceneActorRegistryResetView *actor, void *idle)
{
    actor->target = idle;
}

static inline void MummyInit_InitializeHealth(struct SceneActorRegistryResetView *arg0,
                                              u16 initialHealth, s32 firstLimit, s32 secondLimit)
{
    arg0->flags2D4 = 0;
    MummyInit_InstallIdle(arg0, SceneActorControl_OnIdle);
    arg0->field2B2 = 1;
    arg0->field2B0 = 0;
    arg0->health = MummyInit_ClampHealth(initialHealth);
    arg0->fieldAA = 0;
    if (secondLimit <= 0)
        secondLimit = firstLimit;
    arg0->attachmentLimit = firstLimit;
    arg0->fieldA8 = secondLimit;
    arg0->regionId = 0;
}

static inline void MummyInit_SetInitialHealth(struct SceneActorRegistryResetView *actor, u16 health)
{
    actor->defaultHealth = health;
}

static inline u32 MummyInit_ReadRoute(struct SceneActorRegistryResetView *arg0,
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
        rt->branches[MummyInit_RouteActiveBranch(rt)] = rt->routeId;
        rt->branches[MummyInit_RouteAlternateBranch(rt)] =
            Script_GetPc() ? Script_GetValue() : MummyInit_RouteId(rt);
        Path_GetPointPositionById((u16 *)&point, MummyInit_RouteId(rt), MummyInit_RoutePointId(rt));
        arg0->terrainCell = MonsterInit_CellIndex(&point);
        point.y = arg0->groundHeight2BE =
            MonsterInit_TerrainHeight(MummyInit_GetTerrainCell(arg0), &point);
        MummyInit_BeginRoute(&arg0->body, rt);
        return 1;
    } else {
        rt->routeId = 0;
        rt->parameterB = 0;
        rt->pointId = 0;
        rt->currentPoint = 0;
        group->homeRoute.branches[MummyInit_RouteActiveBranch(rt)] = 0;
        group->homeRoute.branches[MummyInit_RouteAlternateBranch(rt)] = 0;
    }

    return 0;
}
static inline void MummyInit_InitializeBody(void *object, u32 *fill)
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
            MummyInit_SetFirstAttack(arg0, i, MonsterInit_ReadHalfword(0x18));
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
    MummyInit_InitializeGate(&group->common.gate, gateDuration);
    MummyInit_ReadRange(&group->common.control);
    MummyInit_ReadTargeting(&arg0->tracking);
    if (Script_GetKeywordValue(0x62, 0))
        MummyInit_SetStatusFlags(arg0, 0x200);
    arg0->field1F0 = MonsterInit_ReadKeywordHalfword(0x46, 0);
    arg0->field1EC = MonsterInit_ReadKeywordHalfword(0x52, 0);

    if (Script_SeekToKeyword(0x41)) {
        MummyInit_SetInitialHealth(arg0, MonsterInit_ReadHalfword(0));
        group->halfHealth = MummyInit_Half(MummyInit_GetInitialHealth(arg0));
        arg0->field9E = MonsterInit_ReadByte(0);
        arg0->field9F = MonsterInit_ReadByte(0);
        arg0->fieldA0 = MonsterInit_ReadByte(0);
        arg0->fieldA2 = MonsterInit_ReadHalfword(0);
        arg0->fieldA4 = MonsterInit_ReadHalfword(0);
    }

    MummyInit_InitializeHealth(arg0, MummyInit_GetInitialHealth(arg0), firstLimit, secondLimit);

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
        MummyInit_SetStatusFlags(arg0, 0x800);
    if (Script_GetKeywordValue(0x4C, 0))
        MummyInit_SetStatusFlags(arg0, 0x8000);

    SceneBody_Init(body, mode, &origin, 0, 4, arg0);
    rt = &group->homeRoute;
    if (MummyInit_ReadRoute(arg0, group, rt)) {
        MummyInit_SnapToRoute(&arg0->body.position, rt);
        MummyInit_DrawablePosition(arg0);
        arg0->body.direction = Route_GetCurrentPointFacing(rt);
    }

    if (MummyInit_TestMotionFlags(arg0, 0x200))
        TargetRegistry_RegisterActor((u8 *)group + 0x30, body, sub_08080BE0);
    SceneBody_SetSprite(body, MummyInit_GetDrawableKind(arg0) == 0 ? arg0->drawable : 0);
    if (MummyInit_GetCategory(arg0) == 0x15)
        SceneBody_SetMovementProbe(body, (u8 *)arg0 + 0x40, 0x3C, 0x3C);
    else
        SceneBody_SetMovementProbe(body, (u8 *)arg0 + 0x40, 0x40, 0x40);

    group->common.bodyId = arg0->body.id;
    group->common.bodyPosition = &arg0->body.position;
}

struct MummyInitWidget {
    u8 unknown0[0x3A];
    u16 paletteBank;
};
static inline void MummyInit_SetWidgetScriptPalette(struct MummyInitWidget *widget, s32 palette)
{
    widget->paletteBank = palette - 0x5FFF;
}

static inline void MummyInit_SetMotionControl(struct SceneActorRegistryResetView *actor,
                                              void *callback, u32 kind)
{
    actor->word104 = (u32)callback;
    actor->word108 = 0;
    actor->word10C = 0;
    actor->word110 = kind;
    actor->flags114 = 1;
}
static inline void MummyInit_ClearMotion(void *object)
{
    struct SceneActorRegistryResetView *actor = object;
    MummyInit_SetMotionControl(actor, 0, 0);
    actor->flags114 = 0;
}
static inline void MummyInit_ReadPalette(struct SceneActorRegistryResetView *actor, u32 base,
                                         u32 alternate)
{
    s32 palette;
    actor->paletteOffset = Script_GetKeywordValue(0x43, 0);
    actor->basePaletteBank = base;
    actor->paletteAlternateBank = alternate;
    actor->alternateOffset = MummyInit_GetPaletteOffset(actor) + base;
    palette = MummyInit_GetPaletteBase(actor) + MummyInit_GetPaletteOffset(actor);
    if (MummyInit_GetDrawableKind(actor) == 0)
        AnimData_SelectPaletteBank(actor->drawable + 0x2C, palette);
    else
        *(u8 **)(actor->drawable + 0x68) = gPaletteBankData + (palette << 5);
    if (MummyInit_GetDrawableKind(actor) == 1)
        MummyInit_SetWidgetScriptPalette((void *)(actor->drawable + 0x20),
                                         SceneActorScript_GetManagerField62());
}

static inline void MummyInit_InstallPrimary(struct SceneActorRegistryResetView *arg0,
                                            s32 (*cb0)(void *), u16 (*cb1)(void *),
                                            void (*cb2)(void *), void (*cb3)(void *),
                                            void (*cb4)(void *), void (*hidden)(void *),
                                            void (*cb5)(void *), void (*cb6)(void *),
                                            u32 (*defaultReaction)(void), u8 (*cb270)(void *),
                                            void *targetQuery, void *damage, void *contact)
{
    MummyInit_SetBehaviorFlags(arg0, 0x2000);
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
    MummyInit_ClearMotion(arg0);
}

static inline void MummyInit_InitializeGroup(struct SceneActorChaseGroup *group, void *firstBounds,
                                             void *secondBounds, void *downed, void *returnToRoute,
                                             void *recover, void *animate, u8 active,
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
    group->common.control.active = active;
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
    ProximityVolume_TranslateBounds(firstBounds, MummyInit_BodyPosition(body));
}

static inline void MummyInit_SetRouteReset(struct SceneActorRegistryResetView *actor,
                                           void *callback)
{
    actor->callback248 = callback;
}
static inline void MummyInit_SetReactionGate(struct SceneActorRegistryResetView *actor,
                                             void *callback)
{
    actor->field23C = callback;
}

void Mummy_InitProfile(void *);
void Mummy_InitCollision(void *);
void Mummy_InitActionHandlers(void *);
void Mummy_SetBehaviorTable(void *);
s32 Mummy_OnUpdate(void *);
u16 Mummy_GetAnimationStepOffset(void *);
void Mummy_TurnTowardRoute(void *);
void Mummy_UpdateWalking(void *);
void Mummy_UpdateIdle(void *);
void Mummy_UpdateDisplacementRecovery(void *);
void Mummy_UpdateDefeat(void *);
void Mummy_UpdateHiddenRecovery(void *);
void SceneActorTarget_ResolvePendingObject(void);
void Mummy_UpdateDying(void);
void Mummy_ResumeSavedTarget(void);
void Mummy_OnUnusedState(void);
void Mummy_ResetRevivalMotion(void);
void SceneActor_EnterMotionPhase(void);
u8 Mummy_SelectMovementResponse(void *);
void Mummy_OnClearActivityFlag(void);
void Mummy_OnMovementUpdate(void);
void Mummy_OnInteractionUpdate(void);
void Mummy_OnReleaseResources(void);

static inline void MummyInit_SetContactCallback(struct SceneActorRegistryResetView *actor,
                                                void *callback)
{
    actor->contactCallback = callback;
}

static inline void MummyInit_EnableInitialFlags(struct SceneActorRegistryResetView *actor)
{
    MummyInit_SetTargetingFlags(&actor->tracking, 1);
    MummyInit_SetBehaviorFlags(actor, 0x600000);
    MummyInit_SetStatusFlags(actor, 0x8000000);
}

static inline void MummyInit_ReadInitialMode(struct SceneActorRegistryResetView *arg0,
                                             struct SceneActorChaseGroup *group)
{
    s32 i;
    i = Script_GetKeywordValue(0x75, 0);
    arg0->group.effect = 0x21;
    if (i != 0) {
        MummyInit_SetBehaviorFlags(arg0, 0x40000);
        if (i == 2)
            MummyInit_SetBehaviorFlags(arg0, 0x80000);
    }
}

static inline void MummyInit_InitializeRegion(struct SceneActorRegistryResetView *arg0)
{
    if (MummyInit_TestMotionFlags(arg0, 2))
        arg0->regionId = *(u32 *)(gGameState + 0x5A0);
    else
        arg0->regionId = (u16)(u32)Region_FindContaining(&arg0->body.position);
}

static inline struct MummyInitContext *MummyInit_Context(struct SceneActorRegistryResetView *actor)
{
    return actor->context;
}
static inline void MummyInit_InitializeReceiver(struct SceneActorRegistryResetView *actor)
{
    actor->field304 = 0;
}

static inline void MummyInit_InstallCompletionCallbacks(struct SceneActorRegistryResetView *arg0)
{
    arg0->messageCallback = SceneActorScript_OnCommand;
    arg0->updateCallback = Mummy_OnInteractionUpdate;
    arg0->destroyCallback = Mummy_OnReleaseResources;
}

static inline void MummyInit_InitializeSprite(u8 *drawable, u16 resource, u16 sequence)
{
    AnimData_Load(drawable + 0x2C, resource);
    Sprite_Init(drawable, drawable + 0x2C, 0);
    SceneActorAnimation_InitEnemySequence(drawable, sequence);
}

static inline void MummyInit_InitializeWidget(u8 *drawable, u16 script, u16 resource,
                                              struct ContactPosition *position)
{
    void *source = ScriptProc_Start(script, resource);
    *(struct MummyResourceBlock *)drawable = *(struct MummyResourceBlock *)source;
    ResourceHeader_Relocate(drawable, source);
    Widget_InitLinked(drawable + 0x20, drawable, 0, 0, 2, 2, 0x3C, position);
}

static inline void MummyInit_InitializePaletteRange(struct SceneActorRegistryResetView *actor,
                                                    u32 selected, u32 alternate, u32 current)
{
    actor->alternateOffset = selected;
    actor->alternatePalette = MummyInit_GetPaletteBase(actor) + alternate;
    actor->paletteBank = MummyInit_GetPaletteBase(actor) + current;
}

void Mummy_SetControlPreset(void *);
void SceneActorEffects_InitAtActorPosition(void *);
void sub_080D1644(void);
void sub_080D453C(void);

s32 Mummy_Init(void *object)
{
    struct SceneActorRegistryResetView *arg0 = object;
    struct SceneActorRegistryResetView *actor = object;
    struct SceneActorChaseGroup *group = &arg0->group;
    u32 zero;
    struct ContactPosition tmp;
    u32 flag;

    MummyInit_ResetFlags(arg0);
    SceneActorRegistry_SetObjectContext(actor, actor, 0, 1);
    MummyInit_SetCategory(actor, 1);
    Mummy_InitProfile(actor);
    if (MummyInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->drawable, 0x5C);
        heap = MummyInit_Drawable(actor);
        MummyInit_InitializeSprite(heap, 0x48DE, 0xA4B2);
        if (MummyInit_GetCategory(actor) == 0x11 || MummyInit_GetCategory(actor) == 0x17)
            heap[7] = 1;
        actor->body.direction = 0;
    } else {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->drawable, 0x80);
        heap = MummyInit_Drawable(actor);
        MonsterInit_ClearPosition(&zero, &tmp);
        MummyInit_InitializeWidget(heap, 0xCB05, 0x48DE, &tmp);
        actor->animationIndex = 0xFF;
    }
    MummyInit_InitializeBody(arg0, &zero);
    MummyInit_ReadPalette(actor, 0x23, 0x192);
    MummyInit_InitializePaletteRange(actor, MummyInit_GetPaletteBase(actor) + 3, 4, 0);
    Mummy_InitCollision(actor);
    MummyInit_InstallPrimary(actor, Mummy_OnUpdate, Mummy_GetAnimationStepOffset,
                             Mummy_TurnTowardRoute, Mummy_UpdateWalking, Mummy_UpdateIdle,
                             Mummy_UpdateHiddenRecovery, Mummy_UpdateDisplacementRecovery,
                             Mummy_UpdateDefeat, SceneActorReaction_OnEnemyDefault, sub_080D7568,
                             SceneActorTarget_ResolvePendingObject, sub_081363F4,
                             SceneActorPlacement_ResetStateThree);
    MummyInit_InitializeGroup(group, (u8 *)arg0 + 0x6F4, (u8 *)arg0 + 0x718, Mummy_UpdateDying,
                              Mummy_ResumeSavedTarget, SceneActorReaction_BeginTargetRecovery,
                              Mummy_OnUnusedState, 3, &actor->body);
    SceneActorMovement_ResolveRoutePosition(arg0);
    MummyInit_SetRouteReset(actor, Mummy_ResetRevivalMotion);
    MummyInit_SetReactionGate(actor, SceneActor_EnterMotionPhase);
    MummyInit_SetContactCallback(actor, Mummy_SelectMovementResponse);
    group->responseCallback = (u32)Mummy_OnClearActivityFlag;
    group->field5EC = Mummy_OnMovementUpdate;
    group->routeChoiceControl = (u32)sub_080D1644;
    group->common.fallback = sub_080D453C;
    actor->field2E8 = 1;
    MummyInit_EnableInitialFlags(actor);
    MummyInit_ClearColliderFlags((void *)((u8 *)actor + 0x134), ~1);
    SceneActorBehavior_InitializeShadow(actor);
    SceneActorEffects_InitAtActorPosition(arg0);
    SceneActorTerrain_ReadScriptParameters(actor);
    MummyInit_ReadInitialMode(arg0, group);
    flag = 0;
    if (MummyInit_TestMotionFlags(actor, 4)) {
        MummyInit_SetStatusFlags(actor, 0x2000);
        flag = 1;
    }
    if (flag != 1) {
        MummyInit_SetState(actor, 0, 0, 0);
        MummyInit_SetControl(actor, MummyInit_GetStandingControl(actor), 0);
    }
    MummyInit_InitializeRegion(actor);
    if (MummyInit_TestMotionFlags(actor, 0x40))
        MummyInit_ClearColliderFlags((void *)((u8 *)actor + 0x134), ~0x100);
    if (Script_GetKeywordValue(0x53, 0) != 0 || !MummyInit_IsInCurrentRegion(actor))
        SceneActorPlacement_HideActor(actor, 0);
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    if (MummyInit_GetDrawableKind(actor) == 0)
        DisplaySequence_Step(actor->drawable);
    else
        Anim_Step(actor->drawable + 0x20, actor->drawable);
    MummyInit_InitializeReceiver(actor);
    SceneActorBehavior_InstallCallbackTables(actor);
    Mummy_SetControlPreset(actor);
    Mummy_InitActionHandlers(actor);
    Mummy_SetBehaviorTable(actor);

    ((struct MummyInstanceState *)arg0)->field6E8 = actor->tracking.x;
    {
        struct MummyInitContext *context = MummyInit_Context(actor);
        if (Script_SeekToKeyword(0x42)) {
            context->field6CC = MonsterInit_ReadHalfword(0x14);
            context->field6CE = MonsterInit_ReadHalfword(0xB4);
        } else {
            context->field6CC = 0x14;
            context->field6CE = 0xB4;
        }
    }
    {
        struct MummyInitContext *context = MummyInit_Context(actor);
        context->field6D8 = Div(actor->tracking.range * 3, 10);
        context->field6DC = actor->tracking.range;
    }

    MummyInit_SaveControl(MummyInit_SavedControlOf(actor), actor);
    MummyInit_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}
