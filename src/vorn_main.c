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
#include "rotation_phase.h"
#include "scene_vec.h"
#include "scene_actor_contact_response.h"
#include "scene_actor_placement.h"
#include "scene_body_api.h"
#include "render.h"
#include "display_sequence_inline.h"
#include "scene_actor_init_primitives.h"
#include "scene_actor_init_group.h"
#include "scene_actor_init_dispatch.h"
#include "scene_actor_init_graphics.h"

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
    *prepared = 1;
    actor->base.stepDone = 0;
    actor->base.controlKind = kind;
    actor->base.target = control;
    actor->base.field2C8 = 0;
    {
        u32 *flags = &actor->base.pendingControlFlags;
        s32 mask = -2;
        *flags &= mask;
        actor->base.pendingControl = 0;
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


void SceneActorPhase_UpdateConditionalMotion(struct ChaseActor *p)
{
    if (SceneActorControl_TakeFlag(&p->base.field2B1)) {
        Chase_SetCtl(p, p->base.onEntry, 2);
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
        Chase_SetCtl(p, p->base.onStanding, 0);
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
        Chase_SetCtl(p, p->base.onEntry, 2);
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
    if (SceneActorPlacement_ReadFlags32(actor, 0x80000))
        return;
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

/* This class's own copies of the shared chase inlines it compiles differently. */

static inline u8 Vorn_TryRoute(struct ChaseActor *actor, void *route,
                               struct ContactPosition *position, struct ContactPosition *target)
{
    s32 result = Nav_IsPositionInMesh(route, position, target);
    if (result)
        result = Terrain_ApplyBlocked(route, position, target);
    if (result) {
        if (result != 2)
            Memory_Clear(SceneActorChase_MotionOf(actor), 8);
        return 1;
    }
    SceneActorChase_ClearBits(SceneActorChase_MotionFlagsOf(actor), 0x200);
    return 0;
}

// Tentative: searches progressively shorter cardinal routes around the actor.

u8 SceneActorTerrain_CanTraverseCell(u32, u32, u32, u32);

static inline s32 Vorn_RoundDirection(s32 n)
{
    if (n >= 0)
        return n >> 6;
    return -((-n) >> 6);
}

static inline s32 Vorn_MapSlotWidth(struct EngineMap **slot)
{
    return ((struct SceneActorChaseTerrainGrid *)(*slot)->cells)->width;
}

static inline s32 Vorn_SweepEscapeCells(struct ChaseActor *actor, s32 range)
{
    struct EngineMap **cachedMapSlot = &gTerrainMap;
    s32 cell = actor->base.terrainCell;
    u32 height = SceneActorChase_HeightAt(cell, &actor->base.body.position);
    u32 shape = SceneActorChase_CellProperties(cell)->attributes >> 4;
    s32 direction = Vorn_RoundDirection(actor->base.body.direction + 32) & 3;
    s32 next;
    s32 i;
    struct EngineMap **completionMapSlot;

    switch (direction) {
        case 0:
            next = cell + 1;
            i = 0;
            if (i < range)
                do {
                    if (!SceneActorTerrain_CanTraverseCell(cell, next, height, shape))
                        break;
                    next++;
                } while (++i < range);
            goto east;
        case 1:
            next = cell + SceneActorChase_MapWidth(*cachedMapSlot);
            i = 0;
            completionMapSlot = (struct EngineMap **)0x030052F4;
            if (i < range)
                do {
                    if (!SceneActorTerrain_CanTraverseCell(cell, next, height, shape))
                        goto south;
                    next += SceneActorChase_MapWidth(*cachedMapSlot);
                } while (++i < range);
            return next - Vorn_MapSlotWidth(completionMapSlot);
        case 2:
            next = cell - 1;
            i = 0;
            if (i < range)
                do {
                    if (!SceneActorTerrain_CanTraverseCell(cell, next, height, shape))
                        break;
                    next--;
                } while (++i < range);
            goto west;
        case 3:
            next = cell - SceneActorChase_MapWidth(*cachedMapSlot);
            i = 0;
            completionMapSlot = (struct EngineMap **)0x030052F4;
            if (i < range)
                do {
                    if (!SceneActorTerrain_CanTraverseCell(cell, next, height, shape))
                        goto north;
                    next -= SceneActorChase_MapWidth(*cachedMapSlot);
                } while (++i < range);
            return next + Vorn_MapSlotWidth(completionMapSlot);
        default:
            goto home;
    }
east:
    return next - 1;
south:
    return next - Vorn_MapSlotWidth(cachedMapSlot);
west:
    return next + 1;
north:
    return next + Vorn_MapSlotWidth(cachedMapSlot);
home:
    return actor->base.terrainCell;
}

static inline u32 Vorn_HeightFromAttributes(u32 value)
{
    u32 height = 15;
    height &= value;
    return height << 8;
}

static inline void Vorn_WriteHeight(s16 *field, u32 height)
{
    *field = height;
}

#define VORN_CENTER_COORDINATE(out_, coordinate_)                                                  \
    do {                                                                                           \
        (out_) = ((u32)(coordinate_) << 8) + 128;                                                  \
    } while (0)

static inline void Vorn_CellCenter(s32 cell, struct ContactPosition *position)
{
    s32 centeredZ;
    s32 x = Mod(cell, SceneActorChase_MapWidth(SceneActorChase_CurrentMap()));
    s32 z = Div(cell, SceneActorChase_MapWidth(SceneActorChase_CurrentMap()));
    VORN_CENTER_COORDINATE(position->x, x);
    Vorn_WriteHeight(&position->y,
                     Vorn_HeightFromAttributes(SceneActorChase_CellProperties(cell)->attributes));
    VORN_CENTER_COORDINATE(centeredZ, z);
    position->z = centeredZ;
}

u8 Vorn_SelectReachableEscapeCell(struct ChaseActor *actor, u32 savedDirection)
{
    struct ContactPosition target;
    struct SceneActorChaseGroup *group;
    s32 tries;
    u32 found = 0;
    s32 range = 2;
    s32 dest;
    do {
        tries = 0;
        do {
            dest = Vorn_SweepEscapeCells(actor, range);
            if (dest != actor->base.terrainCell) {
                found = 1;
                goto nextRange;
            }
            actor->base.body.direction += 64;
        } while ((++tries) <= 3);
    nextRange:
        range--;

        actor->base.body.direction = savedDirection;
        if (found || (range <= 0))
            goto done;
    } while (1);
done:
    Vorn_CellCenter(dest, &target);

    group = &actor->group;
    if (!Vorn_TryRoute(actor, group->route, &actor->base.body.position, &target)) {
        Callback_InvokeActor(actor, group->fallback);
        return 0;
    }
    return 1;
}

u8 Vorn_SelectReachableEscapeCell(struct ChaseActor *, u32);

struct VornRetreatFlags {
    s16 bits : 16;
};

struct VornRetreatContext {
    u8 unknown0[0x6C4];
    u32 requests, nearDistance, farDistance;
    u16 timer, unknown6D2;
    struct VornRetreatFlags flags;
};

static inline void Vorn_AdjustContextFlags(struct VornRetreatContext *body, u16 set, u32 clear)
{
    u16 bit = set;
    struct VornRetreatFlags *flags = &body->flags;
    flags->bits |= bit;
    flags->bits &= ~clear;
}

static inline u32 Vorn_RandomSample(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return ((const u16 *)RANDOM_TABLE_ADDR)[gRandomIndex];
}

static inline u32 Vorn_ScatterDirection(const u8 *heading, u32 sample)
{
    return (sample & 63) + *heading;
}

static inline u8 Vorn_TryTrackedCell(void *object)
{
    struct ChaseActor *actor = object;
    struct SceneActorChaseGroup *group = SceneActorChase_GroupOf(object);
    struct SceneActorChaseTracking *tracking = &actor->base.tracking;
    u32 cell;
    struct ContactPosition target;
    SceneActorChase_SnapToRoute(group->destination, &group->homeRoute);
    cell = SceneActorChase_CellAt(&tracking->target->position);
    if (cell == actor->base.terrainCell ||
        SceneActorChase_Bits32(&gPlayers[0]->statusFlags,
                               0x20000)) {
        SceneActor_DispatchContextAction(actor->base.context, 0);
        return 0;
    }
    SceneActorChase_CellCenter(cell, &target);
    if (!SceneActorChase_TryRoute(object, group->route, &actor->base.body.position, &target)) {
        Callback_InvokeActor(actor, group->fallback);
        return 0;
    }
    return 1;
}

u32 Vorn_SelectRetreatRoute(struct ChaseActor *actor, u32 reset)
{
    struct VornRetreatContext *body = actor->base.context;
    struct SceneActorChaseGroup *group = &actor->group;
    u8 old;
    if ((!reset) && body->timer)
        return 0;
    body->timer = 60;
    SceneActorChase_ClearActorFlags(actor, 0x10);
    Vorn_AdjustContextFlags(body, 0x10, 0x40);
    SceneActorReaction_ResetResponseState(&group->saved, gUnk_0824E230, 0x486);
    body->requests++;
    if (actor->base.tracking.distance < body->nearDistance) {
        old = actor->base.body.direction;
        actor->base.body.direction = actor->base.tracking.heading + 0x80;
        if (!Vorn_SelectReachableEscapeCell(actor, old))
            return 0;
        SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0x10);
        SceneActorChase_SetBehavior(actor, 0x15);
    } else if (actor->base.tracking.distance > body->farDistance) {
        old = actor->base.body.direction;
        {
            u32 random = Vorn_RandomSample();
            actor->base.body.direction =
                Vorn_ScatterDirection(&actor->base.tracking.heading, random);
        }
        if (!actor->base.tracking.target)
            actor->base.tracking.target =
                &((struct SceneActorChasePlayer *)gPlayers[0])->target;
        if ((!Vorn_TryTrackedCell(actor)) && (!Vorn_SelectReachableEscapeCell(actor, old)))
            return 0;
        SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0x10);
        SceneActorChase_SetBehavior(actor, 0x15);
    } else {
        old = actor->base.body.direction;
        {
            u32 random = Vorn_RandomSample();
            actor->base.body.direction =
                Vorn_ScatterDirection(&actor->base.tracking.heading, random);
        }
        if (!actor->base.tracking.target)
            actor->base.tracking.target =
                &((struct SceneActorChasePlayer *)gPlayers[0])->target;
        if ((!Vorn_TryTrackedCell(actor)) && (!Vorn_SelectReachableEscapeCell(actor, old)))
            return 0;
        SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0x10);
        SceneActorChase_SetBehavior(actor, 0x15);
    }
    {
        u32 bit = 4;
        u32 *field = &actor->base.statusFlags;
        u32 value = *field;
        value |= bit;
        *field = value;
    }
    return 1;
}

