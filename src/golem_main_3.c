#include "global.h"
#include "scene_body_api.h"
#include "golem.h"
#include "render.h"
#include "engine_globals.h"
#include "display_sequence_inline.h"
#include "scene_actor_init_primitives.h"
#include "scene_actor_init_group.h"
#include "scene_actor_init_dispatch.h"
#include "scene_actor_init_graphics.h"

extern const u8 gUnk_0860613C[];

struct S {
    u8 filler[652];
    const u8 *p;
};

void Golem_InitSpecialActionHandlers(struct S *s)
{
    s->p = gUnk_0860613C;
}

void Golem_TickFacingQuadrant(void *);

s32 Callback_InvokeActor(void *, void *);

void SceneActorControl_DispatchControl(void *);

void Golem_UpdateEffectsAndContacts(void *);

struct SOnUpdate {
    u8 filler[0x27c];
    u32 *tbl;
    u8 filler2[0x2a8 - 0x280];
    u8 idx;
    u8 filler3[0x2b8 - 0x2a9];
    u16 h2b8;
    u8 filler4[0x2d4 - 0x2ba];
    u16 flags;
};

s32 Golem_OnUpdate(struct SOnUpdate *s)
{
    u32 m;
    Golem_TickFacingQuadrant(s);
    m = 0x2000;
    if ((s->flags & m) || s->h2b8 == 0) {
        Callback_InvokeActor(s, s->tbl[s->idx]);
        SceneActorControl_DispatchControl(s);
    }
    Golem_UpdateEffectsAndContacts(s);
    return 1;
}

void SceneActor_UpdateBehaviorCallbacks(void);

s32 Golem_OnSharedActorUpdate(void)
{
    SceneActor_UpdateBehaviorCallbacks();
    return 0;
}

void SceneActorEffects_ReleaseResources(void);

s32 Golem_OnDestroy(u8 *unused)
{
    SceneActorEffects_ReleaseResources();
    return 0;
}

void *Heap_Alloc(s32 size); /* first-fit allocation from gHeapHead; NULL on failure */

void Memory_Clear(void *dst, s32 bytes); /* zero fill (bytes, then CpuSet words) */

void SceneActorRegistry_SetObjectContext(void *, void *, s32, s32);

void SceneActorReaction_SetMotionMode(void);

u32 SceneActorReaction_OnEnemyDefault(void);

void SceneActorReaction_ResolvePlacementDamage(void);

void SceneActorScript_OnCommand(void *actor, const u16 *command);

u32 Route_GetCurrentPointFacing(void *);

s32 TargetRegistry_RegisterActor(void *, void *, void *);

void AnimData_SelectPaletteBank(void *, s32);

void SceneActorPlacement_HideActor(void *, s32);

void SceneActorAnimation_SelectDirectionalEntry(struct RoomActor *, s32);

s32 Anim_Step(void *, void *);

void SceneActorBehavior_InstallCallbackTables(void *);

void SceneActorTerrain_ReadScriptParameters(void *);

u8 SceneActorRegistry_LinkActor(void *);

void SceneActorBehavior_InitializeShadow(void *);

void SceneActorMovement_ResolveRoutePosition(void *);

u8 SceneActorTarget_UpdateTrackedContact(void *);

void SceneActorPlacement_ResetStateThree(void);

void SceneActorReaction_BeginTargetRecovery(void);

static inline void GolemInit_ResetFlags(void *object)
{
    struct Golem *actor = object;
    actor->flags628 = 0;
}

