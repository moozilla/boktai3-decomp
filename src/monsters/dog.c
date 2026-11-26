#include "global.h"
#include "engine/math/math_shift.h"
#include "scene/actor/scene_actor_visibility.h"
#include "scene/actor/scene_actor_status.h"
#include "scene/actor/scene_actor_availability.h"
#include "player/player.h"
#include "monsters/dog.h"
#include "scene/actor/scene_actor_reset.h"
#include "scene/actor/scene_actor_chase.h"
#include "engine/math/rotation_phase.h"
#include "scene/actor/scene_actor_profile.h"
#include "scene/actor/scene_actor_palette.h"
#include "scene/actor/scene_actor_control_motion.h"
#include "npc/manager/scene_actor_animation.h"
#include "scene/actor/scene_actor_turn.h"
#include "engine/video/display_sequence_inline.h"
#include "scene/actor/scene_actor_state.h"
#include "scene/actor/scene_actor_registry.h"
#include "engine/video/display_sequence.h"
#include "engine/ram.h"
#include "scene/actor/scene_actor_collision.h"
#include "scene/actor/scene_actor_behavior.h"
#include "script/script_utility.h"
#include "scene/actor/scene_actor_chase_reaction.h"
#include "scene/actor/scene_actor_contact_response.h"
#include "scene/actor/scene_actor_contact_list.h"
#include "scene/actor/scene_actor_effects.h"
#include "scene/actor/contact_actor.h"
#include "engine/engine.h"
#include "engine/heap.h"
#include "engine/resource_registry.h"
#include "engine/math/monster_particle_random.h"
#include "scene/actor/scene_actor_reaction.h"
#include "scene/actor/scene_actor_terrain.h"
#include "scene/actor/scene_actor_control_phase.h"
#include "scene/actor/scene_actor_delayed_response.h"
#include "engine/video/render.h"
#include "engine/engine_globals.h"
#include "monsters/scene_actor_init_primitives.h"
#include "monsters/scene_actor_init_group.h"
#include "scene/actor/scene_actor_init_dispatch.h"
#include "monsters/scene_actor_init_graphics.h"
#include "monsters/scene_actor_init_body.h"
#include "scene/actor/scene_actor_init_actor.h"
#include "monsters/ghoul_disc.h"

s32 Div(s32, s32);

void Actor_InitLaunchVelocity(struct ChaseActor *actor)
{
    struct ContactPosition delta;
    Chase_Delta(&delta, &actor->base.body.position, &actor->base.tracking.target->position);
    actor->base.secondaryMotion.x = Div(delta.x, 0x1c);
    actor->base.secondaryMotion.y = 0x20;
    actor->base.secondaryMotion.z = Div(delta.z, 0x1c);
}

u8 Dog_IsActionTimerExpired(struct ChaseActor *sIn);

void Dog_SelectRetreatOrChase(struct ChaseActor *a);

void Dog_SelectTargetAction(struct ChaseActor *actor);

void SceneActorControl_OnIdle(u8 *);

static inline u8 ContextBlocked(struct Dog *p)
{
    u32 mask = 8;
    if (p->flags & mask)
        return 1;
    return 0;
}

static inline void Or16(u16 *p, u32 mask)
{
    u32 value = mask;
    value |= *p;
    *p = value;
}

static inline void OrMotion(struct SceneActorReactionMotion *p, u32 mask)
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

void Dog_UpdateTargetGate(struct ChaseActor *p)
{
    struct Dog *context = p->base.context;
    u8 active;
    u32 v;
    u16 *f;
    context->actionTimer = context->actionDelay;
    active = SceneActorChase_TrackingFlags(&p->base.tracking, 8);
    if (active) {
        SceneActorChase_GroupSetFlags(&p->group, 4);
        v = 0x80;
        f = &p->base.motionFlags;
        v |= *f;
        *f = v;
        p->group.common.gate.counter = p->group.common.gate.duration;
        if (SceneActorTurn_Flags(f, 0x100) || !Dog_IsActionTimerExpired(p)) {
            if (!ContextBlocked(context))
                Dog_SelectRetreatOrChase(p);
            return;
        } else {
            Dog_SelectTargetAction(p);
        }
    } else {
        SceneActorChase_AndWordFlags(&context->flags, ~12);
        SceneActorChase_SetPhase(p, 1, 0);
        SceneActorChase_SetControl(p, SceneActorControl_OnIdle, 0x10);
    }
    OrMotion((struct SceneActorReactionMotion *)&p->base.tracking.target, 8);
}

/* Dog_RestoreRoute: unless flags2D4 bit 0x100 is set, restores after a delay, records the follow-up control (chase when flags2D4 & 0x80, else the entry target) and routes back toward the chased object via the route point (provisional name) */

// confidence=low: dog cohort variant of the shared delayed restoration routine.

void Dog_RestoreRoute(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    u16 *flags;
    u32 chasing;

    if (SceneActorContact_QueryMotionField(actor, 0x100, (void *)&flags))
        return;
    SceneActorChase_SetState(actor, 3, 1, 0);
    SceneActorChase_SetControl(actor, (void *)SceneActorReaction_RestoreAfterDelay, 0xE);
    chasing = 0x80;
    if (*flags & chasing) {
        actor->base.field2C2 = 0x20;
        SceneActorDelay_Prepare((void *)&actor->group.saved, group->common.gate.control, 0xC, 0x14);
        if (*flags & chasing) {
            u16 *time = &group->common.gate.counter;
            *time += 0x12C;
            if (*time > actor->group.common.gate.duration)
                *time = actor->group.common.gate.duration;
        }
        {
            u16 *field = (u16 *)&group->common.gate.flags;
            u32 mask = 4;
            u32 value = *field;
            mask |= value;
            *field = mask;
        }
    } else {
        SceneActorChase_SetActorFlags(actor, 0x10);
        actor->base.field2C2 = 0x20;
        SceneActorDelay_Prepare((void *)&actor->group.saved, actor->base.onAlternateEntry, 7, 0x3C);
    }
    Chase_SnapToRoute(group->destination, (u8 *)&group->homeRoute);
    (group->destination)->pad = 0;
    if (!Chase_TryRoute(actor, group->route, &actor->base.body.position,
                        &actor->base.tracking.target->position))
        Callback_InvokeActor(actor, group->fallback);
}

// confidence=low: chooses response 0x75/0x82/0x87 from target range, heading, and action timers.

extern const u8 gUnk_0824E2A8[];

static inline u8 Dog_HasAwarenessFlag(struct Dog *context, u32 mask)
{
    u32 value = context->flags;
    if (value & mask)
        return 1;
    return 0;
}

static inline void Response(struct ChaseActor *actor, struct SceneActorChaseGroup *group,
                            u32 behavior, u32 response)
{
    Chase_SetState(actor, 0, 2, behavior);
    Chase_SetCtl(actor, (void *)SceneActorControl_OnIdle, 0x10);
    SceneActorReaction_ResetResponseState(&group->saved, gUnk_0824E2A8, response);
}

void Dog_SelectTargetAction(struct ChaseActor *actor)
{
    struct Dog *context = actor->base.context;
    struct SceneActorChaseGroup *nav = &actor->group;
    if (SceneActorContact_MotionFlags(actor, 0x100))
        return;
    if (actor->base.tracking.distance < actor->base.tracking.range) {
        struct ContactPosition delta;
        s32 angle;
        {
            struct ContactPosition *out = &delta;
            struct ContactPosition *from = &actor->base.body.position;
            struct ContactPosition *to =
                &((struct ChaseTarget *)actor->base.tracking.target)->position;
            SceneActorTerrain_Subtract(out, to, from);
        }
        angle =
            RotationPhase_HeadingDistance(Math_Atan2(delta.x, delta.z), actor->base.body.direction);
        if (angle <= 23) {
            u32 mode = actor->base.paletteOffset;
            if (mode == 0 || mode == 2) {
                if (actor->base.tracking.distance < context->nearRangeSquared &&
                    !context->nearTimer) {
                    Response(actor, nav, 0, 0x75);
                    return;
                }

                if (Dog_HasAwarenessFlag(context, 8))
                    return;
                goto move;
            } else {
                if (actor->base.tracking.distance < context->nearRangeSquared &&
                    !context->nearTimer) {
                    Response(actor, nav, 0, 0x82);
                    return;
                }
                if (actor->base.tracking.distance < context->farRangeSquared && angle <= 5 &&
                    !context->farTimer) {
                    Response(actor, nav, 1, 0x87);
                    return;
                }
                if (Dog_HasAwarenessFlag(context, 8))
                    return;
                goto move;
            }
        }
        if (!Dog_HasAwarenessFlag(context, 8)) {
        move:
            Dog_SelectRetreatOrChase(actor);
        }
    } else {
        if (!Dog_HasAwarenessFlag(context, 8))
            Dog_SelectRetreatOrChase(actor);
    }
}

static inline s32 Absolute(s32 value)
{
    s32 result = value;
    if (result < 0)
        result = -result;
    return result;
}

static inline u16 Motion(struct ChaseActor *a, u32 mask)
{
    u32 bit = mask;
    u16 *field = &a->base.motionFlags;
    {
        u32 result = *field & bit;
        return result;
    }
}

static inline void SetTurn(struct ChaseActor *a, u32 value)
{
    u32 bits = 7;
    bits &= value;
    a->base.nextDirection = bits;
}

u8 Dog_RequestHeadingTurn(struct ChaseActor *a, s32 heading)
{
    s32 difference = heading - a->base.body.direction;
    u16 motion;
    if (difference > 128)
        difference -= 256;
    else if (difference < -128)
        difference += 256;
    if (Absolute(difference) < a->base.tracking.fieldEA || a->base.controlKind == 2)
        return 0;
    Chase_SetCtl(a, SceneActorControl_OnIdle, 0x10);
    motion = Motion(a, 0x80);
    if (motion)
        Chase_SetBehavior(a, 13);
    else
        Chase_SetBehavior(a, 6);
    SetTurn(a, ((u16)heading + 0x70) >> 5);
    return 1;
}

void SceneActor_RefreshLivingCountAndProfile(struct ChaseActor *actor)
{
    u32 category = actor->base.category;
    s32 count = SceneActorRegistry_CountLivingByCategory(category);

    actor->base.profileMode = count;
    actor->base.profileId = SceneActorProfiles_FindOrAdd(category, count);
}

// confidence=low: clears action gates before resolving the route and stepping its drawable.

void SceneActorMovement_ResolveRoutePosition(void *);

void Dog_ResetActionState(struct ChaseActor *actor)
{
    struct Dog *context = actor->base.context;
    context->flags = 0;
    context->actionTimer = 0;
    context->stolenSlot = 0xFFFF;
    context->stolenParameter = 0;
    context->turnClockwise = 0;
    context->retreatCount = 0;
    Memory_Clear(&context->leapTarget, 8);
    SceneActorMovement_ResolveRoutePosition(actor);
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    {
        struct SceneActorControlMotionDrawable *drawable = actor->base.drawable;
        DisplaySequence_StepState(drawable, &drawable->sequence);
    }
}

void Dog_OnClearActivityFlag(struct ChaseActor *actor)
{
    u32 *p = &actor->base.statusFlags;
    u32 m = ~2;
    *p = *p & m;
}

extern const s16 gUnk_086149C4[];

extern const u16 gUnk_086061E4[][8];

void SpriteEffectController_Create(void *, const void *, s32, u32, u32, u32, u32, u32, u32, u32,
                                   u32, u32, s32, s32, s32, s32, s32, s32);

static inline void AddPosition(struct ContactPosition *out, const struct ContactPosition *a,
                               const struct ContactPosition *b)
{
    out->x = b->x + a->x;
    out->y = b->y + a->y;
    out->z = b->z + a->z;
}

static inline void SetDirectionOffset(struct ContactPosition *position, u32 angle, s32 distance)
{
    position->x = Math_ShiftTowardZero(gUnk_086149C4[(angle + 0x40) & 0xFF] * distance, 12);
    position->y = 0;
    position->z = Math_ShiftTowardZero(gUnk_086149C4[angle] * distance, 12);
}

static inline u32 ReadPaletteOffset(u16 *offset)
{
    return *offset;
}

