#include "scene/actor/scene_actor_init_actor.h"
#include "scene/motion/scene_actor_chase_shared.h"
#include "global.h"
#include "scene/collision/scene_actor_delayed_response.h"
#include "scene/motion/scene_actor_chase_reaction.h"
#include "scene/collision/scene_actor_contact_response.h"
#include "scene/actor/scene_actor_visibility.h"
#include "scene/actor/scene_actor_behavior.h"
#include "scene/actor/scene_actor_reaction.h"
#include "scene/motion/scene_actor_turn.h"
#include "scene/motion/scene_actor_route.h"
#include "engine/video/display_sequence.h"
#include "scene/actor/scene_actor_reset.h"
#include "engine/engine.h"
#include "scene/motion/scene_actor_chase.h"
#include "scene/actor/scene_actor_control_phase.h"
#include "debug.h"
#include "scene/actor/scene_actor_registry.h"
#include "script/script_utility.h"
#include "engine/ram.h"
#include "engine/math/monster_particle_random.h"
#include "scene/actor/scene_actor_control_motion.h"
#include "npc/manager/scene_actor_animation.h"
#include "engine/video/display_sequence_inline.h"
#include "player/player.h"
#include "effects/particles/proximity_emitter.h"
#include "monsters/spider.h"
#include "engine/engine_globals.h"
#include "scene/actor/scene_actor_init_dispatch.h"
#include "engine/video/render.h"
#include "monsters/scene_actor_init_group.h"
#include "monsters/scene_actor_init_graphics.h"
#include "monsters/scene_actor_init_body.h"
#include "bosses/vanargand/vanargand_projectiles.h"

inline static u8 blocked2048(u8 *c)
{
    u32 mask;
    u32 *p;
    u32 off;
    if (!c)
        goto zero;
    mask = 0x40;
    off = 0xB5;
    off <<= 2;
    p = (u32 *)(c + off);
    if ((*((u16 *)p)) & mask)
        goto one;
    mask = 0x80;
    mask <<= 10;
    off -= 4;
    p = (u32 *)(c + off);
    if ((*p) & mask)
        goto one;
    if ((*((s16 *)(c + 0x9C))) > 0)
        goto zero;
one:
    return 1;
zero:
    return 0;
}

void Spider_OnAttackCollision(u8 *unused, u8 *obj, u8 *ctx)
{
    int mask;
    u32 *flags;
    if ((blocked2048(ctx) == 0) && ((*((u16 *)(obj + 0xA))) & 0xF)) {
        *((u32 *)(ctx + 0x304)) = *((u16 *)(obj + 4));
        flags = (u32 *)(ctx + 0x2D0);
        mask = 0xFEFFFFFF;
        *flags &= mask;
    }
}

void SceneActorBehavior_ApplyContactReaction(struct SceneActorBehaviorContactView *,
                                             struct SceneActorBehaviorContactView *,
                                             struct RoomActor *);

void sub_08249244(void *, void *, s32, void *);

static inline u8 Unavailable(struct RoomActor *actor)
{
    if (actor) {
        u32 mask = 0x40;
        if (actor->motionFlags & mask)
            return 1;
        mask = 0x20000;
        if (actor->statusFlags & mask)
            return 1;
        if (actor->health <= 0)
            return 1;
    }
    return 0;
}

static inline u32 TestFlag(struct SceneActorBehaviorContactView *s, u32 mask)
{
    return s->flags & mask;
}

void SceneActorContact_ApplyWithinBounds(struct SceneActorBehaviorContactView *source,
                                         struct SceneActorBehaviorContactView *destination,
                                         struct RoomActor *actor)
{
    struct ContactPosition point;
    s32 difference;
    s16 center, radius;
    if (Unavailable(actor))
        return;
    if (TestFlag(source, 0x1000)) {
        SceneActorBehavior_ApplyContactReaction(source, destination, actor);
        return;
    }
    difference = (source->position.y - destination->position.y) >> 1;
    point.x = source->position.x - difference;
    point.z = source->position.z - difference;
    center = destination->position.x;
    radius = destination->bounds.x;
    if (center + radius <= point.x)
        return;
    if (center - radius >= point.x)
        return;
    center = destination->position.z;
    radius = destination->bounds.z;
    if (center + radius <= point.z)
        return;
    if (center - radius >= point.z)
        return;
    if (source->contactCallback)
        sub_08249244(source, destination, source->contactArgument, source->contactCallback);
    SceneActorBehavior_ApplyContactReaction(source, destination, actor);
}

void SceneActorAnimation_SelectDirectionalEntry(struct RoomActor *, s32);

void Spider_SelectActiveAnimation(u8 *p)
{
    if (p[0x2b2] != 0)
        p[0x2b2] = 0;
    SceneActorAnimation_SelectDirectionalEntry(p, 1);
}

void Spider_SelectIdleAnimation(u8 *p)
{
    if (p[0x2b2] != 0)
        p[0x2b2] = 0;
    SceneActorAnimation_SelectDirectionalEntry(p, 0);
}

static inline u8 Spider_TargetDirection(struct ChaseActor *actor)
{
    return ((actor->base.nextDirection + 5) & 7) << 5;
}

static inline void Spider_StepDirection(struct SceneActorBody *body, s32 target, s32 step)
{
    s32 current = body->direction;
    s32 difference = current - target;
    s32 magnitude = difference;
    if (magnitude < 0)
        magnitude = -magnitude;
    if (magnitude <= step)
        body->direction = target;
    else if (current < target) {
        s32 distance = difference;
        if (distance < 0)
            distance = -distance;
        if (distance <= 128)
            body->direction = current + step;
        else
            body->direction = current - step;
    } else if (current > target) {
        if (difference < 0)
            difference = -difference;
        if (difference <= 128)
            body->direction = current - step;
        else
            body->direction = current + step;
    }
}

static inline void Spider_ApplyDuration(struct ChaseActor *actor)
{
    u16 *scale = SceneActorChase_PrepareAnimationScale(actor, 0xA02F);
    if (!actor->base.drawableKind)
        Anim_SetDurationScale(
            &((struct SceneActorAnimationSequenceDrawable *)actor->base.drawable)->sequence,
            *scale);
    else
        Anim_SetFrameDurationScale(
            &((struct SceneActorAnimationTileDrawable *)actor->base.drawable)->tile, *scale);
}

static inline u16 Spider_ReadCardinal(u8 direction)
{
    return ((direction + 0x60) >> 6) & 3;
}

void Spider_AdvanceTurnAnimation(struct ChaseActor *actor)
{
    struct SceneActorBody *body = &actor->base.body;
    struct DisplaySequenceState *sequence;
    s32 direction, facing, difference;
    if (SceneActorReaction_TakePrepared(actor)) {
        body->direction = (SceneActorTurn_Cardinal(body->direction) << 6) - 64;
        direction = ((body->direction + 0x60) >> 6) & 3;
        facing = actor->base.nextDirection;
        facing >>= 1;
        if (direction != facing) {
            difference = facing - direction;
            if (difference < 0)
                difference = -difference;
            if (difference == 2)
                goto turn;
        }
        actor->base.stepDone = 1;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        return;
    turn:
        switch (direction) {
            case 0:
                SceneActorTurn_SelectAnimation(actor, 5, 0, 0);
                break;
            case 1:
                SceneActorTurn_SelectAnimation(actor, 5, 1, 0);
                break;
            case 2:
                SceneActorTurn_SelectAnimation(actor, 5, 0, 4);
                break;
            case 3:
                SceneActorTurn_SelectAnimation(actor, 5, 1, 4);
                break;
        }
    }
    sequence = &((struct SceneActorAnimationSequenceDrawable *)actor->base.drawable)->sequence;
    if (sequence->elapsed == 0) {
        s32 target = Spider_TargetDirection(actor);
        Spider_StepDirection(body, target, 64);
    }
    Spider_ApplyDuration(actor);
    if (actor->base.restart) {
        actor->base.stepDone = 1;
        actor->base.body.direction = (Spider_ReadCardinal(actor->base.body.direction) << 6) - 64;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    }
}

