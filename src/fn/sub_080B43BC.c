#include "global.h"
#include "render.h"
#include "engine_globals.h"
#include "display_sequence_inline.h"
#include "scene_actor_init_primitives.h"
#include "scene_actor_state.h"

struct VornResourceBlock {
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

struct VornInstanceState {
    u8 unknown0[0x624];
    u8 unknown624[0x68];
    u8 field68C;
    u8 unknown68D[0x2C];
    u8 field6B9;
    u8 unknown6BA[0x1A];
    u16 field6D4;
};

static inline u8 *VornInit_Drawable(struct SceneActorRegistryResetView *actor)
{
    return actor->drawable;
}
static inline u8 VornInit_GetDrawableKind(struct SceneActorRegistryResetView *actor)
{
    return actor->drawableKind;
}
static inline void VornInit_SetCategory(void *object, u8 category)
{
    struct SceneActorRegistryResetView *actor = object;
    actor->category = category;
}
static inline u8 VornInit_GetCategory(void *object)
{
    struct SceneActorRegistryResetView *actor = object;
    return actor->category;
}

static inline u16 VornInit_GetInitialHealth(struct SceneActorRegistryResetView *actor)
{
    return actor->defaultHealth;
}

static inline u32 VornInit_GetPaletteBase(struct SceneActorRegistryResetView *actor)
{
    return actor->basePaletteBank;
}

static inline u32 VornInit_GetPaletteOffset(struct SceneActorRegistryResetView *actor)
{
    return actor->paletteOffset;
}

void SceneActorBehavior_InitializeShadow(void *);

void ProximityVolume_TranslateBounds(void *, void *);
void SceneActorMovement_ResolveRoutePosition(void *);
u8 sub_080B3C1C(void *);
void SceneActorPlacement_ResetStateThree(void);
extern const u8 gUnk_08606038[];

static inline u32 VornInit_GetMotionFlags(struct SceneActorRegistryResetView *actor)
{
    return actor->flags2C0;
}
static inline u16 VornInit_GetHeightOffset(struct SceneActorRegistryResetView *actor)
{
    return actor->field2EA;
}
static inline u32 VornInit_GetBehaviorFlags(struct SceneActorRegistryResetView *actor)
{
    return actor->flags2CC;
}
static inline u32 VornInit_GetStatusFlags(struct SceneActorRegistryResetView *actor)
{
    return actor->flags2D0;
}
static inline u8 VornInit_GetState(struct SceneActorRegistryResetView *actor)
{
    return actor->state2A8;
}
static inline u8 VornInit_GetPhase(struct SceneActorRegistryResetView *actor)
{
    return actor->phase2A9;
}
static inline u8 VornInit_GetBehavior(struct SceneActorRegistryResetView *actor)
{
    return actor->behavior2AA;
}
static inline void *VornInit_GetCurrentControl(struct SceneActorRegistryResetView *actor)
{
    return actor->target;
}
static inline u8 VornInit_GetControlKind(struct SceneActorRegistryResetView *actor)
{
    return actor->field2B5;
}
static inline u8 VornInit_GetDirection(struct SceneActorRegistryResetView *actor)
{
    return actor->body.direction;
}
static inline u32 VornInit_GetTerrainCell(struct SceneActorRegistryResetView *actor)
{
    return actor->terrainCell;
}
static inline u32 VornInit_GetTargetingFlags(struct SceneActorChaseTracking *target)
{
    return target->flags;
}
static inline struct SceneActorDefaultControl *
VornInit_SavedControlOf(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorDefaultControl *saved = &actor->saved;
    return saved;
}
static inline struct ContactPosition *VornInit_BodyPosition(void *object)
{
    struct SceneActorBody *body = object;
    struct ContactPosition *position = &body->position;
    return position;
}
static inline void VornInit_SaveControl(struct SceneActorDefaultControl *saved,
                                        struct SceneActorRegistryResetView *actor)
{
    saved->motionFlags = VornInit_GetMotionFlags(actor);
    saved->heightOffset = VornInit_GetHeightOffset(actor);
    saved->behaviorFlags = VornInit_GetBehaviorFlags(actor);
    saved->statusFlags = VornInit_GetStatusFlags(actor);
    saved->direction = VornInit_GetDirection(actor);
    saved->state = VornInit_GetState(actor);
    saved->phase = VornInit_GetPhase(actor);
    saved->behavior = VornInit_GetBehavior(actor);
    saved->control = VornInit_GetCurrentControl(actor);
    saved->kind = VornInit_GetControlKind(actor);
}
static inline void VornInit_ClearColliderFlags(struct ContactCollider *collider, u32 mask)
{
    collider->flags = mask & collider->flags;
}
static inline s32 VornInit_Half(s32 value)
{
    if (value >= 0)
        return value >> 1;
    return -(-value >> 1);
}
static inline void VornInit_SetBehaviorFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    actor->flags2CC |= mask;
}