u16 Dog_UpdateItemDrop(struct ChaseActor *actor)
{
    struct DisplaySequenceState *sequence =
        &((struct SceneActorControlMotionDrawable *)actor->base.drawable)->sequence;
    u32 alternate;
    u16 *paletteOffset;
    u32 palette;
    union SceneActorRegistryPackedPosition position;
    struct ContactPosition offset;
    struct ContactPosition velocity;

    if (sequence->elapsed == 0) {
        alternate = SceneActorChase_QueryMotionFlags(actor, 0x80) != 0;
        MonsterInit_SetStatusFlags(actor, 0x10000);
        palette = ReadPaletteOffset(&actor->base.paletteOffset);
        paletteOffset = &actor->base.paletteOffset;
        if (palette == 3 || palette == 1) {
            if (sequence->index == 7 || sequence->index == 5 || sequence->index == 1) {
                position.packed.x = (ParticleRandom_ReadDirectSample(gRandomTable) & 63) - 48;
                position.packed.z = (ParticleRandom_ReadDirectSample(gRandomTable) & 63) - 48;
                AddPosition(&position.position, &position.position, &actor->base.body.position);
                SetDirectionOffset(&offset, (actor->base.body.direction + 0x80) & 0xFF, 0x50);
                AddPosition(&position.position, &position.position, &offset);
                position.position.y = actor->base.body.position.y;
                ZeroFill8(&velocity);
                if (*paletteOffset == 3) {
                    s32 lifetime =
                        (ParticleRandom_ReadDirectSample((const u16 *)RANDOM_TABLE_ADDR) & 63) + 20;
                    SpriteEffectController_Create(&position, &velocity, 0x1C1A, 0xD1B8, 3, 0, 0x31,
                                                  0, 1, 0, 0, lifetime, 4, 1, 0x200004, 30, 8, 8);
                } else {
                    SpriteEffectController_Create(&position, &velocity, 0x1C1A, 0xD1B8, 0x12, 0,
                                                  0x11, 0, 1, 0, 0, 0, 4, 1, 0x400008, 30, 8, 8);
                }
            }
        }
        return gUnk_086061E4[alternate][sequence->index & 7];
    }
    return 0;
}

void SceneActor_ConfigureSecondaryProfileEntry(struct ChaseActor *actor)
{
    struct SceneActorProfileEntry *entry = (void *)SceneActorProfiles_GetEntryAddress(
        actor->base.profileId, actor->base.profileMode, 0);

    {
        u16 v = SceneActorChase_QueryMotionFlagComponent(actor, 0x280);

        if (v) {
            u8 *q;
            u32 z;
            u32 four = 4;
            z = 0;
            q = &entry[1].value;
            entry[1].value = four;
            q[1] = z;
        } else {
            entry[1].value = 3;
            entry[1].flags = v;
        }
    }

    {
        if (SceneActorChase_QueryMotionFlagComponent(actor, 0x200))
            actor->base.field2C2 = 0x20;
    }
}

void Dog_OnBodyCollision(struct ContactCollider *collider, s32 a, s32 b);

void Dog_OnSensorContact(struct ContactCollider *unused, struct ContactCollider *other,
                         struct ChaseActor *actor);

void Dog_OnAttackContact(struct ContactCollider *collider, struct ContactCollider *other,
                         struct ChaseActor *actor);

void SceneActorCollision_InitBody(struct ChaseActor *, void *, void *, void *, void *);

void SceneActorCollision_InitAttack(struct ChaseActor *, void *, void *, void *, u32, void *);

void SceneActorCollision_InitExternalAttack(struct ChaseActor *, struct ContactCollider *, void *,
                                            void *, void *, u32, void *);

void SceneActorCollision_InitGateColliders(struct ChaseActor *actor)
{
    struct ContactPackedVector bodySize, bodyOffset, attackSize, attackOffset, externalSize,
        externalOffset;
    struct Dog *context = actor->base.context;
    bodySize.x = 0x40;
    bodySize.y = 0x80;
    bodySize.z = 0x40;
    bodyOffset.x = 0;
    bodyOffset.y = 0x80;
    bodyOffset.z = 0;
    SceneActorCollision_InitBody(actor, &bodySize, &bodyOffset, Dog_OnBodyCollision, actor);
    attackSize.x = 0x40;
    attackSize.y = 0x78;
    attackSize.z = 0x40;
    attackOffset.x = 0;
    attackOffset.y = 0x80;
    attackOffset.z = 0;
    SceneActorCollision_InitAttack(actor, &attackSize, &attackOffset, Dog_OnSensorContact, 0,
                                   actor);
    externalSize.x = 0;
    externalSize.y = 0x78;
    externalSize.z = 0;
    externalOffset.x = 0;
    externalOffset.y = 0x80;
    externalOffset.z = 0;
    SceneActorCollision_InitExternalAttack(actor, &context->collider, &externalSize,
                                           &externalOffset, Dog_OnAttackContact, 1, actor);
    {
        u32 mask = 0x100;
        struct ContactCollider *collider = &context->collider;
        mask |= collider->flags;
        collider->flags = mask;
    }
}

// confidence=low: places forward collider and selects direction-dependent contact bounds.

void Collider_SetPosition(void *, void *, u32);

static inline u8 TestContext(struct Dog *context, u32 mask, u32 **out)
{
    u32 *field = &context->flags;
    u32 value = *field & mask;
    *out = field;
    if (value)
        return 1;
    return 0;
}

void Dog_UpdateCollisionBounds(struct ChaseActor *a)
{
    union SceneActorRegistryPackedPosition position;
    union SceneActorRegistryPackedPosition wide;
    union SceneActorRegistryPackedPosition narrow;
    struct Dog *context = a->base.context;
    s32 heading;
    u32 *flags;
    if (SceneActor_ActionsBlocked(a))
        return;
    position.position = a->base.body.position;
    heading = Math_ShiftTowardZero((a->base.body.direction + 32) & 255, 6) << 6;
    {
        if (TestContext(context, 2, &flags)) {
            position.packed.x += Math_ShiftTowardZero(gUnk_086149C4[(heading + 64) & 255] << 9, 12);
            position.packed.z += Math_ShiftTowardZero(gUnk_086149C4[heading & 255] << 9, 12);
        } else {
            position.packed.x += Math_ShiftTowardZero(gUnk_086149C4[(heading + 64) & 255] << 7, 12);
            position.packed.z += Math_ShiftTowardZero(gUnk_086149C4[heading & 255] << 7, 12);
        }
    }
    Collider_SetPosition(&context->collider, &position, 0);
    context->collider.direction = heading;
    position.position = a->base.body.position;
    switch (Math_ShiftTowardZero(heading, 6)) {
        case 0:
        case 2:
            if (WordFlags_Has(2, flags)) {
                wide.packed.x = 240;
                wide.packed.z = 128;
            }
            narrow.packed.x = 120;
            narrow.packed.z = 64;
            break;
        case 1:
        case 3:
            if (WordFlags_Has(2, flags)) {
                wide.packed.x = 128;
                wide.packed.z = 240;
            }
            narrow.packed.x = 64;
            narrow.packed.z = 120;
            break;
    }
    narrow.packed.y = 100;
    {
        struct ContactCollider *c = &a->base.colliderB;
        c->bounds = *(struct ContactPosition *)&narrow;
        Collider_SetPosition(c, &position, 0);
    }
    {
        struct ContactCollider *c = &a->base.colliderA;
        c->bounds = *(struct ContactPosition *)&narrow;
        Collider_SetPosition(c, &position, 0);
    }
    if (WordFlags_Has(2, flags)) {
        struct ContactCollider *c = &context->collider;
        c->bounds = *(struct ContactPosition *)&wide;
    } else {
        struct ContactCollider *c = &context->collider;
        c->bounds = *(struct ContactPosition *)&narrow;
    }
}

void Dog_UpdateActionTimers(struct ChaseActor *actor)
{
    struct Dog *context = actor->base.context;
    u16 *p = &context->nearTimer;
    if (*p != 0)
        *p = *p - 1;
    p = &context->farTimer;
    if (*p != 0)
        *p = *p - 1;
}

void Dog_UpdateActionCooldown(struct ChaseActor *actor)
{
    struct Dog *context = actor->base.context;
    u16 *p = &context->actionCooldown;
    if (*p != 0)
        *p = *p - 1;
}

static inline u8 HasPending(void **slot)
{
    if (((struct Dog *)*slot)->flags & 1)
        return 1;
    return 0;
}

static inline void QueueAction(struct ChaseActor *actor)
{
    Chase_SetBehavior(actor, 15);
    {
        u32 *flags;
        s32 mask;
        struct Dog *ctx;
        ctx = actor->base.context;
        flags = &ctx->flags;
        mask = -2;
        *flags &= mask;
    }
}

void SceneActorControl_TickActionCooldown(struct ChaseActor *actor)
{
    struct Dog *cooldown = actor->base.context;
    u32 flags;
    if (SceneActor_ActionsBlocked(actor))
        return;
    {
        u32 mask = 0xE33F;
        flags = actor->base.motionFlags;
        if (mask & flags)
            return;
    }
    if (SceneActorTurn_ActorFlags(actor, 0x10))
        return;
    if (actor->base.controlKind > 1)
        return;
    if (cooldown->actionTimer) {
        cooldown->actionTimer--;
        return;
    }
    if (flags & 0x80)
        cooldown->actionTimer = cooldown->alternateActionDelay;
    else
        cooldown->actionTimer = cooldown->actionDelay;
    {
        u32 one = 1;
        u32 *p = &cooldown->flags;
        *p |= one;
    }
    if (HasPending(&actor->base.context)) {
        QueueAction(actor);
    }
}

void SceneActorControl_CallContextScript(struct RoomActor *actor)
{
    s32 values[12];
    struct ScriptUtilityArguments arguments;
    struct Dog *context;
    {
        u32 mask = 0x80000;
        u32 *field = &actor->statusFlags;
        if (*field & mask)
            return;
    }
    context = actor->context;
    if (!actor->script1F4)
        return;
    values[0] = actor->body.position.x;
    values[1] = actor->body.position.y;
    values[2] = actor->body.position.z;
    values[3] = actor->parameters1FC[0];
    values[4] = actor->parameters1FC[1];
    values[5] = actor->parameters1FC[2];
    if (context->stolenSlot < 0) {
        values[6] = -1;
        values[7] = -1;
    } else {
        values[6] = context->stolenKind;
        values[7] = context->stolenParameter;
    }
    values[8] = actor->category;
    values[9] = actor->paletteOffset;
    values[10] = actor->hitAttributes;
    values[11] = -1;
    arguments.count = 12;
    arguments.values = values;
    Script_Call(actor->script1F4, &arguments);
}

u8 SceneActorTerrain_CanTraverseCell(u32, u32, u32, u32);

static inline s8 TurnClockwise(struct Dog *context)
{
    return context->turnClockwise;
}

#define SetTurnClockwise(object_, value_)                                                          \
    do {                                                                                           \
        struct Dog *context_ = (object_);                                                          \
        s16 offset_ = (u32) & ((struct Dog *)0)->turnClockwise;                                    \
        *((u8 *)context_ + offset_) = (value_);                                                    \
    } while (0)

static inline void *GroupRoute(void *object)
{
    struct SceneActorChaseGroup *group = object;
    s16 offset = (u32) & ((struct SceneActorChaseGroup *)0)->route;
    return *(void **)((u8 *)group + offset);
}

