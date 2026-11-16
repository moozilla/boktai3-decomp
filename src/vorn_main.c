#include "global.h"
#include "scene_actor_chase.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_reaction.h"
#include "scene_actor_delayed_response.h"
#include "scene_actor_control_phase.h"
#include "debug.h"
#include "scene_actor_reset.h"
#include "vorn.h"
#include "directional_script_args.h"
#include "engine.h"
#include "scene_actor_behavior.h"
#include "scene_actor_registry.h"
#include "script_utility.h"
#include "player.h"
#include "ram.h"
#include "route_sprite_actor.h"
#include "scene_actor_route.h"
#include "scene_actor_terrain.h"
#include "scene_actor_collision.h"
#include "engine_globals.h"

extern const u8 gUnk_0824E218[];

void SceneActorReaction_PlayAnimationBUntilComplete(void);

void SceneActorBehavior_CallActionScript(void *);

void Vorn_SelectRecoveryAction(u8 *, const u8 *, u32);

void Vorn_UpdateScriptedAction(struct ChaseActor *actor)
{
    if (Chase_TakeArrived(actor)) {
        Chase_SetCtl(actor, SceneActorReaction_PlayAnimationBUntilComplete, 10);
        actor->base.body.direction = actor->base.tracking.heading;
        SceneActorBehavior_CallActionScript(actor);
    }
    if (actor->base.stepDone) {
        u8 *group = (u8 *)actor;
        {
            u16 *h = (u16 *)(group + 0x414);
            u32 flag = 4;
            flag |= *h;
            *h = flag;
        }
        {
            u32 flag = 0x80;
            u16 *flags = &actor->base.motionFlags;
            flag |= *flags;
            *flags = flag;
        }
        {
            u16 value = *(u16 *)(group + 0x416);
            *(u16 *)(group + 0x418) = value;
        }
        Vorn_SelectRecoveryAction(actor, gUnk_0824E218, 0x27);
    } else {
        actor->base.counter2AC++;
    }
}

static inline u32 Vorn_RouteFlags(struct ChaseActor *actor, u32 mask)
{
    return actor->base.behaviorFlags & mask;
}

/* Queue pursuit, a home route, or the short waiting response after the delay. */

void Vorn_QueueDelayedResponse(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    u32 zero;
    SceneActorControl_TakeStateEntry(actor);
    if ((u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque))
        return;
    zero = (u8)sub_0812F3CC(actor);
    if (zero)
        return;
    if ((s32)actor->base.counter2AC <= 60)
        goto waiting;
    SceneActorAttachment_ReplaceResponseSlot((u8 *)actor, 0, 2, actor->base.category);
    SceneActorChase_SetState(actor, 3, 1, zero);
    SceneActorChase_SetControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
    if (SceneActorChase_MotionFlags(actor, 0x80)) {
        SceneActorDelay_Prepare((void *)&group->saved, group->common.gate.control, 12, 30);
        SceneActorChase_Arrive(actor);
    } else if (Vorn_RouteFlags(actor, 0x10)) {
        SceneActorDelay_Prepare((void *)&group->saved, actor->base.onAlternateEntry, 7, 30);
        SceneActorChase_SnapToRoute(group->destination, &group->homeRoute);
        if (!SceneActorChase_TryRoute(actor, group->route, &actor->base.body.position,
                                      group->destination)) {
            SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0);
            SceneActorChase_SetState(actor, 3, 0, 0);
        } else
            group->destination->pad = 1;
    } else {
        SceneActorDelay_Prepare((void *)&group->saved, actor->base.onAlternateEntry, 2, 30);
        actor->base.field2C2 = 64;
    }
    return;
waiting:
    actor->base.counter2AC++;
}

extern const u32 gUnk_08605F94[];

void Vorn_InitSpecialActionTable(u8 *p)
{
    *(const u32 **)(p + 0x288) = gUnk_08605F94;
}

// confidence=low: repeats delayed animations before dispatching a context script.

extern const u8 gUnk_0824E224[];

struct Vorn;

void Vorn_BeginResponsePursuit(struct Vorn *, u32, const void *, u32);

void SceneActor_UpdateDelayedAnimation(void);

static inline u32 RetryDuration(void)
{
    u32 index = (gRandomIndex + 1) & 1023;
    u32 value;
    gRandomIndex = index;
    index *= 2;
    value = *(const u16 *)(index + RANDOM_TABLE_ADDR);
    value &= 31;
    return value + 10;
}

void Vorn_UpdateAnimationRetries(struct ChaseActor *actor)
{
    u8 *context = actor->base.context;
    if (Chase_TakeArrived(actor)) {
        Chase_SetCtl(actor, SceneActor_UpdateDelayedAnimation, 0x24);
        context[0x696] = 0;
        context[0x697] = RetryDuration();
        *(u16 *)(context + 0x694) = 0;
    }
    Callback_InvokeActor(actor, actor->base.contactQuery.opaque);
    if (actor->base.stepDone) {
        u8 *retries = context + 0x696;
        if (*retries) {
            Chase_SetCtl(actor, SceneActor_UpdateDelayedAnimation, 0x24);
            --*retries;
            context[0x697] = RetryDuration();
            ++*(u16 *)(context + 0x694);
        } else {
            u16 *flags = (void *)((u8 *)actor->base.context + 0x6D4);
            s32 mask = ~3;
            *flags &= mask;
            Vorn_BeginResponsePursuit(actor, 1, gUnk_0824E224, 0x41);
        }
    } else
        actor->base.counter2AC++;
}

// confidence=low: follows a navigation descriptor, switching between acceleration and turning.

u32 NavMesh_ConsumeProbeEvent(u16 *);

u8 SceneActorBehavior_AccelerateTowardDescriptor(void *, void *, u32, u32, s32);

u8 SceneActorBehavior_RequestDescriptorTurn(void *, void *, u32, u32, s32);

static inline u32 TestFlags(struct ChaseActor *a, u32 mask)
{
    return a->base.motionFlags & mask;
}

void Vorn_UpdateDescriptorPursuit(struct ChaseActor *a)
{
    u8 *group = (u8 *)&a->group.common;
    u8 *context = a->base.context;
    s32 motion;
    s32 event;
    if (a->base.field2B1) {
        a->base.field2B1 = 0;
    }
    Callback_InvokeActor(a, a->base.contactQuery.opaque);
    motion = Callback_InvokeActor(a, *(void **)((u8 *)a + 0x260));
    {
        u16 *rate;
        a->base.field2C2 = 32;
        {
            struct ChaseActor *held = a;
            a = held;
        }
        if (TestFlags(a, 0xA02F)) {
            u16 *p = &a->base.field2C2;
            *p = 64;
            rate = p;
        } else {
            u16 *p = &a->base.field2C2;
            u16 value = *p;
            rate = p;
            if (!value) {
                *rate = 64;
            }
        }
        if (!a->base.drawableKind)
            Anim_SetDurationScale((u8 *)a->base.drawable + 0x48, *rate);
        else
            Anim_SetFrameDurationScale((u8 *)a->base.drawable + 0x20, *rate);
    }
    event = NavMesh_ConsumeProbeEvent(*(void **)(group + 0x1FC));
    if (!event) {
        if (SceneActorBehavior_AccelerateTowardDescriptor(a, *(void **)(group + 0x1FC), 32, 32,
                                                          motion)) {
            {
                u32 state = 3;
                a->base.state2A8 = state;
                a->base.phase2A9 = event;
                a->base.behavior2AA = event;
                a->base.counter2AC = event;
                a->base.field2B1 = 1;
            }
            {
                u32 m = 2;
                u16 *flags;
                flags = (void *)(context + 0x6D4);
                *flags = (*(&m)) | (*flags);
            }
            return;
        }
    } else {
        if (SceneActorBehavior_RequestDescriptorTurn(a, *(void **)(group + 0x1FC), 32, 32, motion))
            Chase_SetBehavior(a, 16);
    }
    a->base.counter2AC++;
}

// confidence=low: completed action chooses a pursuit callback from context flag one.

void Vorn_UpdatePursuitWait(void);

void Vorn_UpdatePursuitDistance(void);