#define gRandomTableIndex gRandomIndex

s32 SceneActorRegistry_CountLiving(void);

s32 SceneActorRegistry_CountLivingByCategory(s32);

void SceneActorReaction_UpdateFacingAttack(void);

static inline u8 Inactive(struct Vorn *p)
{
    if (p) {
        u32 mask = 0x40;
        u32 offset = 0x2D4;
        if (p->chase.base.motionFlags & mask)
            goto inactive;
        {
            u32 mask = 0x20000;
            offset -= 4;
            if (p->chase.base.statusFlags & mask)
                goto inactive;
        }
        if (p->chase.base.health > 0)
            goto active;
    inactive:
        return 1;
    }
active:
    return 0;
}

static inline s32 RandomByte(void)
{
    gRandomIndex = (gRandomIndex + 1) & 0x3FF;
    return *(const u8 *)(gRandomTable + gRandomIndex);
}

static inline void Clear(u32 *p, u32 mask)
{
    *p &= ~mask;
}

inline static s32 RandomByte_TrySoloAction(void)
{
    gRandomIndex = (gRandomIndex + 1) & 0x3FF;
    return (u8)((const u16 *)0x0203B400)[gRandomIndex];
}

void SceneActor_TrySoloAction(struct Vorn *p)
{
    struct Vorn *q = p;
    u8 zero;
    s32 t1;
    u32 timer;
    if (Inactive(p))
        return;
    {
        u32 mask = 0xE33F;
        if (p->chase.base.motionFlags & mask)
            return;
    }
    if (!q->field688)
        return;
    t1 = 4;
    if (!Test(&q->flags6D4, t1))
        return;
    if (SceneActorRegistry_CountLivingByCategory(0x19) > 1)
        return;
    if (SceneActorRegistry_CountLiving() > 1)
        return;
    timer = q->timer68D;
    if ((zero = timer) != 0)
        q->timer68D = timer - 1;
    else {
        q->timer68D = 0xF0;
        if (RandomByte_TrySoloAction() <= 0x9F)
            return;
        if (!q->field688)
            return;
        {
            u32 state = 3;
            p->chase.base.state2A8 = state;
        }
        p->chase.base.phase2A9 = zero;
        p->chase.base.behavior2AA = zero;
        p->chase.base.counter2AC = zero;
        p->chase.base.field2B1 = 1;
        {
            void *callback = SceneActorReaction_UpdateFacingAttack;
            u32 mode = 0x2A;
            p->chase.base.field2B2 = 1;
            p->chase.base.stepDone = zero;
            p->chase.base.controlKind = mode;
            p->chase.base.target = callback;
        }
        p->chase.base.field2C8 = zero;
        Clear(&p->chase.base.pendingControlFlags, 1);
        p->chase.base.pendingControl = zero;
    }
}

void Vorn_ParseField688(struct Vorn *ctx)
{
    if (Script_SeekToKeyword(0x73) != 0) {
        s32 val1;

        if (Script_GetPc() != 0)
            val1 = Script_GetValue();
        else
            val1 = 0;
        ctx->field68C = val1;

        if ((u8)val1 == 1) {
            ctx->field688 = 0;
        } else {
            s32 *p = &ctx->field688;
            s32 val2;

            if (Script_GetPc() != 0)
                val2 = Script_GetValue();
            else
                val2 = 0;
            *p = val2;
        }
    }
}

void Vorn_RunPendingScript(u8 *p)
{
    u32 *q = (u32 *)(p + 0x688);
    if (*q != 0) {
        Script_Call(*q, 0);
        *q = 0;
    }
}