static inline u32 CellShape(struct EngineMap **terrain, u32 cell)
{
    u8 *data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data += 4;
    else
        data = (*terrain)->cells + (cell * 4 + 12);
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

static inline void Dog_CaptureRoutePosition(struct ContactPosition **out, struct ChaseActor *actor)
{
    *out = &actor->base.body.position;
}

/* Scan candidate cells, preserve the interruption map owner, then start the route. */

static inline u32 SelectRetreat(struct ChaseActor *actor, u32 priorDirection, s32 steps)
{
    struct Dog *context = actor->base.context;
    struct SceneActorChaseGroup *group = &actor->group;
    struct ContactPosition center;
    struct ContactPosition *routePosition;
    s32 tries = 0;
    s32 chosen;

    do {
        {
            u32 cell, height, shape;
            s32 next, i;
            struct ContactPosition *position;
            struct EngineMap **terrain, **completion;
            cell = actor->base.terrainCell;
            position = &actor->base.body.position;
            Dog_CaptureRoutePosition(&routePosition, actor);
            terrain = &gTerrainMap;
            height = SceneActorChase_HeightAt(cell, position);
            shape = CellShape(terrain, cell);
            switch (Math_ShiftTowardZero(actor->base.body.direction + 32, 6) & 3) {
                case 0:
                    next = cell + 1;
                    i = 0;
                    do {
                        if (!SceneActorTerrain_CanTraverseCell(cell, next, height, shape))
                            break;
                        next++;
                        i++;
                    } while (i < steps);
                    chosen = next - 1;
                    goto scanDone;
                case 1:
                    next = cell + Ghoul_MapWidth(terrain);
                    i = 0;
                    completion = (struct EngineMap **)0x030052F4;
                    do {
                        if (!SceneActorTerrain_CanTraverseCell(cell, next, height, shape))
                            goto finishSouth;
                        next += Ghoul_MapWidth(terrain);
                        i++;
                    } while (i < steps);
                    chosen = next - Ghoul_MapWidth(completion);
                    goto scanDone;
                case 2:
                    next = cell - 1;
                    i = 0;
                    do {
                        if (!SceneActorTerrain_CanTraverseCell(cell, next, height, shape))
                            break;
                        next--;
                        i++;
                    } while (i < steps);
                    chosen = next + 1;
                    goto scanDone;
                case 3:
                    next = cell - Ghoul_MapWidth(terrain);
                    i = 0;
                    completion = (struct EngineMap **)0x030052F4;
                    do {
                        if (!SceneActorTerrain_CanTraverseCell(cell, next, height, shape))
                            goto finishNorth;
                        next -= Ghoul_MapWidth(terrain);
                        i++;
                    } while (i < steps);
                    chosen = next + Ghoul_MapWidth(completion);
                    goto scanDone;
                finishSouth:
                    chosen = next - Ghoul_MapWidth(terrain);
                    goto scanDone;
                finishNorth:
                    chosen = next + Ghoul_MapWidth(terrain);
                    goto scanDone;
                default:
                    chosen = actor->base.terrainCell;
                    goto scanDone;
            }

        scanDone:;
        }
        if (chosen != actor->base.terrainCell)
            goto selected;
        if (TurnClockwise(context))
            actor->base.body.direction += 64;
        else
            actor->base.body.direction -= 64;
        tries++;
    } while (tries <= 3);
selected: {
    u32 index = (gRandomIndex + 1) & 1023;
    s32 value;
    gRandomIndex = index;
    value = *(const u8 *)(RANDOM_TABLE_ADDR + index * 2);
    if (value <= 127)
        SetTurnClockwise(context, 1);
    else
        SetTurnClockwise(context, 0);
}
    actor->base.body.direction = priorDirection;
    SceneActorChase_CellCenter(chosen, &center);
    {
        if (TryRoute(actor, GroupRoute(group), routePosition, &center)) {
            context->retreatCount++;
            return 1;
        }
        Callback_InvokeActor(actor, group->fallback);
    }
    return 0;
}

u8 Dog_SelectRetreatDirection(struct ChaseActor *actor, u32 priorDirection)
{
    return SelectRetreat(actor, priorDirection, 1);
}

u8 Dog_IsActionTimerExpired(struct ChaseActor *actor)
{
    struct Dog *context = actor->base.context;
    u16 v;
    if (actor->base.paletteOffset == 0 || actor->base.paletteOffset == 2)
        v = context->nearTimer;
    else {
        if (context->nearTimer == 0)
            return 1;
        v = context->farTimer;
    }
    if (v != 0)
        return 0;
    return 1;
}

void SceneActorTarget_ResetContactResponse(struct RoomActor *);

static inline u8 BoolOf(u32 v)
{
    if (v)
        return 1;
    return 0;
}

s32 Dog_ResolvePendingObject(struct ChaseActor *actor)
{
    struct ChaseActor *owner = actor;
    s32 *pendingId = &owner->base.counter304;
    struct SceneActorChaseTracking *state;
    if (*pendingId > 0) {
        struct ContactPosition delta;
        SceneActorTarget_ResetContactResponse((void *)owner);
        SceneActorChase_ClearFlagComponent((void *)&owner->group.common.gate.flags, 2);
        SceneActorChase_SetReactionFlags(owner, 0x80);
        state = &owner->base.tracking;
        SceneActorContact_SetTrackingFlags((struct SceneActorContactTracking *)state, 0x10);
        state->target = (void *)SceneActorContacts_FindObjectById(*pendingId);
        SceneActorContact_SaveTargetCell(owner, Chase_CellAt(&state->target->position));
        Chase_Delta(&delta, &owner->base.body.position, &state->target->position);
        owner->base.body.direction = Math_Atan2(delta.x, delta.z);
        {
            u32 *flags = &((struct Dog *)owner->base.context)->flags;
            u32 mask = ~0xC;
            *flags &= mask;
        }
        return 1;
    } else {
        struct SceneActorChaseTracking *t = &owner->base.tracking;
        u32 m2 = 2;
        u32 v = t->flags & m2;
        state = t;
        if (BoolOf(v)) {
            u32 mask = ~0x10;
            mask &= (state)->flags;
            (state)->flags = mask;
        }
        return 0;
    }
}

u8 Dog_SelectRetreatDirection(struct ChaseActor *actor, u32 priorDirection);

u8 SceneActorTarget_TrySelectNearestParticipant(struct ChaseActor *);

void SceneActorMotion_TestTargetTransition(struct ChaseActor *a);

void Dog_SelectRetreatOrChase(struct ChaseActor *a)
{
    struct Dog *context = a->base.context;
    struct SceneActorChaseGroup *group = &a->group;
    SceneActorReaction_ResetResponseState(&group->saved, gUnk_0824E2A8, 0x2AA);
    if (context->retreatCount <= 3) {
        if (a->base.tracking.distance < context->retreatRangeSquared ||
            !Dog_IsActionTimerExpired(a)) {
            if (context->retreatCount <= 3) {
                u32 saved = a->base.body.direction;
                a->base.body.direction = a->base.tracking.heading + 128;
                if (Dog_SelectRetreatDirection(a, saved)) {
                    Chase_SetCtl(a, SceneActorControl_OnIdle, 0x10);
                    Chase_SetBehavior(a, 17);
                }
                return;
            }
        }
        if (a->base.tracking.distance > context->chaseRangeSquared) {
            struct ChaseTarget *target;
            if (a->base.controlKind == 0x1A)
                return;
            if ((*(u32 *)&a->base.state2A8 & 0xFFFFFF) == 0xC0000)
                return;
            if (Chase_Test(gEngineFlags, 0x4800)) {
                if (!SceneActorTarget_TrySelectNearestParticipant(a))
                    return;
            } else
                SceneActorChase_TrackPlayer(a);
            {
                void *route = group->route;
                struct ContactPosition *from = &a->base.body.position;
                struct ContactPosition *to;
                target = (void *)a->base.tracking.target;
                to = &target->position;
                if (!Chase_TryRoute(a, route, from, to)) {
                    Callback_InvokeActor(a, group->fallback);
                    return;
                }
            }
            Chase_SetCtl(a, group->common.gate.control, 0x1A);
            Chase_SetBehavior(a, 12);
            return;
        }
    }
    {
        u32 state = 3;
        u32 phase = 1;
        u8 *first = &a->base.state2A8;
        u32 zero = 0;
        *first = state;
        a->base.phase2A9 = phase;
        a->base.behavior2AA = zero;
        a->base.counter2AC = zero;
        a->base.field2B1 = phase;
    }
    Chase_SetCtl(a, SceneActorMotion_TestTargetTransition, 0x2B);
}

void Dog_TickCooldown(struct Dog *dog)
{
    u8 *p = &dog->turnDelay;
    if (*p != 0)
        *p = *p - 1;
}

u8 InventorySlot_PickRandom(s16 *, s16 *, s16 *);

s32 FloatingItemEffect_Spawn(s32, s32, s32, s32, void *, void *, s32, void *);

void Inventory_ClearItemSlot(u32);

s32 Math_Atan2(s32, s32);

static inline u8 Dog_TracksPlayer(struct ChaseActor *actor)
{
    u32 mask = 2;
    if (actor->base.placementFlags & mask)
        return 1;
    return 0;
}

static inline void Dog_ClearResponseFlags(void *object)
{
    struct ChaseActor *actor = object;
    actor->base.hitAttributes = 0;
    actor->base.hitReactionFlags = 0;
}

static inline u32 *Dog_ResponseField(void *object, s16 offset)
{
    return (u32 *)((u8 *)object + offset);
}

static inline void Dog_WriteResponseWords(struct ChaseActor *actor, s32 value)
{
    *Dog_ResponseField(actor, (u32) & ((struct ChaseActor *)0)->base.hitAttributes) = value;
    *Dog_ResponseField(actor, (u32) & ((struct ChaseActor *)0)->base.hitReactionFlags) = value;
}

static inline void Dog_CopyContactCost(struct ContactCollider *collider, struct ChaseActor *actor)
{
    collider->cost = actor->base.reactionSoundVariant;
}

void Dog_OnItemContact(void *unused, struct ContactCollider *contact, struct ChaseActor *actor)
{
    struct Dog *context;
    u32 palette;
    struct SceneActorEffectsOffset position;
    struct ContactPosition *actorPosition;
    s32 responseValue, spawned, health;
    if (SceneActorChase_Test(gEngineFlags, 0x4800))
        return;
    if (contact->contactFlags != 1)
        return;
    context = actor->base.context;
    palette = actor->base.paletteOffset;
    if (context->stolenSlot >= 0)
        return;
    if (palette != 0 && palette != 2)
        return;
    position = *(struct SceneActorEffectsOffset *)&((struct SceneActorChasePlayer *)gPlayers[0])
                    ->target.position;
    position.y += 200;
    if (!InventorySlot_PickRandom(&context->stolenSlot, &context->stolenKind,
                                  &context->stolenParameter))
        return;
    spawned = FloatingItemEffect_Spawn(0, context->stolenKind, 3, 0, &position,
                                       &actor->base.body.position, 24, actor);
    actorPosition = &actor->base.body.position;
    if (spawned < 0) {
        SceneActorChase_SetFlagComponent((struct SceneActorChaseFlags16 *)&context->stolenSlot,
                                         0xFFFF);
        return;
    }
    Inventory_ClearItemSlot(context->stolenSlot);
    switch (context->stolenKind) {
        case 2:
        case 5:
        case 14:
            break;
        default:
            return;
    }
    responseValue = Div((u16)actor->base.defaultHealth * context->itemDamagePercent, 100);
    {
        health = actor->base.health;
        if (health - responseValue <= 0)
            actor->base.reactionSoundVariant = health;
        else
            actor->base.reactionSoundVariant = responseValue;
    }
    context->stolenSlot = -1;
    actor->base.tracking.target = &((struct SceneActorChasePlayer *)gPlayers[0])->target;
    actor->base.hitOwners = 1;
    Dog_ClearResponseFlags(actor);
    responseValue = 0;
    Dog_WriteResponseWords(actor, responseValue);
    Dog_CopyContactCost(&actor->base.colliderA, actor);
    actor->base.colliderA.responseParameter = 1;
    actor->base.colliderA.responseValue = 30;
    actor->base.field2E6 = 1;
    if (Dog_TracksPlayer(actor))
        actor->base.tracking.target = &((struct SceneActorChasePlayer *)gPlayers[0])->target;
    else if (!actor->base.tracking.target)
        return;
    {
        struct SceneActorChaseTarget *target = actor->base.tracking.target;
        s32 dx = target->position.x - actorPosition->x;
        s32 dz = target->position.z - actor->base.body.position.z;
        actor->base.tracking.heading = Math_Atan2(dx, dz);
        actor->base.tracking.distance = dx * dx + dz * dz;
    }
}

u32 SceneMovingParticle_CreateWithOptions(void *, void *, s32, s32, s32);

void SceneActorAnimation_SpawnTrailAtPosition(void *position, u32 palette)
{
    struct ContactPackedVector velocity;
    struct ContactPackedVector *planar;
    u32 randomOrigin;
    const u16 *earlyTable;
    u32 mask, sampleOrigin;
    s32 heading;
    s32 speed, vertical;
    struct ContactPackedVector *result;

    /* The recovered bank origin cancels when converting a sample address to its table offset. */
    randomOrigin = (u32)&gHeapHead + RESOURCE_REGISTRY_ENTRIES_ADDR;
    earlyTable = (const u16 *)RANDOM_TABLE_ADDR;
    mask = 255;
    heading = ParticleRandom_ReadDirectSample(earlyTable) & mask;
    {
        s32 raw = ParticleRandom_ReadDirectSample(earlyTable);
        s32 speedMask = 31;
        planar = &velocity;
        raw &= speedMask;
        speed = raw;
    }
    {
        s32 x, rawX;
        rawX = gUnk_086149C4[(heading + 64) & mask] * speed;
        sampleOrigin = randomOrigin;
        x = Math_ShiftTowardZero(rawX, 12);
        planar->x = x;
        planar->y = 0;
        planar->z = Math_ShiftTowardZero(gUnk_086149C4[heading] * speed, 12);
    }
    vertical = ParticleRandom_ReadRelativeSample(sampleOrigin, (const u16 *)RANDOM_TABLE_ADDR);
    vertical &= 63;
    vertical -= 48;
    result = &velocity;
    result->y = vertical;
    if (palette == 3)
        SceneMovingParticle_CreateWithOptions(position, result, 60, 3, 3);
    else
        SceneMovingParticle_CreateWithOptions(position, result, 60, 4, 3);
}

void Dog_OnSensorContact(struct ContactCollider *unused, struct ContactCollider *other,
                         struct ChaseActor *actor)
{
    int mask;
    u32 *flags;
    if ((ActorStatus_IsDown(actor) == 0) && (other->contactFlags & 0xF)) {
        actor->base.counter304 = other->collisionId;
        flags = &actor->base.statusFlags;
        mask = 0xFEFFFFFF;
        *flags &= mask;
    }
}

void Dog_OnAttackContact(struct ContactCollider *collider, struct ContactCollider *other,
                         struct ChaseActor *actor)
{
    union SceneActorRegistryPackedPosition position;
    struct ContactPosition delta;
    s32 direction;
    struct ChaseActor *facingActor = actor;
    if (ActorStatus_IsDown(actor))
        return;
    position.position = *(struct ContactPosition *)&collider->x;
    direction = ((facingActor->base.body.direction + 0x20) & 0xFF) >> 6;
    switch (direction) {
        case 0:
            position.packed.x -= 0x100;
            break;
        case 1:
            position.packed.z -= 0x100;
            break;
        case 2:
            position.packed.x += 0x100;
            break;
        case 3:
            position.packed.z += 0x100;
            break;
    }
    Chase_Delta(&delta, &position.position, (struct ContactPosition *)&other->x);
    collider->direction = Math_Atan2(delta.x, delta.z);
    if (!ActorStatus_IsDown(actor) && (other->contactFlags & 0xF)) {
        u32 *flags;
        s32 mask;
        actor->base.counter304 = other->collisionId;
        flags = &actor->base.statusFlags;
        mask = 0xFEFFFFFF;
        *flags &= mask;
    }
}

void SceneActorReaction_HandleColliderContact(struct ContactCollider *, s32, s32);

void Dog_OnBodyCollision(struct ContactCollider *collider, s32 a, s32 b)
{
    if (SceneActorTurn_Flags(&collider->flags, 0x800) == 0)
        SceneActorReaction_HandleColliderContact(collider, a, b);
}

void SceneActorAnimation_SelectDirectionalEntry(struct RoomActor *, s32);

void Dog_SelectAnimationOne(struct ChaseActor *actor)
{
    if (actor->base.field2B2 != 0)
        actor->base.field2B2 = 0;
    SceneActorAnimation_SelectDirectionalEntry(&actor->base, 1);
}

void Dog_SelectAnimationZero(struct ChaseActor *actor)
{
    if (actor->base.field2B2 != 0)
        actor->base.field2B2 = 0;
    SceneActorAnimation_SelectDirectionalEntry(&actor->base, 0);
}

u32 Route_GetCurrentPointFacing(void *);

/* Half of a facing octant, rounded toward zero. */
void Dog_UpdateCardinalTurn(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    struct SceneActorBody *motion = &actor->base.body;
    struct DisplaySequenceState *sequence;
    u16 *motionFlags;
    u8 prepared = SceneActorReaction_TakePrepared(actor);
    s32 direction, target, delta;
    u32 oldDuration;

    motionFlags = &actor->base.motionFlags;
    if (prepared) {
        actor->base.body.direction =
            (SceneActorTurn_Cardinal(actor->base.body.direction) << 6) - 0x40;
        direction = SceneActorTurn_Cardinal(actor->base.body.direction);
        if (actor->base.behavior2AA != 6 && !SceneActorTurn_Flags(motionFlags, 0x480)) {
            target = (((u16)Route_GetCurrentPointFacing(motion->currentRoute) + 0x70) >> 5) & 7;
            actor->base.nextDirection = target;
        } else {
            target = actor->base.nextDirection;
        }
        if (direction == Math_ShiftTowardZero(target, 1)) {
            actor->base.stepDone = 1;
            if (SceneActorTurn_Flags(motionFlags, 0x400)) {
                SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
                SceneActorChase_SetState(actor, 3, 1, 0);
                SceneActorChase_SetControl(actor, group->turnControl, 0x1B);
            } else {
                SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
            }
            return;
        }
        switch (direction) {
            case 0:
                if ((u32)(target - 2) <= 3) {
                    SceneActorTurn_SelectAnimation(actor, 13, 1, 0);
                    SceneActorChase_SetActorFlags(actor, 0x10000);
                } else {
                    SceneActorTurn_SelectAnimation(actor, 13, 0, 1);
                    SceneActorChase_ClearWordFlags(&actor->base.behaviorFlags, 0x10000);
                }
                break;
            case 1:
                if ((u32)(target - 4) <= 3) {
                    SceneActorTurn_SelectAnimation(actor, 13, 2, 0);
                    SceneActorChase_SetActorFlags(actor, 0x10000);
                } else {
                    SceneActorTurn_SelectAnimation(actor, 13, 3, 0);
                    SceneActorChase_ClearWordFlags(&actor->base.behaviorFlags, 0x10000);
                }
                break;
            case 2:
                if ((u32)(target - 1) > 4) {
                    SceneActorTurn_SelectAnimation(actor, 13, 3, 1);
                    SceneActorChase_SetActorFlags(actor, 0x10000);
                } else {
                    SceneActorTurn_SelectAnimation(actor, 13, 2, 1);
                    SceneActorChase_ClearWordFlags(&actor->base.behaviorFlags, 0x10000);
                }
                break;
            case 3:
                if ((u32)target <= 3) {
                    SceneActorTurn_SelectAnimation(actor, 13, 0, 0);
                    SceneActorChase_SetActorFlags(actor, 0x10000);
                } else {
                    SceneActorTurn_SelectAnimation(actor, 13, 1, 1);
                    SceneActorChase_ClearWordFlags(&actor->base.behaviorFlags, 0x10000);
                }
                break;
        }
        actor->base.restart = 0;
    }
    sequence = &((struct SceneActorAnimationSequenceDrawable *)actor->base.drawable)->sequence;
    if (sequence->elapsed == 1) {
        if (SceneActorTurn_ActorFlags(actor, 0x10000))
            motion->direction += 0x20;
        else
            motion->direction -= 0x20;
    }
    oldDuration = actor->base.field2C2;
    if (SceneActorTurn_Flags(motionFlags, 0x80))
        actor->base.field2C2 = Div(0x40, 3);
    SceneActorTurn_ApplyDuration(actor, motionFlags);
    actor->base.field2C2 = oldDuration;
    if (actor->base.restart) {
        s32 current = ((actor->base.body.direction + 0x70) >> 5) & 7;
        delta = actor->base.nextDirection - current;
        if (delta < 0)
            delta = -delta;
        if (delta <= 0) {
            actor->base.stepDone = 1;
            actor->base.body.direction =
                (SceneActorTurn_Cardinal(actor->base.body.direction) << 6) - 0x40;
            SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
            if (SceneActorTurn_Flags(motionFlags, 0x400)) {
                SceneActorChase_SetState(actor, 3, 1, 0);
                SceneActorChase_SetControl(actor, group->turnControl, 0x1B);
            }
        } else {
            SceneActorChase_SetControl(actor, actor->base.onEntry, 2);
        }
    }
}

void Dog_OnReactionCleanup(void)
{
}

s32 Mod(s32, s32);

void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);