static inline void SelectPursuit(struct ChaseActor *a, void *cb, u32 mode)
{
    void *callback = cb;
    u32 zero = 0;
    u32 kind = mode;
    a->base.pendingControl = (u32)callback;
    a->base.pendingControlData = zero;
    a->base.pendingControlRelease = zero;
    a->base.pendingControlKind = kind;
    a->base.pendingControlFlags = 1;
}

void Vorn_SelectPursuitAfterAction(struct ChaseActor *a)
{
    u8 *context = a->base.context;
    if (Chase_TakeArrived(a))
        Chase_SetCtl(a, a->base.onEntry, 2);
    Callback_InvokeActor(a, a->base.contactQuery.opaque);
    if (a->base.stepDone) {
        Chase_SetBehavior(a, 15);
        if (Chase_Test(*(u16 *)(context + 0x6D4), 1))
            SelectPursuit(a, Vorn_UpdatePursuitWait, 0x23);
        else
            SelectPursuit(a, Vorn_UpdatePursuitDistance, 0x25);
    } else
        a->base.counter2AC++;
}

void Vorn_ClearRouteResumeFlag(u8 *p);

void SceneActorMovement_ResolveRoutePosition(void *);

u8 sub_0812F3CC(struct ChaseActor *);

static inline u32 *Route_SetCtl(struct ChaseActor *actor, void *cb, u32 k)
{
    void *callback = cb;
    u32 kind = k;
    u32 *field;
    actor->base.field2B2 = 1;
    actor->base.stepDone = 0;
    actor->base.controlKind = kind;
    actor->base.target = callback;
    actor->base.field2C8 = 0;
    field = &actor->base.pendingControlFlags;
    {
        u32 mask = ~1;
        *field &= mask;
    }
    actor->base.pendingControl = 0;
    return field;
}

static inline void Route_RestoreMotion(struct ChaseActor *actor, u32 result, u32 *flags)
{
    /* Restore the saved target as its stored 32-bit control value. */
    actor->base.pendingControl = *(u32 *)&actor->base.onAlternateEntry;
    actor->base.pendingControlData = result;
    actor->base.pendingControlRelease = result;
    actor->base.pendingControlKind = 1;
    *flags = 1;
}

// confidence=low: restores route control and movement after an action completes.


void Vorn_RestoreRouteMovement(struct ChaseActor *a)
{
    u8 *context = a->base.context;
    u8 *group = context + 0x3D4;
    u32 *action;
    u32 *flags;
    if (Chase_TakeArrived(a)) {
        DEBUG_PRINT();
    }
    if ((u8)Callback_InvokeActor(a, a->base.contactQuery.opaque))
        return;
    if (sub_0812F3CC(a))
        return;
    if (a->base.stepDone) {
        struct SceneActorRegistryControl *control;
        u32 actionMask;
        Vorn_ClearRouteResumeFlag(context);
        SceneActorMovement_ResolveRoutePosition(a);
        control = (void *)(group + 0x4C);
        actionMask = 0x200000;
        action = &a->base.behaviorFlags;
        if (Chase_Bits32(action, actionMask)) {
            s32 mask;
            u32 *state = &a->base.statusFlags;
            mask = ~0x1000000;
            *state = mask & *state;
            {
                struct ChaseFlags40 *motion = (void *)&a->base.motionFlags;
                s32 motionMask = ~0x200;
                motion->bits &= motionMask;
            }
            control->mode = 0;
            control->counter = 0;
            control->timer = 0;
        }
        {
            s32 mask = ~0x90;
            *action = mask & *action;
        }
        a->base.field2C2 = 64;
        Memory_Clear(*(void **)(group + 0x200), 8);
        Chase_SetBehavior(a, 2);
        flags = Route_SetCtl(a, *(void **)((u8 *)a + 0x22C), 0);
        Route_RestoreMotion(a, 0, flags);
    } else
        a->base.counter2AC++;
}

// confidence=low: accelerates along the route and chooses side-step or turn transitions.

u8 SceneActor_IsFacingNearbyTarget(struct Vorn *);

s32 Callback_InvokeActor(void *, void *);

void SceneActorPhase_SelectLinkedAnimation(void);

void Anim_SetDurationScale(void *, u32);

void Anim_SetFrameDurationScale(void *, u32);

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

static inline void SetState(struct ChaseActor *actor, u32 behavior)
{
    actor->base.state2A8 = 0;
    actor->base.phase2A9 = 0;
    actor->base.behavior2AA = behavior;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}

static inline void SetMotion(struct ChaseActor *actor, void *cb, u32 kind)
{
    actor->base.pendingControl = (u32)cb;
    actor->base.pendingControlData = 0;
    actor->base.pendingControlRelease = 0;
    actor->base.pendingControlKind = kind;
    actor->base.pendingControlFlags = 1;
}

void SceneActorReaction_BeginSideStep(void);

void Vorn_UpdateRouteApproach(struct ChaseActor *actor)
{
    u8 *group = (u8 *)&actor->group.common;
    s32 speed;
    u32 event;
    u16 *scale;
    if (actor->base.field2B1)
        actor->base.field2B1 = 0;
    if ((u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque))
        return;
    if (sub_0812F3CC(actor))
        return;
    SceneActorAnimation_SelectDirectionalEntry(actor, 1);
    speed = Callback_InvokeActor(actor, FIELD(actor, 0x260, void *));
    {
        u32 mask = 0xa02f;
        if (actor->base.motionFlags & mask) {
            u16 *p = &actor->base.field2C2;
            *p = 64;
            scale = p;
        } else {
            u16 *p = &actor->base.field2C2;
            u16 v = *p;
            scale = p;
            if (v == 0)
                *scale = 64;
        }
    }
    if (actor->base.drawableKind == 0)
        Anim_SetDurationScale((u8 *)actor->base.drawable + 0x48, *scale);
    else
        Anim_SetFrameDurationScale((u8 *)actor->base.drawable + 0x20, *scale);
    event = NavMesh_ConsumeProbeEvent(FIELD(group, 0x1fc, void *));
    if (event == 0) {
        if (SceneActorBehavior_AccelerateTowardDescriptor(actor, FIELD(group, 0x1fc, void *), 32,
                                                          32, speed)) {
            SetState(actor, 18);
            SetControl(actor, actor->base.onStanding, 0);
            SetMotion(actor, SceneActorReaction_BeginSideStep, 38);
            actor->base.field2C2 = 64;
            *(u16 *)(group + 0x54) = 0;
            return;
        }
    } else if (SceneActorBehavior_RequestDescriptorTurn(actor, FIELD(group, 0x1fc, void *), 32, 32,
                                                        speed)) {
        SetState(actor, 20);
        SetControl(actor, actor->base.onStanding, 0);
    }
    actor->base.counter2AC++;
}

static inline u8 TakePrepared(u8 *field)
{
    if (*field) {
        *field = 0;
        return 1;
    }
    return 0;
}

static inline void SetControlWords(struct ChaseActor *actor, void *control, u32 kind)
{
    u8 *prepared = &actor->base.field2B2;
    u32 zero = 0;
    *prepared = 1;
    actor->base.stepDone = zero;
    actor->base.controlKind = kind;
    actor->base.target = control;
    actor->base.field2C8 = zero;
    {
        u32 *flags = &actor->base.pendingControlFlags;
        s32 mask = -2;
        *flags &= mask;
        actor->base.pendingControl = zero;
        actor->base.pendingControl =
            *(u32 *)&actor->base.onAlternateEntry; /* a u32 read keeps the store above */
        actor->base.pendingControlData = 0;
        actor->base.pendingControlRelease = 0;
        actor->base.pendingControlKind = 1;
        *flags = 1;
    }
}

// confidence=low: Ghoul_WaitForRouteResult's twin; on a route result, resets to behavior 0x13

// and returns control to savedTarget with word104 = entryTarget228.