static inline void VornInit_SetStatusFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    actor->flags2D0 |= mask;
}

static inline u8 VornInit_TestMotionFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    if (VornInit_GetMotionFlags(actor) & mask)
        return 1;
    return 0;
}

static inline u8 VornInit_TestTargetingFlags(struct SceneActorChaseTracking *target, u32 mask)
{
    if (VornInit_GetTargetingFlags(target) & mask)
        return 1;
    return 0;
}
static inline void VornInit_SetTargetingFlags(struct SceneActorChaseTracking *target, u8 mask)
{
    mask |= target->flags;
    target->flags = mask;
}
static inline void VornInit_ClearControlFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    actor->flags114 &= mask;
}
static inline u8 VornInit_IsInCurrentRegion(struct SceneActorRegistryResetView *actor)
{
    if (*(u32 *)(gGameState + 0x5A0) == actor->regionId)
        return 1;
    return 0;
}
static inline void *VornInit_GetStandingControl(struct SceneActorRegistryResetView *actor)
{
    return actor->savedTarget;
}
static inline void VornInit_SetControl(struct SceneActorRegistryResetView *actor, void *callback,
                                       u32 behavior)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = behavior;
    actor->target = callback;
    actor->field2C8 = 0;
    VornInit_ClearControlFlags(actor, ~1);
    actor->word104 = 0;
}
static inline void VornInit_SetState(struct SceneActorRegistryResetView *actor, u32 state,
                                     u32 phase, u32 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
static inline void VornInit_BeginBehavior(struct SceneActorRegistryResetView *actor, u32 behavior)
{
    VornInit_SetState(actor, 0, 0, behavior);
}
static inline void VornInit_ResetFlags(void *object)
{
    struct VornInstanceState *actor = object;
    actor->field6D4 = 0;
}

static inline void VornInit_InitializeGate(struct SceneActorControlGate *gate, u16 parameter)
{
    gate->flags = 0;
    gate->duration = parameter;
    gate->counter = 0;
    gate->field41A = 0;
    gate->control = 0;
}

static inline void VornInit_ReadTargeting(struct SceneActorChaseTracking *target)
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
    if (VornInit_TestTargetingFlags(target, 4))
        target->range = target->limit;
    else
        target->range = Div(target->limit * 8 - target->limit, 10);
}

static inline u16 VornInit_RangeRadius(struct SceneActorRegistryControl *range)
{
    return range->radius;
}
static inline void VornInit_ReadRange(struct SceneActorRegistryControl *range)
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
    range->radiusSquared = VornInit_RangeRadius(range) * VornInit_RangeRadius(range);
    range->mode = 0;
}

static inline u32 VornInit_RoutePointX(struct SceneActorRegistryRoute *route)
{
    return (u16)route->point->x;
}
static inline u32 VornInit_RoutePointZ(struct SceneActorRegistryRoute *route)
{
    return (u16)route->point->z;
}
static inline void VornInit_SnapToRoute(struct ContactPosition *position,
                                        struct SceneActorRegistryRoute *route)
{
    position->x = VornInit_RoutePointX(route);
    position->z = VornInit_RoutePointZ(route);
    position->y = MonsterInit_TerrainHeight(MonsterInit_CellIndex(position), position);
}
static inline void VornInit_DrawablePosition(struct SceneActorRegistryResetView *actor)
{
    if (VornInit_GetDrawableKind(actor) == 0)
        *(struct ContactPosition *)(actor->drawable + 0x1C) = actor->body.position;
    else
        *(struct ContactPosition *)(actor->drawable + 0x40) = actor->body.position;
}
static inline void VornInit_SetFirstAttack(struct SceneActorRegistryResetView *actor, s32 index,
                                           s32 value)
{
    actor->attacks.first[index] = value;
}