void Dog_UpdateDefeatFinish(struct ChaseActor *actor, s32 frame);

void Dog_UpdateDefeatDelay(struct ChaseActor *actor, s32 frame)
{
    struct SceneActorChaseGroup *group = &actor->group;
    u32 mask;
    if (SceneActorControl_TakeFlag(&actor->base.field2B2)) {
        u32 test;
        u16 *flags;
        u32 value;
        SceneActorAnimation_SelectDirectionalEntry((u8 *)actor, 6);
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
            SceneActorChase_SetControl(actor, Dog_UpdateDefeatFinish, 6);
    }
}

void Dog_UpdateDefeatFinish(struct ChaseActor *actor, s32 frame)
{
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectDirectionalEntry(&actor->base, 7);
        SceneActorChase_ClearBits(&actor->base.motionFlags, 1);
    }
    if (frame == 40)
        SceneActorChase_SetReactionFlags(actor, 0x10);
    if (actor->base.restart && frame > 120)
        actor->base.stepDone = 1;
}

// confidence=low: animation 11 is the route-arrival reaction selected by Dog_ResumeRoute.

void Dog_UpdateArrivalAnimation(struct ChaseActor *a)
{
    u16 saved;
    u16 *flags;
    u16 *scale;
    u16 *originalScale;
    unsigned long long originalOffset;
    if (SceneActorReaction_TakePrepared(a)) {
        SceneActorAnimation_SelectDirectionalEntry(a, 11);
        a->base.restart = 0;
    }
    originalOffset = 0x2C2;
    originalScale = (u16 *)((u8 *)a + originalOffset);
    saved = *originalScale;
    {
        u32 mask = 0x80;
        flags = &a->base.motionFlags;
        if (SceneActorTurn_Flags(flags, mask)) {
            *originalScale = Div(0x40, 3);
        }
    }

    if (SceneActorTurn_Flags(flags, 0xA02F)) {
        u16 *p = &a->base.field2C2;
        *p = 0x40;
        scale = p;
    } else {
        u16 *p = &a->base.field2C2;
        u16 value = *p;
        scale = p;
        if (!value) {
            *scale = 0x40;
        }
    }
    if (!a->base.drawableKind)
        Anim_SetDurationScale(
            &((struct SceneActorAnimationSequenceDrawable *)a->base.drawable)->sequence, *scale);
    else
        Anim_SetFrameDurationScale(
            &((struct SceneActorAnimationTileDrawable *)a->base.drawable)->tile, *scale);
    a->base.field2C2 = saved;
    if (a->base.restart) {
        a->base.stepDone = 1;
    }
}

// confidence=low: animation completion resumes group movement or idle.

void Dog_ResumeAfterReaction(struct ChaseActor *a)
{
    struct SceneActorChaseGroup *group;
    u16 flags;
    if (SceneActorReaction_TakePrepared(a)) {
        SceneActorAnimation_SelectDirectionalEntry(a, 11);
        a->base.restart = 0;
    }
    if (!a->base.restart) {
        return;
    }
    group = &a->group;
    a->base.stepDone = 1;
    flags = SceneActorChase_QueryMotionFlags(a, 0x80);
    if (flags) {
        SceneActor_SetPositionXZ(&a->base.body,
                                 ((struct ChaseRouteProbe *)group->route)->destination.x,
                                 ((struct ChaseRouteProbe *)group->route)->destination.z);
        SceneActorAnimation_SelectDirectionalEntry(a, 0);
        SceneActorChase_SetControl(a, group->common.gate.control, 0x1A);
        SceneActorChase_SetBehavior(a, 12);
        SceneActorChase_GroupSetFlags(&a->group, 4);
        a->base.field2C2 = 0x20;
    } else {
        SceneActorAnimation_SelectDirectionalEntry(a, 0);
        SceneActorChase_SetControl(a, SceneActorControl_OnIdle, 0x10);
        SceneActorChase_SetBehavior(a, 7);
        a->base.behaviorFlags |= 0x10;
    }
}

/* This class's own copies of the shared chase inlines it compiles differently. */

static inline s32 Dog_MapWidth(struct EngineMap *map)
{
    return ((struct SceneActorChaseTerrainGrid *)map->cells)->width;
}

static inline void Dog_CellCenter(s32 cell, struct ContactPosition *position)
{
    s32 x = Mod(cell, Dog_MapWidth(SceneActorChase_CurrentMap()));
    s32 z = Div(cell, Dog_MapWidth(SceneActorChase_CurrentMap()));
    position->x = SceneActorChase_CellCenterCoordinate(x);
    {
        u32 attributes = SceneActorChase_CellProperties(cell)->attributes;
        u32 height = 15;
        height &= attributes;
        position->y = SceneActorChase_TerrainLevelHeight(height);
    }
    position->z = SceneActorChase_CellCenterCoordinate(z);
}