void Vorn_ParseField69A(struct Vorn *ctx)
{
    if (Script_SeekToKeyword(0x50) != 0) {
        ctx->frame69A = (Script_GetPc() != 0) ? Script_GetValue() : 300;
    } else {
        ctx->frame69A = 300;
    }
    ctx->field698 = ctx->frame69A;
}

static inline u32 Vorn_ActorFlags(struct Vorn *actor, u32 mask)
{
    return actor->chase.base.behaviorFlags & mask;
}

static inline u8 Vorn_TestContext(struct Vorn *context, u16 mask, struct VornResponseFlags **field)
{
    struct VornResponseFlags *flags = (void *)&context->flags6D4;
    u16 value = flags->bits;
    value &= mask;
    *field = flags;
    if (value)
        return 1;
    return 0;
}

static inline void Vorn_SetTimedMotion(struct Vorn *actor, void *callback, u32 duration)
{
    actor->chase.base.pendingControl = (u32)callback;
    actor->chase.base.pendingControlData = 0;
    actor->chase.base.pendingControlRelease = 0;
    actor->chase.base.pendingControlKind = duration;
    actor->chase.base.pendingControlFlags = 1;
}

static inline void Vorn_SetTrackingFlags(struct SceneActorChaseTracking *tracking, u8 bits)
{
    tracking->flags = bits | tracking->flags;
}

static inline void Vorn_ResetContext(struct Vorn *a)
{
    struct SceneActorChaseTracking *motion;
    struct SceneActorRegistryControl *control;
    u16 *flags;
    a->chase.base.field2C2 = 64;
    a->chase.group.common.gate.flags = 0;
    a->chase.group.common.gate.counter = 0;
    flags = &a->chase.base.motionFlags;
    {
        s32 mask = ~0x380;
        *flags = mask & (*flags);
    }
    motion = &a->chase.base.tracking;
    {
        s32 mask = ~0x18;
        motion->flags = mask & motion->flags;
    }
    control = &a->chase.group.common.control;
    if (Vorn_ActorFlags(a, 0x200000)) {
        u32 *state = &a->chase.base.statusFlags;
        s32 mask = ~0x1000000;
        *state = mask & (*state);
        {
            s32 mask = ~0x200;
            *flags = mask & (*flags);
        }
        control->mode = 0;
        control->counter = 0;
        control->timer = 0;
    }
    {
        s32 mask = ~0x400;
        *flags = mask & (*flags);
    }
    a->chase.group.turn.turnsRemaining = 255;
}

void Vorn_BeginResponsePursuit(struct Vorn *a, u32 reset, const void *unused, u32 unused2)
{
    struct Vorn *ctx = (void *)a->chase.base.context;
    struct SceneActorChaseGroup *group = &ctx->chase.group;
    struct VornResponseFlags *flags;

    if (ctx->field69E)
        return;
    if (Vorn_TestContext(ctx, 1, &flags) && !Vorn_TestResponseFlags(flags, 2))
        return;
    ctx->field69E = 60;
    Vorn_ResetContext(ctx);
    Vorn_SetResponseFlags(flags, 1);
    flags->bits &= ~2;
    ctx->field698 = ctx->frame69A;
    a->chase.base.body.direction = a->chase.base.reactionDirection;
    if (reset == 1)
        SceneBody_StartRoute(&a->chase.base.body.id, &group->homeRoute,
                             group->homeRoute.branches[1 ^ group->homeRoute.activeBranch], 0, 0);
    Vorn_SelectRouteAwayFromTarget(&a->chase);
    {
        u32 mask = 4;
        a->chase.base.statusFlags = mask | a->chase.base.statusFlags;
    }
    {
        s8 mask = ~16;
        a->chase.base.behaviorFlags = mask & a->chase.base.behaviorFlags;
    }
    SceneActorChase_SetBehavior(a, 15);
    SceneActorChase_SetControl(a, a->chase.base.onStanding, 0);
    Vorn_SetTimedMotion(a, Vorn_UpdatePursuitWait, 35);
    Vorn_SetTrackingFlags(&a->chase.base.tracking, 8);
    {
        u32 mask = 128;
        a->chase.base.behaviorFlags = mask | a->chase.base.behaviorFlags;
    }
}

static inline u32 TestMotionFlag(struct Vorn *p, u32 mask)
{
    return (p->chase.base.motionFlags & mask);
}

u8 SceneActor_IsFacingNearbyTarget(struct Vorn *p)
{
    struct Vorn *q = p->chase.base.context;
    s32 absolute;
    if (q->field6E8)
        return 0;
    if (TestMotionFlag(p, 0x100))
        return 0;
    if (p->chase.base.tracking.distance > 0xFFFF)
        return 0;
    absolute =
        RotationPhase_HeadingDistance(p->chase.base.tracking.heading, p->chase.base.body.direction);
    if (absolute > 0x12)
        return 0;
    return 1;
}

struct QUpdateRecoveryCooldown {
    u8 f[0x6e8];
    u16 v;
};

struct PUpdateRecoveryCooldown {
    u8 f[0x3d0];
    struct QUpdateRecoveryCooldown *q;
};

void Vorn_UpdateRecoveryCooldown(struct PUpdateRecoveryCooldown *p)
{
    u16 *v = &p->q->v;
    if (*v != 0)
        *v = *v - 1;
}

void Vorn_ResetRecoveryMode(u8 *p)
{
    p[0x6c0] = 0;
}

void SceneStimulus_AddTagged(s16 a, s16 b, s16 c, s16 d, u8 e, u16 f);

void Vorn_TickSound(struct ChaseActor *actor)
{
    struct Vorn *ctx = (struct Vorn *)actor->base.context;

    if (ctx->field6EC == 0) {
        struct SceneVec *vec;

        ctx->field6EC = 60;
        Sound_PlaySong(0x1CF);
        vec = Player_GetEffectivePosition(0);
        if (vec != 0) {
            SceneStimulus_AddTagged(vec->x, vec->z, 0, 0, 0x80, (u16)actor->base.body.id);
        }
    } else {
        ctx->field6EC--;
    }
}

static inline u32 Vorn_CellIndex(struct ContactPosition *p)
{
    s32 x = p->x >> 8, z = p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}

static inline u32 Vorn_CellHeight(struct ContactPosition *position, u32 cell)
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
    height -= (u8)position->z;
    goto done;
slopeX:
    height -= (u8)position->x;
done:
    return height;
}

static inline void Vorn_ClearRouteBits(u16 *field, u32 bits)
{
    *field &= ~bits;
}

static inline void Vorn_SetControl(struct Vorn *actor, void *cb, u32 k)
{
    void *callback = cb;
    u32 kind = k;
    actor->chase.base.field2B2 = 1;
    actor->chase.base.stepDone = 0;
    actor->chase.base.controlKind = kind;
    actor->chase.base.target = callback;
    actor->chase.base.field2C8 = 0;
    {
        u32 *field = &actor->chase.base.pendingControlFlags;
        u32 mask = ~1;
        *field &= mask;
    }
    actor->chase.base.pendingControl = 0;
}

