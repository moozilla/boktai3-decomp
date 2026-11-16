#include "global.h"
#include "scene_actor_chase.h"
#include "mummy.h"
#include "render.h"
#include "engine_globals.h"
#include "display_sequence_inline.h"
#include "scene_actor_init_primitives.h"
#include "scene_actor_init_group.h"
#include "scene_actor_init_dispatch.h"
#include "scene_actor_init_graphics.h"
#include "scene_actor_init_body.h"

// confidence=low: checkpoint completion restores player-route control and retries the route.

void SceneActor_UpdateAnimationCheckpoint(void);

void Mummy_UpdatePlayerRoute(struct ChaseActor *actor);

struct MummyCheckpointContext {
    u8 unknown0[0x6F2];
    s16 flags6F2 : 16;
};

static inline void Mummy_ClearMotion(struct ChaseActor *actor, u32 bits)
{
    struct ChaseFlags40 *field = (void *)&actor->base.motionFlags;
    s32 mask = ~bits;
    field->bits &= mask;
}

static inline void Mummy_ClearCheckpointFlags(struct ChaseFlags40 *flags, s32 mask)
{
    flags->bits &= mask;
}

void Mummy_ResumeRouteAfterCheckpoint(struct ChaseActor *actor)
{
    u8 *group = (u8 *)&actor->group.common;
    struct MummyCheckpointContext *body = actor->base.context;
    if (Chase_TakeArrived(actor)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        Chase_SetCtl(actor, SceneActor_UpdateAnimationCheckpoint, 0x26);
    }
    if (actor->base.stepDone) {
        s32 contextBits = 0x800;
        Mummy_ClearMotion(actor, 0x100);
        body->flags6F2 &= ~contextBits;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        Mummy_ClearCheckpointFlags((void *)(group + 0x40), ~6);
        {
            u32 mask = 0x1000;
            body->flags6F2 = mask | body->flags6F2;
        }
        Chase_SetCtl(actor, Mummy_UpdatePlayerRoute, 0x27);
        Chase_SetBehavior(actor, 0x10);
        if (!Chase_TryRoute(actor, *(void **)(group + 0x1FC), &actor->base.body.position,
                            &actor->base.body.position)) {
            Chase_SetCtl(actor, SceneActorControl_OnIdle, 0);
            Chase_SetState(actor, 3, 0, 0);
        }
    }
}

extern const u8 gUnk_086061DC[];

struct SSetBehaviorTable {
    u8 filler[652];
    const u8 *p;
};

void Mummy_SetBehaviorTable(struct SSetBehaviorTable *s)
{
    s->p = gUnk_086061DC;
}

s32 Mummy_UpdateBehavior(void *);

s32 Callback_InvokeActor(void *, void *);

void SceneActorControl_DispatchControl(void *);

s32 Mummy_UpdateSpecialFlags(void *);

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

s32 Mummy_OnUpdate(struct SOnUpdate *s)
{
    u32 m;
    Mummy_UpdateBehavior(s);
    m = 0x2000;
    if ((s->flags & m) || s->h2b8 == 0) {
        Callback_InvokeActor(s, s->tbl[s->idx]);
        SceneActorControl_DispatchControl(s);
    }
    Mummy_UpdateSpecialFlags(s);
    return 1;
}

void SceneActor_UpdateBehaviorCallbacks(void);

s32 Mummy_OnInteractionUpdate(void)
{
    SceneActor_UpdateBehaviorCallbacks();
    return 0;
}

void SceneActorEffects_ReleaseResources(void *);

s32 Mummy_OnReleaseResources(u8 *p)
{
    SceneActorEffects_ReleaseResources(p);
    return 0;
}

void *Heap_Alloc(s32 size); /* first-fit allocation from gHeapHead; NULL on failure */

void Memory_Clear(void *dst, s32 bytes); /* zero fill (bytes, then CpuSet words) */

void SceneActorRegistry_SetObjectContext(void *, void *, s32, s32);

u32 SceneActorReaction_OnEnemyDefault(void);

void SceneActorReaction_ResolvePlacementDamage(void);

void SceneActorScript_OnCommand(void *actor, const u16 *command);

void AnimData_SelectPaletteBank(void *, s32);

void SceneActorPlacement_HideActor(void *, s32);

