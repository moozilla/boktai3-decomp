#include "scene/actor/scene_actor_drawable_flags.h"
#include "scene/motion/scene_actor_availability.h"
#include "global.h"
#include "effects/particles/scene_particle_burst_api.h"
#include "scene/motion/scene_actor_chase.h"
#include "scene/actor/scene_actor_reset.h"
#include "monsters/skeleton/skeleton.h"
#include "scene/actor/scene_actor_reaction.h"
#include "scene/motion/scene_actor_terrain.h"
#include "scene/motion/scene_actor_route.h"
#include "player/player.h"
#include "scene/actor/scene_actor_visibility.h"
#include "scene/actor/scene_actor_control_helpers.h"
#include "monsters/skeleton/scene_actor_timed_behavior.h"
#include "scene/actor/scene_actor_registry.h"
#include "objects/hazards/animated_hazard.h"
#include "scene/actor/scene_actor_palette.h"
#include "scene/actor/scene_actor_profile.h"
#include "engine/math/rotation_phase.h"
#include "scene/actor/scene_actor_script.h"
#include "scene/collision/scene_actor_contact_response.h"
#include "scene/collision/scene_actor_contact_list.h"
#include "scene/actor/scene_actor_behavior.h"
#include "scene/motion/scene_actor_chase_reaction.h"
#include "scene/actor/scene_actor_init_actor.h"
#include "engine/engine_globals.h"
#include "engine/ram.h"
#include "npc/manager/scene_actor_animation.h"
#include "scene/actor/scene_actor_control_phase.h"
#include "scene/motion/scene_actor_chase_shared.h"
#include "engine/engine.h"
#include "scene/motion/scene_actor_turn.h"
#include "engine/math/monster_particle_random.h"
#include "scene/motion/scene_actor_displacement.h"
#include "engine/word_flags.h"
#include "scene/actor/scene_actor_placement.h"
#include "scene/motion/scene_actor_terrain_reaction.h"
#include "bosses/vanargand/vanargand_projectiles.h"
#include "monsters/ghoul_disc.h"
#include "objects/hazards/falling_objects.h"
#include "scene/actor/scene_actor_part_burst.h"

u8 SceneActor_DispatchContextAction(void *, u32);

u8 Skeleton_RerouteToTarget(void *obj)
{
    struct ContactPosition target;
    struct ChaseActor *actor = obj;
    struct ChaseActor *alt = obj;
    struct SceneActorChaseGroup *group = &actor->group;
    struct SceneActorChaseTarget **slot = &actor->base.tracking.target;
    s32 cell;
    SceneActorChase_SnapToRoute(actor->group.destination, &actor->group.homeRoute);
    {
        struct SceneActorChaseTarget *t = *slot;
        cell = Chase_CellIndex(t->position.x >> 8, t->position.z >> 8);
    }
    if (cell == actor->base.terrainCell || Chase_Bits32(&gPlayers[0]->statusFlags, 0x20000)) {
        SceneActor_DispatchContextAction(actor->base.context, 0);
        goto done;
    }
    SceneActorTerrain_GetCenter(cell, &target);
    if (Chase_TryRoute(alt, group->route, &actor->base.body.position, &target))
        return 1;
    Callback_InvokeActor(actor, group->fallback);
done:
    return 0;
}

// foundry draft (worker best 96.2%); what differs is mostly register allocation and instruction order

#define SceneActorControl_EnterResetMode SceneActorPlacement_ResetActorState

void SceneActorPlacement_ResetActorState(struct ChaseActor *, const u16 *);

void SceneActorControl_RestartBehavior57(struct ChaseActor *actor)
{
    u16 mode;
    if (SceneActor_ActionsBlocked(actor))
        return;
    {
        if (SceneActorContact_MotionFlags(actor, 0x800))
            return;
    }
    if (actor->base.controlKind != 0x39)
        return;
    {
        u32 *flags = &actor->base.saved.privateFlags;
        *flags &= ~2;
    }
    {
        u32 *flags = &actor->base.behaviorFlags;
        u32 mask = ~3;
        *flags &= mask;
    }
    SceneActorRegistry_ResetRoute((void *)actor);
    mode = 1;
    SceneActorPlacement_ResetActorState(actor, &mode);
}


static inline u8 Chase(struct ChaseActor *actor, struct SceneActorChaseGroup *group, u32 bits)
{
    void *route = group->route;
    struct ContactPosition *position = &actor->base.body.position;
    struct SceneActorChaseTarget *target = actor->base.tracking.target;
    struct ContactPosition *destination = &target->position;
    s32 result;
    if (!Nav_IsPositionInMesh(route, position, destination)) {
        {
            actor->base.motionFlags &= ~bits;
        }
        return 0;
    } else {
        result = Terrain_ApplyBlocked(route, position, destination);
        if (result) {
            if (result != 2)
                Memory_Clear(&actor->base.body.motion, 8);
            return 1;
        } else {
            {
                actor->base.motionFlags &= ~bits;
            }
            return 0;
        }
    }
}

static inline u16 MotionFlags(struct ChaseActor *actor, u32 mask)
{
    u16 value = actor->base.motionFlags;
    value &= mask;
    return value;
}

static inline void OrFlags(u16 *flags, u32 mask)
{
    u32 value = mask;
    value |= *flags;
    *flags = value;
}

static inline u32 Flag(struct ChaseActor *a, u32 m, u32 off)
{
    return off == 0x2D4 ? *(u16 *)((u8 *)a + off) & m : *(u32 *)((u8 *)a + off) & m;
}

s32 Div(s32, s32);

static inline u8 Pending(u8 *context)
{
    u32 mask = 0x10;
    if (*(u32 *)(context + 0x6DC) & mask)
        return 1;
    return 0;
}

static inline s32 MotionSquared(struct ContactPosition *p)
{
    return p->x * p->x + p->y * p->y + p->z * p->z;
}

// confidence=low: starts pursuit when stationary, with a six-second cooldown.

void Skeleton_BeginStationaryPursuit(struct ChaseActor *a)
{
    struct Skeleton *context;
    struct SceneActorChaseGroup *group;
    if (SceneActor_ActionsBlocked(a))
        return;
    context = a->base.context;
    group = &a->group;
    if (Pending((u8 *)context))
        return;
    if (Flag(a, 0xA03F, 0x2D4))
        return;
    if (Flag(a, 0x100, 0x2D4))
        return;
    if (MotionSquared(&a->base.secondaryMotion))
        return;
    if (context->collisionDelay)
        return;
    if (context->timer6D8)
        return;
    SceneActorAnimation_SelectDirectionalEntry((void *)a, 0);
    a->base.tracking.target = (u32)(((u8 **)gPlayers)[0] + 0x24);
    a->base.field2C2 = Div(64, 3);
    if (MotionFlags(a, 0x80))
        OrFlags((u16 *)&group->common.gate.flags, 4);
    if (MotionFlags(a, 0x80)) {
        SceneActorTerrain_EnterState(a, 0, 0, 12);
        SceneActorChase_SetControl(a, group->common.gate.control, 0x1A);
        if (MotionFlags(a, 0x80)) {
            a->group.common.gate.counter += 300;
            if (a->group.common.gate.counter > a->group.common.gate.duration)
                a->group.common.gate.counter = a->group.common.gate.duration;
        }
    } else {
        SceneActorTerrain_EnterState(a, 0, 0, 7);
        SceneActorChase_SetControl(a, a->base.onAlternateEntry, 1);
    }
    group->destination->pad = 1;
    if (!Chase(a, group, 0x200))
        Callback_InvokeActor(a, group->fallback);
    {
        u16 *p = &context->timer6D8;
        u32 value = 360;
        *p = value;
    }
}

/* actor must be nonnull: its control context is loaded before the availability gate. */

void Skeleton_HandleVisibilityRequest(struct ChaseActor *input, const u16 *inputRequest)
{
    struct ChaseActor *actor = input;
    const u16 *request = inputRequest;
    struct Skeleton *state = actor->base.context;
    u32 unavailable = SceneActor_ActionsBlocked(actor);
    if (unavailable)
        return;
    {
        u32 mask = 0x4000;
        u16 *flags;
        flags = &actor->base.motionFlags;
        if (!(*flags & mask))
            return;
    }
    {
        u32 requested = request[1];
        if (requested == 0) {
            u32 mask = 0x4000;
            u32 *flags;
            u32 active;
            u32 *savedFlags;
            flags = &state->actionFlags;
            active = *flags & mask;
            savedFlags = flags;
            active = SceneActorControl_TestFlags(active, ~0);
            if (!active) {
                u32 hidden = SceneActorVisibility_IsHidden(actor);
                if (hidden) {
                    {
                        u16 *timer;
                        timer = &state->visibilityFrame;
                        *timer = 0;
                    }
                    {
                        u32 mask = ~0x18000;
                        {
                            u32 old = *savedFlags & mask;
                            *savedFlags = old | 0x4000;
                        }
                    }
                    Sound_PlaySong(0x21E);
                }
            }
        } else {
            u32 mask = 0x8000;
            u32 *flags;
            u32 active;
            u32 *savedFlags;
            flags = &state->actionFlags;
            active = *flags & mask;
            savedFlags = flags;
            active = SceneActorControl_TestFlags(active, ~0);
            if (!active) {
                u32 hidden = SceneActorVisibility_IsHidden(actor);
                if (!hidden) {
                    u32 hiddenZero = hidden;
                    {
                        u32 zero = hiddenZero;
                        u16 *timer;
                        timer = &state->visibilityFrame;
                        *timer = zero;
                    }
                    {
                        u32 mask = ~0x14000;
                        {
                            u32 old = *savedFlags & mask;
                            *savedFlags = old | 0x8000;
                        }
                    }
                    Sound_PlaySong(0x21E);
                }
            }
        }
    }
}

static inline void Hide(struct ChaseActor *actor)
{
    ActorStatus_SetDrawableFlags(actor, 1);
    ActorShadow_Disable(actor->base.shadow);
}

static inline void Show(struct ChaseActor *actor)
{
    ActorStatus_ClearDrawableFlags(actor, 1);
    ActorShadow_Enable(actor->base.shadow);
}

void Skeleton_UpdateVisibilityFlicker(struct ChaseActor *actor)
{
    struct Skeleton *state;
    u32 unavailable = SceneActor_ActionsBlocked(actor);
    if (unavailable)
        return;
    {
            u32 mask;
        u32 active;
        state = actor->base.context;
        mask = 0xC000;
        active = SceneActorControl_TestFlags(state->actionFlags, mask);
        if (!active)
            return;
    }
    {
        u32 counter;
        u32 value;
        {
            u16 *field;
            field = &state->visibilityFrame;
            counter = (u32)*field << 16;
        }
        value = counter >> 16;
        if (value <= 19) {
            if ((u8)(counter >> 18) & 1)
                Show(actor);
            else
                Hide(actor);
        } else if (value <= 29) {
            if ((u8)(counter >> 17) & 1)
                Show(actor);
            else
                Hide(actor);
        } else {
            u32 mask = 0x4000;
            u32 *inputFlags;
            u32 *flags;
            u32 active;
            inputFlags = &state->actionFlags;
            active = *inputFlags & mask;
            flags = inputFlags;
            active = SceneActorControl_TestFlags(active, ~0);
            if (active) {
                ActorStatus_ClearDrawableFlags(actor, 1);
                {
                    u32 mask = ~0x4000;
                    *flags &= mask;
                }
                ActorShadow_Enable(actor->base.shadow);
            } else {
                ActorStatus_SetDrawableFlags(actor, 1);
                {
                    u32 mask = ~0x8000;
                    *flags &= mask;
                }
                ActorShadow_Disable(actor->base.shadow);
            }
        }
    }
    {
        u16 *counter = &state->visibilityFrame;
        (*counter)++;
    }
}

void Skeleton_UpdateHopToOpenCell(struct ChaseActor *actor);

void SceneActorControl_BeginTimedBehavior56(struct ChaseActor *s,
                                            struct Skeleton *other)
{
    switch (s->base.category) {
        case MONSTER_CATEGORY_SKELETON_GUARD:
            other->actionCooldown = 60;
            break;
        case MONSTER_CATEGORY_SKELETON:
        case MONSTER_CATEGORY_SKELETON_ARMED:
            other->actionCooldown = 120;
            break;
        default:
            return;
    }
    {
        SceneActorChase_SetControl(s, Skeleton_UpdateHopToOpenCell, 0x38);
        SceneActorTerrain_EnterState(s, 3, 0, 0);
        {
            u16 *flags = &s->base.reactionFlags;
            u32 mask = 0xFFFFF636;
            *flags &= mask;
        }
    }
}



u8 SceneActorControl_CanTurnTowardTarget(struct ChaseActor *s)
{
    struct Skeleton *context = s->base.context;
    s32 x, y, z, angle;
    if (!s->base.tracking.target)
        return 0;
    if (context->actionCooldown)
        return 0;
    if (!SceneActorChase_TrackingFlags(&s->base.tracking, 0x10))
        return 0;
    {
        struct ContactPosition *v = &s->base.secondaryMotion;
        s32 sum;
        x = v->x;
        sum = x * x;
        x = v->y;
        sum += x * x;
        x = v->z;
        if (sum + x * x != 0)
            return 0;
    }
    if (s->base.tracking.distance > 0x24000)
        return 0;
    angle = ((struct SceneActorFacingTarget *)s->base.tracking.target)->heading -
            s->base.body.direction;
    if (angle > 128)
        angle -= 256;
    else if (angle < -128)
        angle += 256;
    x = angle;
    if (x < 0)
        x = -x;
    if (x > 63 && !context->collisionDelay)
        return 1;
    return 0;
}

void Skeleton_UpdateActionCooldown(struct ChaseActor *actor)
{
    struct Skeleton *context = actor->base.context;
    if (context->actionCooldown)
        --context->actionCooldown;
}

// confidence=low: restores the common Skeleton route and reaction state after snapshot recovery

extern const u8 gUnk_0824E250[];

void SceneActorReaction_ResetResponseState(void *, const void *, u32);

void SceneActorMovement_ResolveRoutePosition(void *);


static inline void clearRouteFlag(u32 *flags)
{
    u32 mask = ~0x1000000;
    *flags &= mask;
}


static inline void clearBits16(u16 *field, u32 bits)
{
    u32 mask = ~bits;
    mask &= *field;
    *field = mask;
}