static inline u8 Dog_Arrive(void *obj)
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
            goto invoke;
        }
        cell = ((struct SceneActorChasePlayer *)gPlayers[player])->cell->index;
    } else {
        struct SceneActorChaseTarget *t = tracking->target;
        cell = SceneActorChase_CellAt(&(t)->position);
        player = 0;
    }
    if (cell != actor->base.terrainCell &&
        !SceneActorChase_Bits32(&gPlayers[player]->statusFlags, 0x20000))
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
    return 0;
reroute: {
    Dog_CellCenter(cell, &target);
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

static inline s32 Dog_ReadSecondaryX(const struct ChaseActor *a)
{
    return a->base.secondaryMotion.x;
}

static inline s32 Dog_ReadSecondaryZ(const struct ChaseActor *a)
{
    return a->base.secondaryMotion.z;
}

static inline s32 Dog_ReadDesiredX(const struct ChaseActor *a)
{
    return a->base.reactionStart.x;
}

static inline s32 Dog_ReadDesiredZ(const struct ChaseActor *a)
{
    return a->base.reactionStart.z;
}

static inline s32 Dog_ScaleMagnitude(s32 value, u32 shift)
{
    return value >> shift;
}

static inline s32 Dog_QuarterDisplacement(const s32 *value)
{
    if (*value >= 0)
        return Dog_ScaleMagnitude(*value, 2);
    return -Dog_ScaleMagnitude(-*value, 2);
}

static inline u8 Dog_DisplacementStopped(s32 x, s32 z)
{
    if (!x && !z)
        return 1;
    return 0;
}

static inline s32 Dog_ReadDisplacementAxis(s32 desired, s16 *current, s16 **field)
{
    s32 difference = desired - *current;
    *field = current;
    return Dog_QuarterDisplacement(&difference);
}

static inline s32 Dog_ClampAxis(s32 value, s32 limit)
{
    if (value > limit)
        return limit;
    if (value < -limit)
        return -limit;
    return value;
}

/* Ease the contact displacement before resuming pursuit or the home route. */

void Dog_UpdateContactRecovery(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *group;
    s16 *x, *z;
    s32 dx, dz;
    if (SceneActorReaction_TakePrepared(actor)) {
        u16 *flags;
        u32 mask;
        {
            s32 value = Dog_ReadSecondaryX(actor);
            actor->base.secondaryMotion.x = Dog_QuarterDisplacement(&value);
        }
        {
            s32 value = Dog_ReadSecondaryZ(actor);
            actor->base.secondaryMotion.z = Dog_QuarterDisplacement(&value);
        }
        SceneActorAnimation_SelectDirectionalEntry(&actor->base, 3);
        mask = 0x20;
        flags = &actor->base.motionFlags;
        mask |= *flags;
        mask &= ~0x100;
        *flags = mask;
    }
    {
        u32 mask = 0x10000;
        actor->base.statusFlags |= mask;
    }
    dx = Dog_ReadDisplacementAxis(Dog_ReadDesiredX(actor), &actor->base.reactionEnd.x, &x);
    dz = Dog_ReadDisplacementAxis(Dog_ReadDesiredZ(actor), &actor->base.reactionEnd.z, &z);
    dx = Dog_ClampAxis(dx, 200);
    dz = Dog_ClampAxis(dz, 200);
    actor->base.body.motion.x += dx;
    actor->base.body.motion.z += dz;
    *x += dx;
    *z += dz;
    if (!Dog_DisplacementStopped(dx, dz))
        goto moving;
    if (!actor->base.restart)
        goto moving;
    group = &actor->group;
    actor->base.stepDone = 1;
    SceneActorChase_ClearMotionFlagComponent(actor, 0x20);
    if (actor->base.hitReactionFlags & 0x80000) {
        SceneActorChase_SetReactionFlags(actor, 2);
        return;
    }
    if (SceneActorChase_QueryMotionFlagComponent(actor, 0x80)) {
        SceneActorChase_SetActorFlags(actor, 0x80);
        SceneActorChase_TrackPlayer(actor);
        if (!Dog_Arrive(actor)) {
            if (!SceneActorChase_TryRoute(actor, group->route, &actor->base.body.position,
                                          &actor->base.tracking.target->position)) {
                Callback_InvokeActor(actor, group->fallback);
                return;
            }
        }
        SceneActorChase_SetFlagComponent((struct SceneActorChaseFlags16 *)&group->common.gate.flags,
                                         4);
        SceneActorChase_SetControl(actor, group->common.gate.control, 0x1A);
        SceneActorChase_SetBehavior(actor, 12);
        return;
    }
    SceneActorChase_TrackPlayer(actor);
    SceneActorChase_SetActorFlags(actor, 0x10);
    group->destination->pad = 0;
    if (!Dog_Arrive(actor)) {
        if (!SceneActorChase_TryRoute(actor, group->route, &actor->base.body.position,
                                      &actor->base.tracking.target->position)) {
            Callback_InvokeActor(actor, group->fallback);
            return;
        }
    }
    SceneActorChase_SetControl(actor, actor->base.onAlternateEntry, 1);
    SceneActorChase_SetBehavior(actor, 7);
    actor->base.field2C2 = 16;
    return;
moving:
    SceneActorChase_SetActorFlags(actor, 0x2000);
}

void SceneActorAnimation_SpawnTrailAtPosition(void *, u32);

void SceneActorAnimation_RefreshTimedAttackCollider(struct ChaseActor *a, u32 frame);

void SceneActorAnimation_EmitDirectionalTrail(struct ChaseActor *a, s32 frame)
{
    union SceneActorRegistryPackedPosition p;
    if (SceneActorReaction_TakePrepared(a))
        SceneActorAnimation_SelectDirectionalEntry(&a->base, 17);
    {
        struct DisplaySequenceState *sequence =
            &((struct SceneActorAnimationSequenceDrawable *)a->base.drawable)->sequence;
        sequence->index = 0;
        sequence = &((struct SceneActorAnimationSequenceDrawable *)a->base.drawable)->sequence;
        sequence->elapsed = 0;
    }
    if (Mod(frame, 4) == 0) {
        s32 direction = ((a->base.body.direction + 32) & 255) >> 6;
        p.position = a->base.body.position;
        switch (direction) {
            case 0:
                p.packed.x += 128;
                break;
            case 1:
                p.packed.z += 128;
                break;
            case 2:
                p.packed.x -= 160;
                break;
            case 3:
                p.packed.z -= 128;
                break;
        }
        SceneActorAnimation_SpawnTrailAtPosition(&p, a->base.paletteOffset);
    }
    if (frame > 30) {
        SceneActorChase_SetControl(a, SceneActorAnimation_RefreshTimedAttackCollider, 38);
    }
}

void FeatherEffectPool_CreateDirectional(struct ContactPosition *, u32, u32, u32);

void SceneActorAnimation_RefreshTimedAttackCollider(struct ChaseActor *a, u32 frame)
{
    struct Dog *context = a->base.context;
    if (SceneActorReaction_TakePrepared(a)) {
        u32 variant, palette;
        SceneActorAnimation_SelectDirectionalEntry(&a->base, 17);
        palette = a->base.paletteOffset;
        variant = 0;
        if (palette == 1)
            variant = 1;
        FeatherEffectPool_CreateDirectional(&a->base.body.position, a->base.body.direction, 0,
                                            variant);
        Sound_PlaySong(0x3A5);
    }
    if (frame - 13 <= 10)
        Collider_Refresh(&context->collider);
    if (a->base.restart)
        a->base.stepDone = 1;
}

void Dog_LeapTowardTarget(struct ChaseActor *actor);

void Dog_UpdateLeapWindup(struct ChaseActor *actor)
{
    struct Dog *context = actor->base.context;
    if (SceneActorControl_TakeFlag(&actor->base.field2B2)) {
        SceneActorAnimation_SelectDirectionalEntry(actor, 0xF);
        context->leapTarget = actor->base.tracking.target->position;
    }
    if (actor->base.restart)
        SceneActorChase_SetControl(actor, Dog_LeapTowardTarget, 0x23);
}

/* Dog_LeapTowardTarget: on entry turns the leap velocity toward the context's target position (snapping to it within 16, else turning 16 at a time) and plays sound 0x3A4; halves the velocity while the motion's bit 0 is set, and hands over to the landing reaction (kind 0x24) when flags2D0 & 0x10 (provisional name) */

void Dog_UpdateAnimationSixteenReaction(struct ChaseActor *actor);

static inline void Polar(struct ContactPosition *out, u32 angle, s32 speed)
{
    out->x = Math_ShiftTowardZero(gUnk_086149C4[(angle + 0x40) & 0xFF] * speed, 12);
    out->y = 0;
    out->z = Math_ShiftTowardZero(gUnk_086149C4[angle & 0xFF] * speed, 12);
}

void Dog_LeapTowardTarget(struct ChaseActor *actor)
{
    struct Dog *context = actor->base.context;
    struct ContactPosition velocity;
    if (SceneActorControl_TakeFlag(&actor->base.field2B2)) {
        s32 angle, facing, turn;
        context = actor->base.context;
        Chase_Delta(&velocity, &actor->base.body.position, &context->leapTarget);
        angle = Math_Atan2(velocity.x, velocity.z);
        facing = ((actor->base.body.direction + 0x20) & 0xFF) >> 6 << 6;
        turn = angle - facing;
        if (turn > 0x80)
            turn -= 0x100;
        else if (turn < -0x80)
            turn += 0x100;
        if ((turn < 0 ? -turn : turn) <= 0xF)
            Polar(&velocity, angle, 24);
        else if (turn > 0)
            Polar(&velocity, (facing + 0x10) & 0xFF, 24);
        else
            Polar(&velocity, (facing + 0xEF) & 0xFF, 24);
        actor->base.secondaryMotion.x = velocity.x;
        actor->base.secondaryMotion.y = 0x20;
        actor->base.secondaryMotion.z = velocity.z;
        Sound_PlaySong(0x3A4);
    }
    Collider_Refresh(&context->collider);
    if (*(u8 *)actor->base.body.movementProbe & 1) {
        actor->base.secondaryMotion.x = Math_ShiftTowardZero(actor->base.secondaryMotion.x, 1);
        actor->base.secondaryMotion.z = Math_ShiftTowardZero(actor->base.secondaryMotion.z, 1);
    }
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 5);
    SceneActorChase_SetActorFlags(actor, 0x2000);
    {
        u32 mask = 0x10;
        if (actor->base.statusFlags & mask) {
            SceneActorChase_SetControl(actor, Dog_UpdateAnimationSixteenReaction, 0x24);
        }
    }
}

void Dog_UpdateAnimationSixteenReaction(struct ChaseActor *actor)
{
    if (SceneActorControl_TakeFlag(&actor->base.field2B2))
        SceneActorAnimation_SelectDirectionalEntry(&actor->base, 0x10);
    if (actor->base.restart)
        actor->base.stepDone = 1;
}

void Dog_UpdateAnimationFourteenReaction(struct ChaseActor *actor)
{
    if (SceneActorControl_TakeFlag(&actor->base.field2B2))
        SceneActorAnimation_SelectDirectionalEntry(&actor->base, 0xE);
    if (actor->base.restart)
        actor->base.stepDone = 1;
}

static inline void Dog_ResetAnimation(struct ChaseActor *actor, u32 state, u32 phase)
{
    actor->base.state2A8 = state;
    actor->base.phase2A9 = phase;
    actor->base.behavior2AA = 0;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}

