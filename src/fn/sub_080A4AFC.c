#include "scene_actor_init_dispatch.h"
#include "global.h"
#include "render.h"
#include "engine_globals.h"
#include "display_sequence_inline.h"
#include "scene_actor_init_group.h"
#include "scene_actor_init_graphics.h"
#include "scene_actor_init_body.h"

void *Heap_Alloc(u32);
void Memory_Clear(void *, u32);
void SceneActorRegistry_SetObjectContext(void *, void *, s32, s32);
void SceneActorReaction_SinkAndRemove(struct SceneActorRegistryResetView *, s32);
u32 SceneActorReaction_OnEnemyDefault(void);
void SceneActorReaction_ResolvePlacementDamage(void);
void SceneActorScript_OnCommand(void);
extern SceneActorCommandHandler const gUnk_086108B0[];
void AnimData_SelectPaletteBank(void *, s32);
void SceneActorPlacement_HideActor(void *, s32);
void SceneActorAnimation_SelectDirectionalEntry(void *, s32);
void Anim_Step(void *, void *);
void SceneActorBehavior_InstallCallbackTables(void *);
void SceneActorTerrain_ReadScriptParameters(void *);
u8 SceneActorRegistry_LinkActor(void *);

struct SpiderDeferredControl {
    u8 unknown0[8];
    u8 state, phase, mode, unknownB;
    u32 counter;
    void *callback;
};
struct SpiderInstanceState {
    u8 unknown0[0x624];
    u8 unknown624[8];
    u16 flags62C;
    u16 timer62E;
    u8 bounds630[0x24];
    u8 bounds654[0x24];
};

void Spider_InitProfile(void *);
u32 Spider_OnUpdate(void *);
u32 Spider_GetAnimationEvent(void *);
void Spider_AdvanceTurnAnimation(void *);
void Spider_SelectActiveAnimation(void *);
void Spider_SelectIdleAnimation(void *);
void Spider_UpdateLandingMotion(void *);
void Spider_OnActionEvent(void *);
void Spider_InitColliders(void *);
void Spider_SetBehaviorTables(void *);
void SceneActorPlacement_BeginDefaultResponse(void);
u8 SceneActorPlacement_BeginContactRoute(void *);
u8 SceneActorPlacement_BeginNearbyPlayerContact(void *);
u8 SceneActorPlacement_SelectPlayerApproach(void *);
void SceneActorReaction_ResumeEntryTarget(void);
void SceneActorRoute_ResetStep(void);
void Spider_OnSecondaryColliderContact(void);
void Spider_UpdateHiddenRecovery(void);
void SceneActorReaction_RestoreAlternateAfterDelay(void);
void ScaledShadow_Initialize(void *, void *, s32, s32, s32, s32, s32, s32, s32, s32, s32);

extern u8 *gUnknown_02000580;

void SceneActorMovement_ResolveRoutePosition(void *);
void Spider_UpdateDefeat(void *);
void SceneActorReaction_BeginTargetRecovery(void);
void Spider_OnSecondaryUpdate(void);
void Spider_Destroy(void);
extern const u8 gUnk_08605F34[];

static inline void MonsterInit_EnableInitialFlags(struct SceneActorRegistryResetView *arg0)
{
    MonsterInit_SetTargetingFlags(&arg0->tracking, 1);
    MonsterInit_SetBehaviorFlags(arg0, 0x400000);
    MonsterInit_SetStatusFlags(arg0, 8);
}

static inline void MonsterInit_InstallCompletionCallbacks(struct SceneActorRegistryResetView *arg0)
{
    arg0->messageCallback = SceneActorScript_OnCommand;
    arg0->updateCallback = Spider_OnSecondaryUpdate;
    arg0->destroyCallback = Spider_Destroy;
}

static inline void Spider_SetFlags(struct SpiderInstanceState *actor, u32 mask)
{
    actor->flags62C |= mask;
}
static inline void Spider_InitializeDeferred(struct SpiderDeferredControl *control, void *callback,
                                             u32 mode, u32 timer)
{
    control->counter = timer;
    control->callback = callback;
    control->state = 0;
    control->phase = 0;
    control->mode = mode;
}