static inline void GolemInit_InitializeBody(void *object, u32 *fill)
{
    struct Golem *arg0 = object;
    struct SceneActorChaseGroup *group = &arg0->chase.group;
    struct SceneActorBody *body = &arg0->chase.base.body;
    struct ContactPosition origin;
    s32 mode, secondLimit, firstLimit, gateDuration, identity;
    struct SceneActorRegistryRoute *rt;
    arg0->chase.base.behaviorFlags = 0;
    arg0->chase.base.statusFlags = 0;
    arg0->chase.base.enabled = 0;
    arg0->chase.base.field2E8 = 8;
    arg0->chase.base.placementFlags = MonsterInit_ReadKeywordHalfword('M', 0);
    arg0->chase.base.field2C2 = 0x40;
    MonsterInit_ClearPosition(fill, &origin);
    mode = Script_GetKeywordValue(0x4E, 0);

    MonsterInit_ReadAttacks(&arg0->chase);

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
    MonsterInit_ReadTargeting(&arg0->chase.base.tracking);
    if (Script_GetKeywordValue(0x62, 0))
        MonsterInit_SetStatusFlags(&arg0->chase, 0x200);
    arg0->chase.base.script1F0 = MonsterInit_ReadKeywordHalfword(0x46, 0);
    arg0->chase.base.scriptId = MonsterInit_ReadKeywordHalfword(0x52, 0);

    if (Script_SeekToKeyword(0x41)) {
        MonsterInit_SetInitialHealth(&arg0->chase, MonsterInit_ReadHalfword(0));
        group->halfHealth = MonsterInit_Half(MonsterInit_GetInitialHealth(&arg0->chase));
        arg0->chase.base.field9E = MonsterInit_ReadByte(0);
        arg0->chase.base.defense = MonsterInit_ReadByte(0);
        arg0->chase.base.contactReward = MonsterInit_ReadByte(0);
        arg0->chase.base.fieldA2 = MonsterInit_ReadHalfword(0);
        arg0->chase.base.fieldA4 = MonsterInit_ReadHalfword(0);
    }

    MonsterInit_InitializeHealth(&arg0->chase, MonsterInit_GetInitialHealth(&arg0->chase),
                                 firstLimit, secondLimit);

    if (Script_SeekToKeyword(0x58)) {
        arg0->chase.base.experience = MonsterInit_ReadWord(0);
        arg0->chase.base.fieldA1 = MonsterInit_ReadByte(0);
    }
    arg0->chase.base.hudKind = MonsterInit_ReadKeywordHalfword('K', 2);
    identity = (s16)Script_GetKeywordValue(0x56, -1);
    if (identity < 0)
        identity = 0xFFFF;
    arg0->chase.base.hudLabel = identity;
    if (Script_GetKeywordValue(0x48, 0))
        MonsterInit_SetStatusFlags(&arg0->chase, 0x800);
    if (Script_GetKeywordValue(0x4C, 0))
        MonsterInit_SetStatusFlags(&arg0->chase, 0x8000);

    SceneBody_Init(body, mode, &origin, 0, 4, arg0);
    rt = &group->homeRoute;
    if (MonsterInit_ReadRoute(&arg0->chase, group, rt)) {
        MonsterInit_SnapToRoute(&arg0->chase.base.body.position, rt);
        MonsterInit_DrawablePosition(&arg0->chase);
        arg0->chase.base.body.direction = Route_GetCurrentPointFacing(rt);
    }

    if (MonsterInit_TestMotionFlags(&arg0->chase, 0x200))
        TargetRegistry_RegisterActor(&group->common.placement, body,
                                     SceneActorReaction_SetMotionMode);
    SceneBody_SetSprite(
        body, MonsterInit_GetDrawableKind(&arg0->chase) == 0 ? arg0->chase.base.drawable : 0);
    if (MonsterInit_GetCategory(arg0) == 0x15)
        SceneBody_SetMovementProbe(body, arg0->chase.base.movementProbe, 0x3C, 0x3C);
    else
        SceneBody_SetMovementProbe(body, arg0->chase.base.movementProbe, 0x40, 0x40);

    group->common.bodyId = arg0->chase.base.body.id;
    group->common.bodyPosition = &arg0->chase.base.body.position;
}

void Golem_InitAnimationProfile(void *);

void Golem_InitColliders(void *);

void Golem_InitActionHandlers(void *);

void Golem_InitSpecialActionHandlers(struct S *s);

s32 Golem_OnUpdate(struct SOnUpdate *s);

u16 Skeleton_LookupAnimationEntry(void *);

void Golem_TurnTowardRouteFacing(void *);

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

s32 Golem_OnSharedActorUpdate(void);

s32 Golem_OnDestroy(u8 *);

static inline void GolemInit_EnableInitialFlags(struct Golem *actor)
{
    MonsterInit_SetTargetingFlags(&actor->chase.base.tracking, 1);
    MonsterInit_SetBehaviorFlags(&actor->chase, 0x1600000);
    MonsterInit_SetStatusFlags(&actor->chase, 0x8000008);
}

static inline struct Golem *GolemInit_Context(struct Golem *actor)
{
    return actor->chase.base.context;
}

static inline void GolemInit_InitializeReceiver(struct Golem *actor)
{
    MonsterInit_ClearReceiver(&actor->chase);
    GolemInit_Context(actor)->rangeSquared62C = actor->chase.base.tracking.range;
}

static inline void GolemInit_InstallCompletionCallbacks(struct Golem *arg0)
{
    arg0->chase.base.onMessage = SceneActorScript_OnCommand;
    arg0->chase.base.update = (SceneActorResultCallback)Golem_OnSharedActorUpdate;
    arg0->chase.base.destroy = (SceneActorResultCallback)Golem_OnDestroy;
}

static inline void GolemInit_InitializePaletteRange(struct Golem *actor, u32 alternate, u32 current)
{
    actor->chase.base.alternatePalette = MonsterInit_GetPaletteBase(&actor->chase) + alternate;
    actor->chase.base.paletteBank = MonsterInit_GetPaletteBase(&actor->chase) + current;
}