static inline u8 VornInit_RouteId(struct SceneActorRegistryRoute *route)
{
    return route->routeId;
}
static inline u8 VornInit_RouteParameter(struct SceneActorRegistryRoute *route)
{
    return route->parameterB;
}
static inline u8 VornInit_RoutePointId(struct SceneActorRegistryRoute *route)
{
    return route->pointId;
}
static inline u8 VornInit_RouteActiveBranch(struct SceneActorRegistryRoute *route)
{
    return route->activeBranch;
}
static inline u32 VornInit_RouteAlternateBranch(struct SceneActorRegistryRoute *route)
{
    return 1 ^ route->activeBranch;
}
static inline void VornInit_BeginRoute(struct SceneActorBody *body,
                                       struct SceneActorRegistryRoute *route)
{
    SceneBody_StartRoute(body, route, VornInit_RouteId(route), VornInit_RouteParameter(route),
                         VornInit_RoutePointId(route));
}
static inline s32 VornInit_ClampHealth(u16 raw)
{
    s32 health = (s16)raw;
    if (health <= 0)
        health = 0;
    return health;
}

static inline void VornInit_InstallIdle(struct SceneActorRegistryResetView *actor, void *idle)
{
    actor->target = idle;
}

static inline void VornInit_InitializeHealth(struct SceneActorRegistryResetView *arg0,
                                             u16 initialHealth, s32 firstLimit, s32 secondLimit)
{
    arg0->flags2D4 = 0;
    VornInit_InstallIdle(arg0, SceneActorControl_OnIdle);
    arg0->field2B2 = 1;
    arg0->field2B0 = 0;
    arg0->health = VornInit_ClampHealth(initialHealth);
    arg0->fieldAA = 0;
    if (secondLimit <= 0)
        secondLimit = firstLimit;
    arg0->attachmentLimit = firstLimit;
    arg0->fieldA8 = secondLimit;
    arg0->regionId = 0;
}

static inline void VornInit_SetInitialHealth(struct SceneActorRegistryResetView *actor, u16 health)
{
    actor->defaultHealth = health;
}

static inline u32 VornInit_ReadRoute(struct SceneActorRegistryResetView *arg0,
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
        rt->branches[VornInit_RouteActiveBranch(rt)] = rt->routeId;
        rt->branches[VornInit_RouteAlternateBranch(rt)] =
            Script_GetPc() ? Script_GetValue() : VornInit_RouteId(rt);
        Path_GetPointPositionById((u16 *)&point, VornInit_RouteId(rt), VornInit_RoutePointId(rt));
        arg0->terrainCell = MonsterInit_CellIndex(&point);
        point.y = arg0->groundHeight2BE =
            MonsterInit_TerrainHeight(VornInit_GetTerrainCell(arg0), &point);
        VornInit_BeginRoute(&arg0->body, rt);
        return 1;
    } else {
        rt->routeId = 0;
        rt->parameterB = 0;
        rt->pointId = 0;
        rt->currentPoint = 0;
        group->homeRoute.branches[VornInit_RouteActiveBranch(rt)] = 0;
        group->homeRoute.branches[VornInit_RouteAlternateBranch(rt)] = 0;
    }

    return 0;
}
static inline void VornInit_InitializeBody(void *object, u32 *fill)
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
            VornInit_SetFirstAttack(arg0, i, MonsterInit_ReadHalfword(0x18));
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
    VornInit_InitializeGate(&group->common.gate, gateDuration);
    VornInit_ReadRange(&group->common.control);
    VornInit_ReadTargeting(&arg0->tracking);
    if (Script_GetKeywordValue(0x62, 0))
        VornInit_SetStatusFlags(arg0, 0x200);
    arg0->field1F0 = MonsterInit_ReadKeywordHalfword(0x46, 0);
    arg0->field1EC = MonsterInit_ReadKeywordHalfword(0x52, 0);

    if (Script_SeekToKeyword(0x41)) {
        VornInit_SetInitialHealth(arg0, MonsterInit_ReadHalfword(0));
        group->halfHealth = VornInit_Half(VornInit_GetInitialHealth(arg0));
        arg0->field9E = MonsterInit_ReadByte(0);
        arg0->field9F = MonsterInit_ReadByte(0);
        arg0->fieldA0 = MonsterInit_ReadByte(0);
        arg0->fieldA2 = MonsterInit_ReadHalfword(0);
        arg0->fieldA4 = MonsterInit_ReadHalfword(0);
    }

    VornInit_InitializeHealth(arg0, VornInit_GetInitialHealth(arg0), firstLimit, secondLimit);

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
        VornInit_SetStatusFlags(arg0, 0x800);
    if (Script_GetKeywordValue(0x4C, 0))
        VornInit_SetStatusFlags(arg0, 0x8000);

    SceneBody_Init(body, mode, &origin, 0, 4, arg0);
    rt = &group->homeRoute;
    if (VornInit_ReadRoute(arg0, group, rt)) {
        VornInit_SnapToRoute(&arg0->body.position, rt);
        VornInit_DrawablePosition(arg0);
        arg0->body.direction = Route_GetCurrentPointFacing(rt);
    }

    if (VornInit_TestMotionFlags(arg0, 0x200))
        TargetRegistry_RegisterActor((u8 *)group + 0x30, body, sub_08080BE0);
    SceneBody_SetSprite(body, VornInit_GetDrawableKind(arg0) == 0 ? arg0->drawable : 0);
    if (VornInit_GetCategory(arg0) == 0x15)
        SceneBody_SetMovementProbe(body, (u8 *)arg0 + 0x40, 0x3C, 0x3C);
    else
        SceneBody_SetMovementProbe(body, (u8 *)arg0 + 0x40, 0x40, 0x40);

    group->common.bodyId = arg0->body.id;
    group->common.bodyPosition = &arg0->body.position;
}