// confidence=low: animation completion restores the alternate control after a delay.

void ScaledShadow_Enable(void *);

void ScaledShadow_Destroy(void *);

void SceneActorReaction_RestoreAlternateAfterDelay(void);

static inline void ClearHidden(struct SceneActorReactionMotionFlags *p, u32 mask)
{
    p->bits &= mask;
}

static inline void ClearCollider(struct ContactCollider *c)
{
    u32 mask = ~4;
    c->flags &= mask;
}

static inline void ClearContext(struct Spider *c, u32 mask)
{
    c->flags62C &= mask;
}

void Spider_UpdateHiddenRecovery(struct ChaseActor *a)
{
    struct Spider *ctx = a->base.context;
    if (SceneActorControl_TakeFlag(&a->base.field2B2)) {
        SceneActorAnimation_SelectDirectionalEntry(a, 0);
        ScaledShadow_Enable((u8 *)a + 0x360);
        {
            u32 mask = 0x800;
            a->base.placementFlags = mask | a->base.placementFlags;
        }
        EffectCollision_Disable(&a->base.colliderB);
        EffectCollision_Disable(&a->base.colliderA);
    }
    {
        u32 mask = 0x10;
        if (a->base.statusFlags & mask) {
            if (*((u8 *)a + 0x360))
                ScaledShadow_Destroy((u8 *)a + 0x360);
            ClearHidden((void *)&a->base.placementFlags, ~0x800);
            ClearCollider(&a->base.colliderB);
            ClearCollider(&a->base.colliderA);
            SceneActorDelay_Prepare((void *)&a->group.saved, a->base.onStanding, 5, 20);
            Chase_SetCtl(a, SceneActorReaction_RestoreAlternateAfterDelay, 14);
            Chase_SetState(a, 3, 1, 0);
            ClearContext(ctx, ~1);
        }
    }
}

// confidence=low: selects delayed route recovery or alternate recovery after the context delay.

static inline void ClearContext_SelectRecoveryRoute(struct SceneActorReactionMotionFlags *p, u32 mask)
{
    p->bits &= mask;
}

s32 SceneActorControl_CreateFromPosition(void *);

static inline s32 Random(void)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    {
        const u16 *table = (const u16 *)RANDOM_TABLE_ADDR;
        return *(u16 *)(offset + (u32)table);
    }
}

void Spider_SelectRecoveryRoute(struct ChaseActor *a, s32 frame)
{
    struct Spider *ctx = a->base.context;
    struct SceneActorBody *actor = &a->base.body;
    struct SceneActorChaseGroup *group;
    SceneActorControl_TakeFlag(&a->base.field2B2);
    SceneActorAnimation_SelectDirectionalEntry(a, 14);
    if (frame <= ctx->recoveryDelay)
        return;
    group = &a->group;
    ClearContext_SelectRecoveryRoute((struct SceneActorReactionMotionFlags *)&ctx->flags62C, ~4);
    if (a->base.paletteOffset == 0 && SceneActorControl_CreateFromPosition(actor) >= 0) {
        SceneActorDelay_Prepare((void *)&group->saved, a->base.onAlternateEntry, 7, 60);
        Chase_SetCtl(a, SceneActorReaction_RestoreAlternateAfterDelay, 14);
        Chase_SetState(a, 3, 1, 0);
    } else {
        u32 choice = (Random() >> 3) & 3;
        if (choice == 0) {
            SceneActorDelay_Prepare((void *)&group->saved, a->base.onAlternateEntry, 7, 60);
            Chase_SetCtl(a, SceneActorReaction_RestoreAlternateAfterDelay, 14);
            Chase_SetState(a, 3, 1, 0);
        } else {
            SceneActorDelay_Prepare((void *)&group->saved, a->base.onStanding, 5, 60);
            Chase_SetCtl(a, SceneActorReaction_RestoreAlternateAfterDelay, 14);
            Chase_SetState(a, 3, 1, 0);
        }
    }
}

void Spider_OnSecondaryColliderContact(void)
{
}

void Spider_OnActionEvent(void)
{
}

#define F(p, o, T) (*(T *)((u8 *)(p) + (o)))

void Sound_PlaySong(u32);

void SceneColliderEffect_SpawnAimedSprite(void *, void *, u32, u32, u32);

void Spider_SelectRouteEntryAwayFromTarget(struct ChaseActor *actor);

void SceneActorMovement_ResolveRoutePosition(void *);



// confidence=low: windup emits a contact cue before entering the pursuit phase.

void Spider_UpdateContactWindup(struct ChaseActor *a, s32 n)
{
    struct DisplaySequenceState *anim = &((struct SceneActorControlMotionDrawable *)a->base.drawable)->sequence;
    struct SceneActorRegistryControlGroup *nav = &a->group.common;
    struct Spider *c = a->base.context;
    struct SceneActorBody *transform = &a->base.body;
    if (SceneActorControl_TakeFlag(&a->base.field2B2)) {
        SceneActorReaction_EnableMotionFlags(0x100, &a->base.motionFlags);
        SceneActorAnimation_SelectDirectionalEntry((void *)a, 5);
        {
            u32 *p = &a->base.behaviorFlags;
            u32 m = ~0x4000;
            *p &= m;
        }
        c->windupTarget = a->base.tracking.target;
    }
    if (anim->index == 3 && anim->elapsed == 0) {
        void *v;
        Sound_PlaySong(0x114);
        v = c->windupTarget;
        if (v)
            SceneColliderEffect_SpawnAimedSprite(transform, v, a->base.attacks.first[1],
                                                 a->base.attacks.second[1],
                                                 a->base.attacks.parameter[1]);
    }
    if (a->base.restart && n > 60) {
        {
            struct ChaseFlags40 *p = (void *)&a->base.motionFlags;
            u32 mask = ~0x100;
            p->bits &= mask;
        }
        SceneActorChase_SetBehavior(a, 12);
        Chase_SetCtl(a, nav->gate.control, 26);
        Spider_SelectRouteEntryAwayFromTarget(a);
        SceneActorMovement_ResolveRoutePosition(a);
        SceneActorChase_SetFlagComponent((void *)&a->group.common.gate.flags, 4);
        {
            u32 mask = 128;
            mask |= a->base.motionFlags;
            a->base.motionFlags = mask;
        }
        a->group.common.gate.counter = a->group.common.gate.duration;
    }
}

/* SceneActorReaction_ResumeEntryTarget: tentative */

void SceneActorReaction_ResumeEntryTarget(struct ChaseActor *actor)
{
    struct SceneActorReactionGroup *group = (void *)&actor->group.common;
    if (SceneActorReaction_TakePrepared(actor)) {
        DEBUG_PRINT();
    }
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 1);
    if (group->timer418 == 0) {
        SceneActorChase_ClearMotionFlagComponent(actor, 0x80);
        Chase_SetState(actor, 0, 0, 7);
        Chase_SetCtl(actor, actor->base.onAlternateEntry, 1);
    }
}

// confidence=low: eases landing displacement, then restores route pursuit.


static inline void Spider_ClearContextFlags(struct SceneActorReactionMotionFlags *p, u32 mask)
{
    p->bits &= mask;
}



static inline s32 Spider_QuarterDisplacement(s32 x)
{
    if (x >= 0)
        x >>= 2;
    else
        x = -((-x) >> 2);
    return x;
}

static inline void Spider_AddMotionFlags(struct ChaseActor *a, u32 mask)
{
    u32 value = mask;
    value |= a->base.motionFlags;
    a->base.motionFlags = value;
}

static inline void Spider_AddGroupFlags(struct SceneActorRegistryControlGroup *g, u32 mask)
{
    u16 *flags = &g->gate.flags;
    u32 value = mask;
    value |= *flags;
    *flags = value;
}

static inline void Spider_ClearMotionFlags(struct ChaseActor *a_, u32 mask)
{
    u16 *p = &a_->base.motionFlags;
    *p &= mask;
}