void Dog_UpdateTurnSequence(struct ChaseActor *actor, s32 frame)
{
    struct SceneActorChaseGroup *group = &actor->group;
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        SceneActorChase_SetMotionFlags(actor, 0x400);
        actor->base.field2C2 = 32;
    }
    if (group->turn.turnsRemaining > 0) {
        if (frame > 20) {
            /* data holds the entry control, then the random table. */
            const void *data;
            u32 encoded;
            Dog_ResetAnimation(actor, 3, 1);
            data = actor->base.onEntry;
            SceneActorChase_SetControl(actor, (void *)data, 2);
            if (group->turn.turnsRemaining == 1) {
                encoded = ((group->turn.savedDirection >> 5) + 3) & 7;
                actor->base.nextDirection = encoded;
            } else {
                s32 prior = actor->base.body.direction;
                s32 tries = 0;
                u32 *randomIndex;
                u32 wrap, mask;
                s32 choice;
                data = gRandomTable;
                randomIndex = &gRandomIndex;
                wrap = 1023;
                mask = 3;
                for (;;) {
                    u32 index = (*randomIndex + 1) & wrap;
                    s32 next;
                    *randomIndex = index;
                    next = (((const u16 *)data)[index] & mask) << 6;
                    if (next != prior) {
                        choice = next;
                        break;
                    }
                    if (++tries > 3) {
                        choice = prior;
                        break;
                    }
                }
                encoded = ((choice >> 5) + 3) & 7;
                actor->base.nextDirection = encoded;
            }
            group->turn.turnsRemaining--;
        }
    } else {
        group->turn.turnsRemaining |= -1;
        if (frame > 6) {
            struct SceneActorChaseSavedControl *saved = &group->saved;
            u32 kind = saved->kind;
            void *control = saved->control;
            u32 state, phase, behavior;
            SceneActorChase_SetControl(actor, control, kind);
            state = saved->state;
            phase = saved->phase;
            behavior = saved->behavior;
            actor->base.state2A8 = state;
            actor->base.phase2A9 = phase;
            actor->base.behavior2AA = behavior;
            actor->base.counter2AC = 0;
            actor->base.field2B1 = 1;
            {
                u16 *flags = &actor->base.motionFlags;
                u32 mask = ~0x400;
                *flags = mask & *flags;
            }
        }
    }
}

void Dog_OnSecondaryReactionCleanup(void)
{
}

#define SceneActorMotion_IsVariantTimerReady Dog_IsActionTimerExpired

s32 Nav_IsPositionInMesh(void *, struct ContactPosition *, struct ContactPosition *);

s32 Terrain_ApplyBlocked(void *, struct ContactPosition *, struct ContactPosition *);

void SceneActorMotion_TestTargetTransition(struct ChaseActor *a)
{
    struct Dog *context = a->base.context;
    if (!SceneActorReaction_TakePrepared(a)) {
        SceneActorAnimation_SelectDirectionalEntry(&a->base, 11);
        if ((u8)Dog_IsActionTimerExpired(a)) {
            struct SceneActorChaseGroup *group = &a->group;
            SceneActorAnimation_SelectDirectionalEntry(&a->base, 0);
            if (!SceneActorChase_TryRoute(a, a->group.route, &a->base.body.position,
                                          &a->base.tracking.target->position))
                Callback_InvokeActor(a, group->fallback);
            else {
                u32 *flags = &context->flags;
                s32 mask = -5;
                *flags &= mask;
                Chase_SetBehavior(a, 17);
            }
        }
    }
}

void SceneActorMotion_UpdateTargetTransition(struct ChaseActor *a)
{
    u32 *flags;
    struct Dog *c = a->base.context;
    if (!SceneActorReaction_TakePrepared(a)) {
        if (TestContext(c, 4, &flags)) {
            if (a->base.tracking.distance > c->chaseRangeSquared) {
                Chase_SetBehavior(a, 19);
                a->base.nextDirection = (Math_ShiftTowardZero(a->base.tracking.heading, 5) + 3) & 7;
            } else {
                s32 mask = -5;
                *flags &= mask;
                if (c->retreatCount <= 3) {
                    if ((u8)Dog_SelectRetreatDirection(a, a->base.body.direction))
                        Chase_SetBehavior(a, 17);
                } else {
                    Chase_SetBehavior(a, 19);
                    a->base.nextDirection =
                        (Math_ShiftTowardZero(a->base.tracking.heading, 5) + 3) & 7;
                }
            }
        } else
            SceneActorAnimation_SelectDirectionalEntry(&a->base, 1);
    }
}

/* SceneActorControl_BeginHiddenDefeat: on entry faces forward, releases attachments, plays the category sound and clears the defeat flags; once field2B3 is set either completes the defeat at once (flags2CC & 0x400: scripts, event flag, idle, hide) or hands over to the blink-out (SceneActorControl_CompleteBlinkOut, kind 0x27) (provisional name) */

void *Bestiary_GetEntryForActor(u32, u32);

void Bestiary_UnlockEntry(void *);

u8 Sprite_BlinkUntilExpired(void *, s32, s32, s32);

void SceneActorEffects_AdvanceSteps(void *, u32);

void SceneActorPlacement_PlayCategoryEventSound(struct ChaseActor *);

void SceneActorControl_CompleteBlinkOut(struct ChaseActor *, s32);

void SceneActorMovement_UpdateAirborne(struct ChaseActor *);

void SceneActorMovement_UpdateHeight(struct ChaseActor *);

void SceneActorControl_CallContextScript(struct RoomActor *actor);

void SceneActorControl_BeginHiddenDefeat(struct ChaseActor *actor)
{
    s32 values[8];
    struct ScriptUtilityArguments arguments;
    u16 script;
    u16 *flags;
    if (SceneActorControl_TakeFlag(&actor->base.field2B2)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 10);
        actor->base.restart = 0;
        SceneActorAttachment_ReplaceResponseSlot(actor, 0, 127, 0);
        SceneActorAttachment_ReplaceResponseSlot(actor, 1, 127, 0);
        SceneActorAttachment_ReplaceResponseSlot(actor, 3, 127, 0);
        SceneActorEffects_AdvanceSteps(actor, 4);
        SceneActorPlacement_PlayCategoryEventSound((void *)actor);
        {
            struct SceneActorRegistryControl *control = &actor->group.common.control;
            u32 *field;
            u32 mask;
            field = &actor->base.behaviorFlags;
            mask = ~0x141C0;
            *field &= mask;
            control->counter = 0;
            control->field14 = 0;
            control->timer = 0;
        }
        {
            u16 *colliderFlags = &actor->base.colliderA.flags;
            u32 mask;
            u32 value = *colliderFlags;
            mask = 4;
            mask |= value;
            *colliderFlags = mask;
        }
    }
    SceneActorTerrain_ClearCollisionTimers((void *)actor);
    SceneActorMovement_UpdateAirborne(actor);
    SceneActorMovement_UpdateHeight(actor);
    if (!actor->base.restart)
        return;
    {
        u32 mask = 0x400;
        u32 instant = actor->base.behaviorFlags & mask;
        if (instant) {
            actor->base.stepDone = 1;
            SceneActorControl_CallContextScript(actor);
            script = (u16)actor->base.script1F8;
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
            {
                u32 mask = 0x4000;
                u16 value = actor->base.motionFlags;
                u16 test = value & mask;
                flags = &actor->base.motionFlags;
                if (!test && (u32)(actor->base.category - 11) > 2)
                    Bestiary_UnlockEntry(
                        Bestiary_GetEntryForActor(actor->base.category, actor->base.paletteOffset));
            }
            {

                u16 busy = SceneActorChase_FlagComponentBits((void *)flags, 0x4000);
                if (!busy) {
                    SceneActorChase_SetTypedControl(actor, SceneActorControl_OnIdle, 16);
                    SceneActorChase_SetState(actor, 1, 0, 2);
                }
            }
            ActorShadow_Disable(&actor->base.shadow);
            ActorStatus_SetDrawableFlags(actor, 1);
        } else {
            SceneActorChase_SetControl(actor, SceneActorControl_CompleteBlinkOut, 0x27);
        }
    }
}

// confidence=low: finishes the blink, runs defeat scripts and hides the drawable and shadow.

/* SceneActorControl_CompleteBlinkOut: keeps terrain timers and height updated while the drawable blinks (shadow follows visibility); when the blink expires runs the context and defeat scripts, sets the defeat event flag, goes idle unless flags2D4 & 0x4000, and hides the drawable and shadow (provisional name) */

void SceneActorControl_CompleteBlinkOut(struct ChaseActor *actor, s32 frame)
{
    s32 values[8];
    struct ScriptUtilityArguments arguments;
    u16 script;
    u16 *flags;
    SceneActorTerrain_ClearCollisionTimers((void *)actor);
    SceneActorMovement_UpdateAirborne(actor);
    SceneActorMovement_UpdateHeight(actor);
    if (Sprite_BlinkUntilExpired(actor->base.drawable, frame, 10, 30)) {
        actor->base.stepDone = 1;
        SceneActorControl_CallContextScript(actor);
        script = (u16)actor->base.script1F8;
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
        {
            u32 mask = 0x4000;
            u16 value = actor->base.motionFlags;
            u16 test = value & mask;
            flags = &actor->base.motionFlags;
            if (!test && (u32)(actor->base.category - 11) > 2)
                Bestiary_UnlockEntry(
                    Bestiary_GetEntryForActor(actor->base.category, actor->base.paletteOffset));
        }
        {

            u16 busy = SceneActorChase_FlagComponentBits((void *)flags, 0x4000);
            if (!busy) {
                SceneActorChase_SetTypedControl(actor, SceneActorControl_OnIdle, 16);
                SceneActorChase_SetState(actor, 1, 0, 2);
            }
        }
        ActorShadow_Disable(actor->base.shadow);
        ActorStatus_SetDrawableFlags(actor, 1);
    } else if (SceneActorVisibility_IsHidden(actor))
        ActorShadow_Disable(actor->base.shadow);
    else
        ActorShadow_Enable(actor->base.shadow);
}

/* This class's own copies of the shared chase inlines it compiles differently. */

/* The reaction fields overlap the canonical actor's species storage. */

union DogReactionContext {
    struct ChaseActor base;
    struct Dog reaction;
};

/* React to contact, pursue the target, or select a delayed cardinal turn. */

s32 Dog_ChooseReactionBehavior(struct ChaseActor *a)
{
    union DogReactionContext *context = a->base.context;
    struct SceneActorChaseGroup *group = &a->group;
    if (a->base.reactionSoundVariant == 0)
        Sound_PlaySong(0x211);
    else
        Sound_PlaySong(0x13E);
    if (SceneActorChase_QueryMotionFlags(a, 0x8002)) {
        SceneActorChase_ClearMotionFlags(a, 0x20);
        return 0;
    }
    if (a->base.hitReactionFlags & 0xC0000) {
        SceneActorChase_SetControl(a, a->base.onReaction, 18);
        SceneActorChase_SetState(a, 3, 0, 0);
        SceneActorChase_ClearWordFlags(&a->base.statusFlags, 0x1000000);
        SceneActorChase_ClearWordFlags(&context->reaction.flags, 8);
        return 1;
    }
    SceneActorChase_ClearMotionFlags(a, 0x20);
    if (SceneActorChase_QueryMotionFlags(a, 0x100) || a->base.hitLockTimer)
        return 0;
    if (SceneActorChase_QueryMotionFlags(a, 0x80)) {
        SceneActorChase_BeginContactAnimation(a);
        if (!a->base.tracking.target)
            return 0;
        SceneActorChase_SetActorFlags(a, 0x80);
        SceneActorChase_SetFlagComponent((void *)&group->common.gate.flags, 4);
        SceneActorChase_TrackPlayer(a);
        if (Dog_IsActionTimerExpired(a)) {
            if (!Dog_Arrive(a)) {
                if (!SceneActorChase_TryRoute(a, group->route, &a->base.body.position,
                                              &a->base.tracking.target->position)) {
                    Callback_InvokeActor(a, group->fallback);
                    return 0;
                }
            }
            SceneActorChase_SetControl(a, group->common.gate.control, 26);
            SceneActorChase_SetBehavior(a, 12);
            return 1;
        }
        if (context->reaction.turnDelay)
            return 1;
        SceneActorChase_SetControl(a, SceneActorControl_OnIdle, 16);
        SceneActorChase_SetBehavior(a, 18);
        {
            s32 angle = Math_ShiftTowardZero((a->base.tracking.heading + 0xA0) & 255, 5);
            a->base.nextDirection = (angle + 3) & 7;
        }
        {
            s32 direction = a->base.body.direction;
            a->base.body.direction = (((direction + 0xA0) & 255) >> 6) << 6;
            context->reaction.turnDelay = 90;
            if ((u8)Dog_SelectRetreatDirection(a, direction))
                return 1;
            return 0;
        }
    } else {
        SceneActorChase_BeginContactAnimation(a);
        if (!a->base.tracking.target)
            return 0;
        SceneActorChase_TrackPlayer(a);
        SceneActorChase_SetActorFlags(a, 16);
        group->destination->pad = 0;
        if (!Dog_Arrive(a)) {
            if (!SceneActorChase_TryRoute(a, group->route, &a->base.body.position,
                                          &a->base.tracking.target->position)) {
                Callback_InvokeActor(a, group->fallback);
                return 0;
            }
        }
        SceneActorChase_SetControl(a, a->base.onAlternateEntry, 1);
        SceneActorChase_SetBehavior(a, 7);
        a->base.field2C2 = 16;
        return 1;
    }
}