struct VornInitWidget {
    u8 unknown0[0x3A];
    u16 paletteBank;
};
static inline void VornInit_SetWidgetScriptPalette(struct VornInitWidget *widget, s32 palette)
{
    widget->paletteBank = palette - 0x5FFF;
}

static inline void VornInit_SetMotionControl(struct SceneActorRegistryResetView *actor,
                                             void *callback, u32 kind)
{
    actor->word104 = (u32)callback;
    actor->word108 = 0;
    actor->word10C = 0;
    actor->word110 = kind;
    actor->flags114 = 1;
}
static inline void VornInit_ClearMotion(void *object)
{
    struct SceneActorRegistryResetView *actor = object;
    VornInit_SetMotionControl(actor, 0, 0);
    actor->flags114 = 0;
}
static inline void VornInit_ReadPalette(struct SceneActorRegistryResetView *actor, u32 base,
                                        u32 alternate)
{
    s32 palette;
    actor->paletteOffset = Script_GetKeywordValue(0x43, 0);
    actor->basePaletteBank = base;
    actor->paletteAlternateBank = alternate;
    actor->alternateOffset = VornInit_GetPaletteOffset(actor) + base + 1;
    palette = VornInit_GetPaletteBase(actor) + VornInit_GetPaletteOffset(actor);
    if (VornInit_GetDrawableKind(actor) == 0)
        AnimData_SelectPaletteBank(actor->drawable + 0x2C, palette);
    else
        *(u8 **)(actor->drawable + 0x68) = gPaletteBankData + (palette << 5);
    if (VornInit_GetDrawableKind(actor) == 1)
        VornInit_SetWidgetScriptPalette((void *)(actor->drawable + 0x20),
                                        SceneActorScript_GetManagerField62());
}