static inline u8 Spider_MoveLanding(struct ChaseActor *a)
{
    s16 *cx;
    s16 *cz;
    s32 x, z, delta;

    delta = a->base.reactionStart.x - a->base.reactionEnd.x;
    cx = &a->base.reactionEnd.x;
    x = Spider_QuarterDisplacement(delta);
    delta = a->base.reactionStart.z - a->base.reactionEnd.z;
    cz = &a->base.reactionEnd.z;
    delta = Spider_QuarterDisplacement(delta);
    z = delta;
    if (x > 200)
        x = 200;
    else if (x < -200)
        x = -200;
    if (z > 200)
        z = 200;
    else if (z < -200)
        z = -200;
    a->base.body.motion.x += x;
    a->base.body.motion.z += z;
    *cx += x;
    *cz += z;
    if (!x && !z)
        return 1;
    return 0;
}

void Spider_UpdateLandingMotion(struct ChaseActor *a, s32 frame)
{
    struct DisplaySequenceState *seq =
        &((struct SceneActorAnimationSequenceDrawable *)a->base.drawable)->sequence;
    u32 *flags;
    if (SceneActorControl_TakeFlag(&a->base.field2B2)) {
        Spider_ClearContextFlags((struct SceneActorReactionMotionFlags *)&((struct Spider *)a->base.context)->flags62C, ~6);
        SceneActorAnimation_SelectDirectionalEntry(a, 3);
        Spider_AddMotionFlags(a, 0x20);
    }
    MonsterInit_SetStatusFlags(a, 0x10000);
    MonsterInit_SetBehaviorFlags(a, 0x2000);
    flags = &a->base.behaviorFlags;
    if (frame <= 17 && seq->index == 1)
        seq->elapsed = 0;
    if (Spider_MoveLanding(a)) {
        struct SceneActorRegistryControlGroup *group = &a->group.common;
        a->base.stepDone = 1;
        Chase_SetState(a, 0, 0, 12);
        Chase_SetCtl(a, group->gate.control, 26);
        Spider_SelectRouteEntryAwayFromTarget(a);
        SceneActorMovement_ResolveRoutePosition(a);
        Spider_AddGroupFlags(&a->group.common, 4);
        Spider_AddMotionFlags(a, 0x80);
        a->group.common.gate.counter = a->group.common.gate.duration;
        Spider_ClearMotionFlags(a, ~0x20);
        a->base.body.direction = a->base.tracking.heading;
        {
            u32 mask = 0x10;
            *flags |= mask;
        }
    }
}

void *Bestiary_GetEntryForActor(u32, u32);

void Bestiary_UnlockEntry(void *);

void SceneActorBehavior_CallTerrainScript(struct RoomActor *);

void SceneActorPlacement_PlayCategoryEventSound(struct ChaseActor *);

u8 Sprite_BlinkUntilExpired(void *, s32, s32, s32);


void Spider_UpdateDefeat(struct ChaseActor *actor, s32 frame)
{
    s32 values[8];
    struct ScriptUtilityArguments arguments;
    u16 script;
    u32 *stateFlags;
    if (SceneActorControl_TakeFlag(&actor->base.field2B2)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 3);
        SceneActorPlacement_PlayCategoryEventSound((void *)actor);
        actor->base.defeatTimer = 60;
        {
            u32 index = (gRandomIndex + 1) & 1023;
            u32 value;
            gRandomIndex = index;
            index *= 2;
            {
                const u16 *table = (const u16 *)0x0203B400;
                {
                    const u16 *entry = (void *)(index + (u32)table);
                    value = *entry;
                }
                {
                    u8 *field = (u8 *)actor + 0x2BA;
                    *field = value;
                }
            }
        }
        actor->base.unknown2BA[1] = 1;
    }
    if (SceneActorContact_ReadActorFlags(actor, 0x8000, &stateFlags))
        return;
    if (Sprite_BlinkUntilExpired(actor->base.drawable, frame, 10, 30)) {
        actor->base.stepDone = 1;
        script = actor->base.script1F8;
        if (script) {
            values[0] = actor->base.body.position.x;
            values[1] = actor->base.body.position.y;
            values[2] = actor->base.body.position.z;
            values[3] = actor->base.parameters1FC[0];
            values[4] = actor->base.parameters1FC[1];
            values[5] = actor->base.parameters1FC[2];
            values[6] = actor->base.hitOwners;
            values[7] = actor->base.body.id;
            arguments.count = 8;
            arguments.values = values;
            Script_Call(script, &arguments);
        }
        if (!SceneActorChase_QueryMotionFlagComponent(actor, 0x4000) &&
            (u32)(actor->base.category - 11) > 2)
            Bestiary_UnlockEntry(
                Bestiary_GetEntryForActor(actor->base.category, actor->base.paletteOffset));
        {
            u32 mask = 0x400;
            if (!((*stateFlags) & mask)) {
                SceneActorBehavior_CallTerrainScript((void *)actor);
            }
        }
        SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 16);
        MonsterInit_SetState(actor, 1, 0, 2);
    } else if (SceneActorVisibility_IsHidden(actor)) {
        ActorShadow_Disable(((u8 *)actor) + 0x320);
    } else {
        ActorShadow_Enable(actor->base.shadow);
    }
}

void Spider_ClearGroupFlag(u8 *);

void SceneActorPlacement_UpdateTetherTimer(struct ChaseActor *p);

void SceneActorPlacement_UpdateMovementSlowdown(struct ChaseActor *a);

void SceneActorPlacement_EmitTimedParticles(u8 *);

u32 Spider_UpdatePreAction(u8 *p)
{
    Spider_ClearGroupFlag(p);
    SceneActorPlacement_UpdateTetherTimer(p);
    SceneActorPlacement_UpdateMovementSlowdown(p);
    SceneActorPlacement_EmitTimedParticles(p);
    return 1;
}

/* Spider_HandleUnusedAction: Empty unreferenced handler within the Spider action routines */

void Spider_HandleUnusedAction(void)
{
}

// confidence=low: follows the route probe, then selects a new route point outside link mode.

u8 SceneActor_IsFacingNearbyTarget(struct ChaseActor *);

s32 Callback_InvokeActor(void *, void *);

u8 SceneActorReaction_BeginContactResponse(struct ChaseActor *);

void SceneActorPhase_SelectLinkedAnimation(void);

void Anim_SetDurationScale(void *, u32);

void Anim_SetFrameDurationScale(void *, u32);

u32 NavMesh_ConsumeProbeEvent(void *);

u8 SceneActorBehavior_RequestDescriptorTurn(void *, void *, u32, u32, s32);

u8 SceneActorBehavior_AccelerateTowardDescriptor(void *, void *, u32, u32, s32);

// confidence=low: advances Spider toward its route probe and resumes route control on arrival

void Spider_ApproachRouteProbe(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    s32 speed;
    u16 *scale;
    if (actor->base.field2B1)
        actor->base.field2B1 = 0;
    speed = Callback_InvokeActor(actor, actor->base.animationQuery.opaque);
    scale = SceneActorChase_PrepareAnimationScale(actor, 0xA02F);
    if (!actor->base.drawableKind)
        Anim_SetDurationScale((u8 *)actor->base.drawable + 0x48, *scale);
    else
        Anim_SetFrameDurationScale((u8 *)actor->base.drawable + 0x20, *scale);
    if (NavMesh_ConsumeProbeEvent(group->route)) {
        if (SceneActorBehavior_RequestDescriptorTurn(actor, group->route, 32, 32,
                                                     speed)) {
            Chase_SetBehavior(actor, 13);
            return;
        }
    }
    if (SceneActorBehavior_AccelerateTowardDescriptor(actor, group->route, 32, 32,
                                                      speed)) {
        Chase_SetBehavior(actor, 12);
        SceneActorChase_SetControl(actor, group->common.gate.control, 1);
        Spider_SelectRouteEntryAwayFromTarget(actor);
        SceneActorMovement_ResolveRoutePosition(actor);
    } else {
        actor->base.counter2AC++;
        SceneActorChase_SetActorFlags(actor, 0x2000);
    }
}

// confidence=low: follows the route probe, then selects a new route point outside link mode.

// confidence=low: updates probe navigation and switches between turn, approach, and idle behaviors.

void Spider_UpdateWanderGoal(struct ChaseActor *input);