void Skeleton_ResetRouteReaction(struct ChaseActor *actor)
{
    struct Skeleton *context = actor->base.context;
    context->collisionDelay = 0;
    context->actionCooldown = 0;
    context->actionFlags = actor->base.saved.privateFlags;
    Memory_Clear(actor->group.destination, 8);
    SceneActorReaction_ResetResponseState(&actor->group.saved.control, gUnk_0824E250, 0x120);
    actor->base.field2C2 = 64;
    actor->group.common.gate.flags = 0;
    actor->group.common.gate.counter = 0;
    {
        struct SceneActorRegistryControl *control;
        clearBits16(&actor->base.motionFlags, 0x380);
        {
            struct SceneActorChaseTracking *tracking = &actor->base.tracking;
            s32 mask = ~0x18;
            mask &= tracking->flags;
            tracking->flags = mask;
        }
        control = &actor->group.common.control;
        if (SceneActorTurn_ActorFlags(actor, 0x200000)) {
            clearRouteFlag(&actor->base.statusFlags);
            clearBits16(&actor->base.motionFlags, 0x200);
            control->mode = 0;
            control->counter = 0;
            control->timer = 0;
        }
        clearBits16(&actor->base.motionFlags, 0x400);
    }
    actor->group.turn.turnsRemaining = 255;
    SceneActorMovement_ResolveRoutePosition(actor);
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    if ((u8)SceneActorRoute_IsOutsidePointBounds((struct SceneActorRouteCursor *)&actor->group.homeRoute, &actor->base.body.position,
                                             32, 32))
        Chase_SetBehavior(actor, 2);
    else
        Chase_SetBehavior(actor, 0);
    {
        struct SceneActorAnimationTileDrawable *drawable = actor->base.drawable;
        Anim_Step(&drawable->tile, drawable);
    }
}

void SceneActorCollision_InitExternalBody(struct ChaseActor *, struct ContactCollider *, void *,
                                          void *, void *, void *, u32);

void SceneActorCollision_InitBody(struct ChaseActor *, void *, void *, void *, void *);

void SceneActorCollision_InitAttack(struct ChaseActor *, void *, void *, void *, u32, void *);

void SceneActorCollision_InitExternalAttack(struct ChaseActor *, struct ContactCollider *, void *,
                                            void *, void *, u32, void *);

void SceneActorReaction_TryStartContact(u8 *source, u16 *contact, struct ChaseActor *actor);

void SceneActor_TryForward(void *a, void *b, struct ChaseActor *actor);

void Skeleton_OnAttackCollision(u8 *unused, u8 *obj, u8 *ctx);

void Skeleton_OnContactEvent(u8 *unused, u8 *obj, u8 *ctx);

void SceneActorControl_InitBehaviorColliders(struct ChaseActor *actor)
{
    struct Skeleton *context = actor->base.context;
    union SceneActorRegistryPackedPosition bodySize, bodyOffset;
    union SceneActorRegistryPackedPosition innerSize, innerOffset;
    union SceneActorRegistryPackedPosition attackSize, attackOffset;
    union SceneActorRegistryPackedPosition extraSize, extraOffset;
    bodySize.packed.x = 0x80;
    bodySize.packed.y = 0x80;
    bodySize.packed.z = 0x80;
    *(u32 *)&bodyOffset = 0x800000;
    *(u32 *)((u8 *)&bodyOffset + 4) &= 0xFFFF0000;
    {
        u32 flags = 0x410B;
        SceneActorCollision_InitExternalBody(actor, &context->bodyCollider, &bodySize,
                                             &bodyOffset, SceneActorReaction_TryStartContact, actor,
                                             flags);
    }
    innerSize.packed.x = 0x80;
    innerSize.packed.y = 0x80;
    innerSize.packed.z = 0x80;
    *(u32 *)&innerOffset = 0x800000;
    *(u32 *)((u8 *)&innerOffset + 4) &= 0xFFFF0000;
    SceneActorCollision_InitBody(actor, &innerSize, &innerOffset, SceneActor_TryForward, actor);
    attackSize.packed.x = 0x40;
    attackSize.packed.y = 0x64;
    attackSize.packed.z = 0x40;
    *(u32 *)&attackOffset = 0x800000;
    *(u32 *)((u8 *)&attackOffset + 4) &= 0xFFFF0000;
    SceneActorCollision_InitAttack(actor, &attackSize, &attackOffset, Skeleton_OnAttackCollision, 0,
                                   actor);
    *(u32 *)&extraSize = 0x640000;
    *(u32 *)((u8 *)&extraSize + 4) &= 0xFFFF0000;
    *(u32 *)&extraOffset = 0x800000;
    *(u32 *)((u8 *)&extraOffset + 4) &= 0xFFFF0000;
    SceneActorCollision_InitExternalAttack(actor, &context->attackCollider, &extraSize,
                                           &extraOffset, Skeleton_OnContactEvent, 1, actor);
}

void Vorn_SpawnHazard(struct ChaseActor *actor, s32 index)
{
    struct Skeleton *context = actor->base.context;
    u16 b4 = actor->base.attacks.first[index];
    u16 bc = actor->base.attacks.second[index];
    u32 cc = actor->base.attacks.flags[index];
    u16 c4 = actor->base.attacks.parameter[index];

    Collider_SetAttackProperties(&context->attackCollider, b4, bc, 0, cc, c4);
}

#define gSineTable gUnk_086149C4

/* Descriptive aliases and object roles are tentative (confidence: low). */

static inline u32 ActorFlag(struct ChaseActor *actor, u32 input, u32 offset)
{
    u32 mask = input;
    return offset == 0x2D4 ? *(u16 *)((u8 *)actor + offset) & mask
                           : *(u32 *)((u8 *)actor + offset) & mask;
}

// confidence=low: updates Skeleton body and facing attack bounds while its control is active

static inline void SetBounds(struct ContactCollider *collider,
                             union SceneActorRegistryPackedPosition *bounds)
{
    collider->bounds = *(struct ContactPosition *)bounds;
}

void Skeleton_UpdateFacingColliders(struct ChaseActor *p)
{
    struct Skeleton *context = p->base.context;
    union SceneActorRegistryPackedPosition pos, bounds;
    u32 mask;
    if (SceneActor_ActionsBlocked(p))
        return;
    mask = 0x800;
    if (p->base.motionFlags & mask)
        return;
    if (p->base.controlKind == 0x39)
        return;
    if (p->base.controlKind == 0x3D)
        return;
    if (context->collisionDelay) {
        p->base.colliderA.flags |= 4;
        context->bodyCollider.flags |= 4;
        context->collisionDelay--;
    } else {
        p->base.colliderA.flags &= ~4;
        context->bodyCollider.flags &= ~4;
    }
    pos.position = p->base.body.position;
    Collider_SetPosition(&context->bodyCollider, &pos, 0);
    pos.packed.x += VanargandProjectile_Scale(gUnk_086149C4[(p->base.body.direction + 64) & 255] * 128);
    pos.packed.z += VanargandProjectile_Scale(gUnk_086149C4[p->base.body.direction] * 128);
    switch (((p->base.body.direction + 16) & 255) >> 6) {
        case 0:
        case 2:
            if (p->base.category == MONSTER_CATEGORY_SKELETON_GUARD) {
                bounds.packed.x = 180;
                bounds.packed.z = 84;
            } else {
                bounds.packed.x = 160;
                bounds.packed.z = 64;
            }
            break;
        case 1:
        case 3:
            if (p->base.category == MONSTER_CATEGORY_SKELETON_GUARD) {
                bounds.packed.x = 84;
                bounds.packed.z = 180;
            } else {
                bounds.packed.x = 64;
                bounds.packed.z = 160;
            }
            break;
    }
    bounds.packed.y = 100;
    SetBounds(&context->attackCollider, &bounds);
    Collider_SetPosition(&context->attackCollider, &pos, 0);
    Collider_SetPosition(&context->bodyCollider, &pos, 0);
}

void SceneActor_UpdateProfileForCategory(struct ChaseActor *actor)
{
    s32 category = actor->base.category;
    s32 count = SceneActorRegistry_CountLivingByCategory(category);

    actor->base.profileMode = count;

    switch (category) {
        case MONSTER_CATEGORY_SKELETON:
            actor->base.profileId = SceneActorProfiles_FindOrAdd(MONSTER_CATEGORY_SKELETON, count);
            break;
        case MONSTER_CATEGORY_SKELETON_GUARD:
            actor->base.profileId = SceneActorProfiles_FindOrAdd(MONSTER_CATEGORY_SKELETON_GUARD, count);
            break;
        case MONSTER_CATEGORY_SKELETON_ARMED:
            actor->base.profileId = SceneActorProfiles_FindOrAdd(MONSTER_CATEGORY_SKELETON_ARMED, count);
            break;
        case MONSTER_CATEGORY_SKELETON_SCRIPTED:
            actor->base.profileId = SceneActorProfiles_FindOrAdd(MONSTER_CATEGORY_SKELETON_SCRIPTED, count);
            break;
    }
}

void SceneActorControl_OnIdle(u8 *);

// confidence=low: distance band uses context bounds at 0x708 and 0x70C.


u32 Skeleton_TryEnterIdleRange(struct ChaseActor *s)
{
    struct Skeleton *context = s->base.context;
    if (s->base.tracking.distance > context->nearDistance &&
        s->base.tracking.distance < context->farDistance) {
        SceneActorChase_SetControl(s, SceneActorControl_OnIdle, 0x10);
        SceneActorChase_SetState(s, 0, 2, 0);
        return 1;
    }
    return 0;
}

u8 SceneActorReaction_IsRangeReady(struct ChaseActor *);

void Skeleton_SelectDistanceReaction(struct Skeleton *actor);

static inline u8 IsFlag512Set(struct Skeleton *context)
{
    u32 mask = 0x200;
    if (context->actionFlags & mask)
        return 1;
    return 0;
}

u8 SceneActorControl_TryBeginNearbyBehavior16(struct ChaseActor *s,
                                              struct Skeleton *context,
                                              s32 angle)
{
    if (!SceneActorReaction_IsRangeReady(s)) {
        if (IsFlag512Set(context))
            return 1;
        Skeleton_SelectDistanceReaction(s);
        return 1;
    }
    if (angle <= 24 && s->base.tracking.distance < context->distanceThresholds[0]) {
        SceneActorChase_SetControl(s, SceneActorControl_OnIdle, 0x10);
        SceneActorChase_SetState(s, 0, 2, 0);
        context->attempts = 0;
        return 1;
    }
    return 0;
}

static inline u8 TestBits(struct Skeleton *context, u32 mask)
{
    if (context->actionFlags & mask)
        return 1;
    return 0;
}


// confidence=low: category-six attack gate selects close, ranged or health-gated guard behavior

u8 Skeleton_TryBeginGuardedAttack(struct ChaseActor *actor, struct Skeleton *context,
                                  s32 angle)
{
    if (!SceneActorReaction_IsRangeReady(actor)) {
        if (TestBits(context, 0x200))
            return 1;
        Skeleton_SelectDistanceReaction(actor);
        return 1;
    }
    if (actor->base.tracking.distance < context->distanceThresholds[2] && context->timer6F4 <= 0) {
        if (angle <= 12 && actor->base.health <= context->healthThreshold) {
            SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0x10);
            SceneActorTerrain_EnterState(actor, 0, 2, 2);
            context->attempts = 0;
            return 1;
        }
        if (angle <= 24 && context->pursuitTimer <= 0) {
            SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0x10);
            SceneActorTerrain_EnterState(actor, 0, 2, 0);
            context->attempts = 0;
            return 1;
        }
    } else {
        if (angle <= 24 && actor->base.tracking.distance < context->distanceThresholds[0] &&
            context->pursuitTimer <= 0) {
            SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0x10);
            SceneActorTerrain_EnterState(actor, 0, 2, 0);
            context->attempts = 0;
            return 1;
        }
        if (angle <= 12 && actor->base.tracking.distance < context->distanceThresholds[1] &&
            context->timer6F2 <= 0) {
            SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0x10);
            SceneActorTerrain_EnterState(actor, 0, 2, 1);
            context->attempts = 0;
            return 1;
        }
    }
    return 0;
}

// confidence=low: category-seven angle/range gate chooses close or ranged behavior

u8 Skeleton_TryBeginArmedAttack(struct ChaseActor *actor, struct Skeleton *context,
                                s32 angle)
{
    if (actor->base.tracking.distance < context->nearDistance) {
        if (actor->base.health < context->healthThreshold || TestBits(context, 0x80)) {
            if (angle <= 24) {
                if (actor->base.tracking.distance < context->distanceThresholds[0] && context->pursuitTimer <= 0) {
                    SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0x10);
                    SceneActorTerrain_EnterState(actor, 0, 2, 0);
                    context->attempts = 0;
                    return 1;
                }
                if (angle <= 24 && context->timer6F2 <= 0) {
                    SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0x10);
                    SceneActorTerrain_EnterState(actor, 0, 2, 3);
                    SceneActorChase_AndWordFlags(&context->actionFlags, ~0x80);
                    context->attempts = 0;
                    return 1;
                }
            }
        }
        if (angle <= 24 && context->attempts) {
            SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0x10);
            SceneActorTerrain_EnterState(actor, 0, 2, 3);
            SceneActorChase_AndWordFlags(&context->actionFlags, ~0x80);
            context->attempts = 0;
            return 1;
        }
        Skeleton_SelectDistanceReaction(actor);
        return 1;
    }
    if (angle <= 20) {
        if ((actor->base.tracking.distance < context->distanceThresholds[1] && context->timer6F2 <= 0) ||
            context->attempts) {
            SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0x10);
            SceneActorTerrain_EnterState(actor, 0, 2, 3);
            context->attempts = 0;
            return 1;
        }
    }
    return 0;
}

s32 Math_Atan2(s32, s32);

u8 SceneActorControl_TryBeginNearbyBehavior16(struct ChaseActor *,
                                              struct Skeleton *, s32);

u8 Skeleton_TryBeginGuardedAttack(struct ChaseActor *, struct Skeleton *, s32);

u8 Skeleton_TryBeginArmedAttack(struct ChaseActor *, struct Skeleton *, s32);

void SceneActorControl_SelectBehaviorByTargetAngle(struct ChaseActor *s)
{
    struct Skeleton *context;
    struct ContactPosition delta;
    s32 angle;

    if (!(SceneActorContact_MotionFlags(s, 0x100))) {
        context = s->base.context;
        if (s->base.tracking.distance < s->base.tracking.range) {
            struct ContactPosition *difference = &delta;
            struct ContactPosition *from = &s->base.body.position;
            struct ContactPosition *to =
                &s->base.tracking.target->position;
            SceneActorTerrain_Subtract(difference, to, from);
            angle = RotationPhase_HeadingDistance(Math_Atan2(delta.x, difference->z),
                                                  s->base.body.direction);
            if (angle < s->base.tracking.heading) {
                if (s->base.category == MONSTER_CATEGORY_SKELETON) {
                    if (SceneActorControl_TryBeginNearbyBehavior16(s, context, angle))
                        return;
                } else if (s->base.category == MONSTER_CATEGORY_SKELETON_GUARD) {
                    if (Skeleton_TryBeginGuardedAttack(s, context, angle))
                        return;
                } else if (s->base.category == MONSTER_CATEGORY_SKELETON_ARMED) {
                    if (Skeleton_TryBeginArmedAttack(s, context, angle))
                        return;
                }
                if (IsFlag512Set(context))
                    return;
            } else {
                if (IsFlag512Set(context))
                    return;
            }
            Skeleton_SelectDistanceReaction(s);
        } else {
            if (IsFlag512Set(context))
                return;
            Skeleton_SelectDistanceReaction(s);
        }
    }
}

// foundry draft (worker best 93.0%); what differs is mostly register allocation and instruction order

void SceneActorControl_SelectBehaviorByTargetAngle(struct ChaseActor *);

static inline u8 ContextBlocked(struct Skeleton *p)
{
    u32 mask = 0x200;
    if (p->actionFlags & mask)
        return 1;
    return 0;
}

