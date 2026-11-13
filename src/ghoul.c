#include "ghoul.h"
#include "scene_actor_control_phase.h"
#include "engine.h"
#include "ram.h"
#include "ghoul_disc.h"
#include "scene_actor_terrain.h"
#include "scene_actor_chase_shared.h"
#include "scene_actor_chase_step.h"
#include "scene_actor_init_primitives.h"
#include "display_sequence.h"
#include "scene_actor_chase_reaction.h"
#include "global.h"
#include "scene_actor_restoration.h"
#include "scene_actor_delayed_response.h"
#include "scene_actor_reset.h"
#include "scene_actor_control_motion.h"
#include "scene_actor_animation.h"
#include "display_sequence_inline.h"
#include "scene_actor_route.h"
#include "engine_globals.h"
#include "scene_actor_chase_types.h"
#include "scene_actor_registry.h"
#include "scene_actor_placement.h"
#include "scene_actor_init_dispatch.h"
#include "scene_actor_init_group.h"
#include "render.h"
#include "scene_actor_init_graphics.h"
#include "scene_actor_init_body.h"

// confidence=low: animation-three state eases stored displacement into motion until animation completion.

void SparkParticle_Spawn(void *, u32, void *, u32, u32, u32, u32, u32);

static inline u32 Ghoul_MotionFlag(struct SceneActorRegistryResetView *a, u32 mask)
{
    return a->flags2D4 & mask;
}

static inline u32 Ghoul_RandomEffectVariant(void)
{
    u32 index = (gRandomIndex + 1) & 1023;
    u32 v;
    gRandomIndex = index;
    index *= 2;
    v = *(const u16 *)(index + (u32)RANDOM_TABLE_ADDR);
    v &= 3;
    return v + 1;
}

static inline s32 Ghoul_QuarterDisplacement(s32 value)
{
    s32 result;
    if (value >= 0)
        result = value >> 2;
    else
        result = -((-value) >> 2);
    return result;
}

static inline u8 Ghoul_DisplacementStopped(s32 x, s32 z)
{
    if (!x && !z)
        return 1;
    return 0;
}

void Ghoul_UpdateDisplacementRecovery(struct SceneActorRegistryResetView *actor, s32 frame)
{
    struct DisplaySequenceState *anim =
        &((struct SceneActorControlMotionDrawable *)actor->drawable)->sequence;
    struct ContactPackedVector range;
    s16 *x, *z;
    s32 dx, dz;
    if (SceneActorControl_TakeFlag(&actor->field2B2)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 3);
        Sound_PlaySong(0xD4);
        if (Ghoul_MotionFlag(actor, 8))
            Sound_PlaySong(0x12A);
        actor->field2B3 = 0;
    }
    {
        u32 mask = 0x2000;
        actor->flags2CC |= mask;
    }
    if (frame <= 17 && anim->index == 0 && anim->elapsed == (anim->duration != 0) &&
        anim->duration != 1)
        anim->elapsed = 0;
    if (Mod(frame, 12) == 0 && Ghoul_MotionFlag(actor, 8)) {
        range.x = 60;
        range.y = 60;
        range.z = 40;
        SparkParticle_Spawn(&actor->body.position, Ghoul_RandomEffectVariant(), &range,
                            actor->body.direction + 128, 10, 6, 2, 2);
    }
    {
        s32 current = actor->reactionStart.x;
        s16 *field = &actor->reactionEnd.x;
        s32 difference = current - *field;
        x = field;
        dx = Ghoul_QuarterDisplacement(difference);
    }
    {
        s32 current = actor->reactionStart.z;
        s16 *field = &actor->reactionEnd.z;
        s32 difference = current - *field;
        z = field;
        dz = Ghoul_QuarterDisplacement(difference);
    }
    if (dx > 200)
        dx = 200;
    else if (dx < -200)
        dx = -200;
    if (dz > 200)
        dz = 200;
    else if (dz < -200)
        dz = -200;
    actor->body.motion.x += dx;
    actor->body.motion.z += dz;
    *x += dx;
    *z += dz;
    if (Ghoul_DisplacementStopped(dx, dz) && actor->field2B3)
        actor->field2B0 = 1;
}

u8 SceneActorTerrain_CanTraverseCell(u32, u32, u32, u32);

void SunDiscPool_SpawnDisc(const struct ContactPosition *, const struct ContactPosition *, s32, s32,
                           u32, s32, u32, s32, s32);

void Ghoul_UpdateDiscThrow(struct SceneActorRegistryResetView *actor)
{
    struct Ghoul *context = actor->context;
    struct DisplaySequenceState *sequence =
        &((struct SceneActorControlMotionDrawable *)actor->drawable)->sequence;
    if (SceneActorControl_TakeFlag(&actor->field2B2))
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 5);
    if (sequence->index == 5 && sequence->elapsed == 0) {
        s32 direction = (((actor->body.direction + 32) & 255) >> 6) << 6;
        s32 delta = actor->body.direction - direction;
        struct ContactPosition origin;
        if (delta > 128)
            delta -= 256;
        else if (delta < -128)
            delta += 256;
        if (Ghoul_Abs(delta) < 32) {
            context->discTargets[1] = actor->tracking.target->position;
        } else {
            s32 cell = Ghoul_ForwardCell(actor, 1);
            SceneActorTerrain_GetCenter(cell, &context->discTargets[1]);
            {
                switch (delta = Ghoul_QuarterDirection(actor->body.direction + 32) & 3) {
                    case 0:
                        context->discTargets[1].x += 127;
                        context->discTargets[1].z = actor->body.position.z;
                        break;
                    case 1:
                        context->discTargets[1].x = actor->body.position.x;
                        context->discTargets[1].z += 127;
                        break;
                    case 2:
                        context->discTargets[1].x -= 127;
                        context->discTargets[1].z = actor->body.position.z;
                        break;
                    case 3:
                        context->discTargets[1].x = actor->body.position.x;
                        context->discTargets[1].z -= 127;
                        break;
                }
                {
                    struct ContactPosition *position = &context->discTargets[1];
                    context->discTargets[1].y =
                        SceneActorChase_HeightAt(MonsterInit_CellIndex(position), position);
                }
            }
        }
        origin = actor->body.position;
        origin.y += 220;
        {
            u32 first = actor->attacks.first[1];
            u32 second = actor->attacks.second[1];
            u32 flags = actor->attacks.flags[1];
            u32 parameter = actor->attacks.parameter[1];
            SunDiscPool_SpawnDisc(&origin, &context->discTargets[1], first, second, flags,
                                  parameter, context->discKind, 30, -20);
        }
        Sound_PlaySong(0xD6);
    }
    if (actor->field2B3)
        actor->field2B0 = 1;
}