/* Restores the saved route after a context response; interpretation is tentative. */

void Vorn_TickSound(struct ChaseActor *actor);

void Vorn_SendMessage(struct ChaseActor *actor);

void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);

static inline u32 Vorn_Mask16(struct Vorn *p, u32 mask)
{
    return p->chase.base.motionFlags & mask;
}

static inline u32 Vorn_Mask32(struct Vorn *p, u32 mask)
{
    return p->chase.base.statusFlags & mask;
}

static inline u8 Vorn_IsInactive(struct Vorn *p)
{
    if (p && (Vorn_Mask16(p, 0x40) || Vorn_Mask32(p, 0x20000) || p->chase.base.health <= 0))
        return 1;
    return 0;
}

static inline u8 Vorn_TestBody(void *p, u32 mask)
{
    if (((struct Vorn *)p)->flags6D4 & mask)
        return 1;
    return 0;
}

static inline u32 Vorn_MaskLow(struct Vorn *p, u32 mask)
{
    return p->chase.base.behaviorFlags & mask;
}

static inline void Vorn_SnapTarget(struct ContactPosition *out,
                                   struct SceneActorChaseRoutePoint *point)
{
    out->x = point->x;
    out->z = point->z;
    out->y = Vorn_CellHeight(out, Vorn_CellIndex(out));
}

static inline void Vorn_Resume(struct Vorn *input, struct SceneActorChaseGroup *group, u32 *flags)
{
    struct Vorn *actor = input;
    struct ContactPosition target;
    struct SceneActorRegistryRoute *point;
    actor->chase.base.field2C2 = 64;
    actor->chase.group.common.gate.flags = 0;
    actor->chase.group.common.gate.counter = 0;
    Vorn_ClearRouteBits(&actor->chase.base.motionFlags, 0x380);
    {
        struct SceneActorChaseTracking *motion = &actor->chase.base.tracking;
        u32 mask = ~0x18;
        motion->flags = mask & motion->flags;
    }
    {
        struct SceneActorRegistryControl *control = &actor->chase.group.common.control;
        if (Vorn_MaskLow(actor, 0x200000)) {
            SceneActorChase_ClearWordFlags(flags, 0x1000000);
            Vorn_ClearRouteBits(&actor->chase.base.motionFlags, 0x200);
            {
                control->mode = 0;
                control->counter = 0;
                control->timer = 0;
            }
        }
    }
    Vorn_ClearRouteBits(&actor->chase.base.motionFlags, 0x400);
    actor->chase.group.turn.turnsRemaining = 255;
    SceneActorAttachment_ReplaceResponseSlot(actor, 0, 2, 0x19);
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    Vorn_SetControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
    {
        u8 k1 = 3;
        actor->chase.base.state2A8 = k1;
    }
    actor->chase.base.phase2A9 = 1;
    actor->chase.base.behavior2AA = 0;
    actor->chase.base.counter2AC = 0;
    actor->chase.base.field2B1 = 1;
    {
        struct SceneActorDelayedResponse *command = (void *)&group->saved;
        u32 target = (u32)actor->chase.base.onAlternateEntry;
        u32 kind = 7;
        command->delay = 60;
        command->control = (void *)target;
        command->state = 0;
        command->phase = 0;
        command->behavior = kind;
    }
    {
        u32 mask = 16;
        u32 *field = &actor->chase.base.behaviorFlags;
        u32 value = *field;
        value |= mask;
        *field = value;
    }
    Vorn_ClearRouteBits(&actor->chase.base.reactionFlags, 0x9c9);
    SceneBody_StartRoute(&actor->chase.base.body.id, &group->homeRoute, group->homeRoute.routeId,
                         group->homeRoute.parameterB, group->homeRoute.pointId);
    point = &group->homeRoute;
    Vorn_SnapTarget(&target, point->point);
    group->destination->pad = 1;
    if (!SceneActorChase_TryRoute(&actor->chase, group->route, &input->chase.base.body.position,
                                  &target))
        Callback_InvokeActor(input, group->fallback);
}

void Vorn_ResumeRouteAfterReaction(void *object)
{
    struct Vorn *actor = object;
    struct SceneActorChaseGroup *group = &actor->chase.group;
    struct Vorn *body;
    u32 *flags;
    u32 gateCounter;
    if (Vorn_IsInactive(actor))
        return;
    if (Vorn_Mask16(actor, 0xe33f))
        return;
    if (!Vorn_Mask16(actor, 0x80))
        return;
    {
        u32 mask = 4;
        u32 *field = &actor->chase.base.statusFlags;
        u32 value = *field;
        value |= mask;
        *field = value;
    }
    body = actor->chase.base.context;
    gateCounter = group->common.gate.counter;
    flags = &actor->chase.base.statusFlags;
    if (gateCounter) {
        if ((actor->chase.base.controlKind == 0x25) && Vorn_TestBody(body, 4))
            Vorn_TickSound(object);
        return;
    }
    if (Vorn_TestBody(body, 4)) {
        Vorn_Resume(actor, group, flags);
        Vorn_ClearRouteBits(&((struct Vorn *)actor->chase.base.context)->flags6D4, 6);
        SceneActorChase_ClearWordFlags(flags, 4);
        Vorn_SendMessage(&actor->chase);
    } else if (Vorn_TestBody(body, 16)) {
        Vorn_Resume(actor, group, flags);
        Vorn_ClearRouteBits(&((struct Vorn *)actor->chase.base.context)->flags6D4, 0x70);
        SceneActorChase_ClearWordFlags(flags, 4);
        Vorn_SendMessage(&actor->chase);
    }
}

// confidence=low: dispatches response, pursuit, and retreat actions from context flags.

static inline u8 TestActor(struct Vorn *p, u32 mask)
{
    if (p->flags6D4 & mask)
        return 1;
    return 0;
}

static inline u8 TestValue(u32 value)
{
    if (value)
        return 1;
    return 0;
}

u8 SceneActor_DispatchContextAction(struct Vorn *q, u32 reset)
{
    struct Vorn *p = q;
    u16 *flags;
    u32 first;
    if (TestActor(p, 1)) {
        if (reset == 1) {
            q->field69E = 0;
            SetFlags(2, &q->flags6D4);
        }
        Vorn_BeginResponsePursuit(p, reset, gUnk_0824E230, 0x60B);
        return 1;
    }
    {
        u32 mask = 4;
        u16 *ptr = &q->flags6D4;
        first = *ptr & mask;
        flags = ptr;
        first = TestValue(first);
    }
    if (first) {
        if (reset == 1)
            q->pursuitCooldown = 0;
        Vorn_BeginPursuitRoute(p, reset);
        return 1;
    }
    if (Test(flags, 0x10)) {
        if (reset == 1)
            q->field6D0 = 0;
        if (SceneActor_IsFacingNearbyTarget(p)) {
            Vorn_SelectRecoveryAction(p, gUnk_0824E230, 0x61F);
            return 1;
        }
        Vorn_SelectRetreatRoute(&p->chase, reset);
        return 1;
    }
    return 0;
}

// confidence=low: enters directional animation three when the contact-ready flag is set.