static inline void Or16(u16 *p, u32 mask)
{
    u32 value = mask;
    value |= *p;
    *p = value;
}

static inline void OrMotion(struct SceneActorChaseTracking *p, u32 mask)
{
    u32 v = mask;
    v |= p->flags;
    p->flags = v;
}

static inline void SetFlags(struct ChaseActor *p, u32 mask)
{
    u32 v = mask;
    v |= p->base.motionFlags;
    p->base.motionFlags = v;
}

void SceneActorReaction_UpdateGate(struct ChaseActor *p)
{
    struct Skeleton *context = p->base.context;
    u8 active = SceneActorChase_TrackingFlags(&p->base.tracking, 8);
    u32 v;
    u16 *f;
    if (active) {
        Or16(&p->group.common.gate.flags, 4);
        v = 0x80;
        f = &p->base.motionFlags;
        v |= *f;
        *f = v;
        p->group.common.gate.counter = p->group.common.gate.duration;
        if (SceneActorTurn_Flags(f, 0x100) || !SceneActorReaction_IsRangeReady(p) || context->collisionDelay) {
            if (!ContextBlocked(context))
                Skeleton_SelectDistanceReaction(p);
        } else {
            SceneActorControl_SelectBehaviorByTargetAngle(p);
        }
    } else {
        SceneActorChase_SetState(p, 0, 1, 0);
        SceneActorChase_SetControl(p, SceneActorControl_OnIdle, 0x10);
        OrMotion(&p->base.tracking, 8);
    }
}

void SceneActorTerrain_RequestRetreatRoute(struct ChaseActor *);

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

static inline u8 Chase_RestorePursuitRoute(struct ChaseActor *actor, struct SceneActorChaseGroup *group,
                                           u32 bits)
{
    struct SceneActorChaseTarget *target = actor->base.tracking.target;
    void *route = group->route;
    struct ContactPosition *position = &actor->base.body.position;
    struct ContactPosition *destination = &target->position;
    s32 result;
    if (!Nav_IsPositionInMesh(route, position, destination)) {
        {
            actor->base.motionFlags &= ~bits;
        }
        return 0;
    } else {
        result = Terrain_ApplyBlocked(route, position, destination);
        if (result) {
            if (result != 2)
                Memory_Clear(&actor->base.body.motion, 8);
            return 1;
        } else {
            {
                actor->base.motionFlags &= ~bits;
            }
            return 0;
        }
    }
}

// confidence=low: category-independent callback restores route target and resumes pursuit

void Skeleton_RestorePursuitRoute(struct ChaseActor *actor)
{
    struct Skeleton *context = actor->base.context;
    struct SceneActorChaseGroup *group = &actor->group;
    struct ContactPosition *destination;
    if (MotionFlags(actor, 0x100) || context->collisionDelay)
        return;
    if (MotionFlags(actor, 0x80)) {
        SceneActorTerrain_EnterState(actor, 0, 0, 12);
        SceneActorChase_SetControl(actor, group->common.gate.control, 0x1A);
        if (MotionFlags(actor, 0x80)) {
            group->common.gate.counter += 300;
            if (group->common.gate.counter > group->common.gate.duration)
                group->common.gate.counter = group->common.gate.duration;
        }
        OrFlags((u16 *)&group->common.gate.flags, 4);
    } else {
        struct SceneActorReactionCommand *command;
        SceneActorTerrain_EnterState(actor, 3, 1, 0);
        SceneActorChase_SetControl(actor, SceneActorReaction_RestoreAfterDelay, 0xE);
        {
            u32 mask = 0x10;
            actor->base.behaviorFlags |= mask;
        }
        actor->base.field2C2 = 0x20;
        command = (void *)((u8 *)actor + 0x460);
        {
            void *payload = actor->base.onAlternateEntry;
            u32 kind = 7;
            command->duration = 60;
            command->payload = payload;
            command->first = 0;
            command->second = 0;
            command->kind = kind;
        }
    }
    destination = group->destination;
    {
        struct SceneActorRegistryRoute *link = &group->homeRoute;
        destination->x = link->point->x;
        destination->z = link->point->z;
    }
    destination->y = CellHeight(destination, CellIndex(destination));
    {
        (group->destination)->pad = 0;
        if (!Chase_RestorePursuitRoute(actor, group, 0x200))
            Callback_InvokeActor(actor, group->fallback);
    }
}

#define gSceneActorReactionCategory7TileParameters gUnk_08606078

#define gSceneActorReactionDefaultTileParameters gUnk_08606058

extern const u16 gUnk_08606078[2][8];

extern const u16 gUnk_08606058[2][8];


u16 SceneActorReaction_SelectTileParameter(struct ChaseActor *actor)
{
    struct SceneActorAnimationTileDrawable *drawable = actor->base.drawable;
    struct AnimSprite *tile = &drawable->tile;
    struct AnimSprite *defaultTile = tile;
    u32 variant;
    if (tile->elapsed == 0) {
        u16 mask = 0x80;
        variant = (u16)(actor->base.motionFlags & mask) != 0;
        MonsterInit_SetStatusFlags(actor, 0x10000);
        if (actor->base.category == MONSTER_CATEGORY_SKELETON_ARMED) {
            return gSceneActorReactionCategory7TileParameters[variant][tile->frameIndex & 7];
        } else {
            return gSceneActorReactionDefaultTileParameters[variant][defaultTile->frameIndex & 7];
        }
    }
    return 0;
}

s32 SceneActorTerrain_GetNeighborCell(struct SceneActorTerrainView *actor, s32 direction);

s32 SceneActorTerrain_FindOpenNeighborDirection(struct ChaseActor *actor, s32 angle)
{
    s32 direction = ((angle + 0x10) & 0xff) >> 6;
    s32 opposite = (direction + 2) & 3;
    u8 mask;
    s32 i;
    i = 0;
    do {
        mask = 3;
    } while (0);
    for (; i <= 3; i++) {
        if (direction != opposite) {
            if (SceneActorTerrain_GetNeighborCell((struct SceneActorTerrainView *)&actor->base, direction) >= 0)
                return direction;
        }
        direction = (direction + 1) & mask;
    }
    return -1;
}

static inline s32 NextValue(void)
{
    if (Script_GetPc())
        return Script_GetValue();
    return 0;
}

static inline void SetContextFlags(struct SceneActorScriptPropertiesView *p, u32 mask)
{
    p->flags6DC |= mask;
}

void SceneActorReaction_ParseOptions(struct ChaseActor *actor)
{
    struct SceneActorScriptPropertiesView *context = actor->base.context;
    s32 mode;
    s32 value;
    if (Script_SeekToKeyword('E')) {
        context->parameter6D4 = NextValue();
        context->parameter6D6 = NextValue();
    }
    if (Script_SeekToKeyword('d')) {
        mode = NextValue();
        context->parameter6D2 = NextValue();
        if (Script_GetPc())
            value = Script_GetValue();
        else
            goto mode_flags;
    } else {
        mode = 0;
        value = -1;
    }
    if (value >= 0) {
        SetContextFlags(context, 0x800);
        context->parameter702 = value;
    }
mode_flags:
    if (mode) {
        u32 flags;
        switch (mode) {
            case 1:
                flags = 0xA;
                break;
            case 2:
                flags = 0x1000;
                break;
            case 3:
                flags = 0x100A;
                break;
            default:
                return;
        }
        SetContextFlags(context, flags);
    }
}

void Skeleton_InitVariantMotion(void *pIn)
{
    struct ChaseActor *actor = pIn;
    struct Skeleton *owner = actor->base.context;
    switch (actor->base.category) {
        case 5:
            owner->distanceThresholds[0] = 80000;
            owner->nearDistance = 80000;
            owner->farDistance = actor->base.tracking.range;
            break;
        case 6:
            owner->distanceThresholds[0] = 102400;
            owner->distanceThresholds[1] = actor->base.tracking.range;
            owner->distanceThresholds[2] = 85535;
            owner->nearDistance = 85535;
            owner->farDistance = owner->distanceThresholds[1];
            break;
        case 7:
            owner->distanceThresholds[0] = 102400;
            owner->distanceThresholds[1] = actor->base.tracking.range;
            owner->nearDistance = Div((s32)actor->base.tracking.range * 6, 10);
            owner->farDistance = owner->distanceThresholds[1];
            break;
    }
}

static inline void SetEntry(u32 value, u32 flags, struct SceneActorProfileEntry *entry)
{
    entry->value = value;
    entry->flags = flags;
}

// confidence=low: writes the animation/action profile shared by the three Skeleton categories

void Skeleton_InitializeCategoryProfile(struct ChaseActor *actor)
{
    s32 category = actor->base.category;
    struct SceneActorProfileEntry *entry = (void *)SceneActorProfiles_GetEntryAddress(
        actor->base.profileId, actor->base.profileMode, 0);
    if (!entry)
        return;
    switch (category) {
        case MONSTER_CATEGORY_SKELETON:
            SetEntry(0x0, 0x0, entry + 0);
            SetEntry(0x6, 0x0, entry + 1);
            SetEntry(0xC, 0x0, entry + 5);
            SetEntry(0x18, 0x0, entry + 2);
            SetEntry(0x2D, 0x0, entry + 14);
            SetEntry(0x2F, 0x0, entry + 15);
            SetEntry(0x31, 0x0, entry + 16);
            SetEntry(0x3F, 0x10, entry + 11);
            SetEntry(0x45, 0x0, entry + 6);
            SetEntry(0x3F, 0x0, entry + 12);
            SetEntry(0x45, 0x4, entry + 7);
            SetEntry(0x51, 0x0, entry + 3);
            SetEntry(0x61, 0x0, entry + 10);
            SetEntry(0x57, 0x0, entry + 17);
            SetEntry(0x59, 0x0, entry + 18);
            SetEntry(0x5B, 0x0, entry + 13);
            break;
        case MONSTER_CATEGORY_SKELETON_GUARD:
            SetEntry(0x2, 0x0, entry + 0);
            SetEntry(0x8, 0x0, entry + 1);
            SetEntry(0xE, 0x0, entry + 5);
            SetEntry(0x1B, 0x0, entry + 2);
            SetEntry(0x33, 0x0, entry + 14);
            SetEntry(0x35, 0x0, entry + 15);
            SetEntry(0x37, 0x0, entry + 16);
            SetEntry(0x41, 0x10, entry + 11);
            SetEntry(0x49, 0x0, entry + 6);
            SetEntry(0x41, 0x0, entry + 12);
            SetEntry(0x49, 0x4, entry + 7);
            SetEntry(0x53, 0x0, entry + 3);
            SetEntry(0x61, 0x0, entry + 10);
            SetEntry(0x69, 0x0, entry + 19);
            SetEntry(0x6B, 0x0, entry + 20);
            SetEntry(0x6D, 0x0, entry + 21);
            SetEntry(0x6F, 0x0, entry + 22);
            SetEntry(0x71, 0x0, entry + 23);
            SetEntry(0x57, 0x0, entry + 17);
            SetEntry(0x59, 0x0, entry + 18);
            SetEntry(0x5D, 0x0, entry + 13);
            break;
        case MONSTER_CATEGORY_SKELETON_ARMED:
            SetEntry(0x4, 0x0, entry + 0);
            SetEntry(0xA, 0x0, entry + 1);
            SetEntry(0x10, 0x0, entry + 5);
            SetEntry(0x21, 0x0, entry + 2);
            SetEntry(0x39, 0x0, entry + 14);
            SetEntry(0x3B, 0x0, entry + 15);
            SetEntry(0x3D, 0x0, entry + 16);
            SetEntry(0x43, 0x10, entry + 11);
            SetEntry(0x4D, 0x0, entry + 6);
            SetEntry(0x4F, 0x4, entry + 7);
            SetEntry(0x43, 0x0, entry + 12);
            SetEntry(0x55, 0x0, entry + 3);
            SetEntry(0x61, 0x0, entry + 10);
            SetEntry(0x12, 0x0, entry + 19);
            SetEntry(0x14, 0x0, entry + 20);
            SetEntry(0x16, 0x0, entry + 21);
            SetEntry(0x75, 0x0, entry + 22);
            SetEntry(0x77, 0x0, entry + 23);
            SetEntry(0x57, 0x0, entry + 17);
            SetEntry(0x59, 0x0, entry + 18);
            SetEntry(0x5F, 0x0, entry + 13);
            break;
    }
}

static inline void SetEntry_SetProfileEntries(u32 value, struct SceneActorProfileEntry *p)
{
    p->value = value;
    p->flags = value >> 8;
}

void SceneActorReaction_SetProfileEntries(struct ChaseActor *actor)
{
    s32 category = actor->base.category;
    struct SceneActorProfileEntry *entry = (void *)SceneActorProfiles_GetEntryAddress(
        actor->base.profileId, actor->base.profileMode, 0);
    u32 mask = 0x280;
    u16 flags = actor->base.motionFlags;
    u32 special = mask & flags;
    if (special) {
        switch (category) {
            case MONSTER_CATEGORY_SKELETON:
                SetEntry_SetProfileEntries(0x63, entry);
                SetEntry_SetProfileEntries(0x27, entry + 1);
                break;
            case MONSTER_CATEGORY_SKELETON_GUARD:
                SetEntry_SetProfileEntries(0x65, entry);
                SetEntry_SetProfileEntries(0x29, entry + 1);
                SetEntry_SetProfileEntries(0x1E, entry + 2);
                break;
            case MONSTER_CATEGORY_SKELETON_ARMED:
                SetEntry_SetProfileEntries(0x67, entry);
                SetEntry_SetProfileEntries(0x2B, entry + 1);
                SetEntry_SetProfileEntries(0x24, entry + 2);
                break;
        }
    } else {
        switch (category) {
            case MONSTER_CATEGORY_SKELETON:
                SetEntry_SetProfileEntries(0, entry);
                SetEntry_SetProfileEntries(6, entry + 1);
                break;
            case MONSTER_CATEGORY_SKELETON_GUARD:
                if (flags & 0x200) {
                    SetEntry_SetProfileEntries(0x65, entry);
                    SetEntry_SetProfileEntries(0x29, entry + 1);
                    SetEntry_SetProfileEntries(0x1E, entry + 2);
                } else {
                    SetEntry_SetProfileEntries(2, entry);
                    SetEntry_SetProfileEntries(8, entry + 1);
                    SetEntry_SetProfileEntries(0x1B, entry + 2);
                }
                break;
            case MONSTER_CATEGORY_SKELETON_ARMED:
                if ((u16)(flags & 0x200)) {
                    SetEntry_SetProfileEntries(0x67, entry);
                    SetEntry_SetProfileEntries(0x2B, entry + 1);
                    SetEntry_SetProfileEntries(0x24, entry + 2);
                } else {
                    SetEntry_SetProfileEntries(4, entry);
                    SetEntry_SetProfileEntries(0xA, entry + 1);
                    SetEntry_SetProfileEntries(0x21, entry + 2);
                }
                break;
        }
    }
}

static inline u8 ContextHasFlag(struct SceneActorScriptPropertiesView *p, u32 mask)
{
    if (p->flags6DC & mask)
        return 1;
    return 0;
}

