#include "global.h"
#include "render.h"
#include "engine_globals.h"
#include "display_sequence_inline.h"
#include "scene_actor_init_primitives.h"
#include "scene_actor_init_group.h"
#include "scene_actor_init_dispatch.h"
#include "scene_actor_init_graphics.h"
#include "scene_actor_init_body.h"

void *Heap_Alloc(u32);
void Memory_Clear(void *, u32);
void SceneActorRegistry_SetObjectContext(void *, void *, s32, s32);
u32 SceneActorReaction_OnEnemyDefault(void);
void sub_081363F4(void);
void SceneActorScript_OnCommand(void);
void AnimData_SelectPaletteBank(void *, s32);
void SceneActorPlacement_HideActor(void *, s32);
void SceneActorAnimation_SelectDirectionalEntry(void *, s32);
void Anim_Step(void *, void *);
void SceneActorBehavior_InstallCallbackTables(void *);
void SceneActorTerrain_ReadScriptParameters(void *);
u8 SceneActorRegistry_LinkActor(void *);

struct MummyInitContext {
    u8 unknown0[0x6CC];
    u16 field6CC, field6CE;
    u8 unknown6D0[8];
    u32 field6D8, field6DC;
};

struct MummyActor {
    struct SceneActorRegistryResetView base;
    u8 unknown6EC[6];
    u16 field6F2;
};

void SceneActorBehavior_InitializeShadow(void *);

void SceneActorMovement_ResolveRoutePosition(void *);
u8 sub_080D7568(void *);
void SceneActorPlacement_ResetStateThree(void);
void SceneActorReaction_BeginTargetRecovery(void);

static inline void MummyInit_ResetFlags(void *object)
{
    struct MummyActor *actor = object;
    actor->field6F2 = 0;
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

static inline void MummyInit_EnableInitialFlags(struct SceneActorRegistryResetView *actor)
{
    MonsterInit_SetTargetingFlags(&actor->tracking, 1);
    MonsterInit_SetBehaviorFlags(actor, 0x600000);
    MonsterInit_SetStatusFlags(actor, 0x8000000);
}

static inline struct MummyInitContext *MummyInit_Context(struct SceneActorRegistryResetView *actor)
{
    return actor->context;
}

static inline void MummyInit_InstallCompletionCallbacks(struct SceneActorRegistryResetView *arg0)
{
    arg0->messageCallback = SceneActorScript_OnCommand;
    arg0->updateCallback = Mummy_OnInteractionUpdate;
    arg0->destroyCallback = Mummy_OnReleaseResources;
}

static inline void MummyInit_InitializePaletteRange(struct SceneActorRegistryResetView *actor,
                                                    u32 selected, u32 alternate, u32 current)
{
    actor->alternateOffset = selected;
    actor->alternatePalette = MonsterInit_GetPaletteBase(actor) + alternate;
    actor->paletteBank = MonsterInit_GetPaletteBase(actor) + current;
}

void Mummy_SetControlPreset(void *);
void SceneActorEffects_InitAtActorPosition(void *);
void sub_080D1644(void);
void sub_080D453C(void);

s32 Mummy_Init(void *object)
{
    struct MummyActor *arg0 = object;
    struct SceneActorRegistryResetView *actor = &arg0->base;
    struct SceneActorChaseGroup *group = &arg0->base.group;
    u32 zero;
    struct ContactPosition tmp;
    u32 flag;

    MummyInit_ResetFlags(arg0);
    SceneActorRegistry_SetObjectContext(actor, actor, 0, 1);
    MonsterInit_SetCategory(actor, 1);
    Mummy_InitProfile(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->drawable, 0x5C);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_InitializeSprite(heap, 0x48DE, 0xA4B2);
        if (MonsterInit_GetCategory(actor) == 0x11 || MonsterInit_GetCategory(actor) == 0x17)
            heap[7] = 1;
        actor->body.direction = 0;
    } else {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->drawable, 0x80);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_ClearPosition(&zero, &tmp);
        MonsterInit_InitializeWidget(heap, 0xCB05, 0x48DE, &tmp);
        actor->animationIndex = 0xFF;
    }
    MonsterInit_InitializePursuitBody(&arg0->base, &zero);
    MonsterInit_ReadPalette(actor, 0x23, 0x192);
    MummyInit_InitializePaletteRange(actor, MonsterInit_GetPaletteBase(actor) + 3, 4, 0);
    Mummy_InitCollision(actor);
    MonsterInit_InstallSequenceCallbacks(
        actor, Mummy_OnUpdate, Mummy_GetAnimationStepOffset, Mummy_TurnTowardRoute,
        Mummy_UpdateWalking, Mummy_UpdateIdle, Mummy_UpdateHiddenRecovery,
        Mummy_UpdateDisplacementRecovery, Mummy_UpdateDefeat, SceneActorReaction_OnEnemyDefault,
        sub_080D7568, SceneActorTarget_ResolvePendingObject, sub_081363F4,
        SceneActorPlacement_ResetStateThree);
    MonsterInit_InitializeGroupWithBounds(
        group, (u8 *)arg0 + 0x6F4, (u8 *)arg0 + 0x718, Mummy_UpdateDying, Mummy_ResumeSavedTarget,
        SceneActorReaction_BeginTargetRecovery, Mummy_OnUnusedState, 3, &actor->body);
    SceneActorMovement_ResolveRoutePosition(arg0);
    MonsterInit_SetRouteReset(actor, Mummy_ResetRevivalMotion);
    MonsterInit_SetReactionGate(actor, SceneActor_EnterMotionPhase);
    MonsterInit_SetContactCallback(actor, Mummy_SelectMovementResponse);
    group->responseCallback = Mummy_OnClearActivityFlag;
    group->field5EC = Mummy_OnMovementUpdate;
    group->routeChoiceControl = sub_080D1644;
    group->common.fallback = sub_080D453C;
    actor->field2E8 = 1;
    MummyInit_EnableInitialFlags(actor);
    MonsterInit_MaskColliderFlags(&actor->colliderA, ~1);
    SceneActorBehavior_InitializeShadow(actor);
    SceneActorEffects_InitAtActorPosition(arg0);
    SceneActorTerrain_ReadScriptParameters(actor);
    MonsterInit_ReadInitialMode(&arg0->base);
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
    MonsterInit_ClearReceiver(actor);
    SceneActorBehavior_InstallCallbackTables(actor);
    Mummy_SetControlPreset(actor);
    Mummy_InitActionHandlers(actor);
    Mummy_SetBehaviorTable(actor);

    arg0->base.field6E8 = actor->tracking.x;
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

    MonsterInit_SaveControl(MonsterInit_SavedControlOf(actor), actor);
    MummyInit_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}