static inline u8 HasFlag(struct Spider *c, u32 mask)
{
    if (c->flags62C & mask)
        return 1;
    return 0;
}

static inline void ClearFlag(struct SceneActorReactionMotionFlags *p, u32 mask)
{
    p->bits &= mask;
}


static inline u32 GetFlags(struct ChaseActor *a, u32 mask, u32 **out)
{
    u32 *p = &a->base.behaviorFlags;
    u32 v = *p;
    v &= mask;
    *out = p;
    return v;
}

void Spider_UpdateProbeNavigation(struct ChaseActor *actor)
{
    struct Spider *ctx = actor->base.context;
    struct SceneActorChaseGroup *group = &actor->group;
    s32 speed;
    u32 *flags;
    u16 *scale;
    u32 probe;
    if (actor->base.field2B1)
        actor->base.field2B1 = 0;
    if ((u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque))
        return;
    speed = Callback_InvokeActor(actor, actor->base.animationQuery.opaque);
    scale = SceneActorChase_PrepareAnimationScale(actor, 0xA02F);
    if (!actor->base.drawableKind)
        Anim_SetDurationScale((u8 *)actor->base.drawable + 0x48, *scale);
    else
        Anim_SetFrameDurationScale((u8 *)actor->base.drawable + 0x20, *scale);
    if (!GetFlags(actor, 0x4000, &flags) && !HasFlag(actor->base.context, 4))
        Spider_UpdateWanderGoal(actor);
    probe = NavMesh_ConsumeProbeEvent(group->route);
    if (!probe) {
        void *descriptor = group->route;
        if (SceneActorBehavior_AccelerateTowardDescriptor(actor, descriptor, 32, 32, speed)) {
            Chase_SetBehavior(actor, 5);
            {
                u32 mask = ~0x4000;
                *flags &= mask;
            }
            ClearFlag((struct SceneActorReactionMotionFlags *)&((struct Spider *)actor->base.context)->flags62C, ~2);
            return;
        }
    } else {
        void *descriptor = group->route;
        if (SceneActorBehavior_RequestDescriptorTurn(actor, descriptor, 32, 32, speed))
            Chase_SetBehavior(actor, 6);
    }
    if (!SceneActorChase_Bits32(flags, 0x4000) && ctx->wanderTimer > 0) {
        *flags |= 0x2000;
    }
    actor->base.counter2AC++;
}

/* SceneActorControl_UpdateEntryWait: tentative */




void SceneActorControl_UpdateEntryWait(struct ChaseActor *s)
{
    if (Chase_TakeArrived(s)) {
        SceneActorChase_SetControl(s, s->base.onEntry, 2);
    } else {
        u8 result = Callback_InvokeActor(s, s->base.contactQuery.opaque);
        if (result == 0) {
            if (s->base.stepDone) {
                SceneActorChase_SetState(s, 0, 0, 7);
                Chase_SetCtl(s, s->base.onAlternateEntry, 1);
            } else {
                s->base.counter2AC++;
            }
        }
    }
}

static inline u8 Peek(struct ChaseActor *s)
{
    return s->base.stepDone;
}


void SceneActorControl_UpdateRouteResponse(struct ChaseActor *s)
{
    u8 entered = Chase_TakeArrived(s);
    if (entered) {
        SceneActorChase_SetControl(s, s->base.onEntry, 2);
    } else if (Peek(s)) {
        struct SceneActorChaseGroup *group = &s->group;
        void *fn;
        u32 kind;
        SceneActorChase_SetState(s, 0, 0, 12);
        fn = group->common.gate.control;
        kind = 26;
        s->base.field2B2 = 1;
        s->base.stepDone = entered;
        s->base.controlKind = kind;
        s->base.target = fn;
        s->base.field2C8 = entered;
        {
            u32 *flags = &s->base.pendingControlFlags;
            s32 mask = -2;
            *flags &= mask;
        }
        s->base.pendingControl = entered;
    } else {
        ++s->base.counter2AC;
    }
}

// confidence=low: spider action tick; restores entry state, picks a route or delayed recovery each frame.

void Spider_SelectRandomEntry(struct SceneActorRouteCursor *p);

s32 SceneActorPlacement_TriggerTetherAction(struct ChaseActor *a);

s32 SceneActor_IsWithinTetherRange(struct ChaseActor *actor, s32 xRange, s32 zRange);

void Spider_SelectRecoveryRoute();


void Spider_UpdateIdleWander(struct ChaseActor *a)
{
    u8 *group = (u8 *)&a->group.common;
    struct Spider *ctx = a->base.context;

    if (SceneActorControl_TakeFlag(&a->base.field2B1)) {
        Chase_SetCtl(a, a->base.onStanding, 0);
        a->base.field2C2 = 0x40;
    }
    if ((u8)Callback_InvokeActor(a, a->base.contactQuery.opaque) == 0) {
        u32 four = 4;
        if (SceneActorChase_Test(ctx->flags62C, four)) {
            if ((u8)SceneActor_IsWithinTetherRange(a, 0x40, 0x40)) {
                a->base.field2C2 = 0x40;
                Spider_SelectRandomEntry((void *)(group + 0x74));
                SceneActorMovement_ResolveRoutePosition(a);
                ctx->recoveryDelay = 0;
                Chase_SetState(a, 0, 0, 15);
                Chase_SetCtl(a, Spider_SelectRecoveryRoute, 0x22);
            } else {
                SceneActorPlacement_TriggerTetherAction(a);
            }
        } else if ((s32)a->base.counter2AC > 60) {
            Spider_SelectRandomEntry((void *)(group + 0x74));
            SceneActorMovement_ResolveRoutePosition(a);
            if (a->base.paletteOffset == 0) {
                if ((ParticleRandom_ReadDirectSample(gRandomTable) & 3) != 0) {
                    Chase_SetState(a, 0, 0, 15);
                    Chase_SetCtl(a, Spider_SelectRecoveryRoute, 0x22);
                    ctx->recoveryDelay = (ParticleRandom_ReadSample(gRandomTable) & 0x7F) + 60;
                } else {
                    Chase_SetState(a, 0, 0, 7);
                    Chase_SetCtl(a, a->base.onAlternateEntry, 1);
                }
            } else {
                if ((ParticleRandom_ReadDirectSample(gRandomTable) & 7) == 0) {
                    Chase_SetState(a, 0, 0, 15);
                    Chase_SetCtl(a, Spider_SelectRecoveryRoute, 0x22);
                    ctx->recoveryDelay = (ParticleRandom_ReadSample(gRandomTable) & 0x7F) + 60;
                } else {
                    Chase_SetState(a, 0, 0, 7);
                    Chase_SetCtl(a, a->base.onAlternateEntry, 1);
                }
            }
        } else {
            a->base.counter2AC++;
        }
    }
}

void Spider_TickActionUntilComplete(u8 *p)
{
    if (p[0x2b1])
        p[0x2b1] = 0;
    if (!(*(u8(**)(u8 *))(p + 0x270))(p))
        (*(u32 *)(p + 0x2ac))++;
}

/* Spider_TickActionTimer: Clears the queued flag and unconditionally increments the action timer */

void Spider_TickActionTimer(u8 *p)
{
    if (p[0x2b1])
        p[0x2b1] = 0;
    (*(u32 *)(p + 0x2ac))++;
}

extern const u32 gUnk_08605EE0[];

extern const u32 gUnk_08605F24[];

void Spider_SetBehaviorTables(u8 *p)
{
    *(const u32 **)(p + 0x284) = gUnk_08605EE0;
    *(const u32 **)(p + 0x294) = gUnk_08605F24;
}

struct H {
    u32 lo : 16;
    u32 hi : 16;
};

struct Z {
    struct H a, b;
};

void SceneActorCollision_InitBodyWithFlags(u8 *, struct Z *, struct Z *, void (*)(void), u8 *, u32);

void SceneActorCollision_InitAttack(u8 *, struct Z *, struct Z *, void (*)(void), u32, u8 *);

void SceneActorContact_ApplyWithinBounds(struct SceneActorBehaviorContactView *source,
                                         struct SceneActorBehaviorContactView *destination,
                                         struct RoomActor *actor);