void Dog_UpdateArrivalAnimation(struct ChaseActor *a);

void SceneActorBehavior_CallActionScript(u8 *);

void Dog_UpdateScriptedAction(struct ChaseActor *actor)
{
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        SceneActorChase_SetControl(actor, Dog_UpdateArrivalAnimation, 10);
        actor->base.body.direction = actor->base.tracking.heading;
        SceneActorAttachment_ReplaceResponseSlot(actor, 0, 0, actor->base.category);
        SceneActorBehavior_CallActionScript((u8 *)actor);
        SceneActorChase_GroupSetFlags(&actor->group, 4);
        {
            u32 flag = 0x80;
            u16 *flags = &actor->base.motionFlags;
            flag |= *flags;
            *flags = flag;
        }
        actor->group.common.gate.counter = actor->group.common.gate.duration;
    }
    if (actor->base.stepDone)
        Dog_SelectTargetAction(actor);
    else
        actor->base.counter2AC++;
}

/* Queue pursuit, a home route, or the short waiting response after the delay. */

void Dog_UpdateDelayedPursuit(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *group;
    SceneActorControl_TakeStateEntry(actor);
    if ((u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque))
        return;
    if ((u8)sub_0812F3CC(actor))
        return;
    if ((s32)actor->base.counter2AC <= 60)
        goto waiting;
    group = &actor->group;
    SceneActorAttachment_ReplaceResponseSlot((u8 *)actor, 0, 2, actor->base.category);
    SceneActorChase_SetState(actor, 3, 1, 0);
    SceneActorChase_SetControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
    if (SceneActorChase_MotionFlags(actor, 0x80)) {
        SceneActorDelay_Prepare((void *)&group->saved, group->common.gate.control, 12, 30);
        SceneActorChase_Arrive(actor);
    } else if (SceneActorTurn_ActorFlags(actor, 0x10)) {
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

extern void *gUnk_08606204[];

void Dog_SetPrimaryBehaviorPreset(struct ChaseActor *actor)
{
    actor->base.alternateBehaviorCallbacks = gUnk_08606204;
}

void SceneActor_ConfigureSecondaryProfileEntry(struct ChaseActor *actor);

void SceneActorControl_TickActionCooldown(struct ChaseActor *actor);

s32 Dog_PrepareFrame(void *p)
{
    SceneActor_ConfigureSecondaryProfileEntry(p);
    SceneActorControl_TickActionCooldown(p);
    return 1;
}

void Dog_UpdateCollisionBounds(struct ChaseActor *a);

void Dog_TickCooldown(struct Dog *dog);

s32 Dog_FinishFrame(void *p)
{
    Dog_UpdateCollisionBounds(p);
    Dog_UpdateActionTimers(p);
    Dog_UpdateActionCooldown(p);
    Dog_TickCooldown(p);
    return 1;
}

void Dog_UpdateLeapWindup(struct ChaseActor *actor);

static inline void Dog_ClearAttackFlags(struct Dog *ctx, u32 bits)
{
    SceneActorChase_AndWordFlags(&ctx->flags, ~bits);
}

void Dog_UpdateLeapAttack(struct ChaseActor *a)
{
    struct Dog *context = a->base.context;
    if (SceneActorControl_TakeStateEntry(a)) {
        SceneActorChase_AndWordFlags(&context->flags, ~8);
        SceneActorReaction_EnableMotionFlags(0x100, &a->base.motionFlags);
        SceneActorChase_SetControl(a, Dog_UpdateLeapWindup, 34);
        Dog_ClearAttackFlags(a->base.context, 2);
        {
            void *collider = &context->collider;
            u32 damage = a->base.attacks.first[1];
            u32 response = a->base.attacks.second[1];
            u32 power = a->base.attacks.flags[1];
            u32 extra = a->base.attacks.parameter[1];
            Collider_SetAttackProperties(collider, damage, response, 0, power, extra);
        }
        context->retreatCount = 0;
        context->nearTimer = context->nearDistance;
    }
    if (a->base.stepDone) {
        context->nearTimer = context->nearDistance;
        {
            u16 *flags = &a->base.motionFlags;
            u32 mask = ~0x100;
            *flags = mask & *flags;
        }
        Dog_SelectRetreatOrChase(a);
    } else
        a->base.counter2AC++;
}

void SceneActorAnimation_EmitDirectionalTrail(struct ChaseActor *a, s32 frame);

static inline void Dog_SetContextFlags(struct Dog *context, u32 mask)
{
    context->flags |= mask;
}

void Dog_UpdateTrailAttack(struct ChaseActor *a)
{
    struct Dog *context = a->base.context;
    if (SceneActorControl_TakeStateEntry(a)) {
        SceneActorChase_AndWordFlags(&context->flags, ~8);
        SceneActorReaction_EnableMotionFlags(0x100, &a->base.motionFlags);
        SceneActorChase_SetControl(a, SceneActorAnimation_EmitDirectionalTrail, 37);
        Dog_SetContextFlags(a->base.context, 2);
        {
            void *collider = &context->collider;
            u32 damage = a->base.attacks.first[2];
            u32 response = a->base.attacks.second[2];
            u32 power = a->base.attacks.flags[2];
            u32 extra = a->base.attacks.parameter[2];
            Collider_SetAttackProperties(collider, damage, response, 0, power, extra);
        }
        context->retreatCount = 0;
    }
    if (a->base.stepDone) {
        context->farTimer = context->farDistance;
        {
            u16 *flags = &a->base.motionFlags;
            u32 mask = ~0x100;
            *flags = mask & *flags;
        }
        Dog_SelectRetreatOrChase(a);
    } else
        a->base.counter2AC++;
}

extern void *gUnk_0860620C[];

void Dog_SetSecondaryBehaviorPreset(struct ChaseActor *actor)
{
    actor->base.behaviorCallbacks = gUnk_0860620C;
}

// confidence=low: tracks player zero and switches from the chase animation to a bite in range.

/* Dog_ChasePlayer: on entry starts the chase animation (kind 0x28); bites when a bite frame of the run cycle ends with player 0 in range (records the player's position, extends the chase timer), otherwise once the control finishes either follows the group's chase (flags2D4 & 0x80) or routes back to its route point (provisional name) */

void Dog_UpdateAnimationFourteenReaction(struct ChaseActor *actor);

u8 sub_0812F3CC(struct ChaseActor *);

void Dog_ChasePlayer(struct ChaseActor *actor)
{
    struct Dog *context = actor->base.context;
    struct SceneActorChaseGroup *group = &actor->group;
    struct ContactPosition target;
    if (Chase_TakeArrived(actor))
        Chase_SetCtl(actor, Dog_UpdateAnimationFourteenReaction, 0x28);
    if ((u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque))
        return;
    if (sub_0812F3CC(actor))
        return;
    {
        struct DisplaySequenceState *anim;
        s32 distance;
        Chase_Delta(&target, &actor->base.body.position,
                    &((struct SceneActorChasePlayer *)gPlayers[0])->target.position);
        distance = target.x * target.x + target.z * target.z;
        anim = &((struct SceneActorAnimationSequenceDrawable *)actor->base.drawable)->sequence;
        if ((anim->index == 6 || anim->index == 0xC || anim->index == 0x13 ||
             anim->index == 0x19) &&
            anim->elapsed == anim->duration - 1 && (u32)distance < context->leapRangeSquared) {
            SceneActorAttachment_ReplaceResponseSlot(actor, 0, 1, actor->base.category);
            Chase_SetBehavior(actor, 0x10);
            context->leapTarget = ((struct SceneActorChasePlayer *)gPlayers[0])->target.position;
            {
                u32 chasing = 0x80;
                if (actor->base.motionFlags & chasing) {
                    u16 *time = &actor->group.common.gate.counter;
                    *time += 0x12C;
                    if (*time > actor->group.common.gate.duration)
                        *time = actor->group.common.gate.duration;
                }
            }
            return;
        }
    }
    if (actor->base.stepDone) {
        if (SceneActorChase_QueryMotionFlags(actor, 0x80)) {
            Chase_SetBehavior(actor, 0xC);
            Chase_SetCtl(actor, group->common.gate.control, 0x1A);
            SceneActorChase_GroupSetFlags(group, 4);
            return;
        }
        Chase_SnapToRoute(&target, (u8 *)&group->homeRoute);
        Chase_SetCtl(actor, actor->base.onAlternateEntry, 1);
        Chase_SetBehavior(actor, 2);
        if (!Chase_TryRoute(actor, group->route, &actor->base.body.position, &target))
            Callback_InvokeActor(actor, group->fallback);
    } else
        actor->base.counter2AC++;
}

// confidence=low: retained provisional behavior name from caller and state-transition evidence.

// confidence=low: resumes group chase or the saved route after arrival.

static inline u32 Dog_SetMotionBits(struct ChaseActor *actor, s32 mask)
{
    struct ChaseFlags40 *flags = (void *)&actor->base.motionFlags;
    flags->bits |= mask;
    return Chase_MotionFlags(actor, 0x80);
}

void Dog_ResumeRoute(struct ChaseActor *a)
{
    struct SceneActorChaseGroup *group = &a->group;
    struct Dog *context = a->base.context;
    if (Chase_TakeArrived(a))
        Chase_SetCtl(a, Dog_UpdateArrivalAnimation, 10);
    if ((u8)Callback_InvokeActor(a, a->base.contactQuery.opaque))
        return;
    if (sub_0812F3CC(a))
        return;
    if (a->base.stepDone) {
        if (!Chase_TryRoute(a, group->route, &a->base.body.position, &context->leapTarget)) {
            Callback_InvokeActor(a, group->fallback);
            return;
        }
        if (Dog_SetMotionBits(a, 0x200)) {
            Chase_SetBehavior(a, 12);
            Chase_SetCtl(a, group->common.gate.control, 0x1A);
            SceneActorChase_GroupSetFlags(group, 4);
        } else {
            Chase_SnapToRoute(group->destination, (u8 *)&group->homeRoute);
            group->destination->pad = 0;
            SceneActorChase_SetActorFlags(a, 0x10);
            Chase_SetCtl(a, a->base.onAlternateEntry, 1);
            Chase_SetBehavior(a, 7);
            a->base.field2C2 = 0x20;
        }
    } else
        a->base.counter2AC++;
}

/* This class's own copies of the shared chase inlines it compiles differently. */

static inline void Dog_SaveControl(struct SceneActorChaseSavedControl *packet, void *object)
{
    struct ChaseActor *actor = object;
    packet->control = actor->base.target;
    packet->kind = actor->base.controlKind;
    packet->state = actor->base.state2A8;
    packet->phase = actor->base.phase2A9;
    packet->behavior = actor->base.behavior2AA;
}

static inline s32 Dog_CellAt(struct ContactPosition *position)
{
    return SceneActorChase_CellIndex(position->x >> 8, position->z >> 8);
}

static inline u32 Dog_Height(struct ContactPosition *position)
{
    return SceneActorChase_HeightAt(Dog_CellAt(position), position);
}

static inline void Dog_SnapToRoute(struct ContactPosition *position,
                                   struct SceneActorRegistryRoute *route)
{
    position->x = route->point->x;
    position->z = route->point->z;
    position->y = Dog_Height(position);
}

static inline void Dog_BeginTurn(void *obj, u32 saved, const u16 *randomSamples)
{
    struct ChaseActor *actor = obj;
    struct SceneActorChaseGroup *group = SceneActorChase_GroupOf(obj);
    if (!SceneActorChase_MotionFlags(actor, 0x400) &&
        !SceneActorChase_TestField((u16 *)&group->common.gate.flags, 4)) {
        group->turn.direction =
            SceneActorChase_RandomDirectionExcept(actor->base.body.direction, randomSamples);
        Dog_SaveControl(&group->saved, actor);
        group->turn.savedDirection = saved;
        SceneActorChase_SetControl(actor, group->turnControl, 0x1B);
        SceneActorChase_SetState(actor, 3, 1, 0);
        group->turn.turnsRemaining = 6;
    }
}

static inline u8 Dog_Arrive_BeginDelayedRouteRecovery(void *obj)
{
    struct ContactPosition target;
    struct ChaseActor *actor = obj;
    struct SceneActorChaseGroup *group = SceneActorChase_GroupOf(obj);
    struct SceneActorChaseTracking *tracking = &actor->base.tracking;
    s32 cell, player;
    const u16 *randomSamples = gRandomTable;
    void **callback;
    Dog_SnapToRoute(group->destination, &group->homeRoute);
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
        cell = SceneActorChase_CellIndex((t)->position.x >> 8, (t)->position.z >> 8);
        player = 0;
    }
    if (cell != actor->base.terrainCell &&
        !SceneActorChase_Bits32(&gPlayers[player]->statusFlags, 0x20000))
        goto reroute;
    if (SceneActorChase_MotionFlags(actor, 0x400))
        return 0;
    SceneActorChase_ClearFlagComponent((void *)&group->common.gate.flags, 4);
    if (SceneActorChase_MotionFlags(actor, 0x80)) {
        SceneActorChase_SetControl(actor, group->common.gate.control, 0x1A);
        SceneActorChase_SetBehavior(actor, 12);
    } else {
        SceneActorChase_SetControl(actor, actor->base.onAlternateEntry, 1);
        SceneActorChase_SetBehavior(actor, 5);
        Dog_SnapToRoute(group->destination, &group->homeRoute);
        (group->destination)->y =
            SceneActorChase_HeightAt(Dog_CellAt(group->destination), group->destination);
        (group->destination)->pad = 0;
    }
    {
        u8 saved = actor->base.body.direction;
        Dog_BeginTurn(obj, saved, randomSamples);
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

/* Re-arm the saved controller, then resume the delayed pursuit route. */

void Dog_BeginDelayedRouteRecovery(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    if (SceneActorControl_TakeStateEntry(actor)) {
        SceneActorChase_SetControl(actor, actor->base.onStanding, 0);
        SceneActorChase_ClearWordFlags(&actor->base.behaviorFlags, 0x80);
        SceneActorChase_ClearMotionFlagComponent(actor, 0x200);
    }
    if ((u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque))
        return;
    if ((u8)sub_0812F3CC(actor))
        return;
    if ((s32)actor->base.counter2AC <= 20)
        goto waiting;
    SceneActorChase_ClearFlagComponent((struct SceneActorChaseFlags16 *)&group->common.gate.flags,
                                       6);
    SceneActorChase_SetControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
    SceneActorDelay_Prepare((void *)&group->saved, group->common.gate.control, 12,
                            group->common.gate.field41A);
    SceneActorChase_SetState(actor, 3, 1, 0);
    Dog_Arrive_BeginDelayedRouteRecovery(actor);
    return;
waiting:
    actor->base.counter2AC++;
}

// confidence=low: approaches the navigation probe and selects the next dog behavior.

/* SceneActorControl_UpdateProbeApproach: control phase that moves the actor toward its nav probe descriptor (+0x5D0): turns on a probe event, otherwise accelerates; changes state on arrival */

u8 SceneActorReaction_IsRangeReady(struct ChaseActor *);

s32 Callback_InvokeActor(void *, void *);

void SceneActorMotion_UpdateTargetTransition(struct ChaseActor *a);

u32 NavMesh_ConsumeProbeEvent(void *);

u8 SceneActorBehavior_RequestDescriptorTurn(void *, void *, u32, u32, s32);

u8 SceneActorBehavior_AccelerateTowardDescriptor(void *, void *, u32, u32, s32);

// confidence=low: approaches the navigation probe and selects the next dog state.

void Dog_UpdateProbeApproach(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *nav = &actor->group;
    struct Dog *control = actor->base.context;
    s32 speed;
    u32 event;

    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        SceneActorChase_SetControl(actor, SceneActorMotion_UpdateTargetTransition, 0x2A);
        {
            u32 bit = 8;
            u32 *flags = &control->flags;
            *flags |= bit;
        }
    }
    speed = Callback_InvokeActor(actor, actor->base.animationQuery.opaque);
    actor->base.field2C2 = 0x20;
    SceneActorTurn_ApplyActorDuration(actor);
    event = NavMesh_ConsumeProbeEvent(nav->route);
    if (event) {
        if (SceneActorBehavior_RequestDescriptorTurn(actor, nav->route, 0x20, 0x20, speed)) {
            Chase_SetBehavior(actor, 18);
        }
    } else if (SceneActorBehavior_AccelerateTowardDescriptor(actor, nav->route, 0x20, 0x20,
                                                             speed)) {
        SceneActorChase_SetState(actor, 3, 1, 0);
        {
            u32 bit = 4;
            u32 *flags = &control->flags;
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

void Dog_UpdateActionPhase(struct ChaseActor *p)
{
    if (SceneActorControl_TakeFlag(&p->base.field2B1)) {
        void *control = p->base.onEntry;
        SceneActorChase_SetControl(p, control, 2);
    }
    if (Dog_IsActionTimerExpired(p)) {
        if ((u8)Callback_InvokeActor(p, p->base.contactQuery.opaque))
            return;
        if (sub_0812F3CC(p))
            return;
    }
    if (p->base.stepDone)
        Chase_SetBehavior(p, 0x11);
    else
        ++p->base.counter2AC;
}

/* SceneActorControl_UpdateEntryRecoveryPhase_080E55AC: same code as SceneActorControl_UpdateEntryRecoveryPhase at another address (ported by relatives.py; provisional name) */

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
        SceneActorChase_SetControl(actor, control, 2);
    }
    if (Dog_IsActionTimerExpired(actor)) {
        if ((u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque))
            return;
        if (sub_0812F3CC(actor))
            return;
    }
    if (actor->base.stepDone) {
        SetState_UpdateEntryRecoveryPhase_080E55AC(actor);
        SceneActorChase_SetControl(actor, (void *)SceneActorMotion_TestTargetTransition, 43);
    } else
        ++actor->base.counter2AC;
}

extern void *gUnk_08606214[];

void Dog_SetControlPreset(struct ChaseActor *actor)
{
    actor->base.callbacks284 = gUnk_08606214;
}

s32 Dog_PrepareFrame(void *p);

void SceneActorControl_DispatchControl(void *);

s32 Dog_FinishFrame(void *p);

s32 Dog_Update(struct ChaseActor *actor)
{
    struct ChaseActor *self = actor;
    Dog_PrepareFrame(actor);
    if (actor->base.hitLockTimer == 0) {
        Callback_InvokeActor(actor, actor->base.stateCallbacks[actor->base.state2A8]);
        SceneActorControl_DispatchControl(actor);
    }
    Dog_FinishFrame(self);
    return 1;
}

void SceneActor_UpdateBehaviorCallbacks(void *);

s32 Dog_OnAuxiliaryUpdate(void *actor)
{
    SceneActor_UpdateBehaviorCallbacks(actor);
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

void SceneActor_RefreshLivingCountAndProfile(struct ChaseActor *actor);

void SceneActorCollision_InitGateColliders(struct ChaseActor *actor);

void SceneActorBehavior_InitializeShadow(void *);

void SceneActorTerrain_ReadScriptParameters(void *);

void SceneActorPlacement_HideActor(void *, s32);

s32 Anim_Step(void *, void *);

void SceneActorBehavior_InstallCallbackTables(void *);

u8 SceneActorRegistry_LinkActor(void *);

void SceneActorScript_OnCommand(void *actor, const u16 *command);

void SceneActorPlacement_ResetStateThree(void);

void SceneActorReaction_BeginTargetRecovery(void);

u32 SceneActorReaction_OnEnemyDefault(void);

void SceneActorReaction_ResolvePlacementDamage(void);

s32 Dog_Update(struct ChaseActor *actor);

u16 Dog_UpdateItemDrop(struct ChaseActor *actor);

void Dog_UpdateCardinalTurn(struct ChaseActor *actor);

void Dog_UpdateContactRecovery(struct ChaseActor *actor);

void Dog_SelectAnimationOne(struct ChaseActor *actor);

void Dog_SelectAnimationZero(struct ChaseActor *actor);

void SceneActorControl_BeginHiddenDefeat(struct ChaseActor *actor);

void Dog_OnReactionCleanup(void);

u8 SceneActorTarget_UpdateTrackedContact(void *);

s32 Dog_ResolvePendingObject(struct ChaseActor *actor);

void Dog_UpdateDefeatDelay(struct ChaseActor *, s32);

void Dog_ResumeAfterReaction(struct ChaseActor *a);

void Dog_OnSecondaryReactionCleanup(void);

void Dog_ResetActionState(struct ChaseActor *actor);

void Dog_UpdateTargetGate(struct ChaseActor *p);

s32 Dog_ChooseReactionBehavior(struct ChaseActor *a);

void Dog_OnClearActivityFlag(struct ChaseActor *actor);

void Dog_RestoreRoute(struct ChaseActor *actor);

void Dog_UpdateTurnSequence(struct ChaseActor *actor, s32 frame);

void Dog_SetControlPreset(struct ChaseActor *actor);

void Dog_SetPrimaryBehaviorPreset(struct ChaseActor *actor);

void Dog_SetSecondaryBehaviorPreset(struct ChaseActor *actor);

s32 Dog_OnAuxiliaryUpdate(void *);

s32 Dog_ReleaseResources(u8 *p);

static inline struct Dog *DogInit_Context(struct ChaseActor *actor)
{
    return actor->base.context;
}

static inline void DogInit_Reset(void *object)
{
    struct Dog *context = object;
    context->flags = 0;
    context->turnClockwise = 0;
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
    // concession-ok(const-local): Both CpuSet fills read the same fixed source word.
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
        MonsterInit_ReadScriptWord(&dog->leapRangeSquared, 0);
        dog->leapRangeSquared *= dog->leapRangeSquared;
        dog->actionDelay = (Script_GetPc() ? Script_GetValue() : 640);
        dog->alternateActionDelay = (Script_GetPc() ? Script_GetValue() : 640);
    }
    dog->itemDamagePercent = Script_GetKeywordValue('d', 50);
    if (!dog->itemDamagePercent)
        dog->itemDamagePercent = 50;
    dog->stolenSlot = 0xFFFF;
    dog->stolenParameter = 0;

    MonsterInit_ReadPalette(actor, 0x273, 0x192);
    SceneActorCollision_InitGateColliders(actor);
    MonsterInit_InstallSequenceCallbacks(
        actor, Dog_Update, Dog_UpdateItemDrop, Dog_UpdateCardinalTurn, Dog_SelectAnimationOne,
        Dog_SelectAnimationZero, Dog_OnReactionCleanup, Dog_UpdateContactRecovery,
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
            speed = &context->nearRangeSquared;
            *speed = 0x40000;
            context->retreatRangeSquared = 0xFFFF;
            context->chaseRangeSquared = *speed;
        } else {
            u32 *primary = &context->nearRangeSquared;
            *primary = 0x40000;
            speed = primary;
            context->farRangeSquared = 0x38400;
            context->retreatRangeSquared = 0xFFFF;
            context->chaseRangeSquared = *speed;
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

static inline void clearRouteFlag(u32 *flags)
{
    u32 mask = ~0x1000000;
    *flags &= mask;
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
        if (SceneActorTurn_ActorFlags(actor, 0x200000)) {
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
        SceneActorChase_SetControl(actor, actor->base.onStanding, 0);
    }
    Callback_InvokeActor(actor, actor->base.contactQuery.opaque);
}

void SceneActor_UpdateAlternateEntryPhase(struct ChaseActor *actor)
{
    if (SceneActorControl_TakeStateEntry(actor)) {
        SceneActorChase_SetControl(actor, actor->base.onAlternateEntry, 1);
    }
    if (actor->base.stepDone)
        Chase_SetBehavior(actor, 15);
    else
        Callback_InvokeActor(actor, actor->base.contactQuery.opaque);
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
        SceneActorChase_SetActorFlags(a, 0x2000);
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
    struct SceneActorChaseGroup *group = &actor->group;
    struct DogApproachContext *context = actor->base.context;
    if (Chase_TakeArrived(actor))
        Chase_SetCtl(actor, SceneActor_EnterShadowlessPhase, 0x24);
    if (SceneActorBehavior_RequestDescriptorTurn(actor, group->route, 32, 32, 0)) {
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
            group->common.gate.counter = 0;
        } else
            Chase_SetBehavior(actor, 15);
    } else
        Callback_InvokeActor(actor, actor->base.contactQuery.opaque);
}