void SceneActorAnimation_SelectDirectionalEntry(struct RoomActor *, s32);

s32 Anim_Step(void *, void *);

void SceneActorBehavior_InstallCallbackTables(void *);

void SceneActorTerrain_ReadScriptParameters(void *);

u8 SceneActorRegistry_LinkActor(void *);

void SceneActorBehavior_InitializeShadow(void *);

void SceneActorMovement_ResolveRoutePosition(void *);

u8 Mummy_TryEngageTarget(struct ChaseActor *actor);

void SceneActorPlacement_ResetStateThree(void);

void SceneActorReaction_BeginTargetRecovery(void);

static inline void MummyInit_ResetFlags(void *object)
{
    struct Mummy *actor = object;
    actor->flags6F2 = 0;
}

void Mummy_InitProfile(struct SInitProfile *s);

void Mummy_InitCollision(u8 *p);

void Mummy_InitActionHandlers(struct SInitActionHandlers *s);

void Mummy_SetBehaviorTable(struct SSetBehaviorTable *s);

s32 Mummy_OnUpdate(struct SOnUpdate *s);

s32 Mummy_GetAnimationStepOffset(struct R *s);

void Mummy_TurnTowardRoute(void *);

void Mummy_UpdateWalking(void *);

void Mummy_UpdateIdle(void *);

void Mummy_UpdateDisplacementRecovery(struct ChaseActor *actor, s32 frame);

void Mummy_UpdateDefeat(struct MummyDefeatActor *actor, s32 frame);

void Mummy_UpdateHiddenRecovery(struct ChaseActor *actor, s32 frame);

void SceneActorTarget_ResolvePendingObject(void);

void Mummy_UpdateDying(u8 *s, s32 arg);

void Mummy_ResumeSavedTarget(struct ChaseActor *actor, s32 frame);

void Mummy_OnUnusedState(void);

void Mummy_ResetRevivalMotion(struct ChaseActor *actor);

void SceneActor_EnterMotionPhase(struct ChaseActor *actor);

u8 Mummy_SelectMovementResponse(struct ChaseActor *actor);

void Mummy_OnClearActivityFlag(struct SOnClearActivityFlag *s);

void Mummy_OnMovementUpdate(struct SOnMovementUpdate *s);

s32 Mummy_OnInteractionUpdate(void);

s32 Mummy_OnReleaseResources(u8 *);

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

void Mummy_SetControlPreset(struct SSetControlPreset *s);

void SceneActorEffects_InitAtActorPosition(void *);

void Mummy_UpdateRecoveryWait(void);

void Mummy_TryStimulusRoute(void);

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
    MonsterInit_SetCategory(actor, MONSTER_CATEGORY_MUMMY);
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
        Mummy_TryEngageTarget, SceneActorTarget_ResolvePendingObject,
        SceneActorReaction_ResolvePlacementDamage, SceneActorPlacement_ResetStateThree);
    MonsterInit_InitializeGroupWithBounds(
        group, &arg0->routeProbe, &arg0->destination, Mummy_UpdateDying, Mummy_ResumeSavedTarget,
        SceneActorReaction_BeginTargetRecovery, Mummy_OnUnusedState, 3, &actor->chase.base.body);
    SceneActorMovement_ResolveRoutePosition(arg0);
    MonsterInit_SetRouteReset(&actor->chase, Mummy_ResetRevivalMotion);
    MonsterInit_SetReactionGate(&actor->chase, SceneActor_EnterMotionPhase);
    MonsterInit_SetContactCallback(&actor->chase, Mummy_SelectMovementResponse);
    group->responseCallback = Mummy_OnClearActivityFlag;
    group->routeRestoreCallback = Mummy_OnMovementUpdate;
    group->routeChoiceControl = Mummy_UpdateRecoveryWait;
    group->common.fallback = Mummy_TryStimulusRoute;
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

s32 Mummy_Init(void *object);

void Heap_Free(void *);

void Mummy_Create(void)
{
    u8 *p = Heap_Alloc(0x720);
    if (p != 0) {
        Memory_Clear(p, 0x720);
        if (Mummy_Init(p) < 0) {
            Mummy_OnReleaseResources(p);
            Heap_Free(p);
        }
    }
}