void Spider_OnAttackCollision(u8 *unused, u8 *obj, u8 *ctx);

void Spider_InitColliders(u8 *p)
{
    struct Z s1, s2, s3, s4;
    s1.a.lo = 0xa0;
    s1.a.hi = 0x80;
    s1.b.lo = 0xa0;
    *(u32 *)&s2.a = 0x800000;
    *(u32 *)&s2.b &= 0xFFFF0000;
    SceneActorCollision_InitBodyWithFlags(p, &s1, &s2, SceneActorContact_ApplyWithinBounds, p,
                                          0x510B);
    s3.a.lo = 0x40;
    s3.a.hi = 0x64;
    s3.b.lo = 0x40;
    *(u32 *)&s4.a = 0x800000;
    *(u32 *)&s4.b &= 0xFFFF0000;
    SceneActorCollision_InitAttack(p, &s3, &s4, Spider_OnAttackCollision, 0, p);
}

/* SceneActorRoute_ResetStep: tentative */

void SceneActorRoute_ResetStep(struct ChaseActor *actor)
{
    struct Spider *context = actor->base.context;
    context->wanderTimer = 0;
    context->recoveryDelay = 0;
    context->flags62C = 0;
    SceneActorMovement_ResolveRoutePosition(actor);
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    {
        struct SceneActorControlMotionDrawable *drawable = actor->base.drawable;
        DisplaySequence_StepState(drawable, &drawable->sequence);
    }
}

s32 SceneActorProfiles_FindOrAdd(u32, u32);

void Spider_InitProfile(u8 *p)
{
    *(u32 *)(p + 0x308) = SceneActorProfiles_FindOrAdd(2, 0);
}

// confidence=low: targets player zero and starts the contact pursuit control.

s32 Nav_IsPositionInMesh(void *, struct ContactPosition *, struct ContactPosition *);

s32 Terrain_ApplyBlocked(void *, struct ContactPosition *, struct ContactPosition *);

s32 Div(s32, s32);

static inline u8 Blocked(struct ChaseActor *a)
{
    if (a) {
        u32 mask = 0x40;
        if (a->base.motionFlags & mask)
            return 1;
        mask = 0x20000;
        if (a->base.statusFlags & mask)
            return 1;
        if (a->base.health <= 0)
            return 1;
    }
    return 0;
}

static inline u8 HasFlag_BeginPlayerPursuit(struct Spider *c, u32 mask)
{
    if (c->flags62C & mask)
        return 1;
    return 0;
}

void Spider_BeginPlayerPursuit(struct ChaseActor *a)
{
    struct Spider *ctx;
    struct SceneActorChaseGroup *group;
    u8 ready;
    void **mesh;
    struct ContactPosition **point;
    void *terrain;
    void *dest;
    s32 result;
    u32 motionMask;
    if (Blocked(a))
        return;
    if (!gPlayers[0])
        return;
    ctx = a->base.context;
    group = &a->group;
    ready = HasFlag_BeginPlayerPursuit(ctx, 1);
    if (ready)
        return;
    mesh = &group->route;
    if (!Nav_IsPositionInMesh(*mesh, &a->base.body.position, (struct ContactPosition *)&gPlayers[0]->position))
        return;
    motionMask = 0x200;
    point = &group->destination;
    *(*point) = a->base.body.position;
    (*point)->pad = ready;
    a->base.tracking.target = &((struct SceneActorChasePlayer *)gPlayers[0])->target;
    Chase_SetState(a, 0, 0, 7);
    Chase_SetCtl(a, a->base.onAlternateEntry, 1);
    terrain = *mesh;
    dest = &a->base.tracking.target->position;
    if (!Nav_IsPositionInMesh(terrain, &a->base.body.position, dest)) {
        a->base.motionFlags &= ~motionMask;
    } else {
        result = Terrain_ApplyBlocked(terrain, &a->base.body.position, dest);
        if (result) {
            if (result != 2)
                Memory_Clear(&a->base.body.motion, 8);
        } else {
            a->base.motionFlags &= ~motionMask;
        }
    }
    SceneActorChase_SetBehaviorFlags(a, 0x4010);
    a->base.field2C2 = Div(64, 3);
    ctx->flags62C |= 2;
    ctx->wanderTimer = 0;
}

void Spider_ClearGroupFlag(u8 *s)
{
    if (blocked2048(s) == 0 && (s8)gPlayers[0]->movementAnimationState < 0) {
        struct Spider *ctx = ((struct ChaseActor *)s)->base.context;
        u16 *flags = &ctx->flags62C;
        u32 mask = ~2;
        *flags &= mask;
    }
}

s32 SceneActor_IsWithinTetherRange(struct ChaseActor *actor, s32 xRange, s32 zRange)
{
    struct ContactPosition *target = TetherActor_GetPosition();
    if (!target)
        return 0;
    {
        s16 delta[4];
        s16 *p = delta;
        struct ContactPosition *pos = &actor->base.body.position;
        {
            s16 value = target->x - pos->x;
            s16 *q = delta;
            *q = value;
        }
        p[1] = target->y - pos->y;
        p[2] = target->z - pos->z;
        {
            s32 dx = delta[0];
            if (dx < 0)
                dx = -dx;
            if (dx > xRange)
                return 0;
        }
        {
            s32 dz = p[2];
            if (dz < 0)
                dz = -dz;
            if (dz > zRange)
                return 0;
        }
        return 1;
    }
}

static inline u8 Disabled(struct ChaseActor *a)
{
    if (a) {
        if (SceneActorContact_MotionFlags(a, 64) || SceneActorChase_StatusFlags(a, 0x20000) || F(a, 0x9c, s16) <= 0)
            return 1;
    }
    return 0;
}

struct ContactPosition *TetherActor_GetPosition(void);


static inline u8 Route(struct ChaseActor *a, void *nav, void *pos, void *target)
{
    s32 r;
    if (!Nav_IsPositionInMesh(nav, pos, target)) {
        Chase_ClearBits(&a->base.motionFlags, 0x200);
        return 0;
    }
    r = Terrain_ApplyBlocked(nav, pos, target);
    if (r) {
        if (r != 2)
            Memory_Clear(&a->base.body.motion, 8);
        return 1;
    }
    Chase_ClearBits(&a->base.motionFlags, 0x200);
    return 0;
}

s32 SceneActorPlacement_TriggerTetherAction(struct ChaseActor *a)
{
    struct Spider *c = a->base.context;
    u8 *nav = (u8 *)a + 0x3d4;
    struct ContactPosition *target;
    if (Disabled(a))
        return 0;
    target = TetherActor_GetPosition();
    if (!target)
        return 0;
    if (!Route(a, F(nav, 0x1fc, void *), &a->base.body.position, target))
        return 0;
    {
        struct SceneActorReactionMotionFlags *p = (void *)&c->flags62C;
        p->bits |= 4;
        SceneActorChase_SetControl(a, a->base.onAlternateEntry, 1);
        SceneActorChase_SetBehavior(a, 7);
    }
    a->base.field2C2 = 16;
    return 1;
}

s32 TetherActor_GetParameter(void);

u32 TetherActor_IsStateOne(void);

static inline u8 HasPendingAction(struct Spider *p)
{
    u32 mask = 4;
    if (mask & p->flags62C)
        return 1;
    return 0;
}

void SceneActorPlacement_UpdateTetherTimer(struct ChaseActor *p)
{
    struct Spider *s;
    if (p->base.paletteOffset == 1)
        return;
    s = p->base.context;
    if (TetherActor_GetParameter() != -1 || !TetherActor_IsStateOne()) {
        s->timer62E = 300;
        {
            u16 *flags = &s->flags62C;
            s32 mask = -5;
            mask &= *flags;
            *flags = mask;
        }
    } else if (s->timer62E) {
        s->timer62E--;
    } else {
        s->timer62E = 300;
        if (!HasPendingAction(s))
            SceneActorPlacement_TriggerTetherAction(p);
    }
}