u8 Vorn_BeginContactAnimation(struct ChaseActor *actor)
{
    if (actor->group.hitReactionLatch == 0 && (actor->base.field2E6 & 8)) {
        actor->base.hitLockTimer = 45;
        actor->group.hitReactionLatch = 65;
        SceneActorAnimation_SelectDirectionalEntry(actor, 3);
    }
    return SceneActor_DispatchContextAction(actor, 0);
}

void SceneActorTarget_ResetContactResponse(void *);

u8 Vorn_UpdateContactResponse(struct ChaseActor *actor)
{
    struct SceneActorContactTracking *motion;
    u16 zero;
    u16 *motionFlags;
    void *work;
    void **nodeSlot;
    struct SceneActorContactNode *contact;
    struct SceneActorContactTarget *target;
    u16 cell;
    u32 active;
    u32 *actionFlags;
    work = actor;
    if (SceneActorContact_MotionFlags(SceneActorContact_ActorFromWork(&work), 0x10))
        return 0;
    if ((u8)Callback_InvokeActor(actor, actor->base.targetQuery))
        return 1;
    if (!SceneActorContact_TestTrackingOwned((void *)&actor->base.tracking, 1, &motion))
        return 0;
    motionFlags = SceneActorContact_MotionField(actor);
    {
        u16 flags;
        SCENE_ACTOR_CONTACT_READ_MOTION_FIELD(flags, motionFlags);
        zero = flags & 0x100;
    }
    if (zero)
        return 0;
    nodeSlot = &actor->group.contactNode;
    if (!actor->base.tracking.limit)
        return 0;
    SCENE_ACTOR_CONTACT_FIND_CONTACT(contact, actor);
    if (!contact)
        return 0;
    if (SceneActorContact_QueryMotionField(actor, 0x1000, &work)) {
        SceneActorContacts_ReleaseNode(*nodeSlot);
        SceneActorChase_ClearBits(work, 0x1000);
        *nodeSlot = (void *)zero;
    }
    target = contact->object;
    motion->tracking.target = &target->target;
    cell = target->cell->index;
    work = actor;
    SceneActorChase_ClearFlagComponent(SceneActorContact_GateFromWork(&work), 2);
    if (motion->active) {
        if (SceneActorContact_TestTracking(motion, 8)) {
            if (SceneActorContact_SavedTargetCell(actor) != cell)
                SceneActorContact_SaveTargetCell(actor, cell);
        } else
            SceneActorTarget_ResetContactResponse(actor);
        SceneActorChase_SetReactionFlags(actor, 0x80);
        SceneActorContact_SetTrackingFlags(motion, 0x10);
        return 0;
    }
    active = SceneActorContact_ReadActorFlags(actor, 0x80, &actionFlags);
    if (active)
        return 0;
    *actionFlags |= 0x80;
    SceneActorChase_ClearBits(motionFlags, 0x100);
    SceneActorReaction_ResetResponseState(&actor->group.saved, gUnk_0824E230, 0x689);
    SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0x10);
    SceneActorChase_SetState(actor, 3, 1, 0);
    SceneActorChase_ClearBits(motionFlags, 0x400);
    actor->group.turn.turnsRemaining = -1;
    SceneActorAnimation_SelectDirectionalEntry(SceneActorContact_ActorFromWork(&work), 0);
    actor->group.common.control.timer = 0xB4;
    SceneActorAttachment_ReplaceResponseSlot(actor, 0, 1, actor->base.category);
    SceneActorChase_SetReactionFlags(actor, 0x100);
    return 1;
}

u8 Vorn_BeginContactAnimation(struct ChaseActor *);

static inline u8 Test_SelectHitRecovery(struct Vorn *context, u32 mask)
{
    if (context->flags6D4 & mask)
        return 1;
    return 0;
}

// confidence=low: pending hit flags select recovery control or pursuit/retreat behavior.

u8 Vorn_SelectHitRecovery(struct ChaseActor *actor)
{
    struct Vorn *context = actor->base.context;
    u32 blocked;
    {
        u16 *flags = &context->flags6D4;
        s32 mask = ~8;
        *flags = *flags & mask;
    }
    if (!(actor->base.hitAttributes & 0x20000)) {
        if (actor->base.reactionSoundVariant == 0)
            Sound_PlaySong(0x211);
        else
            Sound_PlaySong(0x1CB);
    }
    {
        u32 mask = 0x8002;
        blocked = mask & actor->base.motionFlags;
    }
    if (blocked) {
        {
            u32 mask = ~0x20;
            mask &= actor->base.motionFlags;
            actor->base.motionFlags = mask;
        }
        return 0;
    }
    if (actor->base.hitReactionFlags & 0xC0000) {
        void *control = actor->base.onReaction;
        u32 kind = 0x12;
        u8 zero8;
        actor->base.field2B2 = 1;
        actor->base.stepDone = blocked;
        actor->base.controlKind = kind;
        actor->base.target = control;
        zero8 = 0;
        actor->base.field2C8 = blocked;
        {
            u32 *flags = &actor->base.pendingControlFlags;
            s32 mask = -2;
            *flags &= mask;
        }
        actor->base.pendingControl = blocked;
        {
            u32 state = 3;
            actor->base.state2A8 = state;
        }
        actor->base.phase2A9 = zero8;
        actor->base.behavior2AA = zero8;
        actor->base.counter2AC = blocked;
        actor->base.field2B1 = 1;
        {
            u32 *flags = &actor->base.statusFlags;
            u32 mask = ~0x1000000;
            *flags &= mask;
        }
        return 1;
    }
    {
        u32 mask = ~0x20;
        actor->base.motionFlags &= mask;
    }
    if (actor->base.motionFlags & 0x100)
        return 0;
    if (actor->base.hitLockTimer)
        return 0;
    if (Test_SelectHitRecovery(context, 0x1D))
        return Vorn_BeginContactAnimation((void *)context);
    if (actor->group.hitReactionLatch == 0 && (actor->base.field2E6 & 8)) {
        actor->base.hitLockTimer = 45;
        actor->group.hitReactionLatch = 65;
        SceneActorAnimation_SelectDirectionalEntry(actor, 3);
    }
    if (!actor->base.tracking.target)
        return 0;
    {
        u32 foreign = context->foreignPosition;
        if (foreign == 0) {
            context->pursuitCooldown = foreign;
            Vorn_BeginPursuitRoute(actor, 1);
        } else {
            Vorn_SelectRetreatRoute(actor, 1);
        }
        return 1;
    }
}

void Vorn_ResumeRouteAfterReaction(void *);

void SceneActor_TickRandomReaction(struct ChaseActor *p);

void Vorn_TryUphillRoute(struct ChaseActor *actor);

void SceneActorPhase_TryBeginSpecialMotion(struct ChaseActor *p);

void SceneActor_TrySoloAction(struct Vorn *p);

s32 Vorn_UpdateBehavior(void *p)
{
    Vorn_ResumeRouteAfterReaction(p);
    SceneActor_TickRandomReaction(p);
    Vorn_TryUphillRoute(p);
    SceneActorPhase_TryBeginSpecialMotion(p);
    SceneActor_TrySoloAction(p);
    return 1;
}