void SceneActorAttachment_ReplaceResponseSlot(u8 *, u32, u32, u32);

void SceneActorBehavior_CallActionScript(u8 *);

void Ghoul_UpdateScriptedAction(struct SceneActorRegistryResetView *obj)
{
    struct Ghoul *control = obj->context;
    if (SceneActorControl_TakeFlag(&obj->field2B1)) {
        u16 *h;
        u16 value;
        SceneActorChase_SetControl(obj, SceneActorReaction_PlayAnimationBUntilComplete, 10);
        {
            u32 byte = obj->tracking.heading;
            obj->body.direction = byte;
        }
        SceneActorAttachment_ReplaceResponseSlot((u8 *)obj, 0, 0, obj->category);
        SceneActorBehavior_CallActionScript((u8 *)obj);
        h = (void *)&obj->group.common.gate.flags;
        {
            u32 flag = 4;
            flag |= *h;
            *h = flag;
        }
        {
            u32 flag = 0x80;
            u16 *flags = &obj->flags2D4;
            flag |= *flags;
            *flags = flag;
        }
        value = obj->group.common.gate.duration;
        obj->group.common.gate.counter = value;
    }
    if (obj->field2B0) {
        Ghoul_EnterIdleControl(obj);
        {
            u32 flag = 4;
            u16 *flags = &control->discFlags;
            flag |= *flags;
            *flags = flag;
        }
    } else {
        ++obj->counter2AC;
    }
}

void SceneActorControl_OnIdle(u8 *);

static inline u32 Ghoul_RouteFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2CC & mask;
}

/* Queue pursuit, a home route, or the short waiting response after the delay. */

void Ghoul_ResumeDelayedChase(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    u32 zero;
    SceneActorControl_TakeStateEntry(actor);
    if ((u8)Callback_InvokeActor(actor, actor->contactQuery.opaque))
        return;
    zero = (u8)sub_0812F3CC(actor);
    if (zero)
        return;
    if ((s32)actor->counter2AC <= 60)
        goto waiting;
    SceneActorAttachment_ReplaceResponseSlot((u8 *)actor, 0, 2, actor->category);
    SceneActorChase_SetState(actor, 3, 1, zero);
    SceneActorChase_SetControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
    if (SceneActorChase_MotionFlags(actor, 0x80)) {
        SceneActorDelay_Prepare((void *)&group->saved, group->common.gate.control, 12, 30);
        SceneActorChase_Arrive(actor);
    } else if (Ghoul_RouteFlags(actor, 0x10)) {
        SceneActorDelay_Prepare((void *)&group->saved, actor->entryTarget228, 7, 30);
        SceneActorChase_SnapToRoute(group->destination, &group->homeRoute);
        if (!SceneActorChase_TryRoute(actor, group->route, &actor->body.position,
                                      group->destination)) {
            SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0);
            SceneActorChase_SetState(actor, 3, 0, 0);
        } else
            group->destination->pad = 1;
    } else {
        SceneActorDelay_Prepare((void *)&group->saved, actor->entryTarget228, 2, 30);
        actor->field2C2 = 64;
    }
    return;
waiting:
    actor->counter2AC++;
}

extern void *gUnk_08605C94[];

void Ghoul_SetBehaviorHandlers(struct SceneActorRegistryResetView *actor)
{
    actor->alternateBehaviorCallbacks = gUnk_08605C94;
}

u8 SceneActorReaction_IsRangeReady(struct SceneActorRegistryResetView *);

void Bat_UpdateProbeReaction(void);

void Anim_SetDurationScale(void *, u32);

void Anim_SetFrameDurationScale(void *, u32);

u32 NavMesh_ConsumeProbeEvent(void *);

u8 SceneActorBehavior_RequestDescriptorTurn(void *, void *, u32, u32, s32);

u8 SceneActorBehavior_AccelerateTowardDescriptor(void *, void *, u32, u32, s32);