u8 SceneActorReaction_IsRangeReady(struct ChaseActor *actor)
{
    struct SceneActorScriptPropertiesView *context = actor->base.context;
    switch (actor->base.paletteOffset) {
        case 0:
        case 5:
        case 12:
            if (actor->base.category == MONSTER_CATEGORY_SKELETON) {
                if (context->timer6F0 <= 0)
                    return 1;
            } else if (actor->base.category == MONSTER_CATEGORY_SKELETON_ARMED) {
                if (actor->base.health <
                        ((struct SceneActorReactionHealthTail *)context)->healthThreshold ||
                    ContextHasFlag(context, 0x80)) {
                    if (context->timer6F0 <= 0)
                        return 1;
                } else {
                    if (context->timer6F2 <= 0)
                        return 1;
                }
            } else {
                if (actor->base.tracking.distance < context->distanceThreshold6E0 &&
                    context->timer6F0 <= 0)
                    return 1;
                if (actor->base.tracking.distance < context->distanceThreshold6E4 &&
                    context->timer6F2 <= 0)
                    return 1;
                if (actor->base.tracking.distance < context->distanceThreshold6E8 &&
                    context->timer6F4 <= 0)
                    return 1;
            }
            break;
    }
    return 0;
}

void Skeleton_TickActionCooldowns(u8 *actor)
{
    u8 *p = *(u8 **)(actor + 0x3D0);
    s16 *t;
    t = (s16 *)(p + 0x6F0);
    if (*t > 0)
        (*t)--;
    t = (s16 *)(p + 0x6F2);
    if (*t > 0)
        (*t)--;
    t = (s16 *)(p + 0x6F4);
    if (*t > 0)
        (*t)--;
    t = (s16 *)(p + 0x6F6);
    if (*t > 0)
        (*t)--;
}

// confidence=low: neighboring Skeleton controls establish provisional role

s32 Mod(s32, s32);

u8 SceneActorTerrain_CanTraverseCell(u32, u32, u32, u32);

static inline u32 CellShape(u32 cell)
{
    u8 *data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data += 4;
    else
        data = gTerrainMap->cells + (cell * 4 + 12);
    return *data >> 4;
}

static inline u8 TryRoute(struct ChaseActor *actor, void *route, struct ContactPosition *position,
                          struct ContactPosition *destination)
{
    s32 result;
    if (Nav_IsPositionInMesh(route, position, destination)) {
        result = Terrain_ApplyBlocked(route, position, destination);
        if (result) {
            if (result != 2)
                Memory_Clear(&actor->base.body.motion, 8);
            return 1;
        }
    }
    {
        u16 *flags = &actor->base.motionFlags;
        u32 mask = ~0x200;
        mask &= *flags;
        *flags = mask;
    }
    return 0;
}

// confidence=low: probes neighboring cells in facing order and routes toward a retreat cell

static inline u32 SelectRetreat(struct ChaseActor *actor, u32 priorDirection, s32 steps)
{
    struct ContactPosition center;
    s32 tries = 0;
    u32 cell, height, shape;
    s32 chosen, next, i;
    void **route;

    do {
        cell = actor->base.terrainCell;
        height = CellHeight(&actor->base.body.position, cell);
        shape = CellShape(cell);
        switch (Ghoul_QuarterDirection(actor->base.body.direction + 32) & 3) {
            case 0:
                next = cell + 1;
                i = 0;
                route = &actor->group.route;
                for (; i < steps; i++) {
                    if (!SceneActorTerrain_CanTraverseCell(cell, next, height, shape)) {
                        chosen = next - 1;
                        goto probed;
                    }
                    next++;
                }
                chosen = next - 1;
                break;
            case 1:
                next = cell + SceneActorTerrain_Width();
                i = 0;
                route = &actor->group.route;
                for (; i < steps; i++) {
                    if (!SceneActorTerrain_CanTraverseCell(cell, next, height, shape)) {
                        chosen = next - SceneActorTerrain_Width();
                        goto probed;
                    }
                    next += SceneActorTerrain_Width();
                }
                chosen = next - SceneActorTerrain_Width();
                break;
            case 2:
                next = cell - 1;
                i = 0;
                route = &actor->group.route;
                for (; i < steps; i++) {
                    if (!SceneActorTerrain_CanTraverseCell(cell, next, height, shape)) {
                        chosen = next + 1;
                        goto probed;
                    }
                    next--;
                }
                chosen = next + 1;
                break;
            case 3:
                next = cell - SceneActorTerrain_Width();
                i = 0;
                route = &actor->group.route;
                for (; i < steps; i++) {
                    if (!SceneActorTerrain_CanTraverseCell(cell, next, height, shape)) {
                        chosen = next + SceneActorTerrain_Width();
                        goto probed;
                    }
                    next -= SceneActorTerrain_Width();
                }
                chosen = next + SceneActorTerrain_Width();
                break;
            default:
                chosen = actor->base.terrainCell;
                route = &actor->group.route;
                break;
        }
    probed:
        if (chosen != actor->base.terrainCell)
            goto selected;
        actor->base.body.direction += 64;
        tries++;
    } while (tries <= 3);
selected:
    actor->base.body.direction = priorDirection;
    SceneActorTerrain_GetCenter(chosen, &center);
    {
        s16 groupOffset = (u32) & ((struct ChaseActor *)0)->group.common;
        u8 *group = (u8 *)actor + groupOffset;
        if (TryRoute(actor, *route, &actor->base.body.position, &center))
            return 1;
        Callback_InvokeActor(actor, *(void **)(group + 0x224));
    }
    return 0;
}

u32 Skeleton_TrySelectRetreatCell(struct ChaseActor *actor, u32 priorDirection)
{
    return SelectRetreat(actor, priorDirection, 1);
}

/* Skeleton variant of SceneActorTarget_ResolvePendingObject that also clears state bits 0x300 */

// confidence=low: resolves the pending contact object and clears Skeleton context state bits.

void SceneActorTarget_ResetContactResponse(struct RoomActor *);

static inline u8 BoolOf(u32 v)
{
    if (v)
        return 1;
    return 0;
}

s32 SceneActorTarget_ResolvePendingObjectSkeleton(struct ChaseActor *actor)
{
    struct ChaseActor *owner = actor;
    struct SceneActorChaseTracking *state;
    s32 *pendingId = &owner->base.counter304;
    if (*pendingId > 0) {
        struct ContactPosition delta;
        struct ContactPosition *target;
        s32 x, z;
        u32 cell;
        SceneActorTarget_ResetContactResponse((void *)owner);
        {
            u16 *flags = &owner->group.common.gate.flags;
            u32 mask;
            mask = ~2;
            mask &= *flags;
            *flags = mask;
        }
        {
            u32 mask = 0x80;
            u16 *flags;
            flags = &owner->base.reactionFlags;
            mask |= *flags;
            *flags = mask;
        }
        state = &owner->base.tracking;
        {
            u32 mask = 0x10;
            state->flags = (*(&mask)) | state->flags;
        }
        {
            struct SceneActorChaseTarget *object = SceneActorContacts_FindObjectById(*pendingId);
            state->target = object;
            x = object->position.x >> 8;
            z = object->position.z >> 8;
        }
        if ((((x < 0) || (z < 0)) || (((u32)x) >= gTerrainWidth)) || (((u32)z) >= gTerrainHeight)) {
            cell = 0;
        } else {
            cell = gTerrainMap->rows[z] + x;
        }
        SceneActorContact_SaveTargetCell(owner, cell);
        {
            struct ContactPosition *out = &delta;
            struct ContactPosition *source = &owner->base.body.position;
            target = &state->target->position;
            {
                u32 xDelta = (u16)target->x;
                struct ContactPosition *first;
                xDelta -= (u16)source->x;
                first = &delta;
                first->x = xDelta;
            }
            out->y = (u16)target->y - (u16)source->y;
            out->z = (u16)target->z - (u16)source->z;
            {
                struct ContactPosition *first = &delta;
                u32 direction;
                u8 *field;
                direction = Math_Atan2(first->x, out->z);
                field = &owner->base.body.direction;
                *field = direction;
            }
        }
        {
            struct Skeleton *context = owner->base.context;
            u32 *sf = &context->actionFlags;
            u32 mask = 0xFFFFFCFF;
            *sf &= mask;
        }
        return 1;
    } else {
        struct SceneActorChaseTracking *link = &owner->base.tracking;
        u32 mask = 2;
        u32 pending = link->flags & mask;
        state = link;
        if (BoolOf(pending)) {
            u32 mask = ~0x10;
            mask &= state->flags;
            state->flags = mask;
        }
        return 0;
    }
}

static inline s32 SceneActorScript_GetValue(void)
{
    if (Script_GetPc())
        return Script_GetValue();
    return 0;
}

void SceneActorScript_ReadPParameters(struct ChaseActor *actor)
{
    struct SceneActorScriptPropertiesView *context = actor->base.context;

    if (Script_SeekToKeyword('P')) {
        s32 i;

        for (i = 0; i <= 3; i++)
            context->parameters6F8[i] = Script_GetPc() ? Script_GetValue() : 0;

        context->parameter706 = SceneActorScript_GetValue();
    }
}

u32 Skeleton_TrySelectRetreatCell(struct ChaseActor *, u32);

static inline void Skeleton_CountDistanceSelection(struct Skeleton *context)
{
    u8 count = context->attempts;
    ++count;
    context->attempts = count;
}

static inline u8 Skeleton_TargetHeading(struct ChaseActor *actor)
{
    return actor->base.tracking.heading;
}

static inline u8 Skeleton_FaceAwayFromTarget(struct ChaseActor *actor)
{
    u8 priorDirection = MonsterInit_GetDirection(actor);
    actor->base.body.direction = Skeleton_TargetHeading(actor) + 0x80;
    return priorDirection;
}

static inline u8 Skeleton_TryRetreatFromTarget(struct ChaseActor *actor)
{
    u8 priorDirection = Skeleton_FaceAwayFromTarget(actor);
    return Skeleton_TrySelectRetreatCell(actor, priorDirection);
}

static inline struct Skeleton *Skeleton_SelectionContext(struct ChaseActor *actor)
{
    return actor->base.context;
}

void Skeleton_SelectDistanceReaction(struct Skeleton *actor)
{
    struct Skeleton *context = Skeleton_SelectionContext(&actor->chase);
    struct SceneActorChaseGroup *group = &actor->chase.group;
    SceneActorReaction_ResetResponseState(&group->saved, gUnk_0824E250, 0x4F5);
    Skeleton_CountDistanceSelection(context);
    if (actor->chase.base.tracking.distance < context->nearDistance) {
        if (!Skeleton_TryRetreatFromTarget(&actor->chase))
            return;
        SceneActorChase_SetTypedControl(&actor->chase, SceneActorControl_OnIdle, 0x10);
        SceneActorChase_SetPhase(&actor->chase, 0, 15);
    } else if (actor->chase.base.tracking.distance > context->farDistance) {
        if (!actor->chase.base.tracking.target)
            SceneActorChase_TrackPlayer(&actor->chase);
        if (SceneActorChase_Arrive(&actor->chase)) {
            SceneActorChase_SetTypedControl(&actor->chase, group->common.gate.control, 0x1A);
            SceneActorChase_SetPhase(&actor->chase, 0, 12);
        }
    } else {
        if (!actor->chase.base.tracking.target)
            SceneActorChase_TrackPlayer(&actor->chase);
        if (SceneActorChase_Arrive(&actor->chase)) {
            SceneActorChase_SetTypedControl(&actor->chase, group->common.gate.control, 0x1A);
            SceneActorChase_SetPhase(&actor->chase, 0, 12);
        }
    }
}

/* Skeleton_SelectRandomBehavior: Selects a different random eight-byte behavior entry and resets its cursor; behavior interpretation is tentative */


u8 *Path_GetPointData(u8 *);

void Skeleton_SelectRandomBehavior(struct ChaseActor *actor)
{
    struct SceneActorRouteCursor *p = (void *)&actor->group.homeRoute;

    u8 n;
    n = Mod(FallingObject_Random(), p->definition->pointCount);
    if (n == p->pointIndex)
        n = Mod(n + 1, p->definition->pointCount);
    p->pointIndex = n;
    p->unknown3 = 0;
    p->point = (struct SceneActorRoutePoint *)Path_GetPointData((void *)p->definition) + p->pointIndex;
}

void SceneActorScript_ReadCallScriptConfig(struct ChaseActor *actor)
{
    struct Skeleton *context = actor->base.context;
    s32 found = Script_SeekToKeyword('x');
    s32 a;
    s32 b;

    if (found) {
        if (Script_GetPc())
            a = Script_GetValue();
        else
            a = 0;
        context->callScriptId = a;

        if (Script_GetPc())
            b = Script_GetValue();
        else
            b = 0;
        context->callLimit = b;
        context->callCount = 0;
    } else {
        context->callScriptId = 0;
        context->callLimit = 0;
        context->callCount = 0;
    }
}

void SceneActorScript_TriggerCallOnCount(struct ChaseActor *actor)
{
    s32 zero = 0;
    struct Skeleton *context = actor->base.context;
    u16 *script = &context->callScriptId;

    if (*script != 0) {
        if (++context->callCount >= context->callLimit) {
            Script_Call(*script, zero);
            *script = 0;
            context->callLimit = 0;
            context->callCount = 0;
        }
    }
}

/* SceneActorReaction_TrySink: when a live, unblocked actor stands on a sinking cell (cell flag 0x40) at or below the ground height, hands control to its sink callback (default SceneActorReaction_SinkAndRemove, kind 0x16) in state 3 and marks it sinking (flags2D0 |= 0x20000); returns whether it did (provisional name) */

void SceneActorReaction_SinkAndRemove(struct ChaseActor *, s32);

static inline u8 Blocked(struct ChaseActor *actor)
{
    if (!actor)
        goto zero;
    {
        if (SceneActorContact_MotionFlags(actor, 0x40))
            goto one;
    }
    {
        if (SceneActorChase_StatusFlags(actor, 0x20000))
            goto one;
    }
    if (actor->base.health > 0)
        goto zero;
one:
    return 1;
zero:
    return 0;
}

static inline void SetState_TrySink(struct ChaseActor *actor, u32 state)
{
    actor->base.state2A8 = state;
    actor->base.phase2A9 = 0;
    actor->base.behavior2AA = 0;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}

u8 SceneActorReaction_TrySink(struct ChaseActor *actor)
{
    if (actor && !Blocked(actor) && actor->base.enabled == 1) {
        u16 cellFlags = *(u16 *)(SceneActorTerrain_GetCell(actor->base.terrainCell) + 2);
        if (cellFlags & 0x40 && actor->base.body.position.y + (s16)actor->base.heightOffset <=
                                    actor->base.groundHeight) {
            void *sink = actor->base.sinkCallback;
            if (!sink)
                SceneActorChase_SetControl(actor, SceneActorReaction_SinkAndRemove, 0x16);
            else
                SceneActorChase_SetControl(actor, sink, 0x16);
            SetState_TrySink(actor, 3);
            {
                u32 mask = 0x20000;
                actor->base.statusFlags |= mask;
            }
            return 1;
        }
    }
    return 0;
}