s32 Golem_Init(void *object)
{
    struct Golem *arg0 = object;
    struct Golem *actor = object;
    struct SceneActorChaseGroup *group = &arg0->chase.group;
    u32 zero;
    struct ContactPosition tmp;
    u32 flag;

    GolemInit_ResetFlags(arg0);
    SceneActorRegistry_SetObjectContext(actor, actor, 0, 1);
    MonsterInit_SetCategory(actor, 3);
    Golem_InitAnimationProfile(actor);
    if (MonsterInit_GetDrawableKind(&actor->chase) == 0) {
        u8 *heap;
        actor->chase.base.drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->chase.base.drawable, 0x5C);
        heap = MonsterInit_Drawable(&actor->chase);
        MonsterInit_InitializeSprite(heap, 0x5290, 0x1E38);
        if (MonsterInit_GetCategory(actor) == 0x11 || MonsterInit_GetCategory(actor) == 0x17)
            heap[7] = 1;
        actor->chase.base.body.direction = 0;
    } else {
        u8 *heap;
        actor->chase.base.drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->chase.base.drawable, 0x80);
        heap = MonsterInit_Drawable(&actor->chase);
        MonsterInit_ClearPosition(&zero, &tmp);
        MonsterInit_InitializeWidget(heap, 0xCB05, 0x5290, &tmp);
        actor->chase.base.animationIndex = 0xFF;
    }
    GolemInit_InitializeBody(arg0, &zero);
    MonsterInit_ReadPalette(&actor->chase, 0x27D, 0x192);
    GolemInit_InitializePaletteRange(actor, 2, 0);
    Golem_InitColliders(actor);
    MonsterInit_InstallSequenceCallbacks(
        &actor->chase, Golem_OnUpdate, Skeleton_LookupAnimationEntry, Golem_TurnTowardRouteFacing,
        Golem_UpdateIdleState, Golem_UpdateIdle, Golem_UpdateRevivalReturn,
        Golem_ResumeBehaviorAfterDisplacement, Golem_UpdateDefeat,
        SceneActorReaction_OnEnemyDefault, SceneActorTarget_UpdateTrackedContact,
        SceneActorTarget_ResolvePendingObject, SceneActorReaction_ResolvePlacementDamage,
        SceneActorPlacement_ResetStateThree);
    MonsterInit_InitializeGroupWithBounds(
        group, &arg0->routeProbe, &arg0->destination, Golem_UpdateDying, Golem_ResumeSavedTarget,
        SceneActorReaction_BeginTargetRecovery, Golem_OnUnusedState, 1, &actor->chase.base.body);
    SceneActorMovement_ResolveRoutePosition(actor);
    MonsterInit_SetRouteReset(&actor->chase, Golem_ResetCombatState);
    MonsterInit_SetReactionGate(&actor->chase, SceneActorControl_RefreshMotionRoute);
    MonsterInit_SetContactCallback(&actor->chase, Golem_UpdateHitReaction);
    group->responseCallback = Golem_ClearControlFlag;
    group->routeRestoreCallback = Golem_RestoreRoute;
    actor->chase.base.field2E8 = 0;
    GolemInit_EnableInitialFlags(actor);
    MonsterInit_MaskColliderFlags(&actor->chase.base.colliderA, ~1);
    SceneActorBehavior_InitializeShadow(actor);
    SceneActorTerrain_ReadScriptParameters(actor);
    MonsterInit_ReadInitialMode(&actor->chase);
    flag = 0;
    if (MonsterInit_TestMotionFlags(&actor->chase, 4)) {
        MonsterInit_SetStatusFlags(&actor->chase, 0x2000);
        flag = 1;
    }
    if (flag != 1) {
        MonsterInit_SetState(&actor->chase, 0, 0, 0);
        MonsterInit_SetControl(&actor->chase, MonsterInit_GetStandingControl(&actor->chase), 0);
    }
    MonsterInit_InitializeRegion(&actor->chase);
    if (MonsterInit_TestMotionFlags(&actor->chase, 0x40))
        MonsterInit_MaskColliderFlags(&actor->chase.base.colliderA, ~0x100);
    if (Script_GetKeywordValue(0x53, 0) != 0 || !MonsterInit_IsInCurrentRegion(&actor->chase))
        SceneActorPlacement_HideActor(actor, 0);
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    if (MonsterInit_GetDrawableKind(&actor->chase) == 0)
        DisplaySequence_Step(actor->chase.base.drawable);
    else
        Anim_Step(actor->chase.base.drawable + 0x20, actor->chase.base.drawable);
    GolemInit_InitializeReceiver(actor);
    SceneActorBehavior_InstallCallbackTables(actor);
    Golem_InitActionHandlers(actor);
    Golem_InitSpecialActionHandlers(actor);

    {
        struct Golem *instance = arg0;
        instance->field68D = 0;
        instance->routeDelay = 0x78;
        instance->counter686 = 0;
        instance->field684 = 2;
        instance->pushSpeed = 0;
        instance->routeDelay = 0xA0;
        instance->field68A = 0;
        instance->responseCount = 0;
        instance->field693 = 0;
    }

    MonsterInit_SaveControl(MonsterInit_SavedControlOf(&actor->chase), &actor->chase);
    GolemInit_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}

s32 Golem_Init(void *object);

void Heap_Free(void *);

void Golem_Create(void)
{
    u8 *p = Heap_Alloc(0x6b0);
    if (p != 0) {
        Memory_Clear(p, 0x6b0);
        if (Golem_Init(p) < 0) {
            Golem_OnDestroy(p);
            Heap_Free(p);
        }
    }
}