static inline u8 SaveControlFlags(struct Spider *c, u16 **out, u32 m)
{
    u16 *p = &c->flags62C;
    u16 value = *p;
    value &= m;
    *out = p;
    if (value)
        return 1;
    return 0;
}

static inline u16 Actor16_UpdateMovementSlowdown(struct ChaseActor *a, u32 m)
{
    u16 value = a->base.motionFlags;
    value &= m;
    return value;
}

static inline u8 Disabled_UpdateMovementSlowdown(struct ChaseActor *a)
{
    if (a) {
        if (Actor16_UpdateMovementSlowdown(a, 64) || SceneActorChase_StatusFlags(a, 0x20000) || a->base.health <= 0)
            return 1;
    }
    return 0;
}


// confidence=low: global flag applies a temporary movement slowdown

void SceneActorPlacement_UpdateMovementSlowdown(struct ChaseActor *a)
{
    struct Spider *c = a->base.context;
    u16 *flags;
    if (Disabled_UpdateMovementSlowdown(a))
        return;
    if (SaveControlFlags(c, &flags, 16)) {
        if (MonsterInit_TestWorldFlags(0x20)) {
            if (!SceneActorChase_TestField(flags, 8)) {
                SceneActorMovement_ResolveRoutePosition(a);
                Chase_SetBehavior(a, 7);
                Chase_SetCtl(a, a->base.onAlternateEntry, 1);
                {
                    u32 v = *flags;
                    u32 m = 8;
                    m |= v;
                    *flags = m;
                }
            }
            a->base.field2C2 = Div(64, 5);
        } else if (SceneActorChase_TestField(flags, 8)) {
            u16 *p = flags;
            u32 m = ~8;
            *p &= m;
            a->base.field2C2 = 64;
        }
    }
}

// confidence=low: periodically probes a randomized heading and updates the Spider route endpoint and cooldown.

extern const s16 gUnk_086149C4[];

static inline u8 Test(struct Spider *ctx, u32 mask)
{
    if (ctx->flags62C & mask)
        return 1;
    return 0;
}

static inline u32 RandomChoice(u32 *counter, const u16 *table, u32 mask)
{
    u32 index = (*counter + 1) & mask;
    u32 value;
    *counter = index;
    value = table[index];
    return value & 3;
}

static inline u8 SelectHeading(u16 random, u8 *direction)
{
    s32 angle = (random & 127) - 63;
    return angle + *direction;
}

void Spider_UpdateWanderGoal(struct ChaseActor *input)
{
    struct Spider *ctx = input->base.context;
    struct SceneActorChaseGroup *group = &input->group;
    struct SceneActorBody *body = &input->base.body;
    struct ContactPosition target;
    const u16 *table = (const u16 *)((u32)gRandomTable & ~((u16)(u32)input >> 16));
    u32 *indexAddress = (u32 *)((u32)&gRandomIndex & ~((u16)(u32)input >> 16));
    u32 rawIndex = *indexAddress + 1;
    u32 mask = 1023 & ~((u16)(u32)input >> 16);
    u16 index = rawIndex & mask;
    u16 distance = table[index];
    u8 heading;
    u8 *field;
    distance &= mask;
    distance += 256;
    index = (index + 1) & mask;
    field = (void *)indexAddress;
    *(u32 *)field = index;
    {
        u16 sample = table[index];
        field = &input->base.body.direction;
        heading = SelectHeading(sample, field);
    }
    if (Mod(input->base.counter2AC, 30) == 0) {
        if (ctx->wanderTimer > 0)
            goto decrement;
        target.x = ((gUnk_086149C4[(heading + 64) & 255] * distance) >> 14) + body->position.x;
        target.y = body->position.y;
        target.z = ((gUnk_086149C4[heading] * distance) >> 14) + body->position.z;
        if (!Nav_IsPositionInMesh(input->group.route, &input->base.body.position, &target)) {
            if (!Test(ctx, 16))
                input->base.field2C2 = 64;
            SceneActorMovement_ResolveRoutePosition(input);
            ctx->wanderTimer = 0;
        } else {
            input->base.reactionStart = target;
            if (!(RandomChoice(indexAddress, table, mask)))
                input->base.field2C2 = 16;
            else
                input->base.field2C2 = 64;
            {
                struct ContactPosition *to;
                void *mesh = group->route;
                struct ContactPosition *from = &body->position;
                s16 moved;
                to = &input->base.reactionStart;
                moved = Chase_TryRoute(input, mesh, from, to);
                if (!moved) {
                    SceneActorMovement_ResolveRoutePosition(input);
                    ctx->wanderTimer = moved;
                }
            }
            if (Test(ctx, 8 | (((u16)mask >> 16) | ((u16)(u32)indexAddress >> 16)))) {
                u32 value = 60;
                s16 *timer = &ctx->wanderTimer;
                *timer = value;
            } else
                ctx->wanderTimer = 120;
        }
    } else if (ctx->wanderTimer > 0) {
    decrement: {
        ctx->wanderTimer--;
    }
    }
}

// confidence=low: starts the contact route toward the object named by field304.

void *SceneActorContacts_FindObjectById(u16);

static inline void SetBehavior(struct ChaseActor *actor, u32 b)
{
    u32 behavior = b;
    actor->base.state2A8 = 0;
    actor->base.phase2A9 = 0;
    actor->base.behavior2AA = behavior;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}

static inline void OrFlags(u16 *flags, u32 mask)
{
    u32 value = mask;
    value |= *flags;
    *flags = value;
}

u8 SceneActorPlacement_BeginContactRoute(struct ChaseActor *p)
{
    struct SceneActorRegistryControlGroup *group;
    s32 result;
    if ((s32)p->base.counter304 <= 0) {
        result = 0;
    } else {
        group = &p->group.common;
        p->base.tracking.target = (u32)SceneActorContacts_FindObjectById(p->base.counter304);
        SetBehavior(p, 12);
        Chase_SetCtl(p, group->gate.control, 26);
        Spider_SelectRouteEntryAwayFromTarget(p);
        SceneActorMovement_ResolveRoutePosition((void *)p);
        OrFlags((u16 *)&group->gate.flags, 4);
        {
            u32 mask = 128;
            u16 *flags = &p->base.motionFlags;
            u32 value = *flags;
            mask |= value;
            *flags = mask;
        }
        group->gate.counter = group->gate.duration;
        result = 1;
    }
    return result;
}

struct SpiderApproachPlayer {
    u8 unknown0[0x20];
    u32 flags;
    u8 unknown24[0xC];
    struct ContactPosition position;
};

static inline u8 HasFlag_SelectPlayerApproach(u8 *c, u32 m)
{
    if (((struct Spider *)c)->flags62C & m)
        return 1;
    return 0;
}


u8 SceneActorPlacement_BeginContactRoute(struct ChaseActor *p);

s32 Math_Atan2(s32, s32);

u8 Terrain_IsSegmentBelowHeightLimit(void *, void *, u32, u32);

void Spider_UpdateContactWindup(struct ChaseActor *a, s32 n);


static inline s32 Facing(struct ChaseActor *a, struct ContactPosition *delta)
{
    s32 direction = a->base.body.direction;
    s32 difference = Math_Atan2(delta->x, delta->z) - direction;
    if (difference > 128)
        difference -= 256;
    else if (difference < -128)
        difference += 256;
    return difference;
}

static inline void BeginContact(struct ChaseActor *a, struct SceneActorRegistryControlGroup *nav)
{
    Chase_SetBehavior(a, 12);
    Chase_SetCtl(a, nav->gate.control, 26);
    Spider_SelectRouteEntryAwayFromTarget(a);
    SceneActorMovement_ResolveRoutePosition(a);
    {
        u16 *p = &a->group.common.gate.flags;
        u32 m = 4;
        u32 v = *p;
        m |= v;
        *p = m;
    }
    {
        u32 m = 128;
        u16 *p = &a->base.motionFlags;
        u32 v = *p;
        m |= v;
        *p = m;
    }
    {
        u32 v = a->group.common.gate.duration;
        a->group.common.gate.counter = v;
    }
}

// confidence=low: detects a player in front and selects close or distant contact behavior

