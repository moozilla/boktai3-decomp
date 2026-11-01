#include "scene_body_api.h"
#include "global.h"
#include "render.h"
#include "engine_globals.h"
#include "display_sequence_inline.h"
#include "scene_actor_init_primitives.h"
#include "scene_actor_init_group.h"
#include "scene_actor_init_dispatch.h"
#include "scene_actor_init_graphics.h"

void *Heap_Alloc(u32);
void Memory_Clear(void *, u32);
void SceneActorRegistry_SetObjectContext(void *, void *, s32, s32);
void SceneActorReaction_SetMotionMode(void);
u32 SceneActorReaction_OnEnemyDefault(void);
void SceneActorReaction_ResolvePlacementDamage(void);
void SceneActorScript_OnCommand(void);
u32 Route_GetCurrentPointFacing(void *);
s32 TargetRegistry_RegisterActor(void *, void *, void *);
void AnimData_SelectPaletteBank(void *, s32);
void SceneActorPlacement_HideActor(void *, s32);
void SceneActorAnimation_SelectDirectionalEntry(void *, s32);
void Anim_Step(void *, void *);
void SceneActorBehavior_InstallCallbackTables(void *);
void SceneActorTerrain_ReadScriptParameters(void *);
u8 SceneActorRegistry_LinkActor(void *);

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

void SceneActorBehavior_InitializeShadow(void *);

void SceneActorMovement_ResolveRoutePosition(void *);
u8 sub_0807F81C(void *);
void SceneActorPlacement_ResetStateThree(void);
void SceneActorReaction_BeginTargetRecovery(void);

static inline void GolemInit_ResetFlags(void *object)
{
    struct GolemInstanceState *actor = object;
    actor->field628 = 0;
}