static inline void VornInit_InstallPrimary(struct SceneActorRegistryResetView *arg0,
                                           s32 (*cb0)(void *), u16 (*cb1)(void *),
                                           void (*cb2)(void *), void (*cb3)(void *),
                                           void (*cb4)(void *), void (*hidden)(void *),
                                           void (*cb5)(void *), void (*cb6)(void *),
                                           u32 (*defaultReaction)(void), u8 (*cb270)(void *),
                                           void *targetQuery, void *damage, void *contact)
{
    VornInit_SetBehaviorFlags(arg0, 0x2000);
    {
        s32 (**slot)(void *) = (void *)&arg0->contextCallback;
        *slot = cb0;
    }
    {
        u16 (**slot)(void *) = (void *)&arg0->animationQuery.sequence;
        *slot = cb1;
    }
    {
        void (**slot)(void *) = (void *)&arg0->entryTarget224;
        *slot = cb2;
    }
    {
        void (**slot)(void *) = (void *)&arg0->entryTarget228;
        *slot = cb3;
    }
    {
        void (**slot)(void *) = (void *)&arg0->savedTarget;
        *slot = cb4;
    }
    {
        void (**slot)(void *) = (void *)&arg0->reactionTarget230;
        *slot = cb5;
    }
    {
        void (**slot)(void *) = (void *)&arg0->deathControl;
        *slot = cb6;
    }
    {
        void (**slot)(void *) = (void *)&arg0->field220;
        *slot = hidden;
    }
    {
        u32 (**slot)(void) = (void *)&arg0->defaultReaction;
        *slot = defaultReaction;
    }
    {
        u8 (**slot)(void *) = (void *)&arg0->contactQuery.predicate;
        *slot = cb270;
    }
    {
        const void **slot = (void *)&arg0->commandTable.opaque;
        *slot = gUnk_086108B0;
    }
    arg0->callback248 = 0;
    arg0->update26C = targetQuery;
    arg0->field238 = SceneActorReaction_SinkAndRemove;
    arg0->damageCallback = damage;
    arg0->contactCallback = contact;
    arg0->field240 = 0;
    VornInit_ClearMotion(arg0);
}

static inline void VornInit_InitializeGroup(struct SceneActorChaseGroup *group, void *firstBounds,
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
    ProximityVolume_TranslateBounds(firstBounds, VornInit_BodyPosition(body));
}

static inline void VornInit_SetRouteReset(struct SceneActorRegistryResetView *actor, void *callback)
{
    actor->callback248 = callback;
}
static inline void VornInit_SetReactionGate(struct SceneActorRegistryResetView *actor,
                                            void *callback)
{
    actor->field23C = callback;
}

void Vorn_InitActorProfile(void *);
void SceneActor_InitContextColliders(void *);
s32 Vorn_OnUpdate(void *);
u16 Vorn_GetAnimationEvent(void *);
void Vorn_UpdateIdleFacing(void *);
void Vorn_ResetStateEntry(void *);
void Vorn_BeginIdle(void *);
void sub_080AD75C(void *);
void Vorn_UpdateDefeat(void *);
void Vorn_UpdateHiddenRecovery(void *);
void SceneActorTarget_ResolvePendingObject(void);
void Vorn_OnReactionNoOp(void);
void SceneActorReaction_ResumeSavedTarget(void);
void SceneActor_ResetContextAnimation(void);
void sub_080B197C(void);
u8 Vorn_SelectHitRecovery(void *);
void Vorn_OnAnimationCleanup(void);
void Vorn_BeginTargetPursuit(void);
void Vorn_OnRender(void);
void Vorn_OnDestroy(void);

static inline void VornInit_SetReactionTable(struct SceneActorRegistryResetView *actor,
                                             const void *table)
{
    {
        const void **slot = (void *)&actor->commandTable.opaque;
        *slot = table;
    }
}
static inline void VornInit_SetContactCallback(struct SceneActorRegistryResetView *actor,
                                               void *callback)
{
    actor->contactCallback = callback;
}

static inline void VornInit_EnableInitialFlags(struct SceneActorRegistryResetView *actor)
{
    VornInit_SetTargetingFlags(&actor->tracking, 1);
    VornInit_SetBehaviorFlags(actor, 0x1600000);
    VornInit_SetStatusFlags(actor, 0x8000000);
}

static inline void VornInit_ReadInitialMode(struct SceneActorRegistryResetView *arg0,
                                            struct SceneActorChaseGroup *group)
{
    s32 i;
    i = Script_GetKeywordValue(0x75, 0);
    arg0->group.effect = 0x21;
    if (i != 0) {
        VornInit_SetBehaviorFlags(arg0, 0x40000);
        if (i == 2)
            VornInit_SetBehaviorFlags(arg0, 0x80000);
    }
}