u8 SceneActorPlacement_SelectPlayerApproach(struct ChaseActor *a)
{
    u8 *transform = (u8 *)&a->base.body.id;
    u8 *c = (void *)SceneActorControl_GetState(a);
    struct ContactPosition delta;
    u8 *player;
    u8 *candidate;
    s32 height;
    s32 angle;
    u32 distance;
    if (!HasFlag_SelectPlayerApproach(c, 2)) {
        if (SceneActorPlacement_BeginContactRoute(a))
            return 1;
        if (SceneActorChase_Bits32(&((struct SpiderApproachPlayer *)gPlayers[0])->flags, 0x20000))
            return 0;
    } else if (SceneActorPlacement_BeginContactRoute(a))
        return 1;
    candidate = gPlayers[0];
    if (!candidate)
        return 0;
    player = candidate;
    Chase_Delta(&delta, &a->base.body.position, &((struct SpiderApproachPlayer *)player)->position);
    height = delta.y;
    if (height < 0)
        height = -height;
    if (height > 255)
        return 0;
    distance = (delta.x * delta.x) + (delta.z * delta.z);
    Math_Atan2(delta.x, delta.z);
    if (distance >= (a->base.tracking.limit * 2))
        return 0;
    Chase_Delta(&delta, &a->base.body.position, &((struct SpiderApproachPlayer *)player)->position);
    angle = Facing(a, &delta);
    if (angle < 0)
        angle = -angle;
    if (angle > 32)
        return 0;
    if (!Terrain_IsSegmentBelowHeightLimit(transform + 12, &((struct SpiderApproachPlayer *)player)->position, 5, 256))
        return 0;
    a->base.tracking.target = (u32)((struct SpiderApproachPlayer *)player)->unknown24;
    if (((s32)distance) <= 0x3fff)
        BeginContact(a, &a->group.common);
    else {
        Chase_SetBehavior(a, 16);
        Chase_SetCtl(a, Spider_UpdateContactWindup, 3);
    }
    return 1;
}

static inline u8 HasFlag_BeginNearbyPlayerContact(u8 *c, u32 m)
{
    if (F(c, 0x62c, u16) & m)
        return 1;
    return 0;
}

static inline u32 SaveFlags(struct ChaseActor *a, u16 **out, u32 m)
{
    u16 *p = &a->base.motionFlags;
    u32 r = *p & m;
    *out = p;
    return r;
}

// confidence=low: starts close player contact when within the actor's stored distance limit

u8 SceneActorPlacement_BeginNearbyPlayerContact(struct ChaseActor *a)
{
    u8 *c = (void *)SceneActorControl_GetState(a);
    struct SceneActorChaseGroup *group = &a->group;
    u8 *transform = (u8 *)a + 0x50;
    u8 *player;
    u16 *flags;
    struct ContactPosition delta;
    s32 angle;
    if (HasFlag_BeginNearbyPlayerContact(c, 4))
        return 0;
    if (SceneActorPlacement_BeginContactRoute(a))
        return 1;
    player = ((u8 **)gPlayers)[0];
    if (SceneActorChase_Bits32((void *)(player + 0x20), 0x20000))
        return 0;
    if (SaveFlags(a, &flags, 128))
        return 0;
    if (a->base.tracking.distance > a->base.tracking.limit)
        return 0;
    Chase_Delta(&delta, &a->base.body.position, (void *)(player + 0x30));
    angle = Facing(a, &delta);
    if (angle < 0)
        angle = -angle;
    if (angle > 32)
        return 0;
    if (!Terrain_IsSegmentBelowHeightLimit(transform + 12, ((u8 **)gPlayers)[0] + 0x30, 5, 256))
        return 0;
    a->base.tracking.target = (u32)(((u8 **)gPlayers)[0] + 0x24);
    Chase_SetBehavior(a, 12);
    Chase_SetCtl(a, group->common.gate.control, 26);
    Spider_SelectRouteEntryAwayFromTarget(a);
    SceneActorMovement_ResolveRoutePosition(a);
    {
        u16 *p = &a->group.common.gate.flags;
        u32 m = 4;
        u32 v = *p;
        m |= v;
        *p = m;
    }
    {
        u32 m = 128;
        u32 v = *flags;
        m |= v;
        *flags = m;
    }
    {
        u32 v = F(a, 0x416, u16);
        F(a, 0x418, u16) = v;
    }
    return 1;
}

u8 *Path_GetPointData(u8 *);

struct RouteCursor {
    u8 pad0[2];
    u8 index;
    u8 unk3;
    u8 *data;
    u16 *entry;
};

static inline s32 Dot(struct ContactPosition *a, struct ContactPosition *b)
{
    return a->x * b->x + a->z * b->z;
}

// scope shape 6 (differences once low registers are set aside)

// confidence=low: aims a route cursor at the first waypoint that lies ahead of the target, else picks a random one

void Spider_SelectRouteEntryAwayFromTarget(struct ChaseActor *actor)
{
    u8 *group = (u8 *)(&actor->group.common);
    struct RouteCursor *route = (struct RouteCursor *)(group + 0x74);
    u8 selected = 0;
    struct SceneActorChaseTarget *t;
    struct ContactPosition dir;
    struct ContactPosition delta;
    u8 angle;
    u16 *entry;
    s32 i;
    u8 found;

    t = actor->base.tracking.target;
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
            if (w == **((u32 **)(group + 0x7C)))
                continue;
            selected = i;
            found = 1;
            break;
        }
        entry += 4;
    }

    if (!found) {
        Spider_SelectRandomEntry((void *)(group + 0x74));
    } else {
        route->index = selected;
        route->unk3 = 0;
        route->entry = entry;
    }
}

s32 Mod(s32, s32);

void Spider_SelectRandomEntry(struct SceneActorRouteCursor *p)
{
    struct SceneActorRouteCursor *q = p;
    u8 n;
    n = Mod(Random(), p->definition->pointCount);
    if (n == p->pointIndex)
        n = Mod(n + 1, p->definition->pointCount);
    q->pointIndex = n;
    q->unknown3 = 0;
    q->point = (void *)(Path_GetPointData((void *)q->definition) +
                       q->pointIndex * sizeof(*q->point));
}

extern const u16 gUnk_08605F30[];

s32 Spider_GetAnimationEvent(struct ChaseActor *s)
{
    struct DisplaySequenceState *sequence =
        &((struct SceneActorAnimationSequenceDrawable *)s->base.drawable)->sequence;
    if (sequence->elapsed == 0) {
        MonsterInit_SetStatusFlags(s, 0x10000);
        return gUnk_08605F30[sequence->index & 1];
    }
    return 0;
}

s32 SceneActorPlacement_BeginDefaultResponse(struct ChaseActor *p)
{
    if (p->base.reactionSoundVariant == 0)
        Sound_PlaySong(0x211);
    else
        Sound_PlaySong(0x112);
    if (p->base.hitReactionFlags & 0x40000) {
        u32 mask = 32;
        u16 *flags = &p->base.motionFlags;
        mask |= *flags;
        *flags = mask;
    }
    Chase_SetCtl(p, p->base.onReaction, 18);
    Chase_SetState(p, 3, 0, 0);
    {
        u32 *flags = &p->base.statusFlags;
        u32 mask = ~0x1000000;
        *flags &= mask;
    }
    {
        u16 *flags = &p->base.reactionFlags;
        s32 mask = (s32)0xFFFFF636;
        mask &= *flags;
        *flags = mask;
    }
    return 1;
}

u32 Spider_UpdatePreAction(u8 *p);

void SceneActorControl_DispatchControl(u8 *);

u32 Spider_OnUpdate(u8 *p)
{
    u32 i;
    u32 *t;
    Spider_UpdatePreAction(p);
    i = p[0x2a8];
    t = *(u32 **)(p + 0x27c);
    Callback_InvokeActor(p, t[i]);
    SceneActorControl_DispatchControl(p);
    return 1;
}

void SceneActor_UpdateBehaviorCallbacks(void *);

u32 Spider_OnSecondaryUpdate(void *actor)
{
    SceneActor_UpdateBehaviorCallbacks(actor);
    return 0;
}

void Sprite_Remove(void *);