void Vorn_UpdateRecoveryCooldown(struct PUpdateRecoveryCooldown *p);

u32 Vorn_UpdateActionTimers(u8 *p)
{
    u8 *q = p;
    u16 *r;
    Vorn_UpdateRecoveryCooldown(p);
    switch (p[0x2B5]) {
        case 12:
        case 13:
        case 34:
        case 35:
        case 37:
        case 41:
        case 42: {
            u32 m = 4;
            u32 *x = (u32 *)(q + 0x2D0);
            *x = *x | m;
        } break;
    }
    r = (u16 *)(p + 0x69E);
    if (*r != 0)
        *r = *r - 1;
    r = (u16 *)(p + 0x6AC);
    if (*r != 0)
        *r = *r - 1;
    r = (u16 *)(p + 0x6D0);
    if (*r != 0)
        *r = *r - 1;
    return 1;
}

void SceneActorRegistry_SendToCategory(s32, s16 *);

void Vorn_SendMessage(struct ChaseActor *actor)
{
    struct Vorn *ctx = (struct Vorn *)actor->base.context;

    if (ctx->field6C1 != 0) {
        s16 message[2];

        message[0] = 7;
        message[1] = actor->base.body.id;
        SceneActorRegistry_SendToCategory(0x19, message);
        ctx->field6C1 = 0;
    }
}

struct QBroadcastPosition {
    u8 f[0x6c1];
    u8 v;
};

struct PBroadcastPosition {
    u8 f[0x50];
    s32 x;
    u8 g[0x3d0 - 0x54];
    struct QBroadcastPosition *q;
};

void Vorn_BroadcastPosition(struct PBroadcastPosition *p)
{
    struct QBroadcastPosition *q = p->q;
    s16 s[2];
    s[0] = 6;
    s[1] = p->x;
    SceneActorRegistry_SendToCategory(0x19, s);
    q->v = 1;
}

struct VornMessageActor {
    u8 unknown0[0x50];
    u32 identity;
    u8 unknown54[0x37C];
    u8 *context;
};

// confidence=low: category messages six/seven toggle the foreign-position notification latch.

void Vorn_LatchForeignPosition(struct RoomActor *actor, const u16 *message)
{
    if (!SceneActorPlacement_IsUnavailable(actor)) {
        struct VornMessageActor *view = (void *)actor;
        if (message[1] != view->identity)
            view->context[0x6C0] = 1;
    }
}

// confidence=low: category messages six/seven toggle the foreign-position notification latch.

void Vorn_ClearForeignPosition(struct RoomActor *actor, const u16 *message)
{
    if (!SceneActorPlacement_IsUnavailable(actor)) {
        struct VornMessageActor *view = (void *)actor;
        if (message[1] != view->identity)
            view->context[0x6C0] = 0;
    }
}

u32 Trigger_ConsumePending(void *receiver, u32 count);

struct SRestoreControlAfterItemLaunch {
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
    u8 pad6a[0x22C - 0x220];
    void (*next)(void);
    u8 pad6[0x2A8 - 0x230];
    u8 a;
    u8 b;
    u8 c;
    u8 pad7;
    u32 count;
    u8 done;
    u8 queued;
    u8 taken;
    u8 pad8[2];
    u8 code;
    u8 pad9[0x2C8 - 0x2B6];
    u16 timer;
    u8 pad10[0x2D0 - 0x2CA];
    u32 flags;
    u16 hflags;
    u8 pad11[0x3D0 - 0x2D6];
    struct SRestoreControlAfterItemLaunch *other;
    u32 at3D4;
};

inline static u8 TakeFlag(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}

s32 SceneActor_TryLaunchItem13(struct SceneActorItemLaunch *p);

// confidence=low: launches the context item, consumes the trigger and restores saved control.

void Vorn_RestoreControlAfterItemLaunch(struct SRestoreControlAfterItemLaunch *s)
{
    struct SRestoreControlAfterItemLaunch *context = s->other;
    if (TakeFlag(&s->queued)) {
        void (*fn)(void);
        u32 zero;
        u8 one;
        u8 zero8;
        u32 *flags;
        u32 two;
        SceneActor_TryLaunchItem13(context);
        ((struct Vorn *)context)->response = 0xFFFF;
        Trigger_ConsumePending(((u8 *)s) + 0xC, 1);
        fn = s->next;
        zero = 0;
        {
            u8 *taken = &s->taken;
            one = 1;
            *taken = one;
        }
        s->done = zero;
        s->code = zero;
        s->callback = fn;
        zero8 = 0;
        s->timer = zero;
        flags = &s->flags114;
        {
            s32 mask = -2;
            *flags &= mask;
        }
        s->at104 = zero;
        two = 2;
        s->a = two;
        s->b = zero8;
        s->c = two;
        s->count = zero;
        s->queued = one;
    } else {
        ++s->count;
    }
}

extern const u32 gUnk_08606010[];

void Vorn_InitActionTable(u8 *p)
{
    *(const u32 **)(p + 0x29c) = gUnk_08606010;
}

s32 Vorn_UpdateBehavior(void *);

u32 Vorn_UpdateActionTimers(u8 *);

void SceneActorControl_DispatchControl(void *);

struct SOnUpdate {
    u8 filler1[0x27c];
    u32 *tbl;
    u8 filler2[0x2a8 - 0x280];
    u8 idx;
    u8 filler3[0x2b8 - 0x2a9];
    u16 h2b8;
    u8 filler4[0x2d4 - 0x2ba];
    u16 flags;
};

s32 Vorn_OnUpdate(struct SOnUpdate *s)
{
    u32 m;
    Vorn_UpdateBehavior(s);
    m = 0x2000;
    if ((s->flags & m) || s->h2b8 == 0) {
        Callback_InvokeActor(s, s->tbl[s->idx]);
        {
            u8 *c = (u8 *)s + 0xDC;
            u32 mask = ~0x20;
            c[0xF] &= mask;
        }
        SceneActorControl_DispatchControl(s);
    }
    Vorn_UpdateActionTimers((u8 *)s);
    return 1;
}

void SceneActor_UpdateBehaviorCallbacks(void);

u32 Vorn_OnRender(void)
{
    SceneActor_UpdateBehaviorCallbacks();
    return 0;
}

void SceneActorEffects_ReleaseResources(void);

u32 Vorn_OnDestroy(u8 *unused)
{
    SceneActorEffects_ReleaseResources();
    return 0;
}

void *Heap_Alloc(s32 size);

void Memory_Clear(void *dst, s32 bytes);

void SceneActorRegistry_SetObjectContext(void *, void *, s32, s32);

void SceneActorReaction_SetMotionMode(void);

u32 SceneActorReaction_OnEnemyDefault(void);

void SceneActorReaction_ResolvePlacementDamage(void);

void SceneActorScript_OnCommand(void *actor, const u16 *command);

u32 Route_GetCurrentPointFacing(void *);

s32 TargetRegistry_RegisterActor(void *, void *, void *);

void AnimData_SelectPaletteBank(void *, s32);

void SceneActorPlacement_HideActor(void *, s32);

void SceneActorBehavior_InstallCallbackTables(void *);

void SceneActorTerrain_ReadScriptParameters(void *);