static inline void VornInit_InitializeRegion(struct SceneActorRegistryResetView *arg0)
{
    if (VornInit_TestMotionFlags(arg0, 2))
        arg0->regionId = *(u32 *)(gGameState + 0x5A0);
    else
        arg0->regionId = (u16)(u32)Region_FindContaining(&arg0->body.position);
}

static inline void VornInit_InitializeReceiver(struct SceneActorRegistryResetView *actor)
{
    actor->field304 = 0;
}

static inline void VornInit_InstallCompletionCallbacks(struct SceneActorRegistryResetView *arg0)
{
    arg0->messageCallback = SceneActorScript_OnCommand;
    arg0->updateCallback = Vorn_OnRender;
    arg0->destroyCallback = Vorn_OnDestroy;
}

static inline void VornInit_InitializeSprite(u8 *drawable, u16 resource, u16 sequence)
{
    AnimData_Load(drawable + 0x2C, resource);
    Sprite_Init(drawable, drawable + 0x2C, 0);
    SceneActorAnimation_InitEnemySequence(drawable, sequence);
}

static inline void VornInit_InitializeWidget(u8 *drawable, u16 script, u16 resource,
                                             struct ContactPosition *position)
{
    void *source = ScriptProc_Start(script, resource);
    *(struct VornResourceBlock *)drawable = *(struct VornResourceBlock *)source;
    ResourceHeader_Relocate(drawable, source);
    Widget_InitLinked(drawable + 0x20, drawable, 0, 0, 2, 2, 0x3C, position);
}

void Vorn_ParseField624(void *);
void Vorn_InitScriptCallback(void *);
void Vorn_InitActionCooldown(void *);
void Vorn_InitTimedAction(void *);
void Vorn_ParseField69A(void *);
void Vorn_ResetRecoveryMode(void *);
void Vorn_InitMotionLimits(void *);
void Vorn_ParseField688(void *);
void Vorn_InitAttackActionTable(void *);
void Vorn_InitSpecialActionTable(void *);
void Vorn_InitReactionActionTable(void *);
void Vorn_InitActionTable(void *);
void SceneActorRegistry_RegisterCategoryReceiver(void *);
void ActorShadow_Disable(void *);
void SceneActorReaction_BeginSideStep(void);
s32 Vorn_TryRestorePatrolRoute(void *);
void Vorn_UpdateKnockbackRecovery(void);
void SceneActorReaction_ResumeOrRunScript(void);
void Vorn_ResumeReactionRoute(void);

