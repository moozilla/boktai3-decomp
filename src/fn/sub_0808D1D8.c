#include "scene_actor_init_dispatch.h"
#include "global.h"
#include "scene_actor_init_group.h"
#include "render.h"
#include "engine_globals.h"
#include "display_sequence_inline.h"
#include "scene_actor_init_primitives.h"
#include "scene_actor_init_graphics.h"
#include "scene_actor_init_body.h"

void Memory_Clear(void *, u32);
void Ghoul_BindProfile(void *);
void Vorn_ParseField624_0808B320(void *);
void Ghoul_Update(void);
u32 Bat_LookupAnimationEntry(void *);
void Ghoul_TurnTowardRoute(void);
void Ghoul_UpdateIdleState(void);
void Ghoul_UpdateStandingState(void);
void sub_08087A70(void);
void Bat_UpdateDefeat(void);
void Ghoul_UpdateHiddenRecovery(void);
void SceneActorReaction_OnEnemyDefault(void);
u8 SceneActorTarget_UpdateTrackedContact(void *);
void SceneActorTarget_ResolvePendingObject(void);
void SceneActorReaction_ResolvePlacementDamage(void);
void SceneActorPlacement_ResetStateThree(void);
void Ghoul_UpdateDyingState(void);
void Bat_ResumeSavedTarget(void);
void SceneActorReaction_BeginTargetRecovery(void);
void Bat_UpdateAnimationRequest(void);
void Ghoul_ResetRouteState(void);
void SceneActorControl_RefreshMotionRoute_0808A90C(void);
void Ghoul_ReactToContact(void);
void Ghoul_OnLifecycleEvent(void);
void Ghoul_ResumeContactRoute(void);
void Ghoul_OnMessage(void);
void Ghoul_Destroy(void);
void SceneActorScript_OnCommand(void);
void Ghoul_InitCollision(void *);
void Ghoul_SetActionHandlers(void *);
void Ghoul_SetBehaviorHandlers(void *);
void Ghoul_SetSpecialHandlers(void *);
void Ghoul_SetBehaviorTable(void *);
void SceneActorMovement_ResolveRoutePosition(void *);
void SceneActorBehavior_InitializeShadow(void *);
void SceneActorPlacement_HideActor(void *, s32);
void SceneActorAnimation_SelectDirectionalEntry(void *, s32);
void SceneActorBehavior_InstallCallbackTables(void *);
void SceneActorTerrain_ReadScriptParameters(void *);

void *Heap_Alloc(u32);
void SceneActorRegistry_SetObjectContext(void *, void *, s32, s32);
void AnimData_SelectPaletteBank(void *, s32);
void Anim_Step(void *, void *);
u8 SceneActorRegistry_LinkActor(void *);

struct GhoulInstanceState {
    u8 unknown0[0x624];
    u16 field624, field626, field628;
    u8 unknown62A[0x14];
    u16 field63E;
    u8 field640;
    u8 unknown641[9];
    u16 field64A;
    u8 unknown64C[0x90];
    u32 field6DC;
    u8 unknown6E0[0x38];
    u32 field718;
};

extern u8 *gUnknown_02000580;

static inline void GhoulInit_InstallRouteDefaults(struct SceneActorRegistryResetView *arg0)
{
    arg0->callback248 = Ghoul_ResetRouteState;
    arg0->field23C = SceneActorControl_RefreshMotionRoute_0808A90C;
}

static inline void GhoulInit_InstallCompletionCallbacks(struct SceneActorRegistryResetView *arg0)
{
    arg0->messageCallback = SceneActorScript_OnCommand;
    arg0->updateCallback = Ghoul_OnMessage;
    arg0->destroyCallback = Ghoul_Destroy;
}

static inline void GhoulInit_InitializePaletteRange(struct SceneActorRegistryResetView *actor,
                                                    u32 alternate, u32 current)
{
    actor->alternatePalette = MonsterInit_GetPaletteBase(actor) + alternate;
    actor->paletteBank = MonsterInit_GetPaletteBase(actor) + current;
}