static inline void Ghoul_ResetProbeState(struct SceneActorRegistryResetView *actor, u32 state,
                                         u32 phase, u32 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

static inline void Ghoul_EnterRetreatBehavior(struct SceneActorRegistryResetView *actor)
{
    u32 behavior = 16;
    actor->state2A8 = 0;
    actor->phase2A9 = 0;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

static inline void Ghoul_ClearPendingRoute(u16 *flags)
{
    u32 mask = ~1;
    *flags &= mask;
}

void Ghoul_UpdateProbeApproach(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    struct Ghoul *control = actor->context;
    s32 speed;
    u32 event;
    u16 *scale;

    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        SceneActorChase_SetControl(actor, Bat_UpdateProbeReaction, 0x24);
    }
    if (!control->discCooldown) {
        if ((u8)Callback_InvokeActor(actor, actor->contactQuery.opaque)) {
            Ghoul_ClearPendingRoute(&control->discFlags);
            return;
        }
        if (sub_0812F3CC(actor)) {
            Ghoul_ClearPendingRoute(&control->discFlags);
            return;
        }
    }
    speed = Callback_InvokeActor(*(&actor), actor->animationQuery.opaque);
    actor->field2C2 = Div(0x40, 6);
    {
        u32 mask = 0xA02F;
        if (actor->flags2D4 & mask) {
            u16 *scaleField = &actor->field2C2;
            *scaleField = 0x40;
            scale = scaleField;
        } else {
            u16 *scaleField = &actor->field2C2;
            u16 durationScale = *scaleField;
            scale = scaleField;
            if (durationScale == 0)
                *scale = 0x40;
        }
    }
    if (actor->drawableKind == 0)
        Anim_SetDurationScale(
            &((struct SceneActorControlMotionDrawable *)actor->drawable)->sequence, *scale);
    else
        Anim_SetFrameDurationScale(
            &((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile, *scale);
    event = NavMesh_ConsumeProbeEvent(group->route);
    if (event) {
        if (SceneActorBehavior_RequestDescriptorTurn(actor, group->route, 0x20, 0x20, speed)) {
            Ghoul_EnterRetreatBehavior(actor);
        }
    } else if (SceneActorBehavior_AccelerateTowardDescriptor(actor, group->route, 0x20, 0x20,
                                                             speed)) {
        Ghoul_ResetProbeState(actor, 3, 0, 0);
        {
            u16 bit = 2;
            u16 *flags = &control->discFlags;
            *flags |= bit;
        }
        return;
    }
    actor->counter2AC++;
}

// confidence=low: restarts saved control or advances behavior after callbacks complete.

static inline void Ghoul_EnterAlternateRouteBehavior(struct SceneActorRegistryResetView *actor)
{
    u32 behavior = 15;
    actor->state2A8 = 0;
    actor->phase2A9 = 0;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

void SceneActorControl_RestartOrAdvance(struct SceneActorRegistryResetView *actor)
{
    struct Ghoul *control = actor->context;
    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        void *target = actor->entryTarget224;
        SceneActorChase_SetControl(actor, target, 2);
    }
    if (!control->discCooldown) {
        if ((u8)Callback_InvokeActor(actor, actor->contactQuery.opaque)) {
            Ghoul_ClearPendingRoute(&control->discFlags);
            return;
        }
        if (sub_0812F3CC(actor)) {
            Ghoul_ClearPendingRoute(&control->discFlags);
            return;
        }
    }
    if (actor->field2B0)
        Ghoul_EnterAlternateRouteBehavior(actor);
    else
        ++actor->counter2AC;
}

static inline u8 Ghoul_TakeStateEntry(u8 *field)
{
    if (*field) {
        *field = 0;
        return 1;
    }
    return 0;
}

void Ghoul_WaitForRouteResult(struct SceneActorRegistryResetView *actor)
{
    u8 busy;
    u8 zero;
    if (Ghoul_TakeStateEntry(&actor->field2B1)) {
        void *control = actor->entryTarget224;
        SceneActorChase_SetControl(actor, control, 2);
    }
    busy = Callback_InvokeActor(actor, actor->contactQuery.opaque);
    if (!busy) {
        zero = sub_0812F3CC(actor);
        if (!zero) {
            if (actor->field2B0) {
                u32 state = 5;
                actor->state2A8 = zero;
                actor->phase2A9 = zero;
                actor->behavior2AA = state;
                actor->counter2AC = zero;
                actor->field2B1 = 1;
                SceneActorChase_SetControl(actor, actor->savedTarget, 0);
            } else
                actor->counter2AC++;
        }
    }
}

extern void *gUnk_08605C9C[];

void Ghoul_SetActionHandlers(struct SceneActorRegistryResetView *actor)
{
    actor->callbacks284 = gUnk_08605C9C;
}

void Ghoul_UpdateAimedDiscThrow(struct SceneActorRegistryResetView *);

static inline void Ghoul_SetMotionFlags(struct SceneActorRegistryResetView *actor, u16 bits)
{
    SceneActorChase_SetFlagComponent((struct SceneActorChaseFlags16 *)&actor->flags2D4, bits);
}

/* Start a disc attack, then restore pursuit after its configured recovery delay. */

void Ghoul_UpdateAimedDiscControl(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    struct Ghoul *context = actor->context;
    if (SceneActorControl_TakeStateEntry(actor)) {
        Ghoul_SetMotionFlags(actor, 0x100);
        SceneActorChase_SetControl(actor, Ghoul_UpdateAimedDiscThrow, 0x23);
        context->discCooldown = context->throwCount;
    }
    if (actor->field2B0) {
        SceneActorChase_ClearMotionFlagComponent(actor, 0x100);
        SceneActorChase_ClearWordFlags(&actor->flags2D0, 4);
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        SceneActorDelay_Prepare((void *)&group->saved, group->common.gate.control, 12,
                                context->recoveryDelay);
        SceneActorChase_SetControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
        SceneActorChase_SetState(actor, 3, 0, 0);
        if (!SceneActorChase_TryRoute(actor, group->route, &actor->body.position,
                                      &context->discTargets[0]))
            Callback_InvokeActor(actor, group->fallback);
    } else {
        actor->counter2AC++;
    }
}

extern void *gUnk_08605CE4[];

void Ghoul_SetSpecialHandlers(struct SceneActorRegistryResetView *actor)
{
    actor->behaviorCallbacks = gUnk_08605CE4;
}

void Ghoul_EnterIdleControl(struct SceneActorRegistryResetView *actor)
{
    SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 16);
    SceneActorChase_SetState(actor, 0, 2, 0);
}

/* SceneActorControl_RefreshMotionRoute_0808A90C: same code as SceneActorControl_RefreshMotionRoute at another address (ported by relatives.py; provisional name) */

static inline u32 Ghoul_TestMotionFlags(u16 *flags, u32 mask)
{
    return *flags & mask;
}

static inline u8 Ghoul_HasActiveMotion(struct SceneActorReactionMotion *motion)
{
    u32 mask = 8;
    if (motion->flags & mask)
        return 1;
    return 0;
}

static inline void Ghoul_RestoreWaitingState(struct SceneActorRegistryResetView *actor, u32 zero)
{
    u32 phase = 1;
    actor->state2A8 = zero;
    actor->phase2A9 = phase;
    actor->behavior2AA = zero;
    actor->counter2AC = zero;
    actor->field2B1 = phase;
}

static inline void Ghoul_RestoreIdleController(struct SceneActorRegistryResetView *actor,
                                               void *control, u32 kind, u32 zero)
{
    actor->field2B2 = 1;
    actor->field2B0 = zero;
    actor->field2B5 = kind;
    actor->target = control;
    actor->field2C8 = zero;
    {
        u32 *flags = &actor->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = zero;
}

void SceneActorControl_RefreshMotionRoute_0808A90C(struct SceneActorRegistryResetView *actor)
{
    u32 active = Ghoul_HasActiveMotion((void *)&actor->tracking.target);
    if (active) {
        {
            u16 *flags = &actor->group.common.gate.flags;
            u32 mask = 4;
            u32 value = *flags;
            mask |= value;
            *flags = mask;
        }
        {
            u32 mask = 128;
            u16 *flags = &actor->flags2D4;
            u32 value = *flags;
            mask |= value;
            *flags = mask;
        }
        actor->group.common.gate.counter = actor->group.common.gate.duration;
        if (Ghoul_TestMotionFlags(&actor->flags2D4, 256))
            return;
        Ghoul_EnterIdleControl(actor);
    } else {
        Ghoul_RestoreWaitingState(actor, active);
        Ghoul_RestoreIdleController(actor, (void *)SceneActorControl_OnIdle, 16, active);
    }
    {
        struct SceneActorReactionMotion *motion = (void *)&actor->tracking.target;
        u32 mask = 8;
        u32 value = motion->flags;
        mask |= value;
        motion->flags = mask;
    }
}

// confidence=low: sets actor timed route-return command and validates the current contact target.

static inline void Ghoul_PrepareReturnAction(struct SceneActorDelayedResponse *input, u32 time,
                                             void *callback, u32 kind)
{
    struct SceneActorDelayedResponse *action = input;
    void *target = callback;
    u32 behavior = kind;
    u32 duration = time;
    action->delay = duration;
    action->control = target;
    action->state = 0;
    action->phase = 0;
    action->behavior = behavior;
}

static inline void Ghoul_PrepareSavedReturnAction(struct SceneActorRegistryResetView *actor,
                                                  u32 time, u32 kind)
{
    struct SceneActorDelayedResponse *action = (void *)&actor->group.saved;
    void *target;
    u32 behavior;
    u32 duration;
    target = actor->entryTarget228;
    behavior = kind;
    duration = time;
    action->delay = duration;
    action->control = target;
    action->state = 0;
    action->phase = 0;
    action->behavior = behavior;
}

static inline void Ghoul_SetRouteFlag(struct SceneActorChaseGroup *group)
{
    u16 *flags = (void *)&group->common.gate.flags;
    u16 mask = 4;
    *flags |= mask;
}

static inline u32 Ghoul_MotionFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2D4 & mask;
}

void Ghoul_ResumeContactRoute(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorChaseGroup *group;
    struct ContactPosition *destination;

    u16 blocked = Ghoul_MotionFlags(actor, 0x100);
    if (blocked)
        return;
    group = &actor->group;
    SceneActorChase_SetState(actor, 3, 1, blocked);
    SceneActorChase_SetControl(actor, SceneActorReaction_RestoreAfterDelay, 14);

    if (Ghoul_MotionFlags(actor, 0x80)) {
        actor->field2C2 = 32;
        Ghoul_PrepareReturnAction((void *)&group->saved, 30, group->common.gate.control, 12);
        Ghoul_SetRouteFlag(group);
        if (Ghoul_MotionFlags(actor, 0x80)) {
            u16 *timer = &group->common.gate.counter;
            *timer += 300;
            if (*timer > group->common.gate.duration)
                *timer = group->common.gate.duration;
        }
    } else {
        {
            u32 mask = 16;
            actor->flags2CC |= mask;
        }
        actor->field2C2 = 32;
        Ghoul_PrepareSavedReturnAction(actor, 60, 7);
    }
    destination = group->destination;
    SceneActorChase_SnapToRoute(destination, &group->homeRoute);
    (group->destination)->pad = 0;
    if (!SceneActorChase_TryRoute(
            actor, group->route, &actor->body.position,
            &((struct SceneActorChaseTarget *)actor->tracking.target)->position)) {
        Callback_InvokeActor(actor, group->fallback);
    }
    return;
}

void SceneActorCollision_InitBody(void *, struct GhoulCollisionVector *,
                                  struct GhoulCollisionVector *, void *, void *);

void SceneActorCollision_InitAttack(void *, struct GhoulCollisionVector *,
                                    struct GhoulCollisionVector *, void *, u32, void *);

void Ghoul_InitCollision(struct SceneActorRegistryResetView *actor)
{
    struct GhoulCollisionVector bounds, offset;
    bounds.xy.first = 0x40;
    bounds.xy.second = 0x80;
    bounds.zw.first = 0x40;
    *(u32 *)&offset.xy = 0x800000;
    *(u32 *)&offset.zw &= 0xFFFF0000;
    SceneActorCollision_InitBody(actor, &bounds, &offset, Ghoul_OnBodyCollision, actor);
    ((struct ContactPosition *)&bounds)->x = 0x40;
    ((struct ContactPosition *)&bounds)->y = 0x64;
    ((struct ContactPosition *)&bounds)->z = 0x40;
    ((struct ContactPosition *)&offset)->x = 0;
    ((struct ContactPosition *)&offset)->y = 0x80;
    ((struct ContactPosition *)&offset)->z = 0;
    SceneActorCollision_InitAttack(actor, &bounds, &offset, Ghoul_OnAttackCollision, 0, actor);
}

/* The command stepper uses the exact shared route-reset implementation. */

void SceneActorMovement_ResolveRoutePosition(void *);

// confidence=low: resets the ghoul route timers, positions, and response state before stepping animation.

extern const u8 gUnk_0824E15C[];

void SceneActorReaction_ResetResponseState(void *, const void *, u32);

void Ghoul_ResetRouteState(struct SceneActorRegistryResetView *actor)
{
    struct Ghoul *context = actor->context;
    context->unknown62A = 0;
    context->discFlags = 0;
    context->discCooldown = 0;
    context->routeDelay = 0;
    Memory_Clear(actor->group.destination, 8);
    Memory_Clear(&context->discTargets[1], 8);
    Memory_Clear(&context->discTargets[0], 8);
    SceneActorReaction_ResetResponseState(&actor->group.saved, gUnk_0824E15C, 0x92);
    SceneActorMovement_ResolveRoutePosition(actor);
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    {
        struct SceneActorControlMotionDrawable *drawable = actor->drawable;
        DisplaySequence_StepState(drawable, &drawable->sequence);
    }
}

void Ghoul_OnLifecycleEvent(void)
{
}

void Ghoul_BindProfile(struct SceneActorRegistryResetView *actor)
{
    actor->profileId = SceneActorProfiles_FindOrAdd(4, 0);
}

extern u16 gUnk_08605CE8[];

u32 Bat_LookupAnimationEntry(void *object)
{
    struct SceneActorRegistryResetView *actor = object;
    struct DisplaySequenceState *sequence =
        &((struct SceneActorControlMotionDrawable *)actor->drawable)->sequence;
    struct DisplaySequenceState *currentSequence = sequence;

    if (sequence->elapsed == 0) {
        u32 flag = 0x10000;
        actor->flags2D0 |= flag;
        if (sequence->index <= 3)
            return gUnk_08605CE8[sequence->index];
        if ((u16)(sequence->index - 10) <= 3)
            return gUnk_08605CE8[currentSequence->index - 10];
    }
    return 0;
}

void Ghoul_TickCooldown(struct SceneActorRegistryResetView *actor)
{
    struct Ghoul *context = actor->context;
    u16 v = context->discCooldown;
    if (v != 0)
        context->discCooldown = v - 1;
}

/* Ghoul_SelectRandomRoutePoint: Chooses a random entry different from the current index and resets its progress byte */

s32 Mod(s32, s32);

u8 *Path_GetPointData(u8 *);

static inline u32 Ghoul_RouteRandomSample(void)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(u16 *)(offset + (u32)RANDOM_TABLE_ADDR);
}

void Ghoul_SelectRandomRoutePoint(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorRouteCursor *route = (void *)&actor->group.homeRoute;
    struct SceneActorRouteCursor *selectedRoute = route;
    u8 pointIndex;
    pointIndex = Mod(Ghoul_RouteRandomSample(), route->definition->pointCount);
    if (pointIndex == route->pointIndex)
        pointIndex = Mod(pointIndex + 1, route->definition->pointCount);
    selectedRoute->pointIndex = pointIndex;
    selectedRoute->unknown3 = 0;
    selectedRoute->point = (void *)Path_GetPointData((void *)selectedRoute->definition) +
                           selectedRoute->pointIndex * 8;
}

s32 Math_Atan2(s32, s32);

static inline void Ghoul_SelectRandomPoint(u8 *cursor)
{
    struct SceneActorChaseRouteSelection *p = (struct SceneActorChaseRouteSelection *)cursor;
    struct SceneActorChaseRouteSelection *q = p;
    u8 n;
    n = Mod(Ghoul_RouteRandomSample(), p->points->count);
    if (n == p->index)
        n = Mod(n + 1, p->points->count);
    q->index = n;
    q->phase = 0;
    q->current = (u16 *)(Path_GetPointData((void *)q->points) + q->index * 8);
}

static inline s32 Ghoul_DotXZ(struct ContactPosition *a, struct ContactPosition *b)
{
    return a->x * b->x + a->z * b->z;
}

static inline s32 Ghoul_CellIndex(s32 x, s32 z)
{
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}

static inline u32 Ghoul_RouteHeightAt(s32 cell, struct ContactPosition *position)
{
    struct SceneActorChaseTerrainCell *data = SceneActorChase_CellProperties(cell);
    u32 kind, height;
    kind = data->attributes >> 4;
    height = (data->attributes & 15) << 8;
    if (kind == 1)
        goto slopeZ;
    if (kind == 2)
        goto slopeX;
    goto done;
slopeZ:
    height -= *(u8 *)&position->z;
    goto done;
slopeX:
    height -= *(u8 *)&position->x;
done:
    return height;
}

static inline s32 Ghoul_RouteCellAt(struct ContactPosition *position)
{
    s32 x = position->x >> 8;
    x = Ghoul_CellIndex(x, position->z >> 8);
    return x;
}

static inline u32 Ghoul_RouteHeight(struct ContactPosition *position)
{
    return Ghoul_RouteHeightAt(Ghoul_RouteCellAt(position), position);
}

static inline void Ghoul_SnapToRoutePoint(struct ContactPosition *p,
                                          struct SceneActorChaseRouteSelection *route)
{
    p->x = ((u16 *)route->current)[0];
    p->z = ((u16 *)route->current)[1];
    p->y = Ghoul_RouteHeight(p);
}

void Ghoul_SelectRetreatRoutePoint(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    struct SceneActorChaseRouteSelection *route = (void *)&actor->group.homeRoute;
    u8 selected = 0;
    struct SceneActorChaseTarget *target;
    struct ContactPosition heading;
    struct ContactPosition offset;
    u8 angle;
    u16 *entry;
    s32 pointIndex;
    u8 found;
    u8 *cursor;
    void *mesh;
    target = (void *)actor->tracking.target;
    angle = Math_Atan2(target->position.x - actor->body.position.x,
                       target->position.z - actor->body.position.z);
    heading.x = (gUnk_086149C4[(angle + 64) & 255] * 10) >> 12;
    heading.z = (gUnk_086149C4[angle] * 10) >> 12;
    entry = (u16 *)Path_GetPointData((void *)route->points);
    found = 0;
    for (pointIndex = 0; pointIndex < (route->points->count); pointIndex++) {
        offset.x = entry[0] - actor->body.position.x;
        offset.z = entry[1] - actor->body.position.z;
        if (Ghoul_DotXZ(&heading, &offset) < 0) {
            u32 coordinates = *((u32 *)entry);
            if (coordinates ==
                (*(u32 *)((struct SceneActorChaseRouteSelection *)&group->homeRoute)->current))
                continue;
            selected = pointIndex;
            found = 1;
            break;
        }
        entry += 4;
    }

    if (!found) {
        cursor = (u8 *)&group->homeRoute;
        Ghoul_SelectRandomPoint(cursor);
    } else {
        route->index = selected;
        route->phase = 0;
        route->current = entry;
        cursor = (u8 *)&group->homeRoute;
    }
    Ghoul_SnapToRoutePoint(group->destination, (void *)cursor);
    mesh = group->route;
    if (!SceneActorChase_TryRoute(actor, mesh, &actor->body.position, group->destination)) {
        SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0);
        SceneActorChase_SetState(actor, 3, 0, 0);
    } else {
        (group->destination)->pad = 1;
    }
}