void Skeleton_UpdateVisibilityFlicker(struct ChaseActor *);

void SceneActorReaction_SetProfileEntries(struct ChaseActor *);

void Skeleton_TickActionCooldowns(u8 *actor);

s32 SceneActorReaction_TickHiddenResponse(struct ChaseActor *actor)
{
    struct ChaseActor *saved = actor;
    u32 mask = 0x4000;
    u16 *field;
    field = &actor->base.motionFlags;
    if (*field & mask) {
        u32 mask = 0x10000;
        u32 *input;
        u32 *flags;
        u32 enabled;
        input = &((struct Skeleton *)actor)->actionFlags;
        enabled = *input & mask;
        actor = (void *)input;
        flags = (void *)actor;
        if (SceneActorControl_TestFlags(enabled, ~0)) {
            ActorStatus_SetDrawableFlags(saved, 1);
            {
                u32 mask = ~0xC000;
                *flags &= mask;
            }
        } else {
            Skeleton_UpdateVisibilityFlicker(saved);
        }
    } else {
        struct Skeleton *context = (void *)actor;
        u16 *timer;
        SceneActorReaction_SetProfileEntries(actor);
        Skeleton_TickActionCooldowns(actor);
        timer = &context->timer6D8;
        if (*timer)
            --*timer;
    }
    return 1;
}

void Skeleton_UpdateFacingColliders(struct ChaseActor *p);

void Skeleton_UpdateActionCooldown(struct ChaseActor *);

s32 Skeleton_UpdateActionFlags(struct Skeleton *p)
{
    u32 m;
    u16 *q;
    Skeleton_UpdateFacingColliders(&p->chase);
    Skeleton_UpdateActionCooldown(&p->chase);
    m = 0x200;
    q = &p->chase.base.motionFlags;
    if ((*q & m) == 0) {
        u32 *vp = &p->actionFlags;
        u32 k = 0xFFFFDFFF;
        *vp = *vp & k;
    }
    return 1;
}

/* Skeleton_OnAttackCollision: records the hit unless the actor is unavailable */

static inline u8 Unavailable_OnAttackCollision(u8 *c)
{
    if (c && (Flag(c, 0x40, 0x2D4) || Flag(c, 0x20000, 0x2D0) || *(s16 *)(c + 0x9C) <= 0))
        return 1;
    return 0;
}

static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}

// confidence=low: records a collision target unless the Skeleton is unavailable.

void Skeleton_OnAttackCollision(u8 *unused, u8 *obj, u8 *ctx)
{
    int mask;
    u32 *flags;
    u8 *base = ctx;
    struct Skeleton *context = ((struct ChaseActor *)ctx)->base.context;
    if (Unavailable_OnAttackCollision(ctx))
        return;
    {
        u32 bit = 0x10;
        u32 *sp = &context->actionFlags;
        u32 *held;
        u32 v;
        v = *sp & bit;
        held = sp;
        if (Test(v, ~0)) {
            u32 bit = 0x800;
            if (Test(*held, bit)) {
                *(u16 *)(base + 0x2EC) = 0;
                *(u16 *)(base + 0x2F0) = 0;
            }
            return;
        }
    }
    if ((Unavailable_OnAttackCollision(ctx) == 0) && ((*((u16 *)(obj + 0xA))) & 0xF)) {
        *((u32 *)(ctx + 0x304)) = *((u16 *)(obj + 4));
        flags = (u32 *)(ctx + 0x2D0);
        mask = 0xFEFFFFFF;
        *flags &= mask;
    }
}

/* Skeleton_OnContactEvent: records the hit and flags the state unless the actor is unavailable */

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

void Skeleton_OnContactEvent(u8 *unused, u8 *obj, u8 *ctx)
{
    int mask;
    u32 *flags;
    u32 *state = *(u32 **)(ctx + 0x3D0);
    if (blocked2048(ctx))
        return;
    {
        u32 bit = 0x10;
        u32 *sp = (u32 *)((u8 *)state + 0x6DC);
        u32 *held;
        u32 v = *sp & bit;
        held = sp;
        state = held;
        if (Test(v, ~0))
            return;
    }
    if ((blocked2048(ctx) == 0) && ((*((u16 *)(obj + 0xA))) & 0xF)) {
        *((u32 *)(ctx + 0x304)) = *((u16 *)(obj + 4));
        flags = (u32 *)(ctx + 0x2D0);
        mask = 0xFEFFFFFF;
        *flags &= mask;
    }
    {
        u32 m = 0x20;
        *state |= m;
    }
}

/* Tentative: passes the call through unless the actor is unavailable or flagged. */

void SceneActorReaction_HandleColliderContact(void *, void *, struct ChaseActor *);

static inline u32 StateFlag(struct SceneActorScriptPropertiesView *p, u32 mask)
{
    return p->flags6DC & mask;
}

void SceneActor_TryForward(void *a, void *b, struct ChaseActor *actor)
{
    struct SceneActorScriptPropertiesView *state = actor->base.context;
    if (SceneActor_ActionsBlocked(actor))
        return;
    if (Test(StateFlag(state, 0x10), ~0))
        return;
    if (state->active6D0)
        return;
    SceneActorReaction_HandleColliderContact(a, b, actor);
}

/* Tentative reaction name, based on the observed fields and calls. */

void SceneActorControl_BeginTimedBehavior56(struct ChaseActor *s,
                                            struct Skeleton *other);

u8 SceneActorControl_CanTurnTowardTarget(struct ChaseActor *);

void SceneActorReaction_TryStartContact(u8 *source, u16 *contact, struct ChaseActor *actor)
{
    struct SceneActorScriptPropertiesView *state = actor->base.context;
    if (SceneActor_ActionsBlocked(actor))
        return;
    if (SceneActorTurn_Flags(&contact[3], 4))
        return;
    if (actor->base.colliderA.responseValue)
        return;
    if (Test(StateFlag(state, 0x10), ~0))
        return;
    if (ActorFlag(actor, 0xA13F, 0x2D4))
        return;
    if (SceneActorChase_Bits32((u32 *)(source + 0x34), 0x10))
        return;
    if (!SceneActorControl_CanTurnTowardTarget(actor))
        return;
    SceneActorControl_BeginTimedBehavior56(actor, (struct Skeleton *)state);
    state->active6D0 = 20;
    actor->base.reactionDirection = actor->base.tracking.heading + 0x80;
}

void Skeleton_UpdateInactiveState(u8 *p)
{
    if (p[0x2b2] != 0)
        p[0x2b2] = 0;
}

void Skeleton_UpdateScriptWaitState(u8 *p)
{
    if (p[0x2b2] != 0)
        p[0x2b2] = 0;
}

void SceneActorAnimation_SelectDirectionalEntry(struct RoomActor *, s32);

void Skeleton_UpdateIdleState(u8 *p)
{
    if (p[0x2b2] != 0)
        p[0x2b2] = 0;
    SceneActorAnimation_SelectDirectionalEntry(p, 0);
}

static inline s32 Skeleton_ScaleMotion(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}

static inline void Skeleton_SetPolarMotion(struct ContactPosition *motion, s32 angle, s32 speed)
{
    motion->x = Skeleton_ScaleMotion(gUnk_086149C4[(angle + 64) & 255] * speed);
    motion->y = 0;
    motion->z = Skeleton_ScaleMotion(gUnk_086149C4[angle & 255] * speed);
}

static inline struct SceneActorChaseFlags16 *Skeleton_GateFlags(struct ChaseActor *actor)
{
    return (struct SceneActorChaseFlags16 *)&actor->group.common.gate.flags;
}

static inline void Skeleton_EnableMotionFlags(struct ChaseActor *actor, u16 bits)
{
    SceneActorChase_SetFlagComponent((struct SceneActorChaseFlags16 *)&actor->base.motionFlags,
                                     bits);
}

static inline void Skeleton_SetContextFlags(struct Skeleton *context,
                                            u32 bits)
{
    context->actionFlags |= bits;
}

static inline u32 Skeleton_MovementFlags(struct ChaseActor *actor, u32 mask)
{
    return SceneActorChase_Bits32(&actor->base.statusFlags, mask);
}

static inline void Skeleton_UpdateArrival(void *obj)
{
    struct ContactPosition target;
    struct ChaseActor *actor = obj;
    struct SceneActorChaseGroup *group = SceneActorChase_GroupOf(obj);
    struct SceneActorChaseTracking *tracking = &actor->base.tracking;
    s32 cell, player;
    const u16 *randomSamples = gRandomTable;
    void **callback;
    SceneActorChase_SnapToRoute(group->destination, &group->homeRoute);
    if (SceneActorChase_Test(gEngineFlags, 0x4800)) {
        struct SceneActorChaseTarget *t = tracking->target;
        if (!t)
            goto invalid_target;
        player = SceneActorChase_PlayerSlot(t);
        if (player < 0) {
        invalid_target:
            callback = &group->fallback;
            Callback_InvokeActor(actor, *callback);
            return;
        }
        cell = ((struct SceneActorChasePlayer *)gPlayers[player])->cell->index;
    } else {
        struct SceneActorChaseTarget *t = tracking->target;
        cell = SceneActorChase_CellAt(&t->position);
        player = 0;
    }
    if (cell != actor->base.terrainCell &&
        !SceneActorChase_Bits32(&gPlayers[player]->statusFlags,
                                0x20000))
        goto reroute;
    if (SceneActorChase_MotionFlags(actor, 0x400))
        return;
    {
        u32 bits = 4;
        group->common.gate.flags &= ~bits;
    }
    if (SceneActorChase_MotionFlags(actor, 0x80)) {
        SceneActorChase_SetControl(actor, group->common.gate.control, 0x1A);
        SceneActorChase_SetBehavior(actor, 12);
    } else {
        SceneActorChase_SetControl(actor, actor->base.onAlternateEntry, 1);
        SceneActorChase_SetBehavior(actor, 5);
        SceneActorChase_SnapToRoute(group->destination, &group->homeRoute);
        (group->destination)->y = SceneActorChase_HeightAt(
            SceneActorChase_CellAt(group->destination), group->destination);
        (group->destination)->pad = 0;
    }
    {
        u8 saved = actor->base.body.direction;
        SceneActorChase_BeginTurn(obj, saved, randomSamples);
    }
    SceneActorTerrain_RequestRetreatRoute(obj);
    actor->base.field2C2 = 0x20;
    return;
reroute: {
    SceneActorChase_CellCenter(cell, &target);
    if (SceneActorChase_TryRoute(obj, group->route, &actor->base.body.position, &target))
        goto success;
}
    callback = &group->fallback;
    Callback_InvokeActor(actor, *callback);
    return;
success:
    return;
}

static inline void Skeleton_StartHop(struct ChaseActor *actor, s32 direction, s32 speed)
{
    Skeleton_SetPolarMotion(&actor->base.secondaryMotion, direction << 6, speed);
    actor->base.body.direction = ((direction + 2) & 3) << 6;
    actor->base.secondaryMotion.y =
        (SceneActorEffects_NextSample((const u16 *)RANDOM_TABLE_ADDR) & 15) + 12;
    SceneActorAnimation_SelectDirectionalEntry(&actor->base, 14);
}

static inline void Skeleton_RestartGate(struct ChaseActor *actor)
{
    actor->group.common.gate.counter = actor->group.common.gate.duration;
}

void Skeleton_UpdateHopToOpenCell(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    if (SceneActorControl_TakeFlag(&actor->base.field2B2)) {
        s32 speed = (SceneActorEffects_NextSample((const u16 *)RANDOM_TABLE_ADDR) & 3) + 10;
        s32 direction =
            SceneActorTerrain_FindOpenNeighborDirection(actor, actor->base.reactionDirection);
        if (direction < 0) {
            struct Skeleton *context;
            SceneActorAnimation_SelectDirectionalEntry(&actor->base, 15);
            SceneActorChase_SetFlagComponent(Skeleton_GateFlags(actor), 4);
            Skeleton_EnableMotionFlags(actor, 0x80);
            Skeleton_RestartGate(actor);
            SceneActorChase_SelectState(actor, 0, 0, 12);
            SceneActorChase_ResetStateEntry(actor);
            SceneActorChase_SetTypedControl(actor, group->common.gate.control, 0x1A);
            Skeleton_UpdateArrival(actor);
            context = actor->base.context;
            Skeleton_SetContextFlags(context, 0x80);
            return;
        }
        Skeleton_StartHop(actor, direction, speed);
    }
    if (Skeleton_MovementFlags(actor, 0x10)) {
        SceneActorAnimation_SelectDirectionalEntry(&actor->base, 15);
        SceneActorChase_SelectState(actor, 0, 0, 12);
        SceneActorChase_ResetStateEntry(actor);
        SceneActorChase_SetTypedControl(actor, group->common.gate.control, 0x1A);
        Skeleton_UpdateArrival(actor);
    }
}

/* Tentative reaction name, based on the observed fields and calls. */

void SceneActorReaction_ApplyThreshold(struct ChaseActor *actor, s32 value);

void SceneActorReaction_TickContactAnimation(struct ChaseActor *p)
{
    struct Skeleton *state = p->base.context;
    struct SceneActorAnimationTileDrawable *drawable = p->base.drawable;
    struct AnimSprite *animation = &drawable->tile;
    u8 *category;
    u32 kind;
    if (SceneActorReaction_TakePrepared(p))
        SceneActorAnimation_SelectDirectionalEntry(&p->base, 5);
    kind = p->base.category;
    category = &p->base.category;
    if (kind == MONSTER_CATEGORY_SKELETON && animation->frameIndex == 2 && animation->elapsed == 0)
        Sound_PlaySong(0x3A9);
    if (animation->frameIndex == 3) {
        Collider_Refresh(&state->attackCollider);
        if (animation->elapsed == 0 && *category == MONSTER_CATEGORY_SKELETON_GUARD)
            Sound_PlaySong(0x3AA);
    }
    if (p->base.restart)
        SceneActorChase_SetControl(p, SceneActorReaction_ApplyThreshold, 0x3E);
}

void SceneActorReaction_ApplyThreshold(struct ChaseActor *actor, s32 value)
{
    struct Skeleton *context = actor->base.context;

    if (SceneActorControl_TakeFlag(&actor->base.field2B2))
        SceneActorAnimation_SelectDirectionalEntry(&actor->base, 0);

    if (value > context->parameter706)
        actor->base.stepDone = 1;
}

/* Tentative reaction name, based on the observed fields and calls. */

void Skeleton_ShedBones(struct ChaseActor *actor);

void Skeleton_UpdateAttackRecovery(struct ChaseActor *p);

void SceneActorReaction_TickQueuedContact(struct ChaseActor *p)
{
    struct Skeleton *state = p->base.context;
    struct SceneActorAnimationTileDrawable *drawable = p->base.drawable;
    struct AnimSprite *animation = &drawable->tile;
    u32 *flags;
    u32 queued;
    if (SceneActorReaction_TakePrepared(p))
        SceneActorAnimation_SelectDirectionalEntry(&p->base, 5);
    if (animation->frameIndex == 2 && animation->elapsed == 0)
        Sound_PlaySong(0x3A9);
    if (animation->frameIndex == 3) {
        Collider_Refresh(&state->attackCollider);
    }
    {
        u32 mask = 0x20;
        u32 *address = &state->actionFlags;
        u32 value = *address & mask;
        flags = address;
        queued = Test(value, ~0);
    }
    if (queued) {
        {
            s32 mask = ~0x20;
            *flags &= mask;
        }
        SceneActorChase_SetControl(p, Skeleton_ShedBones, 0x25);
    } else if (p->base.restart) {
        SceneActorChase_SetControl(p, Skeleton_UpdateAttackRecovery, 0x24);
    }
}