static inline void Spider_InstallContact(struct SceneActorRegistryResetView *actor,
                                         struct SceneActorChaseGroup *group)
{
    MonsterInit_WriteDataPointer(&actor->commandTable.opaque, gUnk_08605F34);
    group->common.gate.control = SceneActorReaction_ResumeEntryTarget;
    if (MonsterInit_GetPaletteOffset(actor) == 0)
        MonsterInit_WriteContactPredicate(&actor->contactQuery.predicate,
                                          SceneActorPlacement_BeginNearbyPlayerContact);
    else
        MonsterInit_WriteContactPredicate(&actor->contactQuery.predicate,
                                          SceneActorPlacement_SelectPlayerApproach);
}
s32 Spider_Init(void *object)
{
    struct SpiderInstanceState *arg0 = object;
    struct SceneActorRegistryResetView *actor = object;
    struct SceneActorChaseGroup *group = &((struct SceneActorRegistryResetView *)arg0)->group;
    u32 zero;
    struct ContactPosition tmp;
    u32 flag;
    s32 extra;

    arg0->flags62C = 0;
    SceneActorRegistry_SetObjectContext(actor, actor, 0, 1);
    MonsterInit_SetCategory(actor, 2);
    Spider_InitProfile(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->drawable, 0x5C);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_InitializeSprite(heap, 0x528F, 0xB6EE);
        if (MonsterInit_GetCategory(actor) == 0x11 || MonsterInit_GetCategory(actor) == 0x17)
            heap[7] = 1;
        actor->body.direction = 0;
    } else {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->drawable, 0x80);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_ClearPosition(&zero, &tmp);
        MonsterInit_InitializeWidget(heap, 0xCB05, 0x528F, &tmp);
        actor->animationIndex = 0xFF;
    }
    MonsterInit_InitializePursuitBody(actor, &zero);
    MonsterInit_ReadPalette(actor, 0xC8, 0x192);
    Spider_InitColliders(actor);
    extra = Script_GetKeywordValue(0x66, 0);
    if (Script_GetKeywordValue(0x54, 0))
        Spider_SetFlags(arg0, 0x10);
    MonsterInit_InstallAnimationCallbacks(
        actor, Spider_OnUpdate, Spider_GetAnimationEvent, Spider_AdvanceTurnAnimation,
        Spider_SelectActiveAnimation, Spider_SelectIdleAnimation, Spider_OnActionEvent,
        Spider_UpdateLandingMotion, Spider_UpdateDefeat, SceneActorReaction_OnEnemyDefault, 0,
        SceneActorPlacement_BeginContactRoute, SceneActorReaction_ResolvePlacementDamage,
        SceneActorPlacement_BeginDefaultResponse);
    MonsterInit_InitializeGroupWithBounds(
        group, arg0->bounds630, arg0->bounds654, Spider_OnSecondaryColliderContact, 0,
        SceneActorReaction_BeginTargetRecovery, 0, 1, &actor->body);
    SceneActorMovement_ResolveRoutePosition(actor);
    MonsterInit_SetRouteReset(actor, SceneActorRoute_ResetStep);
    MonsterInit_SetReactionGate(actor, 0);
    Spider_InstallContact(actor, group);
    MonsterInit_EnableInitialFlags(actor);
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
    MonsterInit_ClearReceiver(actor);
    SceneActorBehavior_InstallCallbackTables(actor);
    Spider_SetBehaviorTables(actor);
    if (extra != 0) {
        actor->field2EA = extra;
        actor->field2E8 = 2;
        MonsterInit_SetControl(actor, Spider_UpdateHiddenRecovery, 0x23);
        MonsterInit_SetState(actor, 3, 0, 0);
        ScaledShadow_Initialize((u8 *)actor + 0x360, &actor->body.position, 0x40, 0x10, 0x60, 0x64,
                                0xA00, 1, 5, 1, 0);
        Spider_SetFlags(arg0, 1);
    } else {
        MonsterInit_SetControl(actor, SceneActorReaction_RestoreAlternateAfterDelay, 0xE);
        Spider_InitializeDeferred((void *)&group->saved, actor->entryTarget228, 7, 0x3C);
    }
    arg0->timer62E = 0x12C;
    MonsterInit_SaveControl(MonsterInit_SavedControlOf(actor), actor);
    MonsterInit_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}
