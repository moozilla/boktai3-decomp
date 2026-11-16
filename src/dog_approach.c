#include "global.h"
#include "scene_actor_control_phase.h"
#include "dog.h"
#include "render.h"
#include "engine_globals.h"
#include "display_sequence_inline.h"
#include "scene_actor_init_primitives.h"
#include "scene_actor_init_group.h"
#include "scene_actor_init_dispatch.h"
#include "scene_actor_init_graphics.h"
#include "scene_actor_init_body.h"
#include "scene_actor_chase.h"
#include "scene_actor_reset.h"

// confidence=low: approaches the navigation probe and selects the next dog behavior.

/* SceneActorControl_UpdateProbeApproach: control phase that moves the actor toward its nav probe descriptor (+0x5D0): turns on a probe event, otherwise accelerates; changes state on arrival */

u8 SceneActorReaction_IsRangeReady(struct ChaseActor *);

s32 Callback_InvokeActor(void *, void *);

u8 sub_0812F3CC(struct ChaseActor *);

void SceneActorMotion_UpdateTargetTransition(void);

u8 Dog_IsActionTimerExpired(void *);

void Anim_SetDurationScale(void *, u32);

void Anim_SetFrameDurationScale(void *, u32);

u32 NavMesh_ConsumeProbeEvent(void *);

u8 SceneActorBehavior_RequestDescriptorTurn(void *, void *, u32, u32, s32);

u8 SceneActorBehavior_AccelerateTowardDescriptor(void *, void *, u32, u32, s32);

#define FIELD(p, off, T) (*(T *)((u8 *)(p) + (off)))

static inline void SetControl(struct ChaseActor *actor, void *control, u32 kind)
{
    actor->base.field2B2 = 1;
    actor->base.stepDone = 0;
    actor->base.controlKind = kind;
    actor->base.target = control;
    actor->base.field2C8 = 0;
    {
        u32 *flags = &actor->base.pendingControlFlags;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->base.pendingControl = 0;
}

static inline void ResetState(struct ChaseActor *actor, u32 state, u32 phase, u32 behavior)
{
    actor->base.state2A8 = state;
    actor->base.phase2A9 = phase;
    actor->base.behavior2AA = behavior;
    actor->base.counter2AC = 0;
    {
        u16 offset = 0x2A9;
        offset += 8;
        *((u8 *)actor + offset) = 1;
    }
}

static inline void SetState(struct ChaseActor *actor)
{
    u32 behavior = 18;
    actor->base.state2A8 = 0;
    actor->base.phase2A9 = 0;
    actor->base.behavior2AA = behavior;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}

// confidence=low: approaches the navigation probe and selects the next dog state.

void Dog_UpdateProbeApproach(struct ChaseActor *actor)
{
    u8 *nav = (u8 *)actor + 0x3D4;
    u8 *control = (u8 *)SceneActorControl_GetState(actor);
    s32 speed;
    u32 event;
    u16 *scale;

    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        SetControl(actor, SceneActorMotion_UpdateTargetTransition, 0x2A);
        {
            u32 bit = 8;
            u32 *flags = (u32 *)(control + 0x678);
            *flags |= bit;
        }
    }
    speed = Callback_InvokeActor(*(&actor), *((void **)(((u8 *)actor) + 0x260)));
    actor->base.field2C2 = 0x20;
    {
        u32 mask = 0xA02F;
        if (actor->base.motionFlags & mask) {
            u16 *p = &actor->base.field2C2;
            *p = 0x40;
            scale = p;
        } else {
            u16 *q = &actor->base.field2C2;
            u16 v = *q;
            scale = q;
            if (v == 0) {
                *scale = 0x40;
            }
        }
    }
    if (actor->base.drawableKind == 0)
        Anim_SetDurationScale((u8 *)actor->base.drawable + 0x48, *scale);
    else
        Anim_SetFrameDurationScale((u8 *)actor->base.drawable + 0x20, *scale);
    event = NavMesh_ConsumeProbeEvent(FIELD(nav, 0x1FC, void *));
    if (event) {
        if (SceneActorBehavior_RequestDescriptorTurn(actor, FIELD(nav, 0x1FC, void *), 0x20, 0x20,
                                                     speed)) {
            SetState(actor);
        }
    } else if (SceneActorBehavior_AccelerateTowardDescriptor(actor, FIELD(nav, 0x1FC, void *), 0x20,
                                                             0x20, speed)) {
        ResetState(actor, 3, 1, 0);
        {
            u32 bit = 4;
            u32 *flags = (u32 *)(control + 0x678);
            *flags |= bit;
        }
        return;
    }
    if (Dog_IsActionTimerExpired(actor)) {
        if ((u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque)) {
            return;
        }
        if (sub_0812F3CC(actor)) {
            return;
        }
    }
    actor->base.counter2AC++;
}