void Skeleton_InitializeCategoryProfile(struct ChaseActor *actor);

void Skeleton_InitVariantMotion(void *);

void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);

void sub_08127590(void *, void *, u32, u32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32);

extern const s16 gUnk_086149C4[];

static inline s32 Skeleton_TruncateFixed6(s32 x)
{
    if (x >= 0)
        return x >> 6;
    return -((-x) >> 6);
}

static inline s32 Skeleton_TruncateFixed12(s32 x)
{
    if (x >= 0)
        return x >> 12;
    return -((-x) >> 12);
}

static inline u16 Skeleton_RandomSample_ShedBones(void)
{
    const u16 *samples = (const u16 *)RANDOM_TABLE_ADDR;
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    index *= 2;
    return *(const u16 *)((u32)samples + index);
}

static inline void Skeleton_SetPolarMotion_ShedBones(struct ContactPosition *v, s32 direction,
                                                    s32 radius)
{
    v->x = Skeleton_TruncateFixed12(gUnk_086149C4[(direction + 64) & 255] * radius);
    v->y = 0;
    v->z = Skeleton_TruncateFixed12(gUnk_086149C4[direction & 255] * radius);
}

static inline void Skeleton_AddPosition(struct ContactPosition *position,
                                        const struct ContactPosition *base,
                                        const struct ContactPosition *delta)
{
    position->x = delta->x + base->x;
    position->y = delta->y + base->y;
    position->z = delta->z + base->z;
}

static inline void Skeleton_SetCategory(struct ChaseActor *actor, u8 category)
{
    actor->base.category = category;
}

static inline void Skeleton_EmitBone(struct ContactPosition *position,
                                     struct ContactPosition *velocity, u32 sequence)
{
    sub_08127590(position, velocity, 0xD45E, 0x1DC3, sequence, 30, 2, 0, 316, 0, 0, 0, 0, 0, 0);
}

/* Emit two bone fragments and restore the threshold reaction. */

void Skeleton_ShedBones(struct ChaseActor *actor)
{
    struct ContactPosition headingMotion;
    struct ContactPosition velocity;
    struct ContactPosition position;
    s32 direction;
    struct Skeleton *context = actor->base.context;

    if (SceneActorControl_TakeFlag(&actor->base.field2B2)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0x16);
        direction = Skeleton_TruncateFixed6((s32)((actor->base.body.direction + 0x10) & 0xFF)) << 6;
        Skeleton_SetPolarMotion_ShedBones(&headingMotion, direction, 20);
        direction = Skeleton_RandomSample_ShedBones();
        direction &= 255;
        Skeleton_SetPolarMotion_ShedBones(&velocity, direction, 10);
        velocity.y = 30;
        position = actor->base.body.position;
        position.y += 0x104;
        Skeleton_AddPosition(&position, &position, &velocity);
        Skeleton_EmitBone(&position, &velocity, 2);
        direction = Skeleton_RandomSample_ShedBones();
        direction &= 255;
        Skeleton_SetPolarMotion_ShedBones(&velocity, direction, 10);
        velocity.y = 30;
        position.y += 0xDC;
        Skeleton_AddPosition(&position, &position, &velocity);
        Skeleton_EmitBone(&position, &velocity, 3);
        Skeleton_SetCategory(actor, MONSTER_CATEGORY_SKELETON);
        Skeleton_InitializeCategoryProfile(actor);
        Skeleton_InitVariantMotion(actor);
        {
            u16 first = actor->base.attacks.first[3];
            u16 second = actor->base.attacks.second[3];
            u32 flags = actor->base.attacks.flags[3];
            u16 parameter = actor->base.attacks.parameter[3];
            Collider_SetAttackProperties(&context->attackCollider, first, second, 0, flags,
                                         parameter);
        }
    }
    if (actor->base.restart)
        SceneActorChase_SetControl(actor, SceneActorReaction_ApplyThreshold, 0x3E);
}

void Skeleton_UpdateAttackRecovery(struct ChaseActor *p)
{
    if (SceneActorControl_TakeFlag(&p->base.field2B2))
        SceneActorAnimation_SelectDirectionalEntry(&p->base, 0x17);
    if (p->base.restart)
        SceneActorChase_SetControl(p, SceneActorReaction_ApplyThreshold, 0x3E);
}

void SceneActorReaction_TickForwardBurst(struct ChaseActor *p_);

void Skeleton_UpdateAttackWindup(struct ChaseActor *p)
{
    if (SceneActorControl_TakeFlag(&p->base.field2B2))
        SceneActorAnimation_SelectDirectionalEntry(&p->base, 0x13);
    if (p->base.restart)
        SceneActorChase_SetControl(p, SceneActorReaction_TickForwardBurst, 0x28);
}

void Skeleton_TickDecayingLunge(struct ChaseActor *p, s32 tick_);

static inline s32 Truncate(s32 value)
{
    return value >= 0 ? value >> 12 : -((-value) >> 12);
}

static inline void HeadingStep(struct ContactPosition *v, u8 *heading, s32 speed)
{
    u32 direction = *heading;
    v->x = Truncate(gUnk_086149C4[(direction + 0x40) & 0xFF] * speed);
    v->y = 0;
    v->z = Truncate(speed * gUnk_086149C4[direction]);
}

static inline void CpuSet_2(int ctl, u32 *src, struct ContactPosition *dst)
{
    CpuSet(src, dst, ctl);
}

static inline void AddHalf(s16 *dst, const s16 *src)
{
    *dst += *src;
}

static inline void AddZ(s16 *dst, const struct ContactPosition *src)
{
    *dst += src->z;
}

void SceneActorReaction_TickForwardBurst(struct ChaseActor *p_)
{
    u8 *heading;
    struct ChaseActor *p = p_;
    struct {
        u32 zero;
        struct ContactPosition step;
    } scratch;
    struct ContactPosition jitter;
    struct ContactPosition *step;
    if (SceneActorReaction_TakePrepared(p)) {
        SceneActorAnimation_SelectDirectionalEntry(&p->base, 0x14);
        Sound_PlaySong(0x3AB);
    }
    heading = &p->base.body.direction;
    HeadingStep(&scratch.step, heading, 32);
    {
        struct ContactPosition *v = &scratch.step;
        AddHalf(&p->base.body.motion.x, &v->x);
        v = &p->base.body.motion;
        AddZ(&v->z, &scratch.step);
    }
    step = &scratch.step;
    {
        u32 mask = 0x2000;
        u32 *fl = &p->base.behaviorFlags;
        *fl |= mask;
    }
    scratch.zero = 0;
    CpuSet_2(0x05000002, &scratch.zero, step);
    jitter.x = 0x52;
    jitter.y = 0;
    jitter.z = 0x52;
    SceneParticleBurst_CreateWithPause(&p->base.body.position, step, &jitter, 1, 2, 4);
    if (p->base.restart)
        SceneActorChase_SetControl(p, Skeleton_TickDecayingLunge, 0x29);
}

void Skeleton_UpdateAlternateAttackRecovery(struct ChaseActor *p);

s32 Collider_Refresh(void *);

static inline void HeadingStep_TickDecayingLunge(u8 *heading, u8 *speedp, struct ContactPosition *v)
{
    u32 direction = *heading;
    u32 speed = *speedp;
    v->x = Truncate(gUnk_086149C4[(direction + 0x40) & 0xFF] * speed);
    v->y = 0;
    v->z = Truncate(speed * gUnk_086149C4[direction]);
}

static inline void Clear(const void *src, void *dst, u32 ctl)
{
    CpuSet(src, dst, ctl);
}

static inline s32 Half(s32 value)
{
    return value >= 0 ? value >> 1 : -((-value) >> 1);
}

void Skeleton_TickDecayingLunge(struct ChaseActor *p, s32 tick_)
{
    struct Skeleton *context;
    s32 tick = tick_;
    struct {
        u32 zero;
        struct ContactPosition step;
    } scratch;
    struct ContactPosition jitter;
    struct ContactPosition empty;
    struct ContactPosition *np;
    context = p->base.context;
    if (SceneActorReaction_TakePrepared(p)) {
        SceneActorAnimation_SelectDirectionalEntry(&p->base, 0x15);
        context->unknown6D1 = 0x48;
    }
    HeadingStep_TickDecayingLunge(&p->base.body.direction, &context->unknown6D1, &scratch.step);
    {
        struct ContactPosition *v = &scratch.step;
        AddHalf(&p->base.body.motion.x, &v->x);
        v = &p->base.body.motion;
        AddZ(&v->z, &scratch.step);
    }
    {
        u32 mask = 0x2000;
        u32 *fl = &p->base.behaviorFlags;
        *fl |= mask;
    }
    if (Mod(tick, 4) == 0) {
        u32 z;
        s32 h = Half(context->unknown6D1);
        u8 *cp = &context->unknown6D1;
        z = 0;
        *cp = h;
        jitter.x = 0x52;
        jitter.y = 0;
        jitter.z = 0x52;
        np = &empty;
        scratch.zero = z;
        Clear(&scratch.zero, np, 0x05000002);
        SceneParticleBurst_CreateWithPause(&p->base.body.position, np, &jitter, 1, 2, 4);
    }
    Collider_Refresh(&context->attackCollider);
    if (p->base.restart && context->unknown6D1 == 0)
        SceneActorChase_SetControl(p, Skeleton_UpdateAlternateAttackRecovery, 0x2A);
}

void Skeleton_UpdateAlternateAttackRecovery(struct ChaseActor *p)
{
    if (SceneActorControl_TakeFlag(&p->base.field2B2))
        SceneActorAnimation_SelectDirectionalEntry(&p->base, 0x16);
    if (p->base.restart)
        SceneActorChase_SetControl(p, SceneActorReaction_ApplyThreshold, 0x3E);
}

/* Tentative reaction name, based on the observed fields and calls. */

void SceneActorReaction_TickMultiHitAnimation(struct ChaseActor *p)
{
    struct Skeleton *state = p->base.context;
    struct SceneActorAnimationTileDrawable *drawable = p->base.drawable;
    struct AnimSprite *animation = &drawable->tile;
    u32 frame;
    if (SceneActorReaction_TakePrepared(p))
        SceneActorAnimation_SelectDirectionalEntry(&p->base, 0x17);
    frame = animation->frameIndex;
    if ((u16)(frame - 3) <= 2 || (u16)(frame - 7) <= 2)
        Collider_Refresh(&state->attackCollider);
    if ((animation->frameIndex == 4 || animation->frameIndex == 7) && animation->elapsed == 0)
        Sound_PlaySong(0x3AA);
    if (p->base.restart)
        SceneActorChase_SetControl(p, SceneActorReaction_ApplyThreshold, 0x3E);
}

void Skeleton_UpdateHomingBurst(struct ChaseActor *p, s32 tick);

void Skeleton_BeginHomingBurst(struct ChaseActor *p, s32 n)
{
    if (SceneActorControl_TakeFlag(&p->base.field2B2))
        SceneActorAnimation_SelectDirectionalEntry(&p->base, 0x13);
    if (n > 0x14 && p->base.restart)
        SceneActorChase_SetControl(p, Skeleton_UpdateHomingBurst, 0x2D);
}

void Skeleton_UpdateHeavyAttackRecovery(struct ChaseActor *p);

void MovingSprite_Create(struct ContactPackedVector *, u32, u32, u32, u32, u32, s32);

void Sound_PlaySong(u32);

static inline void HeadingStep_UpdateHomingBurst(struct ContactPosition *v, u32 direction,
                                                 u32 speed)
{
    v->x = Truncate(gUnk_086149C4[(direction + 0x40) & 0xFF] * speed);
    v->y = 0;
    v->z = Truncate(speed * gUnk_086149C4[direction]);
}

static inline void Difference(struct ContactPosition *d, struct ContactPosition *a,
                              struct ContactPosition *b)
{
    d->x = a->x - b->x;
    d->y = a->y - b->y;
    d->z = a->z - b->z;
}

static inline s32 Wrap(s32 d)
{
    if (d > 0x80)
        d -= 0x100;
    else if (d < -0x80)
        d += 0x100;
    return d;
}

static inline s32 Quarter(s32 value)
{
    return value >= 0 ? value >> 6 : -((-value) >> 6);
}

void Skeleton_UpdateHomingBurst(struct ChaseActor *p, s32 tick)
{
    u8 *context = p->base.context;
    if (SceneActorReaction_TakePrepared(p)) {
        struct ContactPackedVector spawn = *(struct ContactPackedVector *)&p->base.body.position;
        struct ContactPosition delta;
        struct ContactPosition step;
        u32 base = (u8)(Quarter((p->base.body.direction + 0x20) & 0xFF) << 6);
        s32 angle, diff;
        struct ContactPosition *dp = &delta;
        struct ContactPosition *pos = &p->base.body.position;
        struct ContactPosition *target;
        target = (struct ContactPosition *)(context + 0x710);
        Difference(dp, target, pos);
        angle = Math_Atan2(dp->x, dp->z);
        diff = Wrap(angle - base);
        HeadingStep_UpdateHomingBurst(&step, base, 10);
        spawn.x += step.x;
        spawn.y += 0x104;
        spawn.z += step.z;
        if (diff > 12)
            base = (u8)(base + 12);
        else if (diff < -12)
            base = (u8)(base - 12);
        else
            base = (u8)angle;
        {
            struct ContactPackedVector *sp = &spawn;
            u32 b8, c0, d4, c8;
            b8 = p->base.attacks.first[2];
            c0 = p->base.attacks.second[2];
            d4 = p->base.attacks.flags[2];
            c8 = p->base.attacks.parameter[2];
            MovingSprite_Create(sp, base, b8, c0, d4, c8, 0x2A);
        }
        Sound_PlaySong(0x3A8);
    }
    SceneActorAnimation_SelectDirectionalEntry(&p->base, 0x14);
    if (tick > 0x18)
        SceneActorChase_SetControl(p, Skeleton_UpdateHeavyAttackRecovery, 0x2E);
}

void Skeleton_UpdateHeavyAttackRecovery(struct ChaseActor *p)
{
    if (SceneActorControl_TakeFlag(&p->base.field2B2))
        SceneActorAnimation_SelectDirectionalEntry(&p->base, 0x15);
    if (p->base.restart)
        SceneActorChase_SetControl(p, SceneActorReaction_ApplyThreshold, 0x3E);
}

u32 Route_GetCurrentPointFacing(void *);

static inline s32 SkeletonTurn_Octant(s16 direction)
{
    return (direction + 0x70) >> 5;
}

static inline s32 SkeletonTurn_OctantDistance(s32 target, s32 current)
{
    s32 delta = (target - current) & 7;
    if (delta > 4)
        delta = 8 - delta;
    return delta;
}