void SceneActorControl_WaitForRouteResult(struct ChaseActor *actor)
{
    u8 busy;
    u8 zero;
    if (TakePrepared(&actor->base.field2B1)) {
        void *control = actor->base.onEntry;
        SetControl(actor, control, 2);
    }
    busy = Callback_InvokeActor(actor, actor->base.contactQuery.opaque);
    if (!busy) {
        zero = sub_0812F3CC(actor);
        if (!zero) {
            if (actor->base.stepDone) {
                u32 state = 0x13;
                actor->base.state2A8 = zero;
                actor->base.phase2A9 = zero;
                actor->base.behavior2AA = state;
                actor->base.counter2AC = zero;
                actor->base.field2B1 = 1;
                SetControlWords(actor, actor->base.onStanding, 0);
            } else
                actor->base.counter2AC++;
        }
    }
}

/* SceneActorControl_UpdateProbeApproach_080B04F4: SceneActorControl_UpdateProbeApproach (SceneActorControl_UpdateProbeApproach) with control SceneActorPhase_SelectLinkedAnimation and halfword flags at +0x6D4 (provisional name) */

static inline void SetState_UpdateProbeApproach_080B04F4(struct ChaseActor *actor)
{
    u32 behavior = 22;
    actor->base.state2A8 = 0;
    actor->base.phase2A9 = 0;
    actor->base.behavior2AA = behavior;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}

void SceneActorControl_UpdateProbeApproach_080B04F4(struct ChaseActor *actor)
{
    u8 *nav = (u8 *)actor + 0x3D4;
    u8 *control = (u8 *)SceneActorControl_GetState(actor);
    s32 speed;
    u32 event;
    u16 *scale;

    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        SetControl(actor, SceneActorPhase_SelectLinkedAnimation, 0x27);
        {
            u32 bit = 0x20;
            u16 *flags = (u16 *)(control + 0x6D4);
            bit |= *flags;
            *flags = bit;
        }
    }
    if (SceneActor_IsFacingNearbyTarget(actor)) {
        if ((u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque)) {
            return;
        }
        if (sub_0812F3CC(actor)) {
            return;
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
            SetState_UpdateProbeApproach_080B04F4(actor);
            SetControl(actor, actor->base.onStanding, 0);
            return;
        }
    } else if (SceneActorBehavior_AccelerateTowardDescriptor(actor, FIELD(nav, 0x1FC, void *), 0x20,
                                                             0x20, speed)) {
        ResetState(actor, 3, 1, 0);
        {
            u32 bit = 0x40;
            u16 *flags = (u16 *)(control + 0x6D4);
            bit |= *flags;
            *flags = bit;
        }
        return;
    }
    actor->base.counter2AC++;
}

/* Tentative phase-control interpretation; names have low confidence. */

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

void SceneActorPhase_UpdateConditionalMotion(struct ChaseActor *p)
{
    if (SceneActorControl_TakeFlag(&p->base.field2B1)) {
        void *control = p->base.onEntry;
        u32 kind = 2;
        SelectControl(p, control, kind);
    }
    if (SceneActor_IsFacingNearbyTarget(p)) {
        if ((u8)Callback_InvokeActor(p, p->base.contactQuery.opaque))
            return;
        if (sub_0812F3CC(p))
            return;
    }
    if (p->base.stepDone) {
        u32 phase = 0x15;
        p->base.state2A8 = 0;
        p->base.phase2A9 = 0;
        p->base.behavior2AA = phase;
        p->base.counter2AC = 0;
        p->base.field2B1 = 1;
        SelectControl(p, p->base.onStanding, 0);
    } else
        ++p->base.counter2AC;
}

void Vorn_UpdateApproachRange(struct ChaseActor *);

static inline u8 PhaseComplete(struct ChaseActor *p)
{
    return p->base.stepDone;
}

void SceneActorPhase_BeginTimedMotion(struct ChaseActor *p)
{
    if (SceneActorControl_TakeFlag(&p->base.field2B1)) {
        void *control = p->base.onEntry;
        u32 kind = 2;
        SelectControl(p, control, kind);
    }
    if (SceneActor_IsFacingNearbyTarget(p)) {
        if ((u8)Callback_InvokeActor(p, p->base.contactQuery.opaque))
            return;
        if (sub_0812F3CC(p))
            return;
    }
    if (PhaseComplete(p)) {
        u32 state = 3;
        u16 one = 1;
        u8 *field = &p->base.state2A8;
        u32 zero = 0;
        *field = state;
        p->base.phase2A9 = one;
        p->base.behavior2AA = zero;
        p->base.counter2AC = zero;
        p->base.field2B1 = one;
        {
            void *control = p->base.onStanding;
            u32 *flags;
            p->base.field2B2 = one;
            *(u8 *)((u32)p + 0x2B0) = zero;
            *(u8 *)((u32)p + 0x2B5) = zero;
            p->base.target = control;
            p->base.field2C8 = zero;
            flags = &p->base.pendingControlFlags;
            {
                s32 mask = -2;
                *flags &= mask;
            }
            {
                u16 duration;
                u32 *word104 = &p->base.pendingControl;
                u32 handler = (u32)Vorn_UpdateApproachRange;
                duration = 40;
                *word104 = handler;
                p->base.pendingControlData = zero;
                p->base.pendingControlRelease = zero;
                p->base.pendingControlKind = duration;
                *flags = one;
            }
        }
    } else
        ++p->base.counter2AC;
}

void Vorn_UpdateActionCompletion(u8 *p)
{
    if (p[0x2b1])
        p[0x2b1] = 0;
    if (p[0x2b0]) {
        struct {
            u8 pad[0xf];
            u8 flags;
        } *sprite = (void *)(p + 0xdc);
        u8 m = 8;
        sprite->flags |= m;
        Vorn_SelectRecoveryAction(p, gUnk_0824E224, 0x19b);
    } else
        (*(u32 *)(p + 0x2ac))++;
}

extern const u32 gUnk_08605F9C[];

void Vorn_InitAttackActionTable(u8 *p)
{
    *(const u32 **)(p + 0x284) = gUnk_08605F9C;
}

void SceneActor_UpdateTargetedAttack(struct ChaseActor *);

u32 Vorn_SelectRetreatRoute(struct ChaseActor *actor, u32 reset);

void SceneActorPhase_UpdateCooldown(struct ChaseActor *p)
{
    struct SceneActorAnimationLink *link = p->base.context;
    if (SceneActorControl_TakeFlag(&p->base.field2B1)) {
        {
            s32 mask = 0x100;
            struct ChaseFlags40 *flags = (void *)&p->base.motionFlags;
            flags->bits |= mask;
        }
        Chase_SetCtl(p, SceneActor_UpdateTargetedAttack, 0x22);
        link->phaseDuration = 120;
        {
            u32 mask = 4;
            p->base.statusFlags |= mask;
        }
    }
    if (p->base.stepDone) {
        u16 *flags = &p->base.motionFlags;
        u32 mask = ~0x100;
        *flags = mask & *flags;
        SceneActorAnimation_SelectDirectionalEntry((void *)p, 0);
        Vorn_SelectRetreatRoute(p, 1);
    } else
        ++p->base.counter2AC;
}

extern const u32 gUnk_08606000[];

void Vorn_InitReactionActionTable(u8 *p)
{
    *(const u32 **)(p + 0x28c) = gUnk_08606000;
}

void Vorn_UpdateScriptedAnimation(struct ChaseActor *);

static inline u8 IsUnavailable(void *input)
{
    struct ChaseActor *p = input;
    if (p) {
        u32 mask = 0x40;
        if (p->base.motionFlags & mask)
            return 1;
        mask = 0x20000;
        if (p->base.statusFlags & mask)
            return 1;
        if (p->base.health <= 0)
            return 1;
    }
    return 0;
}

static inline u8 HasFlags(u16 *flags, u32 mask)
{
    if (*flags & mask)
        return 1;
    return 0;
}

static inline u32 MaskedFlags(struct ChaseActor *p, u32 mask)
{
    return p->base.motionFlags & mask;
}

static inline u8 TestContext(u8 *base, u32 mask, u16 **out)
{
    u16 *field = (u16 *)(base + 0x6D4);
    u32 value = *field & mask;
    *out = field;
    if (value)
        return 1;
    return 0;
}