/* Dog_UpdateActionPhase: queues control 2 once, runs the callback while the action timer has expired, else switches phase */

static inline void SelectControl(struct ChaseActor *p, void *control, u32 kind)
{
    p->base.field2B2 = 1;
    p->base.stepDone = 0;
    p->base.controlKind = kind;
    p->base.target = control;
    p->base.field2C8 = 0;
    {
        u32 *flags = &p->base.pendingControlFlags;
        s32 mask = -2;
        *flags &= mask;
    }
    p->base.pendingControl = 0;
}

void Dog_UpdateActionPhase(struct ChaseActor *p)
{
    if (SceneActorControl_TakeFlag(&p->base.field2B1)) {
        void *control = p->base.onEntry;
        u32 kind = 2;
        SelectControl(p, control, kind);
    }
    if (Dog_IsActionTimerExpired(p)) {
        if ((u8)Callback_InvokeActor(p, p->base.contactQuery.opaque))
            return;
        if (sub_0812F3CC(p))
            return;
    }
    if (p->base.stepDone) {
        u32 phase = 0x11;
        p->base.state2A8 = 0;
        p->base.phase2A9 = 0;
        p->base.behavior2AA = phase;
        p->base.counter2AC = 0;
        p->base.field2B1 = 1;
    } else
        ++p->base.counter2AC;
}

/* SceneActorControl_UpdateEntryRecoveryPhase_080E55AC: same code as SceneActorControl_UpdateEntryRecoveryPhase at another address (ported by relatives.py; provisional name) */

void SceneActorMotion_TestTargetTransition(void);

static inline void SetState_UpdateEntryRecoveryPhase_080E55AC(struct ChaseActor *actor)
{
    u32 state = 3;
    u32 phase = 1;
    actor->base.state2A8 = state;
    actor->base.phase2A9 = phase;
    actor->base.behavior2AA = 0;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}

void SceneActorControl_UpdateEntryRecoveryPhase_080E55AC(struct ChaseActor *actor)
{
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        void *control = actor->base.onEntry;
        SetControl(actor, control, 2);
    }
    if (Dog_IsActionTimerExpired(actor)) {
        if ((u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque))
            return;
        if (sub_0812F3CC(actor))
            return;
    }
    if (actor->base.stepDone) {
        SetState_UpdateEntryRecoveryPhase_080E55AC(actor);
        SetControl(actor, (void *)SceneActorMotion_TestTargetTransition, 43);
    } else
        ++actor->base.counter2AC;
}

extern const u8 gUnk_08606214[];

struct S {
    u8 filler[644];
    const u8 *p;
};

void Dog_SetControlPreset(struct S *s)
{
    s->p = gUnk_08606214;
}

void Dog_PrepareFrame(void *);

void SceneActorControl_DispatchControl(void *);

void Dog_FinishFrame(void *);

struct SUpdate {
    u8 filler[0x27c];
    u32 *tbl;
    u8 filler2[0x2a8 - 0x280];
    u8 idx;
    u8 filler3[0x2b8 - 0x2a9];
    u16 h2b8;
};

s32 Dog_Update(struct SUpdate *s)
{
    struct SUpdate *s2 = s;
    Dog_PrepareFrame(s);
    if (s->h2b8 == 0) {
        Callback_InvokeActor(s, s->tbl[s->idx]);
        SceneActorControl_DispatchControl(s);
    }
    Dog_FinishFrame(s2);
    return 1;
}

void SceneActor_UpdateBehaviorCallbacks(void);

s32 Dog_OnAuxiliaryUpdate(void)
{
    SceneActor_UpdateBehaviorCallbacks();
    return 0;
}

void SceneActorEffects_ReleaseResources(void *);

s32 Dog_ReleaseResources(u8 *p)
{
    SceneActorEffects_ReleaseResources(p);
    return 0;
}

void *Heap_Alloc(s32 size); /* first-fit allocation from gHeapHead; NULL on failure */

void Memory_Clear(void *dst, s32 bytes); /* zero fill (bytes, then CpuSet words) */

void SceneActorRegistry_SetObjectContext(void *, void *, s32, s32);

void SceneActor_RefreshLivingCountAndProfile(void *);

void SceneActorCollision_InitGateColliders(void *);

void SceneActorMovement_ResolveRoutePosition(void *);