u8 SceneActorRegistry_LinkActor(void *);

void SceneActorBehavior_InitializeShadow(void *);

u8 Vorn_UpdateContactResponse(struct ChaseActor *actor);

void SceneActorPlacement_ResetStateThree(void);

extern const u8 gUnk_08606038[];

static inline void VornInit_BeginBehavior(struct ChaseActor *actor, u32 behavior)
{
    MonsterInit_SetState(actor, 0, 0, behavior);
}

static inline void VornInit_ResetFlags(void *object)
{
    struct Vorn *actor = object;
    actor->flags6D4 = 0;
}

static inline void VornInit_InitializeBody(void *object, u32 *fill)
{
    struct ChaseActor *arg0 = object;
    struct SceneActorChaseGroup *group = &arg0->group;
    struct SceneActorBody *body = &arg0->base.body;
    struct ContactPosition origin;
    s32 mode, secondLimit, firstLimit, gateDuration, identity;
    struct SceneActorRegistryRoute *rt;
    arg0->base.behaviorFlags = 0;
    arg0->base.statusFlags = 0;
    arg0->base.enabled = 0;
    arg0->base.field2E8 = 8;
    arg0->base.placementFlags = MonsterInit_ReadKeywordHalfword('M', 0);
    arg0->base.field2C2 = 0x40;
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
    MonsterInit_ReadTargeting(&arg0->base.tracking);
    if (Script_GetKeywordValue(0x62, 0))
        MonsterInit_SetStatusFlags(arg0, 0x200);
    arg0->base.script1F0 = MonsterInit_ReadKeywordHalfword(0x46, 0);
    arg0->base.scriptId = MonsterInit_ReadKeywordHalfword(0x52, 0);

    if (Script_SeekToKeyword(0x41)) {
        MonsterInit_SetInitialHealth(arg0, MonsterInit_ReadHalfword(0));
        group->halfHealth = MonsterInit_Half(MonsterInit_GetInitialHealth(arg0));
        arg0->base.field9E = MonsterInit_ReadByte(0);
        arg0->base.defense = MonsterInit_ReadByte(0);
        arg0->base.contactReward = MonsterInit_ReadByte(0);
        arg0->base.fieldA2 = MonsterInit_ReadHalfword(0);
        arg0->base.fieldA4 = MonsterInit_ReadHalfword(0);
    }

    MonsterInit_InitializeHealth(arg0, MonsterInit_GetInitialHealth(arg0), firstLimit, secondLimit);

    if (Script_SeekToKeyword(0x58)) {
        arg0->base.experience = MonsterInit_ReadWord(0);
        arg0->base.fieldA1 = MonsterInit_ReadByte(0);
    }
    arg0->base.hudKind = Script_GetKeywordValue(0x4B, 2);
    identity = (s16)Script_GetKeywordValue(0x56, -1);
    if (identity < 0)
        identity = 0xFFFF;
    arg0->base.hudLabel = identity;
    if (Script_GetKeywordValue(0x48, 0))
        MonsterInit_SetStatusFlags(arg0, 0x800);
    if (Script_GetKeywordValue(0x4C, 0))
        MonsterInit_SetStatusFlags(arg0, 0x8000);

    SceneBody_Init(body, mode, &origin, 0, 4, arg0);
    rt = &group->homeRoute;
    if (MonsterInit_ReadRoute(arg0, group, rt)) {
        MonsterInit_SnapToRoute(&arg0->base.body.position, rt);
        MonsterInit_DrawablePosition(arg0);
        arg0->base.body.direction = Route_GetCurrentPointFacing(rt);
    }

    if (MonsterInit_TestMotionFlags(arg0, 0x200))
        TargetRegistry_RegisterActor(&group->common.placement, body,
                                     SceneActorReaction_SetMotionMode);
    SceneBody_SetSprite(body, MonsterInit_GetDrawableKind(arg0) == 0 ? arg0->base.drawable : 0);
    if (MonsterInit_GetCategory(arg0) == 0x15)
        SceneBody_SetMovementProbe(body, arg0->base.movementProbe, 0x3C, 0x3C);
    else
        SceneBody_SetMovementProbe(body, arg0->base.movementProbe, 0x40, 0x40);

    group->common.bodyId = arg0->base.body.id;
    group->common.bodyPosition = &arg0->base.body.position;
}

static inline void VornInit_ReadPalette(struct ChaseActor *actor, u32 base, u32 alternate)
{
    s32 palette;
    actor->base.paletteOffset = Script_GetKeywordValue(0x43, 0);
    actor->base.basePaletteBank = base;
    actor->base.paletteAlternateBank = alternate;
    actor->base.alternateOffset = MonsterInit_GetPaletteOffset(actor) + base + 1;
    palette = MonsterInit_GetPaletteBase(actor) + MonsterInit_GetPaletteOffset(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0)
        AnimData_SelectPaletteBank(actor->base.drawable + 0x2C, palette);
    else
        *(u8 **)(actor->base.drawable + 0x68) = gPaletteBankData + (palette << 5);
    if (MonsterInit_GetDrawableKind(actor) == 1)
        MonsterInit_SetWidgetScriptPalette((void *)(actor->base.drawable + 0x20),
                                           SceneActorScript_GetManagerField62());
}

void Vorn_InitActorProfile(struct PInitActorProfile *p);

void SceneActor_InitContextColliders(struct ChaseActor *p);

s32 Vorn_OnUpdate(struct SOnUpdate *s);

u16 Vorn_GetAnimationEvent(u8 *p);

void Vorn_UpdateIdleFacing(void *);

void Vorn_ResetStateEntry(void *);

void Vorn_BeginIdle(void *);

void Vorn_UpdateStaggerRecovery(void *);

void Vorn_UpdateDefeat(void *);

void Vorn_UpdateHiddenRecovery(void *);

void SceneActorTarget_ResolvePendingObject(void);

void Vorn_OnReactionNoOp(void);

void SceneActorReaction_ResumeSavedTarget(void);

void SceneActor_ResetContextAnimation(struct Vorn *p);

void Vorn_SelectReactionControl(struct ChaseActor *a);

u8 Vorn_SelectHitRecovery(struct ChaseActor *actor);

void Vorn_OnAnimationCleanup(void);

void Vorn_BeginTargetPursuit(struct Vorn *actor);

u32 Vorn_OnRender(void);

u32 Vorn_OnDestroy(u8 *);

static inline void VornInit_SetReactionTable(struct ChaseActor *actor, const void *table)
{
    MonsterInit_WriteDataPointer(&actor->base.commandTable.opaque, table);
}

static inline void VornInit_EnableInitialFlags(struct ChaseActor *actor)
{
    MonsterInit_SetTargetingFlags(&actor->base.tracking, 1);
    MonsterInit_SetBehaviorFlags(actor, 0x1600000);
    MonsterInit_SetStatusFlags(actor, 0x8000000);
}

static inline void VornInit_InstallCompletionCallbacks(struct ChaseActor *arg0)
{
    arg0->base.onMessage = SceneActorScript_OnCommand;
    arg0->base.update = (SceneActorResultCallback)Vorn_OnRender;
    arg0->base.destroy = (SceneActorResultCallback)Vorn_OnDestroy;
}