s32 Vorn_Init(void *object)
{
    struct SceneActorRegistryResetView *arg0 = object;
    struct SceneActorRegistryResetView *actor = object;
    struct SceneActorChaseGroup *group = &arg0->group;
    struct SceneActorBody *body = &arg0->body;
    u32 zero;
    struct ContactPosition tmp;
    u32 flag;
    s32 mode;

    VornInit_ResetFlags(arg0);
    SceneActorRegistry_SetObjectContext(actor, actor, 1, 1);
    VornInit_SetCategory(actor, 0x19);
    Vorn_InitActorProfile(actor);
    Vorn_ParseField624(actor);
    Vorn_InitScriptCallback(actor);
    Vorn_InitActionCooldown(actor);
    Vorn_InitTimedAction(actor);
    Vorn_ParseField69A(actor);
    Vorn_ResetRecoveryMode(actor);
    Vorn_InitMotionLimits(actor);
    Vorn_ParseField688(actor);
    if (VornInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->drawable, 0x5C);
        heap = VornInit_Drawable(actor);
        VornInit_InitializeSprite(heap, 0x3DB6, 0);
        if (VornInit_GetCategory(actor) == 0x11 || VornInit_GetCategory(actor) == 0x17)
            heap[7] = 1;
        actor->body.direction = 0;
    } else {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->drawable, 0x80);
        heap = VornInit_Drawable(actor);
        MonsterInit_ClearPosition(&zero, &tmp);
        VornInit_InitializeWidget(heap, 0xCB05, 0x3DB6, &tmp);
        actor->animationIndex = 0xFF;
    }
    VornInit_InitializeBody(arg0, &zero);
    VornInit_ReadPalette(actor, 0x31B, 0x192);
    SceneActor_InitContextColliders(actor);
    VornInit_InstallPrimary(actor, Vorn_OnUpdate, Vorn_GetAnimationEvent, Vorn_UpdateIdleFacing,
                            Vorn_ResetStateEntry, Vorn_BeginIdle, Vorn_UpdateHiddenRecovery,
                            sub_080AD75C, Vorn_UpdateDefeat, SceneActorReaction_OnEnemyDefault,
                            sub_080B3C1C, SceneActorTarget_ResolvePendingObject, sub_081363F4,
                            SceneActorPlacement_ResetStateThree);
    VornInit_InitializeGroup(group, (u8 *)arg0 + 0x6F4, (u8 *)arg0 + 0x718, Vorn_OnReactionNoOp,
                             SceneActorReaction_ResumeSavedTarget, Vorn_TryRestorePatrolRoute, 0, 1,
                             body);
    SceneActorMovement_ResolveRoutePosition(arg0);
    VornInit_SetRouteReset(actor, SceneActor_ResetContextAnimation);
    VornInit_SetReactionGate(actor, sub_080B197C);
    VornInit_SetContactCallback(actor, Vorn_SelectHitRecovery);
    VornInit_SetReactionTable(actor, gUnk_08606038);
    group->contactControl = Vorn_UpdateKnockbackRecovery;
    group->field5E0 = SceneActorReaction_ResumeOrRunScript;
    group->responseCallback = Vorn_OnAnimationCleanup;
    group->field5EC = Vorn_BeginTargetPursuit;
    group->field614 = Vorn_ResumeReactionRoute;
    actor->field2E8 = 3;
    VornInit_EnableInitialFlags(actor);
    SceneActorBehavior_InitializeShadow(actor);
    SceneActorTerrain_ReadScriptParameters(actor);
    VornInit_ReadInitialMode(actor, group);
    flag = 0;
    if (VornInit_TestMotionFlags(actor, 4)) {
        VornInit_SetStatusFlags(actor, 0x2000);
        flag = 1;
    }
    if (flag != 1) {
        VornInit_SetState(actor, 0, 0, 0);
        VornInit_SetControl(actor, VornInit_GetStandingControl(actor), 0);
    }
    VornInit_InitializeRegion(actor);
    if (VornInit_TestMotionFlags(actor, 0x40))
        VornInit_ClearColliderFlags((void *)((u8 *)actor + 0x134), ~0x100);
    if (Script_GetKeywordValue(0x53, 0) != 0 || !VornInit_IsInCurrentRegion(actor))
        SceneActorPlacement_HideActor(actor, 0);
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    if (VornInit_GetDrawableKind(actor) == 0)
        DisplaySequence_Step(actor->drawable);
    else
        Anim_Step(actor->drawable + 0x20, actor->drawable);
    VornInit_InitializeReceiver(actor);
    SceneActorRegistry_RegisterCategoryReceiver(actor);
    SceneActorBehavior_InstallCallbackTables(actor);
    Vorn_InitAttackActionTable(actor);
    Vorn_InitSpecialActionTable(actor);
    Vorn_InitReactionActionTable(actor);
    Vorn_InitActionTable(actor);
    VornInit_SaveControl(VornInit_SavedControlOf(actor), actor);

    flag = ((struct VornInstanceState *)arg0)->field68C;
    if (flag == 1) {
        if (VornInit_GetDrawableKind(actor) == 0)
            *(u32 *)actor->drawable |= flag;
        else {
            u8 *widget = actor->drawable + 0x20;
            *(u32 *)(widget + 8) |= flag;
        }
        ActorShadow_Disable((u8 *)actor + 0x320);
        VornInit_SetState(actor, 3, 0, 0);
        VornInit_SetControl(actor, actor->field220, 0xC);
    } else if ((mode = Script_GetKeywordValue(0x54, 0)) != 0) {
        if (mode == 1)
            ((struct VornInstanceState *)arg0)->field6B9 = 0;
        else
            ((struct VornInstanceState *)arg0)->field6B9 = 1;
        VornInit_BeginBehavior(actor, 0x12);
        VornInit_SetControl(actor, actor->savedTarget, 0);
        VornInit_SetMotionControl(actor, SceneActorReaction_BeginSideStep, 0x26);
    }
    VornInit_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}