s32 Ghoul_Init(void *object)
{
    struct SceneActorRegistryResetView *arg0 = object;
    struct SceneActorRegistryResetView *actor = object;
    struct SceneActorChaseGroup *group = &arg0->group;
    struct SceneActorBody *body = &arg0->body;
    u32 zero;
    struct ContactPosition tmp;
    u32 flag;
    s32 mode;

    ((struct GhoulInstanceState *)arg0)->field628 = 0;
    SceneActorRegistry_SetObjectContext(actor, actor, 0, 1);
    MonsterInit_SetCategory(actor, 4);
    Ghoul_BindProfile(actor);
    Vorn_ParseField624_0808B320(actor);

    if (MonsterInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->drawable, 0x5C);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_InitializeSprite(heap, 0x9247, 0x1DF8);
        if (MonsterInit_GetCategory(actor) == 0x11 || MonsterInit_GetCategory(actor) == 0x17)
            heap[7] = 1;
        actor->body.direction = 0;
    } else {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->drawable, 0x80);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_ClearPosition(&zero, &tmp);
        MonsterInit_InitializeWidget(heap, 0xCB05, 0x9247, &tmp);
        actor->animationIndex = 0xFF;
    }
    MonsterInit_InitializePursuitBody(arg0, &zero);
    if (Script_SeekToKeyword('P')) {
        ((struct GhoulInstanceState *)arg0)->field640 = MonsterInit_ReadByte(0x78);
        ((struct GhoulInstanceState *)arg0)->field64A = MonsterInit_ReadHalfword(0x78);
        ((struct GhoulInstanceState *)arg0)->field63E = MonsterInit_ReadHalfword(0x78);
    } else {
        ((struct GhoulInstanceState *)arg0)->field640 = 0x78;
        ((struct GhoulInstanceState *)arg0)->field64A = 0x78;
    }
    MonsterInit_ReadPalette(actor, 0x192, 0x192);
    GhoulInit_InitializePaletteRange(actor, 4, 0);
    Ghoul_InitCollision(actor);
    MonsterInit_InstallAnimationCallbacks(
        actor, Ghoul_Update, Bat_LookupAnimationEntry, Ghoul_TurnTowardRoute, Ghoul_UpdateIdleState,
        Ghoul_UpdateStandingState, Ghoul_UpdateHiddenRecovery, sub_08087A70, Bat_UpdateDefeat,
        SceneActorReaction_OnEnemyDefault, SceneActorTarget_UpdateTrackedContact, SceneActorTarget_ResolvePendingObject,
        SceneActorReaction_ResolvePlacementDamage, SceneActorPlacement_ResetStateThree);
    MonsterInit_InitializeGroupWithBounds(group, (u8 *)arg0 + 0x64C, (u8 *)arg0 + 0x670,
                                          Ghoul_UpdateDyingState, Bat_ResumeSavedTarget,
                                          SceneActorReaction_BeginTargetRecovery,
                                          Bat_UpdateAnimationRequest, 1, body);
    SceneActorMovement_ResolveRoutePosition(arg0);
    GhoulInit_InstallRouteDefaults(actor);
    MonsterInit_SetContactCallback(actor, Ghoul_ReactToContact);
    group->responseCallback = Ghoul_OnLifecycleEvent;
    group->field5EC = Ghoul_ResumeContactRoute;
    MonsterInit_SetTargetingFlags(&actor->tracking, 1);
    MonsterInit_SetBehaviorFlags(actor, 0x1600000);
    MonsterInit_SetStatusFlags(actor, 0x8000000);
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
    if (Script_GetKeywordValue('S', 0) != 0 || !MonsterInit_IsInCurrentRegion(actor))
        SceneActorPlacement_HideActor(actor, 0);
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    if (MonsterInit_GetDrawableKind(actor) == 0)
        DisplaySequence_Step(actor->drawable);
    else
        Anim_Step(actor->drawable + 0x20, actor->drawable);
    MonsterInit_InitializeReceiver(actor);
    SceneActorBehavior_InstallCallbackTables(actor);
    Ghoul_SetActionHandlers(actor);
    Ghoul_SetBehaviorHandlers(actor);
    Ghoul_SetSpecialHandlers(actor);
    Ghoul_SetBehaviorTable(actor);
    MonsterInit_SaveControl(MonsterInit_SavedControlOf(actor), actor);
    GhoulInit_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}