void SceneActorBehavior_InitializeShadow(void *);

void SceneActorTerrain_ReadScriptParameters(void *);

void SceneActorPlacement_HideActor(void *, s32);

void SceneActorAnimation_SelectDirectionalEntry(struct RoomActor *, s32);

s32 Anim_Step(void *, void *);

void SceneActorBehavior_InstallCallbackTables(void *);

u8 SceneActorRegistry_LinkActor(void *);

void SceneActorScript_OnCommand(void *actor, const u16 *command);

void SceneActorPlacement_ResetStateThree(void);

void SceneActorReaction_BeginTargetRecovery(void);

u32 SceneActorReaction_OnEnemyDefault(void);

void SceneActorReaction_ResolvePlacementDamage(void);

s32 Dog_Update(struct SUpdate *s);

u16 Dog_UpdateItemDrop(void *);

void Dog_UpdateCardinalTurn(void *);

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

void Dog_SetControlPreset(struct S *s);

void Dog_SetPrimaryBehaviorPreset(void *);

void Dog_SetSecondaryBehaviorPreset(void *);

s32 Dog_OnAuxiliaryUpdate(void);

s32 Dog_ReleaseResources(u8 *p);

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
    actor->base.onMessage = SceneActorScript_OnCommand;
    actor->base.update = (SceneActorResultCallback)Dog_OnAuxiliaryUpdate;
    actor->base.destroy = (SceneActorResultCallback)Dog_ReleaseResources;
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
    MonsterInit_SetCategory(actor, MONSTER_CATEGORY_DOG);
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
        actor, Dog_Update, Dog_UpdateItemDrop, Dog_UpdateCardinalTurn, Dog_SelectAnimationOne,
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
    group->routeRestoreCallback = Dog_RestoreRoute;
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

s32 Dog_Init(void *object);

void Heap_Free(void *);

void Dog_Create(void)
{
    u8 *p = Heap_Alloc(0x6e0);
    if (p != 0) {
        Memory_Clear(p, 0x6e0);
        if (Dog_Init(p) < 0) {
            Dog_ReleaseResources(p);
            Heap_Free(p);
        }
    }
}

static inline void clearFlag(u32 *flags)
{
    u32 mask = ~1;
    *flags &= mask;
}

static inline void clearRouteFlag(u32 *flags)
{
    u32 mask = ~0x1000000;
    *flags &= mask;
}

static inline void resetState(struct ChaseActor *actor, u8 behavior)
{
    actor->base.state2A8 = 0;
    actor->base.phase2A9 = 0;
    actor->base.behavior2AA = behavior;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}

static inline void setControl(struct ChaseActor *actor, void *control, u8 kind)
{
    actor->base.field2B2 = 1;
    actor->base.stepDone = 0;
    actor->base.controlKind = kind;
    actor->base.target = control;
    actor->base.field2C8 = 0;
    clearFlag(&actor->base.pendingControlFlags);
    actor->base.pendingControl = 0;
}

static inline u32 testFlags(struct ChaseActor *actor, u32 mask)
{
    return actor->base.behaviorFlags & mask;
}

void Dog_ResetIdleControl(struct ChaseActor *actor)
{
    if (Chase_TakeArrived(actor)) {
        struct SceneActorReactionMotion *motion;
        struct SceneActorRegistryControl *control;
        u16 *flags;
        actor->base.field2C2 = 0x40;
        actor->group.common.gate.flags = 0;
        actor->group.common.gate.counter = 0;
        flags = &actor->base.motionFlags;
        {
            u32 mask = ~0x380;
            mask &= *flags;
            *flags = mask;
        }
        motion = (void *)&actor->base.tracking.target;
        {
            s32 mask = ~0x18;
            mask &= motion->flags;
            motion->flags = mask;
        }
        control = &actor->group.common.control;
        if (testFlags(actor, 0x200000)) {
            clearRouteFlag(&actor->base.statusFlags);
            {
                u32 mask = ~0x200;
                mask &= *flags;
                *flags = mask;
            }
            control->mode = 0;
            control->counter = 0;
            control->timer = 0;
        }
        {
            u32 mask = ~0x400;
            mask &= *flags;
            *flags = mask;
        }
        actor->group.turn.turnsRemaining = 0xFF;

        motion->fieldDC = 0;
        setControl(actor, actor->base.onStanding, 0);
    }
    Callback_InvokeActor(actor, actor->base.contactQuery.opaque);
}

static inline u8 TakeEntry(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return 1;
    }
    return 0;
}