void SceneActorPhase_TryBeginSpecialMotion(struct ChaseActor *p)
{
    u8 *base = (u8 *)p;

    if (!IsUnavailable(base) && *(s16 *)(base + 0x682) == 1 && p->base.controlKind != 0x29 &&
        !MaskedFlags(p, 0xE3BF)) {
        u16 *flags;
        if (!TestContext(base, 0xC, &flags) && HasFlags(flags, 2)) {
            u32 state = 0x18;
            u8 *field = &p->base.state2A8;
            u32 zero = 0;
            *field = zero;
            p->base.phase2A9 = zero;
            p->base.behavior2AA = state;
            p->base.counter2AC = zero;
            {
                u8 *ready = &p->base.field2B1;
                u32 one = 1;
                void *control;
                u32 *motionFlags;
                *ready = one;
                control = p->base.onStanding;
                p->base.field2B2 = one;
                p->base.stepDone = zero;
                p->base.controlKind = zero;
                p->base.target = control;
                p->base.field2C8 = zero;
                motionFlags = &p->base.pendingControlFlags;
                {
                    s32 mask = -2;
                    *motionFlags &= mask;
                }
                {
                    u32 duration;
                    u32 *slot = &p->base.pendingControl;
                    u32 callback = (u32)Vorn_UpdateScriptedAnimation;
                    duration = 0x29;
                    *slot = callback;
                    p->base.pendingControlData = zero;
                    p->base.pendingControlRelease = zero;
                    p->base.pendingControlKind = duration;
                    *motionFlags = one;
                }
            }
        }
    }
}

s32 Script_GetKeywordValue(s32, s32);

void Vorn_InitScriptCallback(struct Vorn *p)
{
    p->response = 0xffff;
    p->field67E = -1;
    p->field680 = -1;
    p->field684 = Script_GetKeywordValue(100, 0);
}

/* Descriptive aliases and object roles are tentative (confidence: low). */

struct SceneActorPositionScript {
    u8 unknown0[0x5c];
    s16 x, y, z;
    u8 unknown62[0x67c - 0x62];
    u16 field67C;
    s16 argument, field680;
    u16 field682;
    u32 script;
    u8 unknown688[0x6d4 - 0x688];
    u16 flags;
};

void Sound_PlaySong(u32);

void SceneActor_RunPositionScript(struct SceneActorPositionScript *p)
{
    s32 values[4];
    struct DirectionalScriptArgs args;
    if (p->script) {
        values[0] = p->x;
        values[1] = p->y;
        values[2] = p->z;
        values[3] = p->argument;
        args.count = 4;
        args.values = values;
        Script_Call(p->script, &args);
        p->field67C = 0xffff;
        p->argument = -1;
        p->field680 = -1;
        p->field682 = 0;
        {
            u16 *flags = &p->flags;
            s32 mask = -2;
            *flags = mask & *flags;
        }
        Sound_PlaySong(0xd6);
    }
}

struct SceneActorScriptContext {
    u8 unknown0[0x67C];
    s16 mode, firstParameter, secondParameter;
};

void SceneActor_CallContextScriptB(struct RoomActor *actor)
{
    s32 values[12];
    struct ScriptUtilityArguments arguments;
    struct SceneActorScriptContext *context;
    {
        u32 mask = 0x80000;
        u32 offset = 0x2D0;
        u32 *field = (void *)((u32)actor + offset);
        if (*field & mask)
            return;
    }
    context = ((struct ChaseActor *)actor)->base.context;
    if (!(u16)actor->script1F4)
        return;
    values[0] = actor->body.position.x;
    values[1] = actor->body.position.y;
    values[2] = actor->body.position.z;
    values[3] = actor->parameters1FC[0];
    values[4] = actor->parameters1FC[1];
    values[5] = actor->parameters1FC[2];
    if (context->mode < 0) {
        values[6] = -1;
        values[7] = -1;
    } else {
        values[6] = context->firstParameter;
        values[7] = 0;
    }
    values[8] = actor->category;
    values[9] = actor->paletteOffset;
    values[10] = actor->hitAttributes;
    values[11] = -1;
    arguments.count = 12;
    arguments.values = values;
    Script_Call(actor->script1F4, &arguments);
}

s32 Inventory_GetItemPackedState(s32 i);

s32 Inventory_GetItemId(u32);

u8 SceneInventory_FindSlotByValue(s16 *p0, s16 *p1, s16 *p2, s32 p3)
{
    s16 i;
    for (i = 0; i <= 15; i++) {
        s32 v = Inventory_GetItemId(i);
        *p1 = v;
        if ((s16)v < 0)
            continue;
        if (*p1 != p3)
            continue;
        *p0 = i;
        {
            s32 w = Inventory_GetItemPackedState(*p0);
            *p2 = w;
        }
        return 1;
    }
    return 0;
}

/* Descriptive aliases and object roles are tentative (confidence: low). */

struct PackedItemPosition {
    s32 x : 16, y : 16, z : 16, pad : 16;
};

struct ItemLaunchPlayer {
    u8 unknown0[0x30];
    struct PackedItemPosition position;
};

struct SceneActorItemLaunch {
    u8 unknown0[0x5c];
    struct PackedItemPosition position;
    u8 unknown64[0x67c - 0x64];
    s16 slot, kind, parameter, field682;
    u8 unknown684[0x6d4 - 0x684];
    u16 flags;
};

u8 SceneInventory_FindSlotByValue(s16 *, s16 *, s16 *, s32);

s32 FloatingItemEffect_Spawn(s32, s32, s32, s32, void *, void *, s32, void *);

void Inventory_ClearItemSlot(u32);

static inline u8 busy(struct SceneActorItemLaunch *p)
{
    u32 mask = 1;
    if (p->flags & mask)
        return 1;
    return 0;
}

static inline u8 occupied(struct SceneActorItemLaunch *p)
{
    if (busy(p) || p->slot >= 0)
        return 1;
    return 0;
}

s32 SceneActor_TryLaunchItem13(struct SceneActorItemLaunch *p)
{
    struct SceneActorItemLaunch *q = p;
    struct PackedItemPosition pos = ((struct ItemLaunchPlayer **)gPlayers)[0]->position;
    pos.y += 200;
    if (occupied(p))
        return -1;
    if (!SceneInventory_FindSlotByValue(&p->slot, &p->kind, &p->parameter, 13))
        return -1;
    if (FloatingItemEffect_Spawn(0, p->kind, 3, 0, &pos, &q->position, 24, p) < 0) {
        p->slot = 0xffff;
        return -1;
    }
    Inventory_ClearItemSlot(p->slot);
    switch (p->kind) {
        case 2:
        case 5:
        case 14:
            return 0;
    }
    Sound_PlaySong(0x1ce);
    return 1;
}

/* Descriptive aliases and object roles are tentative (confidence: low). */

u8 InventorySlot_PickRandom(s16 *, s16 *, s16 *);

s32 SceneActor_TryLaunchSelectedItem(struct SceneActorItemLaunch *p)
{
    struct SceneActorItemLaunch *q = p;
    struct PackedItemPosition pos = ((struct ItemLaunchPlayer **)gPlayers)[0]->position;
    pos.y += 200;
    if (occupied(p))
        return -1;
    if (!InventorySlot_PickRandom(&p->slot, &p->kind, &p->parameter))
        return -1;
    if (FloatingItemEffect_Spawn(0, p->kind, 3, 0, &pos, &q->position, 24, p) < 0) {
        p->slot = 0xffff;
        return -1;
    }
    Inventory_ClearItemSlot(p->slot);
    switch (p->kind) {
        case 2:
        case 5:
        case 14:
            return 0;
    }
    Sound_PlaySong(0x1ce);
    return 1;
}

#define gSineTable gUnk_086149C4

/* Descriptive aliases and object roles are tentative (confidence: low). */

struct SceneActorForwardCollider {
    u8 unknown0[0x628];
    struct ContactCollider collider;
};

static inline u8 unavailable(struct ChaseActor *p)
{
    if (p) {
        u32 mask = 0x40;
        if (p->base.motionFlags & mask)
            return 1;
        {
            u32 mask = 0x20000;
            if (p->base.statusFlags & mask)
                return 1;
        }
        if (p->base.health <= 0)
            return 1;
    }
    return 0;
}

static inline s32 scaled(s32 x)
{
    if (x >= 0)
        return x >> 12;
    return -(-x >> 12);
}