void Anim_Remove(void *);

void Heap_Free(void *);

void TerrainOverlay_QueueRemoveActor(void *);

void SceneBody_Unregister(void *);

void Collider_Remove(void *);

void SceneActorRegistry_UnlinkActor(void *);

s32 Spider_Destroy(u8 *p)
{
    u8 *gfx = *(u8 **)(p + 4);
    if (!p[8])
        Sprite_Remove(gfx);
    else
        Anim_Remove(gfx + 0x20);
    Heap_Free(gfx);
    if (p[0x360])
        ScaledShadow_Destroy(p + 0x360);
    TerrainOverlay_QueueRemoveActor(p + 0x50);
    SceneBody_Unregister(p + 0x50);
    Collider_Remove(p + 0x134);
    SceneActorRegistry_UnlinkActor(p);
    return 0;
}

void *Heap_Alloc(s32 size); /* first-fit allocation from gHeapHead; NULL on failure */

void Memory_Clear(void *dst, s32 bytes); /* zero fill (bytes, then CpuSet words) */

void SceneActorRegistry_SetObjectContext(void *, void *, s32, s32);

void SceneActorReaction_SinkAndRemove(struct ChaseActor *, s32);

u32 SceneActorReaction_OnEnemyDefault(void);

void SceneActorReaction_ResolvePlacementDamage(void);

void SceneActorScript_OnCommand(void *actor, const u16 *command);

extern SceneActorCommandHandler const gUnk_086108B0[];

void AnimData_SelectPaletteBank(void *, s32);

void SceneActorPlacement_HideActor(void *, s32);

s32 Anim_Step(void *, void *);

void SceneActorBehavior_InstallCallbackTables(void *);

void SceneActorTerrain_ReadScriptParameters(void *);

u8 SceneActorRegistry_LinkActor(void *);

void Spider_InitProfile(u8 *p);

u32 Spider_OnUpdate(u8 *p);

s32 Spider_GetAnimationEvent(struct ChaseActor *s);

void Spider_AdvanceTurnAnimation(struct ChaseActor *actor);

void Spider_SelectActiveAnimation(u8 *p);

void Spider_SelectIdleAnimation(u8 *p);

void Spider_UpdateLandingMotion(struct ChaseActor *a, s32 frame);

void Spider_OnActionEvent(void);

void Spider_InitColliders(u8 *p);

void Spider_SetBehaviorTables(u8 *p);

s32 SceneActorPlacement_BeginDefaultResponse(struct ChaseActor *p);

u8 SceneActorPlacement_BeginNearbyPlayerContact(struct ChaseActor *a);

u8 SceneActorPlacement_SelectPlayerApproach(struct ChaseActor *a);

void SceneActorReaction_ResumeEntryTarget(struct ChaseActor *actor);

void SceneActorRoute_ResetStep(struct ChaseActor *actor);

void Spider_OnSecondaryColliderContact(void);

void Spider_UpdateHiddenRecovery(struct ChaseActor *a);

void ScaledShadow_Initialize(void *, void *, s32, s32, s32, s32, s32, s32, s32, s32, s32);

void Spider_UpdateDefeat(struct ChaseActor *actor, s32 frame);

void SceneActorReaction_BeginTargetRecovery(void);

u32 Spider_OnSecondaryUpdate(void *);

s32 Spider_Destroy(u8 *p);

extern const u8 gUnk_08605F34[];

static inline void MonsterInit_EnableInitialFlags(struct ChaseActor *arg0)
{
    MonsterInit_SetTargetingFlags(&arg0->base.tracking, 1);
    MonsterInit_SetBehaviorFlags(arg0, 0x400000);
    MonsterInit_SetStatusFlags(arg0, 8);
}

static inline void MonsterInit_InstallCompletionCallbacks(struct ChaseActor *arg0)
{
    arg0->base.onMessage = SceneActorScript_OnCommand;
    arg0->base.update = (SceneActorResultCallback)Spider_OnSecondaryUpdate;
    arg0->base.destroy = (SceneActorResultCallback)Spider_Destroy;
}

static inline void Spider_SetFlags(struct Spider *actor, u32 mask)
{
    actor->flags62C |= mask;
}

static inline void Spider_InstallContact(struct ChaseActor *actor,
                                         struct SceneActorChaseGroup *group)
{
    MonsterInit_WriteDataPointer(&actor->base.commandTable.opaque, gUnk_08605F34);
    group->common.gate.control = SceneActorReaction_ResumeEntryTarget;
    if (MonsterInit_GetPaletteOffset(actor) == 0)
        MonsterInit_WriteContactPredicate(&actor->base.contactQuery.predicate,
                                          SceneActorPlacement_BeginNearbyPlayerContact);
    else
        MonsterInit_WriteContactPredicate(&actor->base.contactQuery.predicate,
                                          SceneActorPlacement_SelectPlayerApproach);
}

s32 Spider_Init(void *object)
{
    struct Spider *arg0 = object;
    struct ChaseActor *actor = object;
    struct SceneActorChaseGroup *group = &arg0->chase.group;
    u32 zero; // concession-ok(const-local): CpuSet reads this source word for a fixed position fill.
    struct ContactPosition tmp;
    u32 flag;
    s32 extra;

    arg0->flags62C = 0;
    SceneActorRegistry_SetObjectContext(actor, actor, 0, 1);
    MonsterInit_SetCategory(actor, MONSTER_CATEGORY_SPIDER);
    Spider_InitProfile(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->base.drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->base.drawable, 0x5C);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_InitializeSprite(heap, 0x528F, 0xB6EE);
        if (MonsterInit_GetCategory(actor) == 0x11 || MonsterInit_GetCategory(actor) == 0x17)
            heap[7] = 1;
        actor->base.body.direction = 0;
    } else {
        u8 *heap;
        actor->base.drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->base.drawable, 0x80);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_ClearPosition(&zero, &tmp);
        MonsterInit_InitializeWidget(heap, 0xCB05, 0x528F, &tmp);
        actor->base.animationIndex = 0xFF;
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
        group, &arg0->routeProbe, &arg0->destination, Spider_OnSecondaryColliderContact, 0,
        SceneActorReaction_BeginTargetRecovery, 0, 1, &actor->base.body);
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
        MonsterInit_MaskColliderFlags(&actor->base.colliderA, ~0x100);
    if (Script_GetKeywordValue(0x53, 0) != 0 || !MonsterInit_IsInCurrentRegion(actor))
        SceneActorPlacement_HideActor(actor, 0);
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    if (MonsterInit_GetDrawableKind(actor) == 0)
        DisplaySequence_Step(actor->base.drawable);
    else
        Anim_Step(actor->base.drawable + 0x20, actor->base.drawable);
    MonsterInit_ClearReceiver(actor);
    SceneActorBehavior_InstallCallbackTables(actor);
    Spider_SetBehaviorTables(actor);
    if (extra != 0) {
        actor->base.heightOffset = extra;
        actor->base.field2E8 = 2;
        MonsterInit_SetControl(actor, Spider_UpdateHiddenRecovery, 0x23);
        MonsterInit_SetState(actor, 3, 0, 0);
        ScaledShadow_Initialize((u8 *)actor + 0x360, &actor->base.body.position, 0x40, 0x10, 0x60,
                                0x64, 0xA00, 1, 5, 1, 0);
        Spider_SetFlags(arg0, 1);
    } else {
        MonsterInit_SetControl(actor, SceneActorReaction_RestoreAlternateAfterDelay, 0xE);
        SceneActorDelay_Prepare((void *)&group->saved, actor->base.onAlternateEntry, 7, 0x3C);
    }
    arg0->timer62E = 0x12C;
    MonsterInit_SaveControl(MonsterInit_SavedControlOf(actor), actor);
    MonsterInit_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}

s32 Spider_Init(void *);

void Spider_Create(void)
{
    void *m = Heap_Alloc(sizeof(struct Spider));
    if (m != 0) {
        Memory_Clear(m, sizeof(struct Spider));
        if (Spider_Init(m) < 0) {
            Spider_Destroy(m);
            Heap_Free(m);
        }
    }
}