void Vorn_ParseField624(struct ChaseActor *actor);

void Vorn_InitScriptCallback(struct Vorn *p);

void Vorn_InitActionCooldown(u8 *p);

void Vorn_InitTimedAction(struct P *p);

void Vorn_ParseField69A(struct Vorn *ctx);

void Vorn_ResetRecoveryMode(u8 *p);

void Vorn_InitMotionLimits(struct PInitMotionLimits *p);

void Vorn_ParseField688(struct Vorn *ctx);

void Vorn_InitAttackActionTable(u8 *p);

void Vorn_InitSpecialActionTable(u8 *p);

void Vorn_InitReactionActionTable(u8 *p);

void Vorn_InitActionTable(u8 *p);

void ActorShadow_Disable(void *);

s32 Vorn_TryRestorePatrolRoute(void *);

void Vorn_UpdateKnockbackRecovery(void);

void SceneActorReaction_ResumeOrRunScript(void);

void Vorn_ResumeReactionRoute(void);

s32 Vorn_Init(void *object)
{
    struct Vorn *vorn = object;
    struct ChaseActor *actor = object;
    struct SceneActorChaseGroup *group = &vorn->chase.group;
    struct SceneActorBody *body = &vorn->chase.base.body;
    u32 zero;
    struct ContactPosition tmp;
    u32 flag;
    s32 mode;

    VornInit_ResetFlags(vorn);
    SceneActorRegistry_SetObjectContext(actor, actor, 1, 1);
    MonsterInit_SetCategory(actor, MONSTER_CATEGORY_VORN);
    Vorn_InitActorProfile(actor);
    Vorn_ParseField624(actor);
    Vorn_InitScriptCallback(actor);
    Vorn_InitActionCooldown(actor);
    Vorn_InitTimedAction(actor);
    Vorn_ParseField69A(actor);
    Vorn_ResetRecoveryMode(actor);
    Vorn_InitMotionLimits(actor);
    Vorn_ParseField688(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->base.drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->base.drawable, 0x5C);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_InitializeSprite(heap, 0x3DB6, 0);
        if (MonsterInit_GetCategory(actor) == 0x11 || MonsterInit_GetCategory(actor) == 0x17)
            heap[7] = 1;
        actor->base.body.direction = 0;
    } else {
        u8 *heap;
        actor->base.drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->base.drawable, 0x80);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_ClearPosition(&zero, &tmp);
        MonsterInit_InitializeWidget(heap, 0xCB05, 0x3DB6, &tmp);
        actor->base.animationIndex = 0xFF;
    }
    VornInit_InitializeBody(vorn, &zero);
    VornInit_ReadPalette(actor, 0x31B, 0x192);
    SceneActor_InitContextColliders(actor);
    MonsterInit_InstallSequenceCallbacks(
        actor, Vorn_OnUpdate, Vorn_GetAnimationEvent, Vorn_UpdateIdleFacing, Vorn_ResetStateEntry,
        Vorn_BeginIdle, Vorn_UpdateHiddenRecovery, Vorn_UpdateStaggerRecovery, Vorn_UpdateDefeat,
        SceneActorReaction_OnEnemyDefault, Vorn_UpdateContactResponse,
        SceneActorTarget_ResolvePendingObject, SceneActorReaction_ResolvePlacementDamage,
        SceneActorPlacement_ResetStateThree);
    MonsterInit_InitializeGroupWithBounds(group, &vorn->routeProbe, &vorn->destination,
                                          Vorn_OnReactionNoOp, SceneActorReaction_ResumeSavedTarget,
                                          Vorn_TryRestorePatrolRoute, 0, 1, body);
    SceneActorMovement_ResolveRoutePosition(vorn);
    MonsterInit_SetRouteReset(actor, SceneActor_ResetContextAnimation);
    MonsterInit_SetReactionGate(actor, Vorn_SelectReactionControl);
    MonsterInit_SetContactCallback(actor, Vorn_SelectHitRecovery);
    VornInit_SetReactionTable(actor, gUnk_08606038);
    group->contactControl = Vorn_UpdateKnockbackRecovery;
    group->animationEndControl = SceneActorReaction_ResumeOrRunScript;
    group->responseCallback = Vorn_OnAnimationCleanup;
    group->routeRestoreCallback = Vorn_BeginTargetPursuit;
    group->releaseControl = Vorn_ResumeReactionRoute;
    actor->base.field2E8 = 3;
    VornInit_EnableInitialFlags(actor);
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
        MonsterInit_MaskColliderFlags(&actor->base.colliderA, ~0x100);
    if (Script_GetKeywordValue(0x53, 0) != 0 || !MonsterInit_IsInCurrentRegion(actor))
        SceneActorPlacement_HideActor(actor, 0);
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    if (MonsterInit_GetDrawableKind(actor) == 0)
        DisplaySequence_Step(actor->base.drawable);
    else
        Anim_Step(actor->base.drawable + 0x20, actor->base.drawable);
    MonsterInit_ClearReceiver(actor);
    SceneActorRegistry_RegisterCategoryReceiver(actor);
    SceneActorBehavior_InstallCallbackTables(actor);
    Vorn_InitAttackActionTable(actor);
    Vorn_InitSpecialActionTable(actor);
    Vorn_InitReactionActionTable(actor);
    Vorn_InitActionTable(actor);
    MonsterInit_SaveControl(MonsterInit_SavedControlOf(actor), actor);

    flag = vorn->field68C;
    if (flag == 1) {
        if (MonsterInit_GetDrawableKind(actor) == 0)
            *(u32 *)actor->base.drawable |= flag;
        else {
            u8 *widget = actor->base.drawable + 0x20;
            *(u32 *)(widget + 8) |= flag;
        }
        ActorShadow_Disable(actor->base.shadow);
        MonsterInit_SetState(actor, 3, 0, 0);
        MonsterInit_SetControl(actor, actor->base.onHidden, 0xC);
    } else if ((mode = Script_GetKeywordValue(0x54, 0)) != 0) {
        if (mode == 1)
            vorn->axis = 0;
        else
            vorn->axis = 1;
        VornInit_BeginBehavior(actor, 0x12);
        MonsterInit_SetControl(actor, actor->base.onStanding, 0);
        MonsterInit_SetMotionControl(actor, SceneActorReaction_BeginSideStep, 0x26);
    }
    VornInit_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}

s32 Vorn_Init(void *object);

void Heap_Free(void *);

void Vorn_Create(void)
{
    u8 *p = Heap_Alloc(0x720);
    if (p != 0) {
        Memory_Clear(p, 0x720);
        if (Vorn_Init(p) < 0) {
            Vorn_OnDestroy(p);
            Heap_Free(p);
        }
    }
}

static inline u8 Test_HasActiveResponse(struct Vorn *actor, u32 mask)
{
    if (actor->flags6D4 & mask)
        return 1;
    return 0;
}

// confidence=low: reports whether a response flag or nonnegative response distance is present.

u8 Vorn_HasActiveResponse(struct Vorn *context)
{
    if (!Test_HasActiveResponse(context, 1) && context->response < 0)
        return 0;
    return 1;
}