// confidence=low: copies the owner context's 0x64A timer into 0x648, marks the context flags

// (set bit 0, clear bit 1), enters behavior 15 and starts the group's other route.

void SceneBody_StartRoute(void *, void *, u32, u32, u32);

struct GhoulHalfFlags {
    s16 bits : 16;
};

static inline void Ghoul_SetContextFlags(u16 bit, u16 *field)
{
    struct GhoulHalfFlags *p = (void *)field;
    p->bits |= bit;
}

static inline void Ghoul_ClearContextFlags(u16 *field, u32 bit)
{
    struct GhoulHalfFlags *p = (void *)field;
    p->bits &= ~bit;
}

static inline void Ghoul_SetStatusFlags(u32 bit, u32 *field)
{
    *field |= bit;
}

static inline void Ghoul_EnterRouteBehavior(struct SceneActorRegistryResetView *actor, u32 behavior)
{
    actor->state2A8 = 0;
    actor->phase2A9 = 0;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

static inline void Ghoul_StartAlternateRoute(void *body, void *path,
                                             struct SceneActorChaseGroup *group)
{
    u32 index = group->homeRoute.activeBranch ^ 1;
    u8 *routes = group->homeRoute.branches;
    SceneBody_StartRoute(body, path, routes[index], 0, 0);
}

void Ghoul_BeginAlternateRoute(struct SceneActorRegistryResetView *actor)
{
    struct Ghoul *context = actor->context;
    {
        u16 timer = context->recoveryDelay;
        ((struct GhoulHalfFlags *)&context->routeDelay)->bits = timer;
    }
    Ghoul_SetContextFlags(1, &context->discFlags);
    Ghoul_ClearContextFlags(&context->discFlags, 2);
    Ghoul_SetStatusFlags(4, &actor->flags2D0);
    Ghoul_EnterRouteBehavior(actor, 15);
    Ghoul_StartAlternateRoute(&actor->body, &actor->group.homeRoute, &actor->group);
    Ghoul_SelectRetreatRoutePoint(actor);
    Ghoul_ClearContextFlags(&context->discFlags, 2);
}

/* Vorn_ParseField624_0808B320: same code as Vorn_ParseField624 at another address (ported by relatives.py; provisional name) */

void Vorn_ParseField624_0808B320(struct SceneActorRegistryResetView *actor)
{
    struct Ghoul *context = (struct Ghoul *)actor->context;

    if (Script_SeekToKeyword(0x78) != 0) {
        s32 script, delay;

        if (Script_GetPc() != 0)
            script = Script_GetValue();
        else
            script = 0;
        context->delayedScript = script;

        if (Script_GetPc() != 0)
            delay = Script_GetValue();
        else
            delay = 0;
        context->scriptDelay = delay;
        context->scriptElapsed = 0;
    } else {
        context->delayedScript = 0;
        context->scriptDelay = 0;
        context->scriptElapsed = 0;
    }
}

void Bat_TickDelayedScript(struct SceneActorRegistryResetView *actor)
{
    s32 zero = 0;
    struct Ghoul *context = actor->context;
    u16 *script = &context->delayedScript;
    u8 *count;
    u8 *limit;

    if (*script != 0) {
        count = &context->scriptElapsed;
        *count += 1;
        limit = &context->scriptDelay;
        if (*count >= *limit) {
            u8 zeroByte;
            Script_Call(*script, (void *)zero);
            zeroByte = 0;
            *script = 0;
            *limit = zeroByte;
            *count = zeroByte;
        }
    }
}

/* Resolve contact flags, the hurt animation, and pursuit recovery. */

u8 Ghoul_ReactToContact(struct SceneActorRegistryResetView *actor)
{
    struct Ghoul *context = actor->context;
    struct SceneActorChaseGroup *group = &actor->group;
    if (!(actor->field2D8 & 0x20000)) {
        if (actor->reactionSoundVariant == 0)
            Sound_PlaySong(0x211);
        else
            Sound_PlaySong(0xD4);
    }
    Bat_TickDelayedScript(actor);
    if (SceneActorChase_QueryMotionFlags(actor, 0x2000)) {
        actor->body.direction = actor->field2B6 + 0x80;
        SceneActorAnimation_SelectDirectionalEntry((struct SceneActorAnimationView *)actor, 0);
        SceneActorChase_ClearMotionFlags(actor, 0x2020);
        SceneActorChase_SetActorFlags(actor, 0x10);
        group->destination->pad = 0;
        SceneActorChase_TrackPlayer(actor);
        if (!SceneActorChase_Arrive(actor)) {
            if (!SceneActorChase_TryRoute(actor, group->route, &actor->body.position,
                                          &actor->tracking.target->position)) {
                Callback_InvokeActor(actor, group->fallback);
                return 0;
            }
        }
        SceneActorChase_SetControl(actor, actor->entryTarget228, 1);
        SceneActorChase_SetBehavior(actor, 7);
        actor->field2C2 = 32;
        return 1;
    }
    if (SceneActorChase_QueryMotionFlags(actor, 0x8002)) {
        SceneActorChase_ClearMotionFlags(actor, 0x20);
        return 0;
    }
    if (actor->field2DC & 0xC0000) {
        SceneActorChase_SetControl(actor, actor->reactionTarget230, 0x12);
        SceneActorChase_SetState(actor, 3, 0, 0);
        SceneActorChase_ClearWordFlags(&actor->flags2D0, 0x1000000);
        return 1;
    }
    SceneActorChase_ClearMotionFlags(actor, 0x20);
    if (SceneActorChase_QueryMotionFlags(actor, 0x100))
        return 0;
    if (actor->field2B8)
        return 0;
    SceneActorChase_ClearBits(&context->discFlags, 1);
    if (SceneActorChase_QueryMotionFlags(actor, 0x80)) {
        SceneActorChase_BeginContactAnimation(actor);
        if (!actor->tracking.target)
            return 0;
        SceneActorChase_TrackPlayer(actor);
        SceneActorChase_SetActorFlags(actor, 0x80);
        SceneActorChase_GroupSetFlags(group, 4);
        if (!SceneActorChase_Arrive(actor)) {
            if (!SceneActorChase_TryRoute(actor, group->route, &actor->body.position,
                                          &actor->tracking.target->position)) {
                Callback_InvokeActor(actor, group->fallback);
                return 0;
            }
        }
        SceneActorChase_SetControl(actor, group->common.gate.control, 0x1A);
        SceneActorChase_SetBehavior(actor, 12);
        return 1;
    }
    SceneActorChase_BeginContactAnimation(actor);
    if (!actor->tracking.target)
        return 0;
    SceneActorChase_TrackPlayer(actor);
    SceneActorChase_SetActorFlags(actor, 0x10);
    group->destination->pad = 0;
    if (!SceneActorChase_Arrive(actor)) {
        if (!SceneActorChase_TryRoute(actor, group->route, &actor->body.position,
                                      &actor->tracking.target->position)) {
            Callback_InvokeActor(actor, group->fallback);
            return 0;
        }
    }
    SceneActorChase_SetControl(actor, actor->entryTarget228, 1);
    SceneActorChase_SetBehavior(actor, 7);
    actor->field2C2 = Div(64, 3);
    return 1;
}

/* Tentative: the context pending flag requests a delayed restart from the route origin. */

void SceneActor_SetPositionXZ(void *, s32, s32);

static inline u8 Ghoul_ActionsUnavailable(struct SceneActorRegistryResetView *actor)
{
    if (actor) {
        u32 mask = 0x40;
        if (actor->flags2D4 & mask)
            return 1;
        mask = 0x20000;
        if (actor->flags2D0 & mask)
            return 1;
        if (actor->health <= 0)
            return 1;
    }
    return 0;
}

static inline void Ghoul_PrepareRouteDelay(struct SceneActorDelayedResponse *packet, void *control,
                                           u32 behavior, u32 delay)
{
    packet->delay = delay;
    packet->control = control;
    packet->state = 0;
    packet->phase = 0;
    packet->behavior = behavior;
}

static inline u8 Ghoul_TestPendingRoute(struct Ghoul *context, u32 mask, u16 **pending)
{
    u16 *pointer = &context->discFlags;
    u32 value = *pointer & mask;
    *pending = pointer;
    if (value)
        return 1;
    return 0;
}

s32 Ghoul_ResumePendingRoute(struct SceneActorRegistryResetView *actor)
{
    struct Ghoul *context = actor->context;
    struct SceneActorChaseGroup *group = &actor->group;
    struct GhoulRouteOrigin **routeSlot;
    u16 *flags;
    u16 *pending;
    u16 *timer;
    struct GhoulRouteOrigin *route;
    void *body;
    if (Ghoul_ActionsUnavailable(actor))
        return 1;
    if (!Ghoul_TestPendingRoute(context, 1, &pending))
        return 1;
    {
        u32 mask = 0xA107;
        flags = &actor->flags2D4;
        if (mask & (*flags))
            return 1;
    }
    timer = &context->routeDelay;
    if (*timer)
        (*timer)--;
    else {
        {
            u32 *p = &actor->flags2D0;
            u32 mask = ~4;
            *p = mask & (*p);
        }
        body = &actor->body.id;
        routeSlot = (void *)&group->route;
        route = *routeSlot;
        SceneActor_SetPositionXZ(body, route->x, route->z);
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        Ghoul_PrepareRouteDelay((void *)&group->saved, group->common.gate.control, 12, 30);
        SceneActorChase_SetControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
        SceneActorChase_SetState(actor, 3, 1, 0);
        SceneBody_StartRoute(body, &group->homeRoute, group->homeRoute.routeId,
                             group->homeRoute.parameterB, group->homeRoute.pointId);
        {
            void *route = *routeSlot;
            struct ContactPosition *position = &actor->body.position;
            struct ContactPosition *target = Ghoul_InitialDiscTarget(context);
            s32 result = Nav_IsPositionInMesh(route, position, target);
            if (!result) {
                u32 mask = ~0x200;
                *flags = mask & (*flags);
            } else {
                result = Terrain_ApplyBlocked(route, position, target);
                if (result) {
                    if (result != 2)
                        Memory_Clear(&actor->body.motion, 8);
                } else {
                    u32 mask = ~0x200;
                    *flags = mask & (*flags);
                }
            }
        }
        {
            u32 mask = ~1;
            *pending = mask & (*pending);
        }
    }
    return 1;
}

u32 Ghoul_UpdateCooldown(struct SceneActorRegistryResetView *actor)
{
    Ghoul_TickCooldown(actor);
    return 1;
}

void Ghoul_UpdateSpecialActionControl(struct SceneActorRegistryResetView *actor)
{
    if (SceneActorControl_TakeStateEntry(actor))
        SceneActorChase_SetControl(actor, Ghoul_UpdateDiscThrow, 35);
    if (actor->field2B0) {
        Trigger_ConsumePending(&actor->unknown9[3], 1);
        SceneActorChase_SetControl(actor, actor->savedTarget, 0);
        SceneActorChase_SetState(actor, 2, 0, 2);
    } else {
        ++actor->counter2AC;
    }
}

void Ghoul_UpdateAnimationElevenControl(struct SceneActorRegistryResetView *actor)
{
    if (SceneActorControl_TakeStateEntry(actor))
        SceneActorChase_SetControl(actor, SceneActorReaction_PlayAnimationBUntilComplete, 10);
    if (actor->field2B0) {
        Trigger_ConsumePending(&actor->unknown9[3], 1);
        SceneActorChase_SetControl(actor, actor->savedTarget, 0);
        SceneActorChase_SetState(actor, 2, 0, 2);
    } else {
        ++actor->counter2AC;
    }
}

void Ghoul_UpdateAttackControl(struct SceneActorRegistryResetView *actor)
{
    if (SceneActorControl_TakeStateEntry(actor))
        SceneActorChase_SetControl(actor, Ghoul_UpdateDisplacementRecovery, 18);
    if (actor->field2B0) {
        Trigger_ConsumePending(&actor->unknown9[3], 1);
        SceneActorChase_SetControl(actor, actor->savedTarget, 0);
        SceneActorChase_SetState(actor, 2, 0, 2);
    } else {
        ++actor->counter2AC;
    }
}

extern void *gUnk_08605CF0[];

void Ghoul_SetBehaviorTable(struct SceneActorRegistryResetView *actor)
{
    actor->callbacks29C = gUnk_08605CF0;
}

void SceneActorControl_DispatchControl(void *);

s32 Ghoul_Update(struct SceneActorRegistryResetView *actor)
{
    u32 mask;
    Ghoul_ResumePendingRoute(actor);
    mask = 0x2000;
    if ((actor->flags2D4 & mask) || actor->field2B8 == 0) {
        Callback_InvokeActor(actor, actor->callbacks27C[actor->state2A8]);
        SceneActorControl_DispatchControl(actor);
    }
    Ghoul_UpdateCooldown(actor);
    return 1;
}

void SceneActor_UpdateBehaviorCallbacks(void *);

u32 Ghoul_OnMessage(struct SceneActorRegistryResetView *actor)
{
    SceneActor_UpdateBehaviorCallbacks(actor);
    return 0;
}

void SceneActorEffects_ReleaseResources(void *);

u32 Ghoul_Destroy(struct SceneActorRegistryResetView *actor)
{
    SceneActorEffects_ReleaseResources(actor);
    return 0;
}

void sub_08087A70(void);

void Bat_UpdateDefeat(void);

void SceneActorReaction_OnEnemyDefault(void);

u8 SceneActorTarget_UpdateTrackedContact(void *);

void SceneActorTarget_ResolvePendingObject(void);

void SceneActorReaction_ResolvePlacementDamage(void);

void SceneActorPlacement_ResetStateThree(void);

void Bat_ResumeSavedTarget(void);

void SceneActorReaction_BeginTargetRecovery(void);

void Bat_UpdateAnimationRequest(void);

void SceneActorScript_OnCommand(void);

void SceneActorBehavior_InitializeShadow(void *);

void SceneActorPlacement_HideActor(void *, s32);

void SceneActorBehavior_InstallCallbackTables(void *);

void SceneActorTerrain_ReadScriptParameters(void *);

void SceneActorRegistry_SetObjectContext(void *, void *, s32, s32);

void AnimData_SelectPaletteBank(void *, s32);

u8 SceneActorRegistry_LinkActor(void *);

static inline void GhoulInit_InstallRouteDefaults(struct SceneActorRegistryResetView *rootActor)
{
    rootActor->callback248 = Ghoul_ResetRouteState;
    rootActor->field23C = SceneActorControl_RefreshMotionRoute_0808A90C;
}

static inline void
GhoulInit_InstallCompletionCallbacks(struct SceneActorRegistryResetView *rootActor)
{
    rootActor->messageCallback = SceneActorScript_OnCommand;
    rootActor->updateCallback = Ghoul_OnMessage;
    rootActor->destroyCallback = Ghoul_Destroy;
}

static inline void GhoulInit_InitializePaletteRange(struct SceneActorRegistryResetView *actor,
                                                    u32 alternate, u32 current)
{
    actor->alternatePalette = MonsterInit_GetPaletteBase(actor) + alternate;
    actor->paletteBank = MonsterInit_GetPaletteBase(actor) + current;
}

s32 Ghoul_Init(void *object)
{
    struct SceneActorRegistryResetView *rootActor = object;
    struct SceneActorRegistryResetView *actor = object;
    struct SceneActorChaseGroup *group = &rootActor->group;
    struct SceneActorBody *body = &rootActor->body;
    u32 zero;
    struct ContactPosition origin;
    u32 flag;
    s32 mode;

    ((struct Ghoul *)rootActor)->discFlags = 0;
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
        MonsterInit_ClearPosition(&zero, &origin);
        MonsterInit_InitializeWidget(heap, 0xCB05, 0x9247, &origin);
        actor->animationIndex = 0xFF;
    }
    MonsterInit_InitializePursuitBody(rootActor, &zero);
    if (Script_SeekToKeyword('P')) {
        ((struct Ghoul *)rootActor)->discKind = MonsterInit_ReadByte(0x78);
        ((struct Ghoul *)rootActor)->recoveryDelay = MonsterInit_ReadHalfword(0x78);
        ((struct Ghoul *)rootActor)->throwCount = MonsterInit_ReadHalfword(0x78);
    } else {
        ((struct Ghoul *)rootActor)->discKind = 0x78;
        ((struct Ghoul *)rootActor)->recoveryDelay = 0x78;
    }
    MonsterInit_ReadPalette(actor, 0x192, 0x192);
    GhoulInit_InitializePaletteRange(actor, 4, 0);
    Ghoul_InitCollision(actor);
    MonsterInit_InstallAnimationCallbacks(
        actor, Ghoul_Update, Bat_LookupAnimationEntry, Ghoul_TurnTowardRoute, Ghoul_UpdateIdleState,
        Ghoul_UpdateStandingState, Ghoul_UpdateHiddenRecovery, sub_08087A70, Bat_UpdateDefeat,
        SceneActorReaction_OnEnemyDefault, SceneActorTarget_UpdateTrackedContact,
        SceneActorTarget_ResolvePendingObject, SceneActorReaction_ResolvePlacementDamage,
        SceneActorPlacement_ResetStateThree);
    MonsterInit_InitializeGroupWithBounds(
        group, ((struct Ghoul *)rootActor)->routeBounds,
        &((struct Ghoul *)rootActor)->routePosition, Ghoul_UpdateDyingState, Bat_ResumeSavedTarget,
        SceneActorReaction_BeginTargetRecovery, Bat_UpdateAnimationRequest, 1, body);
    SceneActorMovement_ResolveRoutePosition(rootActor);
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
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    if (MonsterInit_GetDrawableKind(actor) == 0)
        DisplaySequence_Step(actor->drawable);
    else
        Anim_Step(&((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile,
                  actor->drawable);
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

void Ghoul_Create(void)
{
    struct Ghoul *ghoul = Heap_Alloc(sizeof(*ghoul));
    if (ghoul != 0) {
        Memory_Clear(ghoul, sizeof(*ghoul));
        if (Ghoul_Init(ghoul) < 0) {
            Ghoul_Destroy((void *)ghoul);
            Heap_Free(ghoul);
        }
    }
}