void SceneActor_UpdateForwardCollider(struct ChaseActor *p)
{
    struct SceneActorForwardCollider *context = p->base.context;
    union SceneActorRegistryPackedPosition pos;
    union SceneActorRegistryPackedPosition bounds;
    if (!unavailable(p) && p->base.controlKind == 0x22) {
        pos.position = p->base.body.position;
        pos.packed.x += scaled(180 * gUnk_086149C4[(p->base.body.direction + 64) & 255]);
        pos.packed.z += scaled(180 * gUnk_086149C4[p->base.body.direction]);
        bounds.packed.x = 64;
        bounds.packed.z = 64;
        bounds.packed.y = 100;
        {
            struct ContactCollider *collider = &context->collider;
            collider->bounds = *(struct ContactPosition *)&bounds;
            Collider_SetPosition(collider, &pos, 0);
        }
    }
}

void Vorn_InitActionCooldown(u8 *p)
{
    *(u16 *)(p + 0x692) = 0x78;
    *(u16 *)(p + 0x690) = 0x78;
}

struct SceneActorRandomReaction {
    u8 unknown0[0x690];
    u16 timer, period;
    u8 unknown694[0x6d4 - 0x694];
    u16 flags;
};

static inline u8 active(struct SceneActorRandomReaction *p)
{
    u32 mask = 1;
    if (p->flags & mask)
        return 1;
    return 0;
}

static inline u32 blocked(struct ChaseActor *p)
{
    u32 mask = 0xe33f;
    return p->base.motionFlags & mask;
}

void SceneActor_TickRandomReaction(struct ChaseActor *p)
{
    struct SceneActorRandomReaction *context = p->base.context;
    if (!unavailable(p) && !blocked(p) && p->base.controlKind == 0x23 && active(context)) {
        if (context->timer) {
            context->timer--;
        } else {
            context->timer = context->period;
            gRandomIndex = (gRandomIndex + 1) & 0x3ff;
            {
                s32 random = *(u8 *)&gRandomTable[gRandomIndex];
                if (random > 0xbd) {
                    u32 behavior = 0x11;
                    p->base.state2A8 = 0;
                    p->base.phase2A9 = 0;
                    p->base.behavior2AA = behavior;
                    p->base.counter2AC = 0;
                    p->base.field2B1 = 1;
                }
            }
        }
    }
}

struct P {
    u8 f[0x6ba];
    u16 a, b, c;
    u8 g[0x6b8 + 0x100 - 0x6c0];
};

struct Q {
    u8 f[0x6b8];
    u8 v;
};

void Vorn_InitTimedAction(struct P *p)
{
    p->c = 0x78;
    p->b = 0x78;
    p->a = 0xf0;
    *((u8 *)p + 0x6b8) = Script_GetKeywordValue(0x68, -1);
}

void Vorn_ClearRouteResumeFlag(u8 *p)
{
    u16 *q = (u16 *)(p + 0x6d4);
    u32 m = ~8;
    *q = m & *q;
}

struct SceneActorRouteSelection {
    u8 unknown0[0x2be];
    u16 groundHeight;
    u8 unknown2C0[0x6b8 - 0x2c0];
    s8 routeId;
};

void Route_Start(struct SceneActorRegistryRoute *, u32, u32, u32);

u8 *Path_GetPointData(void *);

s32 Mod(s32, s32);

struct RouteSelectionPosition {
    u32 x : 16, y : 16, z : 16;
};

static inline s32 randomValue(void)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u16 *)(offset + RANDOM_TABLE_ADDR);
}

union RouteSelectionCandidate {
    struct RouteSpriteRoute route;
    struct SceneActorRouteCursor actor;
};

void SceneActor_SelectDistantRoutePoint(struct SceneActorRouteSelection *p,
                                        struct ContactPosition *out, struct ContactPosition *origin,
                                        s32 limit)
{
    struct SceneActorRouteSelection *actor = p;
    struct RouteSpriteRoute route;
    union RouteSelectionCandidate candidate;
    struct RouteSelectionPosition delta;
    s32 best;
    s32 selected;
    s32 i;

    Route_Start(&route, p->routeId, 0, 0);
    best = 0;
    selected = -1;
    i = 0;
    for (; i < route.resource->pointCount; i++) {
        s32 distance;
        candidate.route.current =
            (struct RouteSpriteRoutePoint *)Path_GetPointData(route.resource) + i;
        *(struct ContactPosition *)&delta = *origin;
        {
            u16 x = candidate.actor.point->x - ((struct ContactPosition *)&delta)->x;
            ((u32 *)&delta)[0] = (((u32 *)&delta)[0] & 0xffff0000) | x;
        }
        {
            u16 z = candidate.actor.point->z - ((struct ContactPosition *)&delta)->z;
            ((u32 *)&delta)[1] = (((u32 *)&delta)[1] & 0xffff0000) | z;
        }
        distance = ((struct ContactPosition *)&delta)->x * ((struct ContactPosition *)&delta)->x +
                   ((struct ContactPosition *)&delta)->z * ((struct ContactPosition *)&delta)->z;
        if (distance <= limit && best < distance) {
            selected = i;
            best = distance;
        }
    }
    if (!best) {
        selected = Mod(randomValue(), route.resource->pointCount);
    }
    route.index = selected;
    route.ended = 0;
    route.current = (struct RouteSpriteRoutePoint *)Path_GetPointData(route.resource);
    route.current += route.index;
    {
        struct ContactPosition *dest = out;
        dest->x = route.current->x;
        dest->y = actor->groundHeight;
        dest->z = route.current->z;
    }
}

struct VornProbe {
    s32 x : 16;
    s32 y : 16;
    s16 z, pad;
};

static inline s32 Height(struct ContactPosition *p)
{
    s32 cell = SceneActorTerrain_GetCellIndex(p);
    u32 kind;
    s32 height;
    u8 *data = SceneActorTerrain_GetCell(cell);
    kind = *data >> 4;
    height = (*data & 15) << 8;
    if (kind == 1)
        goto slopeZ;
    if (kind == 2)
        goto slopeX;
    goto done;
slopeZ:
    height -= ((u8 *)p)[4];
    goto done;
slopeX:
    height -= ((u8 *)p)[0];
done:
    return height;
}

// confidence=low: biases turn weights toward a neighboring cell with higher terrain.

u8 Vorn_SelectUphillTurn(void *contextIn, void *positionIn)
{
    struct Vorn *context = (struct Vorn *)contextIn;
    struct ContactPosition *position = (struct ContactPosition *)positionIn;
    struct VornProbe probe;
    s32 height, heightX, heightZ;
    s32 result;
    height = Height(position);
    probe = *(struct VornProbe *)position;
    probe.x += 256;
    heightX = Height((void *)&probe);
    probe = *(struct VornProbe *)position;
    probe.z += 256;
    heightZ = Height((void *)&probe);
    if (heightX - height > 0) {
        context->turnX += 110;
        context->turnZ += 100;
        context->axis = 0;
        result = 1;
    } else if (heightZ - height <= 0) {
        result = 0;
    } else {
        context->turnX += 100;
        context->turnZ += 110;
        context->axis = 1;
        result = 1;
    }
    return result;
}

void SceneActor_SelectDistantRoutePoint(struct SceneActorRouteSelection *p,
                                        struct ContactPosition *out, struct ContactPosition *origin,
                                        s32 limit);

u8 Vorn_SelectUphillTurn(void *, void *);

static inline u32 MotionFlags(struct ChaseActor *a, u32 mask)
{
    return a->base.motionFlags & mask;
}

static inline u32 StatusFlags(struct ChaseActor *a, u32 mask)
{
    return a->base.statusFlags & mask;
}

static inline u8 ContextFlags(u8 *p, u32 mask, u16 **out)
{
    u16 *field = (void *)(p + 0x6D4);
    u32 value = *field & mask;
    *out = field;
    if (value)
        return 1;
    return 0;
}

static inline u8 Unavailable(struct ChaseActor *a)
{
    if (a && (MotionFlags(a, 64) || StatusFlags(a, 0x20000) || a->base.health <= 0))
        return 1;
    return 0;
}

// confidence=low: periodically probes uphill destinations and queues a delayed route action.