void SceneActor_UpdateAlternateEntryPhase(struct ChaseActor *actor)
{
    if (TakeEntry(&actor->base.field2B1)) {
        void *target = actor->base.onAlternateEntry;
        u32 one = 1;
        u32 zero;
        u32 *flags;
        s32 mask;
        u8 *prepared = &actor->base.field2B2;
        zero = 0;
        *prepared = one;
        actor->base.stepDone = zero;
        actor->base.controlKind = one;
        actor->base.target = target;
        actor->base.field2C8 = zero;
        flags = &actor->base.pendingControlFlags;
        mask = ~1;
        *flags &= mask;
        actor->base.pendingControl = zero;
    }
    if (actor->base.stepDone) {
        u32 behavior = 15;
        u32 zero;
        u8 *state = &actor->base.state2A8;
        zero = 0;
        *state = zero;
        actor->base.phase2A9 = zero;
        actor->base.behavior2AA = behavior;
        actor->base.counter2AC = zero;
        actor->base.field2B1 = 1;
    } else {
        Callback_InvokeActor(actor, actor->base.contactQuery.opaque);
    }
}

void SceneActor_WaitForCategoryAttachment(void);

void SceneActor_StartFlaggedAnimationPhase(struct Dog *actor)
{
    if (Chase_TakeArrived(&actor->chase)) {
        struct Dog *context;
        actor->chase.base.tracking.target = NULL;
        context = actor->chase.base.context;
        {
            u16 mask = 1;
            struct ChaseFlags40 *flags = (void *)&context->collider.reactionFlags;
            flags->bits |= mask;
        }
        Chase_SetCtl(&actor->chase, SceneActor_WaitForCategoryAttachment, 0x25);
    }
    if (actor->chase.base.stepDone) {
        Chase_SetBehavior(&actor->chase, 2);
    } else {
        Callback_InvokeActor(actor, actor->chase.base.contactQuery.opaque);
    }
}

void SceneActor_LeapTowardTarget(void);

void Dog_UpdateLeapControl(struct ChaseActor *a)
{
    if (Chase_TakeArrived(a))
        Chase_SetCtl(a, SceneActor_LeapTowardTarget, 0x23);
    if (a->base.stepDone)
        Chase_SetBehavior(a, 0x10);
    else {
        Callback_InvokeActor(a, a->base.contactQuery.opaque);
        {
            u32 bit = 0x2000;
            u32 *flags = &a->base.behaviorFlags;
            *flags |= bit;
        }
    }
}

// confidence=low: resolves the shadowless approach against the saved context position.

void SceneActor_EnterShadowlessPhase(void);

u8 SceneActor_IsCollisionTargetUnavailable(void *);

struct DogApproachContext {
    u8 unknown0[0x658];
    u16 terrainCell;
    u8 unknown65A[2];
    u16 flags;
};

static inline u8 Dog_TestApproachFlag(struct DogApproachContext *context, u32 mask, u16 **flags)
{
    u16 *field = &context->flags;
    u32 value = *field & mask;
    *flags = field;
    if (value)
        return 1;
    return 0;
}

void Dog_UpdateShadowlessApproach(struct ChaseActor *actor)
{
    u8 *group = (u8 *)&actor->group.common;
    struct DogApproachContext *context = actor->base.context;
    if (Chase_TakeArrived(actor))
        Chase_SetCtl(actor, SceneActor_EnterShadowlessPhase, 0x24);
    if (SceneActorBehavior_RequestDescriptorTurn(actor, *(void **)(group + 0x1FC), 32, 32, 0)) {
        actor->base.secondaryMotion.x = 0;
        actor->base.secondaryMotion.z = 0;
    }
    if (actor->base.stepDone) {
        u16 *flags;
        u32 active = Dog_TestApproachFlag(context, 1, &flags);
        if (active) {
            if (context->terrainCell == actor->base.terrainCell) {
                u32 bit = 1;

                ((struct ChaseFlags40 *)flags)->bits &= ~bit;
                actor->base.state2A8 = 0;
                actor->base.phase2A9 = 0;
                actor->base.behavior2AA = 0;
                actor->base.counter2AC = 0;
                actor->base.field2B1 = bit;
            } else
                Chase_SetBehavior(actor, 15);
        } else if (SceneActor_IsCollisionTargetUnavailable(actor)) {
            Chase_SetState(actor, 3, 0, 0);
            Chase_SetCtl(actor, SceneActorControl_OnIdle, 16);
            *(u16 *)(group + 0x44) = 0;
        } else
            Chase_SetBehavior(actor, 15);
    } else
        Callback_InvokeActor(actor, actor->base.contactQuery.opaque);
}