/* Turn toward the route octant with the profile-selected animation. */

void Skeleton_TurnTowardRouteFacing(struct ChaseActor *a)
{
    struct SceneActorChaseGroup *group = &a->group;
    struct SceneActorBody *motion = &a->base.body;
    struct SceneActorRegistryRoute *route = &group->homeRoute;
    u16 *motionFlags;
    u8 prepared = SceneActorReaction_TakePrepared(a);
    s32 direction, target, delta;
    const u8 *entry;
    struct AnimSprite *tile;

    motionFlags = &a->base.motionFlags;
    if (prepared) {
        a->base.body.direction = (SceneActorTurn_Cardinal(a->base.body.direction) << 6) - 0x40;
        direction = SceneActorTurn_Cardinal(a->base.body.direction);
        if (a->base.behavior2AA != 6 && a->base.behavior2AA != 0x14 &&
            !SceneActorTurn_Flags(motionFlags, 0x4480)) {
            target = (((u16)Route_GetCurrentPointFacing(route) + 0x70) >> 5) & 7;
            a->base.nextDirection = target;
        } else {
            target = a->base.nextDirection;
        }
        if (direction == (target >> 1)) {
            a->base.stepDone = 1;
            if (SceneActorTurn_Flags(motionFlags, 0x400)) {
                SceneActorChase_SetState(a, 3, 1, 0);
                SceneActorChase_SetControl(a, group->turnControl, 0x1B);
            }
            return;
        }
        entry = SceneActorProfiles_GetEntryAddress(a->base.profileId, a->base.profileMode, 2);
        switch (direction) {
            case 0:
                if ((u32)(target - 2) <= 3) {
                    SceneActorTurn_SelectAnimation(a, *entry, 1, 0);
                    SceneActorChase_SetActorFlags(a, 0x10000);
                } else {
                    SceneActorTurn_SelectAnimation(a, *entry, 0, 4);
                    SceneActorChase_ClearWordFlags(&a->base.behaviorFlags, 0x10000);
                }
                break;
            case 1:
                if ((u32)(target - 4) <= 3) {
                    SceneActorTurn_SelectAnimation(a, *entry, 2, 0);
                    SceneActorChase_SetActorFlags(a, 0x10000);
                } else {
                    SceneActorTurn_SelectAnimation(a, *entry, 1, 4);
                    SceneActorChase_ClearWordFlags(&a->base.behaviorFlags, 0x10000);
                }
                break;
            case 2:
                if ((u32)(target - 1) > 4) {
                    SceneActorTurn_SelectAnimation(a, *entry, 1, 5);
                    SceneActorChase_SetActorFlags(a, 0x10000);
                } else {
                    SceneActorTurn_SelectAnimation(a, *entry, 2, 4);
                    SceneActorChase_ClearWordFlags(&a->base.behaviorFlags, 0x10000);
                }
                break;
            case 3:
                if ((u32)target <= 3) {
                    SceneActorTurn_SelectAnimation(a, *entry, 0, 0);
                    SceneActorChase_SetActorFlags(a, 0x10000);
                } else {
                    SceneActorTurn_SelectAnimation(a, *entry, 1, 1);
                    SceneActorChase_ClearWordFlags(&a->base.behaviorFlags, 0x10000);
                }
                break;
        }
    }
    tile = &((struct SceneActorAnimationTileDrawable *)a->base.drawable)->tile;
    if (tile->elapsed == 0 && (u16)(tile->frameIndex - 1) <= 1) {
        s32 targetDirection = SceneActorTurn_OctantDirection(a->base.nextDirection);
        s32 difference = motion->direction - targetDirection;
        if (difference < 0)
            difference = -difference;
        if (difference <= 0x20)
            motion->direction = targetDirection;
        else if (SceneActorTurn_ActorFlags(a, 0x10000))
            motion->direction += 0x20;
        else
            motion->direction -= 0x20;
    }
    SceneActorTurn_ApplyDuration(a, motionFlags);
    if (a->base.restart) {
        u8 *facing = &a->base.body.direction;
        s32 current = SkeletonTurn_Octant(*facing);
        s32 octantMask = -8;
        a->base.stepDone = 1;
        delta = SkeletonTurn_OctantDistance(a->base.nextDirection, current | octantMask);
        if (delta < 0)
            delta = -delta;
        if (delta == 0) {
            a->base.stepDone = 1;
            *facing = (a->base.nextDirection + 5) << 5;
            SceneActorAnimation_SelectDirectionalEntry((void *)a, 0);
            if (SceneActorTurn_Flags(motionFlags, 0x400)) {
                SceneActorChase_SetState(a, 3, 1, 0);
                SceneActorChase_SetControl(a, group->turnControl, 0x1B);
            }
        } else {
            SceneActorChase_SetControl(a, a->base.onEntry, 2);
        }
    }
}

void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);

void Skeleton_UpdateRevivalState(struct ChaseActor *p, s32 n);

void Skeleton_UpdateDownedState(struct ChaseActor *actor, s32 frame)
{
    struct SceneActorChaseGroup *group = &actor->group;
    u32 mask;
    if (SceneActorControl_TakeFlag(&actor->base.field2B2)) {
        u32 test;
        u16 *flags;
        u32 value;
        SceneActorAnimation_SelectDirectionalEntry((u8 *)actor, 8);
        test = 2;
        flags = &actor->base.motionFlags;
        value = *flags;
        test &= value;
        if (test == 0) {
            test = 1;
            test |= value;
            *flags = test;
        }
    }
    if (Mod(frame, 60) == 0)
        SceneActorAttachment_ReplaceResponseSlot((u8 *)actor, 2, 0, actor->base.category);
    if ((s16)group->field5C6 <= 0) {
        mask = 0x1000;
        if ((actor->base.statusFlags & mask) == 0)
            SceneActorChase_SetControl(actor, Skeleton_UpdateRevivalState, 6);
    }
}

static inline u32 orr(u32 k, u16 *a)
{
    return k | *a;
}

void Skeleton_UpdateRevivalState(struct ChaseActor *p, s32 n)
{
    if (SceneActorControl_TakeFlag(&p->base.field2B2)) {
        s32 m;
        u16 *a;
        SceneActorAnimation_SelectDirectionalEntry(p, 7);
        a = &p->base.motionFlags;
        m = -2;
        *a = m & *a;
    }
    if (n == 0x28) {
        u16 k = 0x10;
        p->base.reactionFlags = k | p->base.reactionFlags;
    }
    if (p->base.restart && n > 0x78)
        p->base.stepDone = 1;
}

/* Skeleton_BeginKnockedFlight: on entry makes the body's colliders inert, launches the actor along its facing at a random speed (0x1C-0x2B) with a random upward speed, and shows its shadow; on landing (motion bit 0) stops, and once flags2D0 & 0x10 plays the bone sound (if enabled) and resets the actor (provisional name) */

static inline s32 Scale12(s32 n)
{
    if (n >= 0)
        return n >> 12;
    return -((-n) >> 12);
}

#define SCALE_AXIS(out, sample, speed)                                                             \
    do {                                                                                           \
        (out) = (sample) * (speed);                                                                \
    } while (0)

static inline void Polar(struct ContactPosition *out, u8 angle, s32 speed)
{
    s32 sample, product;
    sample = gUnk_086149C4[(angle + 0x40) & 0xFF];
    SCALE_AXIS(product, sample, speed);
    out->x = Scale12(product);
    out->y = 0;
    sample = gUnk_086149C4[angle];
    SCALE_AXIS(product, sample, speed);
    out->z = Scale12(product);
}


void Skeleton_BeginKnockedFlight(struct ChaseActor *input)
{
    struct ChaseActor *actor = input;
    struct Skeleton *context = actor->base.context;
    u32 origin;
    u8 taken = SceneActorControl_TakeFlag(&actor->base.field2B2);
    if (taken) {
        ContactCollider_SetFlags(&context->bodyCollider, 4);
        ContactCollider_SetFlags(&context->attackCollider, 4);
        ContactCollider_SetFlags(&actor->base.colliderA, 4);
        ContactCollider_ClearFlags(&actor->base.colliderB, 4);
        {
            u8 direction = actor->base.body.direction;
            s32 random;
            PARTICLE_RANDOM_CAPTURE_ORIGIN(origin);
            random = ParticleRandom_ReadDirectSample((const u16 *)RANDOM_TABLE_ADDR);
            random &= 0xF;
            {
                u32 angle = direction;
                Polar(&actor->base.secondaryMotion, angle, random + 0x1C);
            }
        }
        actor->base.secondaryMotion.y =
            (ParticleRandom_ReadRelativeSample(origin, (const u16 *)RANDOM_TABLE_ADDR) & 0x1F) +
            0x18;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0x11);
        ActorShadow_Enable(actor->base.shadow);
    } else {
        if (**(u8 **)((u8 *)actor + 0x6C) & 1) {
            actor->base.secondaryMotion.x = taken;
            actor->base.secondaryMotion.z = taken;
        }
        {
            if (SceneActorChase_StatusFlags(actor, 0x10)) {
                u16 argument;
                if (WordFlags_Has(0x800, &context->actionFlags))
                    Sound_PlaySong(0x3AD);
                ContactCollider_SetFlags(&actor->base.colliderB, 4);
                argument = 1;
                SceneActorPlacement_ResetActorState(actor, &argument);
            }
        }
    }
}

#define SceneActorReaction_RestoreControl SceneActorPlacement_ResetActorState

/* Tentative reaction name, based on the observed fields and calls. */

u8 SceneActorReaction_TrySink(struct ChaseActor *);

void SceneActorReaction_TickHiddenTimeout(struct ChaseActor *p, s32 elapsed)
{
    struct SceneActorScriptPropertiesView *state = p->base.context;
    u16 mode;
    if (SceneActorReaction_TakePrepared(p)) {
        EffectCollision_Disable(&p->base.colliderB);
        EffectCollision_Disable(&p->base.colliderA);
        ((struct ContactCollider *)((u8 *)state + 0x624))->flags |= 4;
        ((struct ContactCollider *)((u8 *)state + 0x678))->flags |= 4;
        SceneActorAnimation_SelectDirectionalEntry(&p->base, 0x11);
        ActorShadow_Disable(p->base.shadow);
    } else {
        if (Mod(elapsed, 10) == 0 && SceneActorReaction_TrySink(p))
            return;
        if (Test(StateFlag(state, 2), ~0))
            return;
        if (((struct Skeleton *)state)->revivalDelay > 19999)
            return;
        if (((struct Skeleton *)state)->revivalDelay >= elapsed)
            return;
        mode = 1;
        SceneActorPlacement_ResetActorState(p, &mode);
    }
}

/* Skeleton_UpdateReactionTransition: plays the hurt entry or, after a long wait, restores the saved snapshot */

void Actor_RestoreSnapshot(struct ChaseActor *s);

void Skeleton_UpdateReactionTransition(u8 *s, s32 arg)
{
    struct ChaseActor *actor = (void *)s;
    u32 queued = SceneActorReaction_TakePrepared(actor);
    if (queued || arg == 0x1E || arg == 0x37) {
        SceneActorAnimation_SelectDirectionalEntry(&actor->base, 0x12);
    } else if (arg > 0x5F) {
        SceneActorChase_SetControl(actor, Actor_RestoreSnapshot, 13);
    }
}

/* Re-enable the shadow, then restore the actor's live control fields from its saved snapshot (+0x118..) and reset its state; goes idle when flag 0x4000 is set (tentative) */

void ActorShadow_Enable(void *);

void SceneActorPalette_ClearCompletedTransitions(void);

void Skeleton_ResetRouteReaction(struct ChaseActor *actor);

u32 Trigger_ConsumePending(void *receiver, u32 count);

static inline u8 take_flag(struct ChaseActor *s)
{
    if (s->base.field2B2) {
        s->base.field2B2 = 0;
        return 1;
    }
    return 0;
}

static inline u8 peek_flag(struct ChaseActor *s)
{
    return s->base.restart;
}

void Actor_RestoreSnapshot(struct ChaseActor *s)
{
    struct Skeleton *ctx = s->base.context;
    s32 angle;
    s32 n;
    u8 saved;
    u32 keep;
    if (take_flag(s)) {
        ActorShadow_Enable(s->base.shadow);
        SceneActorAnimation_SelectDirectionalEntry(&s->base, 13);
        Sound_PlaySong(0x3AE);
    }
    if (!peek_flag(s))
        return;
    s->base.colliderA.flags &= ~4;
    s->base.colliderB.flags &= ~4;
    ctx->bodyCollider.flags &= ~4;
    ctx->attackCollider.flags &= ~4;
    {
        u16 *f = &s->base.motionFlags;
        u32 mask = ~0x800;
        *f &= mask;
    }
    angle = s->base.body.direction; /* read as an int first: puts the flags pointer in r6 as in the ROM */
    saved = angle;
    keep = (s->base.motionFlags & 0x4000) ? 0x4000 : 0;
    s->base.behaviorFlags = s->base.saved.behaviorFlags;
    s->base.statusFlags = s->base.saved.statusFlags;
    s->base.body.direction = s->base.saved.direction;
    s->base.state2A8 = s->base.saved.state;
    s->base.phase2A9 = s->base.saved.phase;
    s->base.behavior2AA = s->base.saved.behavior;
    s->base.target = (void *)s->base.saved.control;
    s->base.controlKind = s->base.saved.kind;
    s->base.placementFlags = s->base.saved.motionFlags;
    s->base.heightOffset = s->base.saved.heightOffset;
    n = s->base.defaultHealth;
    if (n <= 0)
        n = 0;
    s->base.health = n;
    s->base.reactionFlags = 0;
    s->base.motionFlags = 0;
    s->base.stepDone = 0;
    s->base.field2B1 = 1;
    s->base.field2B2 = 1;
    s->base.restart = 0;
    s->base.counter304 = 0;
    s->base.reactionDirection = 0;
    s->base.field2C2 = 64;
    s->base.hitAttributes = 0;
    s->base.hitReactionFlags = 0;
    s->base.hitOwners = 0;
    s->base.field2E6 = 0;
    s->base.field3CC = 0;
    s->base.tracking.target = 0;
    SceneActorPalette_ClearCompletedTransitions();
    s->base.motionFlags |= keep;
    s->base.body.direction = saved;
    Skeleton_ResetRouteReaction(s);
    {
        u32 mask = 0x4000;
        if (s->base.motionFlags & mask) {
            s->base.state2A8 = 2;
            s->base.phase2A9 = 0;
            s->base.behavior2AA = 2;
            s->base.counter2AC = 0;
            s->base.field2B1 = 1;
            {
                long long mask1 = ~1;
                void *control = SceneActorControl_OnIdle;
                u32 kind = 16;
                s->base.field2B2 = 1;
                s->base.stepDone = 0;
                s->base.controlKind = kind;
                s->base.target = control;
                s->base.field2C8 = 0;
                {
                    u32 *f = &s->base.pendingControlFlags;
                    s32 m = mask1;
                    *f &= m;
                }
                s->base.pendingControl = 0;
            }
            Trigger_ConsumePending(s->base.receiver, 1);
        }
    }
}

/* After 60 frames, return to the group route point and hand control to the entry target. */

void SceneActor_SetPositionXZ(void *, s32, s32);

