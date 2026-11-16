#include "global.h"
#include "dog.h"
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
void SceneActor_RefreshLivingCountAndProfile(void *);
void SceneActorCollision_InitGateColliders(void *);
void SceneActorMovement_ResolveRoutePosition(void *);
void SceneActorBehavior_InitializeShadow(void *);
void SceneActorTerrain_ReadScriptParameters(void *);
void SceneActorPlacement_HideActor(void *, s32);
void SceneActorAnimation_SelectDirectionalEntry(void *, s32);
void Anim_Step(void *, void *);
void SceneActorBehavior_InstallCallbackTables(void *);
u8 SceneActorRegistry_LinkActor(void *);
void SceneActorScript_OnCommand(void);
void SceneActorPlacement_ResetStateThree(void);
void SceneActorReaction_BeginTargetRecovery(void);
u32 SceneActorReaction_OnEnemyDefault(void);
void SceneActorReaction_ResolvePlacementDamage(void);
s32 Dog_Update(void *);
u16 sub_080DE6E0(void *);
void sub_080DFC34(void *);
void sub_080E0750(void *);
void Dog_SelectAnimationOne(void *);
void Dog_SelectAnimationZero(void *);
void SceneActorControl_BeginHiddenDefeat(void *);
void Dog_OnReactionCleanup(void *);
u8 SceneActorTarget_UpdateTrackedContact(void *);
void Dog_ResolvePendingObject(void);
void Dog_UpdateDefeatDelay(void);
void Dog_ResumeAfterReaction(void);
void Dog_OnSecondaryReactionCleanup(void);
void Dog_ResetActionState(void);
void Dog_UpdateTargetGate(void);
s32 Dog_ChooseReactionBehavior(void *);
void Dog_OnClearActivityFlag(void);
void Dog_RestoreRoute(void);
void Dog_UpdateTurnSequence(void);
void Dog_SetControlPreset(void *);
void Dog_SetPrimaryBehaviorPreset(void *);
void Dog_SetSecondaryBehaviorPreset(void *);
void Dog_OnAuxiliaryUpdate(void);
void Dog_ReleaseResources(void);

static inline struct Dog *DogInit_Context(struct ChaseActor *actor)
{
    return actor->base.context;
}

static inline void DogInit_Reset(void *object)
{
    struct Dog *context = object;
    context->flags = 0;
    context->state6A6 = 0;
}

static inline void DogInit_EnableInitialFlags(struct ChaseActor *actor)
{
    MonsterInit_SetTargetingFlags(&actor->base.tracking, 1);
    MonsterInit_SetBehaviorFlags(actor, 0x1600000);
    MonsterInit_SetStatusFlags(actor, 8);
}

static inline void DogInit_InstallCompletionCallbacks(struct ChaseActor *actor)
{
    actor->base.messageCallback = SceneActorScript_OnCommand;
    actor->base.updateCallback = Dog_OnAuxiliaryUpdate;
    actor->base.destroyCallback = Dog_ReleaseResources;
}

