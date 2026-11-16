#include "global.h"
#include "mummy.h"
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
void SceneActorReaction_ResolvePlacementDamage(void);
void SceneActorScript_OnCommand(void *actor, const u16 *command);
void AnimData_SelectPaletteBank(void *, s32);
void SceneActorPlacement_HideActor(void *, s32);
void SceneActorAnimation_SelectDirectionalEntry(void *, s32);
void Anim_Step(void *, void *);
void SceneActorBehavior_InstallCallbackTables(void *);
void SceneActorTerrain_ReadScriptParameters(void *);
u8 SceneActorRegistry_LinkActor(void *);

void SceneActorBehavior_InitializeShadow(void *);

void SceneActorMovement_ResolveRoutePosition(void *);
u8 sub_080D7568(void *);
void SceneActorPlacement_ResetStateThree(void);
void SceneActorReaction_BeginTargetRecovery(void);

static inline void MummyInit_ResetFlags(void *object)
{
    struct Mummy *actor = object;
    actor->flags6F2 = 0;
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

static inline void MummyInit_EnableInitialFlags(struct Mummy *actor)
{
    MonsterInit_SetTargetingFlags(&actor->chase.base.tracking, 1);
    MonsterInit_SetBehaviorFlags(&actor->chase, 0x600000);
    MonsterInit_SetStatusFlags(&actor->chase, 0x8000000);
}

static inline struct Mummy *MummyInit_Context(struct Mummy *actor)
{
    return actor->chase.base.context;
}

static inline void MummyInit_InstallCompletionCallbacks(struct Mummy *arg0)
{
    arg0->chase.base.onMessage = SceneActorScript_OnCommand;
    arg0->chase.base.update = (SceneActorResultCallback)Mummy_OnInteractionUpdate;
    arg0->chase.base.destroy = (SceneActorResultCallback)Mummy_OnReleaseResources;
}

static inline void MummyInit_InitializePaletteRange(struct Mummy *actor, u32 selected,
                                                    u32 alternate, u32 current)
{
    actor->chase.base.alternateOffset = selected;
    actor->chase.base.alternatePalette = MonsterInit_GetPaletteBase(&actor->chase) + alternate;
    actor->chase.base.paletteBank = MonsterInit_GetPaletteBase(&actor->chase) + current;
}

void Mummy_SetControlPreset(void *);
void SceneActorEffects_InitAtActorPosition(void *);
void sub_080D1644(void);
void sub_080D453C(void);

s32 Mummy_Init(void *object)
{
    struct Mummy *arg0 = object;
    struct Mummy *actor = object;
    struct SceneActorChaseGroup *group = &arg0->chase.group;
    u32 zero;
    struct ContactPosition tmp;
    u32 flag;

    MummyInit_ResetFlags(arg0);
    SceneActorRegistry_SetObjectContext(actor, actor, 0, 1);
    MonsterInit_SetCategory(actor, 1);
    Mummy_InitProfile(actor);
    if (MonsterInit_GetDrawableKind(&actor->chase) == 0) {
        u8 *heap;
        actor->chase.base.drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->chase.base.drawable, 0x5C);
        heap = MonsterInit_Drawable(&actor->chase);
        MonsterInit_InitializeSprite(heap, 0x48DE, 0xA4B2);
        if (MonsterInit_GetCategory(actor) == 0x11 || MonsterInit_GetCategory(actor) == 0x17)
            heap[7] = 1;
        actor->chase.base.body.direction = 0;
    } else {
        u8 *heap;
        actor->chase.base.drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->chase.base.drawable, 0x80);
        heap = MonsterInit_Drawable(&actor->chase);
        MonsterInit_ClearPosition(&zero, &tmp);
        MonsterInit_InitializeWidget(heap, 0xCB05, 0x48DE, &tmp);
        actor->chase.base.animationIndex = 0xFF;
    }
    MonsterInit_InitializePursuitBody(arg0, &zero);
    MonsterInit_ReadPalette(&actor->chase, 0x23, 0x192);
    MummyInit_InitializePaletteRange(actor, MonsterInit_GetPaletteBase(&actor->chase) + 3, 4, 0);
    Mummy_InitCollision(actor);
    MonsterInit_InstallSequenceCallbacks(
        &actor->chase, Mummy_OnUpdate, Mummy_GetAnimationStepOffset, Mummy_TurnTowardRoute,
        Mummy_UpdateWalking, Mummy_UpdateIdle, Mummy_UpdateHiddenRecovery,
        Mummy_UpdateDisplacementRecovery, Mummy_UpdateDefeat, SceneActorReaction_OnEnemyDefault,
        sub_080D7568, SceneActorTarget_ResolvePendingObject,
        SceneActorReaction_ResolvePlacementDamage, SceneActorPlacement_ResetStateThree);
    MonsterInit_InitializeGroupWithBounds(
        group, &arg0->routeProbe, &arg0->destination, Mummy_UpdateDying, Mummy_ResumeSavedTarget,
        SceneActorReaction_BeginTargetRecovery, Mummy_OnUnusedState, 3, &actor->chase.base.body);
    SceneActorMovement_ResolveRoutePosition(arg0);
    MonsterInit_SetRouteReset(&actor->chase, Mummy_ResetRevivalMotion);
    MonsterInit_SetReactionGate(&actor->chase, SceneActor_EnterMotionPhase);
    MonsterInit_SetContactCallback(&actor->chase, Mummy_SelectMovementResponse);
    group->responseCallback = Mummy_OnClearActivityFlag;
    group->field5EC = Mummy_OnMovementUpdate;
    group->routeChoiceControl = sub_080D1644;
    group->common.fallback = sub_080D453C;
    actor->chase.base.field2E8 = 1;
    MummyInit_EnableInitialFlags(actor);
    MonsterInit_MaskColliderFlags(&actor->chase.base.colliderA, ~1);
    SceneActorBehavior_InitializeShadow(actor);
    SceneActorEffects_InitAtActorPosition(arg0);
    SceneActorTerrain_ReadScriptParameters(actor);
    MonsterInit_ReadInitialMode(&arg0->chase);
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
    MonsterInit_ClearReceiver(&actor->chase);
    SceneActorBehavior_InstallCallbackTables(actor);
    Mummy_SetControlPreset(actor);
    Mummy_InitActionHandlers(actor);
    Mummy_SetBehaviorTable(actor);

    arg0->field6E8 = actor->chase.base.tracking.x;
    {
        struct Mummy *context = MummyInit_Context(actor);
        if (Script_SeekToKeyword(0x42)) {
            context->field6CC = MonsterInit_ReadHalfword(0x14);
            context->currentTick = MonsterInit_ReadHalfword(0xB4);
        } else {
            context->field6CC = 0x14;
            context->currentTick = 0xB4;
        }
    }
    {
        struct Mummy *context = MummyInit_Context(actor);
        context->limit = Div(actor->chase.base.tracking.range * 3, 10);
        context->field6DC = actor->chase.base.tracking.range;
    }

    MonsterInit_SaveControl(MonsterInit_SavedControlOf(&actor->chase), &actor->chase);
    MummyInit_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}