static inline void Chase_SetState_1(struct ChaseActor *actor, u32 s, u32 p)
{
    actor->base.state2A8 = s;
    actor->base.phase2A9 = p;
    actor->base.behavior2AA = 0;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}

static inline u8 Chase_Test_(const u16 *value, u32 mask)
{
    if (*value & mask)
        return 1;
    return 0;
}

inline static u32 Chase_Bits32_1(struct ChaseActor *actor_, u32 mask)
{
    u32 *field = &actor_->base.behaviorFlags;
    return (*field) & mask;
}

static inline u32 Vorn_GetRandomBits(u32 mask)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return ((const u16 *)0x0203B400)[index] & mask;
}

void Vorn_TryUphillRoute(struct ChaseActor *actor)
{
    u8 *context = actor->base.context;
    u8 *group = context + 0x3D4;
    u16 *flags;
    struct ContactPosition *point;
    if (Unavailable(actor))
        return;
    if (MotionFlags(actor, 0xE3BF))
        return;
    if (Chase_Bits32_1(actor, 16))
        return;
    if (actor->base.controlKind != 1)
        return;
    if (ContextFlags(context, 0x1D, &flags))
        return;
    if (*((u16 *)(context + 0x6BC))) {
        --(*((u16 *)(context + 0x6BC)));
        return;
    }
    *((u16 *)(context + 0x6BC)) = *((u16 *)(context + 0x6BE));
    if (((s32)Vorn_GetRandomBits(255)) <= 189)
        return;
    if (Chase_Test_(flags, 0x1D))
        return;
    point = (void *)(context + 0x6B0);
    SceneActor_SelectDistantRoutePoint(context, point, &actor->base.body.position, 0x400000);
    if (!Vorn_SelectUphillTurn(context, point))
        return;
    if (!Chase_TryRoute(actor, *((void **)(context + 0x5D0)), &actor->base.body.position, point)) {
        Callback_InvokeActor(actor, *((void **)(context + 0x5F8)));
        return;
    }
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    Chase_SetState_1(actor, 3, 1);
    Chase_SetCtl(actor, SceneActorReaction_RestoreAfterDelay, 14);
    {
        u32 duration = (((u16)Vorn_GetRandomBits(0xFFFF)) & 15) + 30;
        struct SceneActorReactionCommand *command = (void *)(group + 0x8C);
        void *payload = actor->base.onAlternateEntry;
        u32 kind = 19;
        command->duration = duration;
        command->payload = payload;
        command->first = 0;
        command->second = 0;
        command->kind = kind;
    }
    {
        u16 mask = 8;
        *flags = mask | (*flags);
    }
}

void SceneActorControl_OnIdle(u8 *);

struct S {
    u8 pad[0x9C];
    s16 z;
    u8 pad2[0xDC - 0x9E];
    u8 extra[0x10];
    u8 pad3[0x104 - 0xEC];
    u32 at104;
    u8 pad4[0x114 - 0x108];
    u32 flags114;
    u8 pad5[0x21C - 0x118];
    void (*callback)(void);
    u8 pad6[0x2A8 - 0x220];
    u8 a, b, c, pad7;
    u32 count;
    u8 done, queued, taken;
    u8 pad8[2];
    u8 code;
    u8 pad9[0x2C8 - 0x2B6];
    u16 timer;
    u8 pad10[0x2D0 - 0x2CA];
    u32 flags;
    u16 hflags;
    u8 pad11[0x3D0 - 0x2D6];
    struct S *other;
    u32 at3D4;
};

void Vorn_BeginAction16(struct S *s)
{
    void (*fn)(void) = SceneActorControl_OnIdle;
    u8 code = 0x10;
    u32 zero;
    u8 one;
    u8 zero8;
    u32 *flags;
    u8 *taken;
    u32 two;
    taken = &s->taken;
    zero = 0;
    one = 1;
    *taken = one;
    s->done = zero;
    s->code = code;
    s->callback = fn;
    zero8 = 0;
    s->timer = zero;
    flags = &s->flags114;
    {
        u32 mask = -2;
        *flags &= mask;
    }
    s->at104 = zero;
    two = 2;
    s->a = zero8;
    s->b = two;
    s->c = zero8;
    s->count = zero;
    s->queued = one;
}

void Vorn_BeginAction16(struct S *s);

u8 Vorn_BeginPursuitRoute(struct Vorn *actor, u32 reset);

void Vorn_SelectRecoveryAction(u8 *p, const u8 *unused, u32 unused2)
{
    if ((*(u8 **)(p + 0x3d0))[0x6c0]) {
        if (SceneActor_IsFacingNearbyTarget(p))
            Vorn_BeginAction16(p);
        else
            Vorn_SelectRetreatRoute(p, 1);
    } else
        Vorn_BeginPursuitRoute(p, 1);
}

extern const u8 gUnk_0824E230[];

u8 SceneActor_DispatchContextAction(struct Vorn *, u32);

static inline u8 HasFlag(u32 value, u32 mask)
{
    mask &= value;
    if (mask)
        return 1;
    return 0;
}

// The u16 parameter here and the u8 mask of MotionFlag lengthen the actor pointer's life by a

// few insns before combine, enough that `active` (8 refs) outranks it for r4.

static inline u8 ShouldReact(struct Vorn *ctx, u16 value)
{
    if (HasFlag(value, 1))
        return 1;
    if (ctx->response >= 0)
        return 1;
    return 0;
}

static inline u8 MotionFlag(struct SceneActorReactionMotion *m, u8 mask)
{
    if (m->flags & mask)
        return 1;
    return 0;
}

static inline void SetMotion_SelectReactionControl(struct SceneActorReactionMotion *m, u8 mask)
{
    m->flags = mask | m->flags;
}

// confidence=low: selects a response script or idle control from context state.

static inline void MarkReactionMotion(struct SceneActorReactionMotion *m)
{
    u8 mask = 8;
    m->flags = mask | m->flags;
}

void Vorn_SelectReactionControl(struct ChaseActor *a)
{
    struct Vorn *ctx = a->base.context;
    u16 *flags = &ctx->flags6D4;
    u32 value = ~8;
    value &= *flags;
    *flags = value;
    if (ShouldReact(ctx, value)) {
        Vorn_BeginResponsePursuit(a, 1, gUnk_0824E230, 0x279);
        MarkReactionMotion((void *)&a->base.tracking.target);
        {
            u32 mask = 0x80;
            a->base.behaviorFlags = mask | a->base.behaviorFlags;
        }
    } else {
        u32 active = MotionFlag((void *)&a->base.tracking.target, 8);
        if (active)
            SceneActor_DispatchContextAction(ctx, 0);
        else {
            Chase_SetState(a, active, 1, active);
            Chase_SetCtl(a, SceneActorControl_OnIdle, 16);
        }
        SetMotion_SelectReactionControl((void *)&a->base.tracking.target, 8);
    }
}

// confidence=low: route role inferred from callers and terrain/path tests.

static inline u32 CellIndex(struct ContactPosition *p)
{
    s32 x = p->x >> 8, z = p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}

static inline u32 CellHeight(struct ContactPosition *position, u32 cell)
{
    u32 shape, height;
    u8 *data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data += 4;
    else {
        struct EngineMap *map = gTerrainMap;
        u32 offset = cell * 4 + 12;
        data = map->cells + offset;
    }
    shape = *data >> 4;
    height = (*data & 15) << 8;
    if (shape == 1)
        goto slopeZ;
    if (shape == 2)
        goto slopeX;
    goto done;
slopeZ:
    height -= *((u8 *)position + 4);
    goto done;
slopeX:
    height -= *(u8 *)position;
done:
    return height;
}

static inline void ClearRouteBits(u16 *field, u32 bits)
{
    *field &= ~bits;
}

static inline u8 TryRoute(struct Vorn *actor, void *route, struct ContactPosition *position,
                          struct ContactPosition *target)
{
    s32 result = Nav_IsPositionInMesh(route, position, target);
    if (!result) {
        ClearRouteBits(&actor->chase.base.motionFlags, 0x200);
        return 0;
    }
    result = Terrain_ApplyBlocked(route, position, target);
    if (result) {
        if (result != 2)
            Memory_Clear(&actor->chase.base.body.motion, 8);
        return 1;
    }
    ClearRouteBits(&actor->chase.base.motionFlags, 0x200);
    return 0;
}