static inline u8 IsActive(u32 value)
{
    if (value)
        return 1;
    return 0;
}


void SceneActorReaction_ReturnToRouteEntry(struct ChaseActor *actor, s32 frame)
{
    struct Skeleton *context = actor->base.context;
    struct SceneActorChaseGroup *group = &actor->group;
    if (SceneActorReaction_TakePrepared(actor)) {
        u32 *flags;
        if (!SceneActorContact_MotionFlags(actor, 128)) {
            u32 mask = 0x2000;
            u32 *address = &context->actionFlags;
            u32 value = *address & mask;
            flags = address;
            if (!IsActive(value))
                goto twelve;
        }
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        goto checked;
    twelve:
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 12);
        {
            u32 mask = 0x2000;
            *flags |= mask;
        }
    checked:;
    }
    if (frame > 60) {
        u16 blocked;
        actor->base.stepDone = 1;
        blocked = SceneActorContact_MotionFlags(actor, 128);
        if (blocked) {
            void *target;
            SceneActor_SetPositionXZ(
                &actor->base.body.id,
                ((struct ChaseRouteProbe *)group->route)->destination.x,
                ((struct ChaseRouteProbe *)group->route)->destination.z);
            SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
            target = group->common.gate.control;
            SceneActorChase_SetControl(actor, target, 26);
            Skeleton_ReturnToBehavior(actor, 12);
            {
                u16 *f = &group->common.gate.flags;
                u16 four = 4;
                *f |= four;
            }
            actor->base.field2C2 = 32;
        } else {
            void *target;
            SceneActor_SetPositionXZ(
                &actor->base.body.id,
                ((struct ChaseRouteProbe *)group->route)->destination.x,
                ((struct ChaseRouteProbe *)group->route)->destination.z);
            SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
            target = actor->base.onAlternateEntry;
            SceneActorChase_SetControl(actor, target, 1);
            Skeleton_ReturnToBehavior(actor, 7);
            {
                u32 mask = 16;
                actor->base.behaviorFlags |= mask;
            }
            actor->base.field2C2 = 32;
        }
    }
}

static inline void Skeleton_SnapRecoveryRoute(struct ContactPosition *position,
                                              struct SceneActorRegistryRoute *route)
{
    position->x = route->point->x;
    position->z = route->point->z;
    position->y = SceneActorChase_HeightAt(SceneActorChase_CellAt(position), position);
}

static inline void Skeleton_BeginRecoveryTurn(void *obj, u32 saved, const u16 *randomSamples)
{
    struct ChaseActor *actor = obj;
    struct SceneActorChaseGroup *group = &actor->group;
    if (!SceneActorChase_MotionFlags(actor, 0x400) &&
        !SceneActorChase_TestField((u16 *)&group->common.gate.flags, 4)) {
        group->turn.direction =
            SceneActorChase_RandomDirectionExcept(actor->base.body.direction, randomSamples);
        SceneActorChase_SaveControl(&group->saved, actor);
        group->turn.savedDirection = saved;
        SceneActorChase_SetControl(actor, group->turnControl, 0x1B);
        SceneActorChase_SetState(actor, 3, 1, 0);
        group->turn.turnsRemaining = 6;
    }
}

/* A successful cell-center reroute returns one. Other paths leave the
 * caller to try the separate partner-position route. */

static inline u8 Skeleton_ArriveAfterRecovery(void *obj)
{
    struct ContactPosition target;
    struct ChaseActor *actor = obj;
    struct SceneActorChaseGroup *group = &actor->group;
    struct SceneActorChaseTracking *tracking = &actor->base.tracking;
    s32 cell, player;
    const u16 *randomSamples = gRandomTable;
    void **callback;
    Skeleton_SnapRecoveryRoute(group->destination, &group->homeRoute);
    if (SceneActorChase_Test(gEngineFlags, 0x4800)) {
        struct SceneActorChaseTarget *t = tracking->target;
        if (!t)
            goto invalid_target;
        player = SceneActorChase_PlayerSlot(t);
        if (player < 0) {
        invalid_target:
            callback = &group->fallback;
            goto invoke;
        }
        cell = ((struct SceneActorChasePlayer *)gPlayers[player])->cell->index;
    } else {
        struct SceneActorChaseTarget *t = tracking->target;
        cell = SceneActorChase_CellAt(&t->position);
        player = 0;
    }
    if (cell != actor->base.terrainCell &&
        !SceneActorChase_Bits32(&gPlayers[player]->statusFlags,
                                0x20000))
        goto reroute;
    if (SceneActorChase_MotionFlags(actor, 0x400))
        return 0;
    {
        u32 bits = 4;
        group->common.gate.flags &= ~bits;
    }
    if (SceneActorChase_MotionFlags(actor, 0x80)) {
        SceneActorChase_SetControl(actor, group->common.gate.control, 0x1A);
        SceneActorChase_SetBehavior(actor, 12);
    } else {
        SceneActorChase_SetControl(actor, actor->base.onAlternateEntry, 1);
        SceneActorChase_SetBehavior(actor, 5);
        Skeleton_SnapRecoveryRoute(group->destination, &group->homeRoute);
        (group->destination)->y = SceneActorChase_HeightAt(
            SceneActorChase_CellAt(group->destination), group->destination);
        (group->destination)->pad = 0;
    }
    {
        u8 saved = actor->base.body.direction;
        Skeleton_BeginRecoveryTurn(obj, saved, randomSamples);
    }
    SceneActorTerrain_RequestRetreatRoute(obj);
    actor->base.field2C2 = 0x20;
    return 0;
reroute: {
    SceneActorChase_CellCenter(cell, &target);
    if (SceneActorChase_TryRoute(obj, group->route, &actor->base.body.position, &target))
        goto success;
}
    callback = &group->fallback;
invoke:
    Callback_InvokeActor(actor, *callback);
    return 0;
success:
    return 1;
}

static inline u8 Skeleton_DisplacementStopped(s32 dx, s32 dz)
{
    if (dx == 0 && dz == 0)
        return 1;
    return 0;
}

static inline u8 Skeleton_Drift(struct ChaseActor *actor)
{
    s32 dx, dz;
    s16 *x, *z;
    if (SceneActorReaction_TakePrepared(actor))
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 3);
    SceneActorChase_SetActorFlags(actor, 0x2000);
    {
        u32 mask = 0x10000;
        actor->base.statusFlags |= mask;
    }
    dx = SceneActorDisplacement_ReadAxis(actor->base.reactionStart.x, &actor->base.reactionEnd.x,
                                         &x);
    dz = SceneActorDisplacement_ReadAxis(actor->base.reactionStart.z, &actor->base.reactionEnd.z,
                                         &z);
    if (dx > 200)
        dx = 200;
    else if (dx < -200)
        dx = -200;
    if (dz > 200)
        dz = 200;
    else if (dz < -200)
        dz = -200;
    actor->base.body.motion.x += dx;
    actor->base.body.motion.z += dz;
    *x += dx;
    *z += dz;
    return Skeleton_DisplacementStopped(dx, dz);
}

static inline void Skeleton_Settle(struct ChaseActor *actor, struct SceneActorChaseGroup *group,
                                   struct Skeleton *context)
{
    SceneActorChase_ClearMotionFlagComponent(actor, 0x120);
    SceneActorChase_ClearActorFlags(actor, 0x80);
    if (actor->base.hitReactionFlags & 0x80000) {
        SceneActorChase_SetReactionFlags(actor, 2);
        return;
    }
    if (SceneActorChase_QueryMotionFlagComponent(actor, 0x80)) {
        actor->base.behaviorFlags |= 0x80;
        SceneActorChase_SetFlagComponent((struct SceneActorChaseFlags16 *)&group->common.gate.flags,
                                         4);
        SceneActorChase_TrackPlayer(actor);
        if (!Skeleton_ArriveAfterRecovery(actor)) {
            if (!SceneActorChase_TryRoute(actor, group->route, &actor->base.body.position,
                                          &actor->base.tracking.target->position)) {
                Callback_InvokeActor(actor, group->fallback);
                return;
            }
        }
        SceneActorChase_SetControl(actor, group->common.gate.control, 0x1A);
        SceneActorChase_SetState(actor, 0, 0, 12);
    } else {
        SceneActorChase_TrackPlayer(actor);
        actor->base.behaviorFlags |= 0x10;
        group->destination->pad = 0;
        if (!Skeleton_ArriveAfterRecovery(actor)) {
            context->pursuitTimer = 0;
            if (!SceneActorChase_TryRoute(actor, group->route, &actor->base.body.position,
                                          &actor->base.tracking.target->position)) {
                Callback_InvokeActor(actor, group->fallback);
                return;
            }
        }
        SceneActorChase_SetControl(actor, actor->base.onAlternateEntry, 1);
        SceneActorChase_SetState(actor, 0, 0, 7);
        actor->base.field2C2 = 0x20;
    }
}

void Skeleton_ConvergeDisplacedPosition(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    if (Skeleton_Drift(actor)) {
        struct Skeleton *context = actor->base.context;
        actor->base.stepDone = 1;
        Skeleton_Settle(actor, group, context);
    }
}

s32 Callback_InvokeActor(void *, void *);


static inline u8 TryMove(struct ChaseActor *actor, struct SceneActorChaseGroup *group)
{
    void *route = group->route;
    struct ContactPosition *position = &actor->base.body.position;
    struct ContactPosition *destination = &actor->base.tracking.target->position;
    s32 result;
    if (!Nav_IsPositionInMesh(route, position, destination)) {
        SceneActorPlacement_ClearFlags(&actor->base.motionFlags, ~0x200);
        return 0;
    } else {
        result = Terrain_ApplyBlocked(route, position, destination);
        if (result) {
            if (result != 2)
                Memory_Clear(&actor->base.body.motion, 8);
            return 1;
        } else {
            SceneActorPlacement_ClearFlags(&actor->base.motionFlags, ~0x200);
            return 0;
        }
    }
}

void SceneActorApproach_UpdateRange(struct ChaseActor *actor)
{
    struct Skeleton *context = actor->base.context;
    struct SceneActorChaseGroup *group = &actor->group;
    if (SceneActorReaction_TakePrepared(actor))
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    else if (actor->base.tracking.distance > context->farDistance ||
             actor->base.tracking.distance < context->nearDistance) {
        SceneActorChase_AndWordFlags(&context->actionFlags, ~0x100);
        Skeleton_SelectDistanceReaction(actor);
    } else if (SceneActorReaction_IsRangeReady(actor)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        if (!TryMove(actor, group))
            Callback_InvokeActor(actor, group->fallback);
        else {
            SceneActorChase_AndWordFlags(&context->actionFlags, ~0x100);
            Skeleton_ReturnToBehavior(actor, 15);
        }
    }
}

/* Skeleton_UpdateTargetTransition: skeleton variant of the target-motion transition */

void Skeleton_UpdateTargetTransition(struct ChaseActor *a)
{
    struct Skeleton *c = a->base.context;
    if (!SceneActorReaction_TakePrepared(a)) {
        u32 flag = 0x100;
        u32 *address = &c->actionFlags;
        u32 value = *address & flag;
        u32 *flags = address;
        if (IsActive(value)) {
            if (a->base.tracking.distance > c->farDistance) {
                Skeleton_ReturnToBehavior(a, 17);
                {
                    s32 angle = a->base.tracking.heading;
                    if (angle >= 0)
                        angle >>= 5;
                    else
                        angle = -(-angle >> 5);
                    a->base.nextDirection = (angle + 3) & 7;
                }
            } else {
                s32 mask = ~0x100;
                *flags &= mask;
                Skeleton_SelectDistanceReaction(a);
            }
        } else
            SceneActorAnimation_SelectDirectionalEntry(&a->base, 1);
    }
}

/* Sinks the actor into the ground after frame 6, then starts the hidden defeat. */

void SceneActorEffects_AdvanceSteps(void *, u32);

void SceneActorReaction_BeginHiddenDefeat(struct ChaseActor *);

static inline u8 IsControlled(struct ChaseActor *a)
{
    if (a->base.object == (void *)1)
        return 1;
    return 0;
}

static inline u8 ContextFlag(u8 *context)
{
    u32 mask = 0x10;
    if (*(u32 *)(context + 0x6DC) & mask)
        return 1;
    return 0;
}

static inline void ClearRoute(struct ChaseActor *a, struct SceneActorRegistryControlGroup *group)
{
    struct SceneActorRegistryControl *route = &group->control;
    SceneActorChase_ClearWordFlags(&a->base.behaviorFlags, 0x141C0);
    route->counter = 0;
    route->field14 = 0;
    route->timer = 0;
}

void SceneActorReaction_SinkThenDefeat(struct ChaseActor *actor, s32 frame)
{
    u32 state;
    struct SceneActorRegistryControlGroup *group = &actor->group.common;
    if (SceneActorReaction_TakePrepared(actor)) {
        ClearRoute(actor, group);
        if (!ContextFlag(actor->base.context)) {
            SceneActorAnimation_SelectDirectionalEntry((void *)actor, 3);
            SceneActorAttachment_ReplaceResponseSlot(actor, 0, 1, actor->base.category);
        }
        {
            u16 *destination = &actor->base.colliderA.flags;
            u32 flags = *destination;
            u32 mask = 4;
            mask |= flags;
            *destination = mask;
        }
        ActorStatus_ClearDrawableFlags(actor, 1);
        {
            u32 zero;
            u8 *first;
            state = 3;
            first = &actor->base.state2A8;
            zero = 0;
            *first = state;
            actor->base.phase2A9 = zero;
            actor->base.behavior2AA = zero;
            actor->base.counter2AC = zero;
            actor->base.field2B1 = 1;
        }
        actor->base.attachmentTimer = 0;
        actor->base.restart = 0;
        actor->base.colliderA.responseValue = 0;
        actor->base.hitLockTimer = 0;
        {
            u32 mask = 0x20000;
            u32 *flags = &actor->base.statusFlags;
            *flags |= mask;
        }
    }
    if (frame > 6) {
        actor->base.body.position.y -= (u32)(frame - 6) * 30;
        if (!actor->base.drawableKind)
            ((struct SceneActorRegistrySequenceDrawable *)actor->base.drawable)->position =
                actor->base.body.position;
        else
            ((struct SceneActorRegistryTileDrawable *)actor->base.drawable)->position =
                actor->base.body.position;
        if (frame > 8) {
            u32 zeroWord = 0;
            {
                s16 health = zeroWord;
                actor->base.health = health;
            }
            {
                u32 mask = 8;
                u16 *field = &actor->base.motionFlags;
                mask |= *field;
                *field = mask;
            }
            {
                u32 mask = 0x400;
                u32 *flags = &actor->base.behaviorFlags;
                *flags |= mask;
            }
            SceneActorAttachment_ReplaceResponseSlot(actor, 0, 127, 0);
            SceneActorAttachment_ReplaceResponseSlot(actor, 1, 127, 0);
            SceneActorAttachment_ReplaceResponseSlot(actor, 3, 127, 0);
            SceneActorEffects_AdvanceSteps(actor, 4);
            SceneActorChase_SetControl(actor, SceneActorReaction_BeginHiddenDefeat, 23);
        }
    }
}