s32 Dog_Init(void *object)
{
    struct Dog *dog = object;
    struct ChaseActor *actor = &dog->chase;
    struct SceneActorChaseGroup *group = &dog->chase.group;
    u32 zero;
    struct ContactPosition tmp;
    u32 flag;

    DogInit_Reset(dog);
    SceneActorRegistry_SetObjectContext(actor, actor, 0, 1);
    MonsterInit_SetCategory(actor, 0x15);
    SceneActor_RefreshLivingCountAndProfile(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->base.drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->base.drawable, 0x5C);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_InitializeSprite(heap, 0xD635, 0xF6E7);
        if (MonsterInit_GetCategory(actor) == 0x11 || MonsterInit_GetCategory(actor) == 0x17)
            heap[7] = 1;
        actor->base.body.direction = 0;
    } else {
        u8 *heap;
        actor->base.drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->base.drawable, 0x80);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_ClearPosition(&zero, &tmp);
        MonsterInit_InitializeWidget(heap, 0xCB05, 0xD635, &tmp);
        actor->base.animationIndex = 0xFF;
    }
    MonsterInit_InitializePursuitBody(&dog->chase, &zero);

    if (Script_SeekToKeyword('P')) {
        dog->nearDistance = (Script_GetPc() ? Script_GetValue() : 200);
        dog->farDistance = (Script_GetPc() ? Script_GetValue() : 320);
    }
    if (Script_SeekToKeyword('Y')) {
        MonsterInit_ReadScriptWord(&dog->rangeSquared, 0);
        dog->rangeSquared *= dog->rangeSquared;
        dog->limitX = (Script_GetPc() ? Script_GetValue() : 640);
        dog->limitZ = (Script_GetPc() ? Script_GetValue() : 640);
    }
    dog->interval = Script_GetKeywordValue('d', 50);
    if (!dog->interval)
        dog->interval = 50;
    dog->destinationCell = 0xFFFF;
    dog->timer6A4 = 0;

    MonsterInit_ReadPalette(actor, 0x273, 0x192);
    SceneActorCollision_InitGateColliders(actor);
    MonsterInit_InstallSequenceCallbacks(
        actor, Dog_Update, sub_080DE6E0, sub_080DFC34, Dog_SelectAnimationOne,
        Dog_SelectAnimationZero, Dog_OnReactionCleanup, sub_080E0750,
        SceneActorControl_BeginHiddenDefeat, SceneActorReaction_OnEnemyDefault,
        SceneActorTarget_UpdateTrackedContact, Dog_ResolvePendingObject,
        SceneActorReaction_ResolvePlacementDamage, SceneActorPlacement_ResetStateThree);
    MonsterInit_InitializeGroupWithBounds(group, &dog->routeProbe, &dog->destination,
                                          Dog_UpdateDefeatDelay, Dog_ResumeAfterReaction,
                                          SceneActorReaction_BeginTargetRecovery,
                                          Dog_OnSecondaryReactionCleanup, 1, &actor->base.body);
    SceneActorMovement_ResolveRoutePosition(dog);
    MonsterInit_SetRouteReset(actor, Dog_ResetActionState);
    MonsterInit_SetReactionGate(actor, Dog_UpdateTargetGate);
    MonsterInit_SetContactCallback(actor, Dog_ChooseReactionBehavior);
    group->responseCallback = Dog_OnClearActivityFlag;
    group->field5EC = Dog_RestoreRoute;
    group->turnControl = Dog_UpdateTurnSequence;
    group->common.gate.field41A = 20;
    actor->base.field2E8 = 0;
    DogInit_EnableInitialFlags(actor);
    SceneActorBehavior_InitializeShadow(actor);
    SceneActorTerrain_ReadScriptParameters(actor);
    MonsterInit_ReadInitialMode(&dog->chase);
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
        MonsterInit_MaskColliderFlags(&actor->base.colliderA, ~0x100);
    if (Script_GetKeywordValue('S', 0) != 0 || !MonsterInit_IsInCurrentRegion(actor))
        SceneActorPlacement_HideActor(actor, 0);
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    if (MonsterInit_GetDrawableKind(actor) == 0)
        DisplaySequence_Step(actor->base.drawable);
    else
        Anim_Step(actor->base.drawable + 0x20, actor->base.drawable);
    MonsterInit_ClearReceiver(actor);

    {
        struct Dog *context = DogInit_Context(actor);
        u32 *speed;
        if (MonsterInit_GetPaletteOffset(actor) == 0 || MonsterInit_GetPaletteOffset(actor) == 2) {
            speed = &context->speed;
            *speed = 0x40000;
            context->lastCell = 0xFFFF;
            context->currentSpeed = *speed;
        } else {
            u32 *primary = &context->speed;
            *primary = 0x40000;
            speed = primary;
            context->alternateSpeed = 0x38400;
            context->lastCell = 0xFFFF;
            context->currentSpeed = *speed;
        }
    }
    SceneActorBehavior_InstallCallbackTables(actor);
    Dog_SetControlPreset(actor);
    Dog_SetPrimaryBehaviorPreset(actor);
    Dog_SetSecondaryBehaviorPreset(actor);
    MonsterInit_SaveControl(MonsterInit_SavedControlOf(actor), actor);
    DogInit_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}