static inline u8 Test(u16 *p, u32 mask)
{
    if (*p & mask)
        return 1;
    return 0;
}

// confidence=low: routes toward the current target after a reaction.

static inline u8 TestContext_BeginTargetPursuit(struct Vorn *p, u32 mask)
{
    if (p->flags6D4 & mask)
        return 1;
    return 0;
}

static inline u8 NeedsResponse(struct Vorn *p)
{
    if (TestContext_BeginTargetPursuit(p, 1) || p->response >= 0)
        return 1;
    return 0;
}

void Vorn_BeginTargetPursuit(struct Vorn *actor)
{
    u8 *group;
    struct ContactPosition *dest;
    struct RouteLink {
        u8 pad[8];
        u16 *point;
    };
    struct RouteLink *point;
    ClearRouteBits((void *)((u8 *)actor->chase.base.context + 0x6d4), 8);
    if (NeedsResponse(actor->chase.base.context)) {
        Vorn_BeginResponsePursuit(actor, 1, gUnk_0824E230, 0x29a);
        return;
    }
    if (MotionFlags(&actor->chase, 0x100))
        return;
    group = (u8 *)&actor->chase.group.common;
    if (TestContext_BeginTargetPursuit(actor->chase.base.context, 0x1d)) {
        actor->chase.base.field2C2 = 32;
        if (TestContext_BeginTargetPursuit(actor->chase.base.context, 4) &&
            MotionFlags(&actor->chase, 0x80)) {
            actor->chase.group.common.gate.counter += 300;
            if (actor->chase.group.common.gate.counter > actor->chase.group.common.gate.duration)
                actor->chase.group.common.gate.counter = actor->chase.group.common.gate.duration;
        }
        if (SceneActor_DispatchContextAction(actor->chase.base.context, 1))
            return;
    }
    {
        u32 state = 3, phase = 1;
        actor->chase.base.state2A8 = state;
        actor->chase.base.phase2A9 = phase;
        actor->chase.base.behavior2AA = 0;
        actor->chase.base.counter2AC = 0;
        actor->chase.base.field2B1 = 1;
    }
    SetControl(&actor->chase, SceneActorReaction_RestoreAfterDelay, 14);
    {
        u32 mask = 16;
        u32 *p = &actor->chase.base.behaviorFlags;
        u32 value = *p;
        value |= mask;
        *p = value;
    }
    actor->chase.base.field2C2 = 32;
    {
        struct SceneActorReactionCommand *command = (void *)(group + 0x8c);
        void *payload = actor->chase.base.onAlternateEntry;
        u32 kind = 7;
        command->duration = 60;
        command->payload = payload;
        command->first = 0;
        command->second = 0;
        command->kind = kind;
    }
    dest = *(struct ContactPosition **)(group + 0x200);
    point = (struct RouteLink *)(group + 0x74);
    dest->x = point->point[0];
    dest->z = point->point[1];
    dest->y = CellHeight(dest, CellIndex(dest));
    (*(struct ContactPosition **)(group + 0x200))->pad = 0;
    if (!TryRoute(actor, *(void **)(group + 0x1fc), &actor->chase.base.body.position,
                  &actor->chase.base.tracking.target->position))
        Callback_InvokeActor(actor, *(void **)(group + 0x224));
}

// Tentative scene-actor family names; behavior inferred from static accesses.

void SceneActorCollision_InitBody(void *, void *, void *, void *, void *);

void SceneActorCollision_InitAttack(void *, void *, void *, void *, u32, void *);

void SceneActorCollision_InitExternalAttack(void *, void *, void *, void *, void *, u32, void *);

void Vorn_OnCollisionResponse(void);

void Vorn_OnContactEvent(void);

void Vorn_OnScriptActionRequest(void);

void SceneActor_InitContextColliders(struct ChaseActor *p)
{
    struct ContactPackedVector size, offset;
    u8 *q = p->base.context;
    size.x = 0x40;
    size.y = 0x80;
    size.z = 0x40;
    *(u32 *)&offset = 0x800000;
    *((u32 *)&offset + 1) &= 0xFFFF0000;
    SceneActorCollision_InitBody(p, &size, &offset, Vorn_OnCollisionResponse, p);
    ((struct ContactPosition *)&size)->x = 0x40;
    ((struct ContactPosition *)&size)->y = 0x64;
    ((struct ContactPosition *)&size)->z = 0x40;
    ((struct ContactPosition *)&offset)->x = 0;
    ((struct ContactPosition *)&offset)->y = 0x80;
    ((struct ContactPosition *)&offset)->z = 0;
    SceneActorCollision_InitAttack(p, &size, &offset, Vorn_OnContactEvent, 0, p);
    ((struct ContactPosition *)&size)->x = 0x40;
    ((struct ContactPosition *)&size)->y = 0x64;
    ((struct ContactPosition *)&size)->z = 0x40;
    ((struct ContactPosition *)&offset)->x = 0;
    ((struct ContactPosition *)&offset)->y = 0x80;
    ((struct ContactPosition *)&offset)->z = 0;
    SceneActorCollision_InitExternalAttack(p, q + 0x628, &size, &offset, Vorn_OnScriptActionRequest,
                                           1, p);
}

// Tentative scene-actor family names; behavior inferred from static accesses.

void SceneActorReaction_ResetResponseState(void *, const void *, u32);

void SceneActorAnimation_SelectDirectionalEntry(struct RoomActor *, s32);

s32 Anim_Step(void *, void *);

void SceneActor_ResetContextAnimation(struct Vorn *p)
{
    struct Vorn *q = p->chase.base.context;
    q->field6D6 = 0;
    q->flags6D4 = 0;
    q->field6E8 = 0;
    q->field698 = 0;
    Memory_Clear(p->chase.group.destination, 8);
    Memory_Clear(&q->vector6E0, 8);
    Memory_Clear(&q->vector6D8, 8);
    SceneActorReaction_ResetResponseState(&p->chase.group.saved.control, gUnk_0824E230, 0x2F8);
    SceneActorMovement_ResolveRoutePosition(p);
    SceneActorAnimation_SelectDirectionalEntry(p, 0);
    Anim_Step((u8 *)p->chase.base.drawable + 0x20, p->chase.base.drawable);
}

void Vorn_OnAnimationCleanup(void)
{
}

struct PInitActorProfile {
    u8 f[0x2c4];
    u32 a;
    u8 g[0x40];
    u32 b;
};

s32 SceneActorProfiles_FindOrAdd(u32, u32);

void Vorn_InitActorProfile(struct PInitActorProfile *p)
{
    p->a = 0;
    p->b = SceneActorProfiles_FindOrAdd(0x19, 0);
}

extern const u16 gUnk_08606004[];

u16 Vorn_GetAnimationEvent(u8 *p)
{
    u8 *anim = *(u8 **)(p + 4) + 0x20;
    u16 r;
    if (*(u16 *)(anim + 0xc)) {
        r = 0;
    } else {
        u32 m = 0x10000;
        *(u32 *)(p + 0x2d0) |= m;
        r = gUnk_08606004[*(u16 *)(anim + 0x14)];
    }
    return r;
}

/* Vorn_SelectRandomBehavior: Selects a different random eight-byte behavior entry and resets its cursor; behavior interpretation is tentative */


static inline u32 Random(void)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(u16 *)(offset + (u32)gRandomTable);
}

struct S080B1F48 {
    u8 pad0[2];
    u8 index;
    u8 unk3;
    u8 *data;
    u8 *entry;
};

void Vorn_SelectRandomBehavior(u8 *owner)
{
    struct S080B1F48 *p = (struct S080B1F48 *)(owner + 0x448);
    struct S080B1F48 *q = p;
    u8 n;
    n = Mod(Random(), *p->data);
    if (n == p->index)
        n = Mod(n + 1, *p->data);
    q->index = n;
    q->unk3 = 0;
    q->entry = Path_GetPointData(q->data) + q->index * 8;
}

s32 Math_Atan2(s32, s32);

struct RouteCursor {
    u8 pad0[2];
    u8 index;
    u8 unk3;
    u8 *data;
    u16 *entry;
};