static inline void GolemInit_InitializeBody(void *object, u32 *fill)
{
    struct SceneActorRegistryResetView *arg0 = object;
    struct SceneActorChaseGroup *group = &arg0->group;
    struct SceneActorBody *body = &arg0->body;
    struct ContactPosition origin;
    s32 mode, secondLimit, firstLimit, gateDuration, identity;
    struct SceneActorRegistryRoute *rt;
    arg0->flags2CC = 0;
    arg0->flags2D0 = 0;
    arg0->enabled = 0;
    arg0->field2E8 = 8;
    arg0->flags2C0 = MonsterInit_ReadKeywordHalfword('M', 0);
    arg0->field2C2 = 0x40;
    MonsterInit_ClearPosition(fill, &origin);
    mode = Script_GetKeywordValue(0x4E, 0);

    MonsterInit_ReadAttacks(arg0);

    if (Script_SeekToKeyword(0x6C)) {
        firstLimit = MonsterInit_ReadWord(0x12C);
        secondLimit = MonsterInit_ReadWord(0x12C);
        gateDuration = MonsterInit_ReadWord(0x12C);
    } else {
        firstLimit = 0;
        secondLimit = 0;
        gateDuration = 0;
    }
    MonsterInit_InitializeGate(&group->common.gate, gateDuration);
    MonsterInit_ReadRange(&group->common.control);
    MonsterInit_ReadTargeting(&arg0->tracking);
    if (Script_GetKeywordValue(0x62, 0))
        MonsterInit_SetStatusFlags(arg0, 0x200);
    arg0->field1F0 = MonsterInit_ReadKeywordHalfword(0x46, 0);
    arg0->field1EC = MonsterInit_ReadKeywordHalfword(0x52, 0);

    if (Script_SeekToKeyword(0x41)) {
        MonsterInit_SetInitialHealth(arg0, MonsterInit_ReadHalfword(0));
        group->halfHealth = MonsterInit_Half(MonsterInit_GetInitialHealth(arg0));
        arg0->field9E = MonsterInit_ReadByte(0);
        arg0->field9F = MonsterInit_ReadByte(0);
        arg0->fieldA0 = MonsterInit_ReadByte(0);
        arg0->fieldA2 = MonsterInit_ReadHalfword(0);
        arg0->fieldA4 = MonsterInit_ReadHalfword(0);
    }

    MonsterInit_InitializeHealth(arg0, MonsterInit_GetInitialHealth(arg0), firstLimit, secondLimit);

    if (Script_SeekToKeyword(0x58)) {
        arg0->fieldAC = MonsterInit_ReadWord(0);
        arg0->fieldA1 = MonsterInit_ReadByte(0);
    }
    arg0->fieldB2 = MonsterInit_ReadKeywordHalfword('K', 2);
    identity = (s16)Script_GetKeywordValue(0x56, -1);
    if (identity < 0)
        identity = 0xFFFF;
    arg0->field9A = identity;
    if (Script_GetKeywordValue(0x48, 0))
        MonsterInit_SetStatusFlags(arg0, 0x800);
    if (Script_GetKeywordValue(0x4C, 0))
        MonsterInit_SetStatusFlags(arg0, 0x8000);

    SceneBody_Init(body, mode, &origin, 0, 4, arg0);
    rt = &group->homeRoute;
    if (MonsterInit_ReadRoute(arg0, group, rt)) {
        MonsterInit_SnapToRoute(&arg0->body.position, rt);
        MonsterInit_DrawablePosition(arg0);
        arg0->body.direction = Route_GetCurrentPointFacing(rt);
    }

    if (MonsterInit_TestMotionFlags(arg0, 0x200))
        TargetRegistry_RegisterActor((u8 *)group + 0x30, body, SceneActorReaction_SetMotionMode);
    SceneBody_SetSprite(body, MonsterInit_GetDrawableKind(arg0) == 0 ? arg0->drawable : 0);
    if (MonsterInit_GetCategory(arg0) == 0x15)
        SceneBody_SetMovementProbe(body, (u8 *)arg0 + 0x40, 0x3C, 0x3C);
    else
        SceneBody_SetMovementProbe(body, (u8 *)arg0 + 0x40, 0x40, 0x40);

    group->common.bodyId = arg0->body.id;
    group->common.bodyPosition = &arg0->body.position;
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

static inline void GolemInit_EnableInitialFlags(struct SceneActorRegistryResetView *actor)
{
    MonsterInit_SetTargetingFlags(&actor->tracking, 1);
    MonsterInit_SetBehaviorFlags(actor, 0x1600000);
    MonsterInit_SetStatusFlags(actor, 0x8000008);
}

static inline struct GolemInitContext *GolemInit_Context(struct SceneActorRegistryResetView *actor)
{
    return actor->context;
}
static inline void GolemInit_InitializeReceiver(struct SceneActorRegistryResetView *actor)
{
    MonsterInit_ClearReceiver(actor);
    GolemInit_Context(actor)->rangeSquared62C = actor->tracking.range;
}

static inline void GolemInit_InstallCompletionCallbacks(struct SceneActorRegistryResetView *arg0)
{
    arg0->messageCallback = SceneActorScript_OnCommand;
    arg0->updateCallback = Golem_OnSharedActorUpdate;
    arg0->destroyCallback = Golem_OnDestroy;
}

static inline void GolemInit_InitializePaletteRange(struct SceneActorRegistryResetView *actor,
                                                    u32 alternate, u32 current)
{
    actor->alternatePalette = MonsterInit_GetPaletteBase(actor) + alternate;
    actor->paletteBank = MonsterInit_GetPaletteBase(actor) + current;
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
    MonsterInit_SetCategory(actor, 3);
    Golem_InitAnimationProfile(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->drawable, 0x5C);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_InitializeSprite(heap, 0x5290, 0x1E38);
        if (MonsterInit_GetCategory(actor) == 0x11 || MonsterInit_GetCategory(actor) == 0x17)
            heap[7] = 1;
        actor->body.direction = 0;
    } else {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->drawable, 0x80);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_ClearPosition(&zero, &tmp);
        MonsterInit_InitializeWidget(heap, 0xCB05, 0x5290, &tmp);
        actor->animationIndex = 0xFF;
    }
    GolemInit_InitializeBody(arg0, &zero);
    MonsterInit_ReadPalette(actor, 0x27D, 0x192);
    GolemInit_InitializePaletteRange(actor, 2, 0);
    Golem_InitColliders(actor);
    MonsterInit_InstallSequenceCallbacks(
        actor, Golem_OnUpdate, Skeleton_LookupAnimationEntry, sub_080C5930, Golem_UpdateIdleState,
        Golem_UpdateIdle, Golem_UpdateRevivalReturn, Golem_ResumeBehaviorAfterDisplacement,
        Golem_UpdateDefeat, SceneActorReaction_OnEnemyDefault, sub_0807F81C,
        SceneActorTarget_ResolvePendingObject, SceneActorReaction_ResolvePlacementDamage, SceneActorPlacement_ResetStateThree);
    MonsterInit_InitializeGroupWithBounds(
        group, (u8 *)arg0 + 0x638, (u8 *)arg0 + 0x65C, Golem_UpdateDying, Golem_ResumeSavedTarget,
        SceneActorReaction_BeginTargetRecovery, Golem_OnUnusedState, 1, &actor->body);
    SceneActorMovement_ResolveRoutePosition(actor);
    MonsterInit_SetRouteReset(actor, Golem_ResetCombatState);
    MonsterInit_SetReactionGate(actor, SceneActorControl_RefreshMotionRoute);
    MonsterInit_SetContactCallback(actor, Golem_UpdateHitReaction);
    group->responseCallback = Golem_ClearControlFlag;
    group->field5EC = Golem_RestoreRoute;
    actor->field2E8 = 0;
    GolemInit_EnableInitialFlags(actor);
    MonsterInit_MaskColliderFlags(&actor->colliderA, ~1);
    SceneActorBehavior_InitializeShadow(actor);
    SceneActorTerrain_ReadScriptParameters(actor);
    MonsterInit_ReadInitialMode(actor);
    flag = 0;
    if (MonsterInit_TestMotionFlags(actor, 4)) {
        MonsterInit_SetStatusFlags(actor, 0x2000);
        flag = 1;
    }
    if (flag != 1) {
        MonsterInit_SetState(actor, 0, 0, 0);
        MonsterInit_SetControl(actor, MonsterInit_GetStandingControl(actor), 0);
    }
    MonsterInit_InitializeRegion(actor);
    if (MonsterInit_TestMotionFlags(actor, 0x40))
        MonsterInit_MaskColliderFlags(&actor->colliderA, ~0x100);
    if (Script_GetKeywordValue(0x53, 0) != 0 || !MonsterInit_IsInCurrentRegion(actor))
        SceneActorPlacement_HideActor(actor, 0);
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    if (MonsterInit_GetDrawableKind(actor) == 0)
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

    MonsterInit_SaveControl(MonsterInit_SavedControlOf(actor), actor);
    GolemInit_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}