static inline u32 Route_Random(void)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(u16 *)(offset + (u32)RANDOM_TABLE_ADDR);
}

static inline void Route_SelectRandom(u8 *cursor)
{
    struct RouteCursor *p = (struct RouteCursor *)cursor;
    struct RouteCursor *q = p;
    u8 n;
    n = Mod(Route_Random(), *p->data);
    if (n == p->index)
        n = Mod(n + 1, *p->data);
    q->index = n;
    q->unk3 = 0;
    q->entry = (u16 *)(Path_GetPointData(q->data) + q->index * 8);
}

static inline s32 Dot(struct ContactPosition *a, struct ContactPosition *b)
{
    return a->x * b->x + a->z * b->z;
}

/* Chase_CellAt, Chase_Height and Chase_SnapToRoute as this function spells them: the cell index is
   computed into x itself (x = Chase_CellIndex(x, z)), so x, the index and the result share one
   register (fix.py sp_result_into_arg). */

static inline s32 Chase_CellAt_(struct ContactPosition *position)
{
    s32 x = position->x >> 8;
    x = Chase_CellIndex(x, position->z >> 8);
    return x;
}

static inline u32 Chase_Height_(struct ContactPosition *position)
{
    return Chase_HeightAt(Chase_CellAt_(position), position);
}

static inline void Chase_SnapToRoute_(struct ContactPosition *p, u8 *route)
{
    p->x = (*(u16 **)(route + 8))[0];
    p->z = (*(u16 **)(route + 8))[1];
    p->y = Chase_Height_(p);
}

void Vorn_SelectRouteAwayFromTarget(struct ChaseActor *actor)
{
    u8 *group = (u8 *)(&actor->group.common);
    struct RouteCursor *route = (struct RouteCursor *)(((u8 *)actor) + 0x448);
    u8 selected = 0;
    struct ChaseTarget *t;
    struct ContactPosition dir;
    struct ContactPosition delta;
    u8 angle;
    u16 *entry;
    s32 i;
    u8 found;
    u8 *cursor;
    void *nav;
    t = *((struct ChaseTarget **)(((u8 *)actor) + 0xDC));
    angle = Math_Atan2(t->position.x - actor->base.body.position.x,
                       t->position.z - actor->base.body.position.z);
    dir.x = (gUnk_086149C4[(angle + 64) & 255] * 10) >> 12;
    dir.z = (gUnk_086149C4[angle] * 10) >> 12;
    entry = (u16 *)Path_GetPointData(route->data);
    found = 0;
    for (i = 0; i < (*route->data); i++) {
        delta.x = entry[0] - actor->base.body.position.x;
        delta.z = entry[1] - actor->base.body.position.z;
        if (Dot(&dir, &delta) < 0) {
            u32 w = *((u32 *)entry);
            if (w == (*(*((u32 **)(group + 0x7C)))))
                continue;
            selected = i;
            found = 1;
            break;
        }
        entry += 4;
    }

    if (!found) {
        cursor = group + 0x74;
        Route_SelectRandom(cursor);
    } else {
        route->index = selected;
        route->unk3 = 0;
        route->entry = entry;
        cursor = group + 0x74;
    }
    Chase_SnapToRoute_(*((struct ContactPosition **)(group + 0x200)), cursor);
    nav = *((void **)(group + 0x1FC));
    if (!Chase_TryRoute(actor, nav, &actor->base.body.position,
                        *((struct ContactPosition **)(group + 0x200)))) {
        Chase_SetCtl(actor, SceneActorControl_OnIdle, 0);
        Chase_SetState(actor, 3, 0, 0);
    } else {
        (*((struct ContactPosition **)(group + 0x200)))->pad = 1;
    }
}

void Vorn_ParseField624(struct ChaseActor *actor)
{
    struct Vorn *ctx = (struct Vorn *)actor->base.context;

    if (Script_SeekToKeyword(0x78) != 0) {
        s32 val1, val2;

        if (Script_GetPc() != 0)
            val1 = Script_GetValue();
        else
            val1 = 0;
        ctx->field624 = val1;

        if (Script_GetPc() != 0)
            val2 = Script_GetValue();
        else
            val2 = 0;
        ctx->field626 = val2;
        ctx->field627 = 0;
    } else {
        ctx->field624 = 0;
        ctx->field626 = 0;
        ctx->field627 = 0;
    }
}

struct VornDeferredScript {
    u8 unknown0[0x624];
    u16 script;
    u8 duration, elapsed;
};

// confidence=low: context script 624 is invoked once the byte-sized delay expires.


void Vorn_UpdateDelayedScript(struct ChaseActor *actor)
{
    struct VornDeferredScript *context = actor->base.context;
    if (context->script) {
        context->elapsed++;
        if (context->elapsed >= context->duration) {
            Script_Call(context->script, 0);
            context->script = 0;
            context->duration = 0;
            context->elapsed = 0;
        }
    }
}

// confidence=low: starts the target-pursuit route and resets pursuit timers and callbacks.

void SceneBody_StartRoute(void *, void *, u32, u32, u32);

void Vorn_SelectRouteAwayFromTarget(struct ChaseActor *actor);

struct VornResponseFlags {
    s16 bits : 16;
};

static inline u8 Vorn_HasPursuitFlag(struct VornResponseFlags *const *cursor, u16 mask)
{
    u16 value = (*cursor)->bits;
    value &= mask;
    if (value)
        return 1;
    return 0;
}

static inline u8 Vorn_TestResponseFlags(struct VornResponseFlags *flags, u32 mask)
{
    u16 value = flags->bits;
    if (value & mask)
        return 1;
    return 0;
}

static inline void Vorn_SetResponseFlags(struct VornResponseFlags *flags, u16 bits)
{
    flags->bits |= bits;
}

static inline void SetFlags(u16 bit, u16 *field)
{
    struct Flags {
        s16 bits : 16;
    };
    struct Flags *p = (void *)field;
    p->bits |= bit;
}

static inline void StartRoute(void *body, void *path, u8 *group)
{
    u32 index = group[0x83] ^ 1;
    u8 *routes = group + 0x81;
    SceneBody_StartRoute(body, path, routes[index], 0, 0);
}

u8 Vorn_BeginPursuitRoute(struct Vorn *actor, u32 reset)
{
    struct Vorn *context = actor->chase.base.context;
    u8 *group = (void *)&context->chase.group.common;
    struct VornResponseFlags *flags;
    if (context->pursuitCooldown)
        return 0;
    flags = (struct VornResponseFlags *)&context->flags6D4;
    if (reset != 1 && Vorn_HasPursuitFlag(&flags, 4) && !Vorn_TestResponseFlags(flags, 2))
        return 0;
    context->pursuitCooldown = 60;
    {
        Vorn_SetResponseFlags(flags, 4);
        flags->bits &= ~2;
    }
    *((u16 *)(((u8 *)context) + 0x6ec)) = 60;
    if (reset == 1)
        StartRoute(&actor->chase.base.body.id, group + 0x74, group);
    Vorn_SelectRouteAwayFromTarget(&actor->chase);
    {
        u32 *field = &actor->chase.base.behaviorFlags;
        s32 mask = ~16;
        *field &= mask;
    }
    SetState(&actor->chase, 15);
    SetControl(&actor->chase, actor->chase.base.onStanding, 0);
    SetMotion(&actor->chase, Vorn_UpdatePursuitDistance, 37);
    {
        u16 bit = 4;
        u16 *field = &actor->chase.group.common.gate.flags;
        bit |= *field;
        *field = bit;
    }
    SetFlags(0x80, &actor->chase.base.motionFlags);
    {
        u32 timer = actor->chase.group.common.gate.duration;
        actor->chase.group.common.gate.counter = timer;
    }
    {
        u8 *field = ((u8 *)actor) + 0xdc;
        u32 bit = 8;
        bit |= field[15];
        field[15] = bit;
    }
    actor->chase.base.behaviorFlags |= 0x80;
    return 1;
}

struct PInitMotionLimits {
    u8 f[0x6c4];
    u32 a;
    u32 b;
    u32 c;
};

void Vorn_InitMotionLimits(struct PInitMotionLimits *p)
{
    p->a = 0;
    p->b = 0x10000;
    p->c = 0x90000;
}
