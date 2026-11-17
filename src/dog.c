#include "global.h"
#include "player.h"
#include "dog.h"
#include "scene_actor_reset.h"
#include "scene_actor_chase.h"
#include "rotation_phase.h"
#include "scene_actor_profile.h"
#include "scene_actor_palette.h"
#include "scene_actor_control_motion.h"
#include "scene_actor_animation.h"
#include "scene_actor_turn.h"
#include "display_sequence_inline.h"
#include "scene_actor_state.h"
#include "scene_actor_registry.h"
#include "display_sequence.h"
#include "ram.h"
#include "scene_actor_collision.h"
#include "scene_actor_behavior.h"
#include "script_utility.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_contact_response.h"
#include "scene_actor_contact_list.h"
#include "scene_actor_effects.h"
#include "contact_actor.h"
#include "engine.h"
#include "heap.h"
#include "resource_registry.h"
#include "monster_particle_random.h"
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"
#include "scene_actor_control_phase.h"
#include "scene_actor_delayed_response.h"
#include "render.h"
#include "engine_globals.h"
#include "scene_actor_init_primitives.h"
#include "scene_actor_init_group.h"
#include "scene_actor_init_dispatch.h"
#include "scene_actor_init_graphics.h"
#include "scene_actor_init_body.h"

s32 Div(s32, s32);

struct ActorPosition {
    s16 x;
    s16 y;
    s16 z;
};

struct TargetEntity {
    u8 unk0[0xc];
    struct ActorPosition pos;
};

struct Actor {
    u8 unk0[0x5c];
    struct ActorPosition pos;
    u8 unk62[0xdc - 0x64];
    struct TargetEntity *target;
    u8 unke0[0x2ec - 0xe0];
    struct ActorPosition vel;
};

void Actor_InitLaunchVelocity(struct Actor *actor)
{
    s16 diff[3];
    s16 *pdiff = diff;
    struct ActorPosition *apos = &actor->pos;
    struct TargetEntity *target = actor->target;
    struct ActorPosition *tpos = &target->pos;

    {
        s16 v = tpos->x - apos->x;
        s16 *q = diff;
        *q = v;
    }
    pdiff[1] = tpos->y - apos->y;
    pdiff[2] = tpos->z - apos->z;

    actor->vel.x = Div(diff[0], 0x1c);
    actor->vel.y = 0x20;
    actor->vel.z = Div(pdiff[2], 0x1c);
}

u8 Dog_IsActionTimerExpired(struct ChaseActor *sIn);

void Dog_SelectRetreatOrChase(struct ChaseActor *a);

void Dog_SelectTargetAction(struct ChaseActor *actor);

void SceneActorControl_OnIdle(u8 *);

static inline u8 HasReaction(struct ChaseActor *p)
{
    struct SceneActorReactionMotion *motion = (void *)&p->base.tracking.target;
    u32 mask = 8;
    if (mask & motion->flags)
        return 1;
    return 0;
}

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

static inline u32 Test16(u16 *p, u32 mask)
{
    return *p & mask;
}

static inline void And32(u32 *p, u32 mask)
{
    *p &= mask;
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
    context->timer690 = context->limitX;
    active = HasReaction(p);
    if (active) {
        Or16(&p->group.common.gate.flags, 4);
        v = 0x80;
        f = &p->base.motionFlags;
        v |= *f;
        *f = v;
        p->group.common.gate.counter = p->group.common.gate.duration;
        if (Test16(f, 0x100) || !Dog_IsActionTimerExpired(p)) {
            if (!ContextBlocked(context))
                Dog_SelectRetreatOrChase(p);
            return;
        } else {
            Dog_SelectTargetAction(p);
        }
    } else {
        u32 one;
        void (*callback)(void);
        u32 kind;
        And32(&context->flags, ~12);
        one = 1;
        p->base.state2A8 = active;
        p->base.phase2A9 = one;
        p->base.behavior2AA = active;
        p->base.counter2AC = active;
        p->base.field2B1 = one;
        callback = SceneActorControl_OnIdle;
        kind = 0x10;
        p->base.field2B2 = one;
        p->base.stepDone = active;
        p->base.controlKind = kind;
        p->base.target = callback;
        p->base.field2C8 = active;
        And32(&p->base.pendingControlFlags, ~1);
        p->base.pendingControl = active;
    }
    OrMotion((struct SceneActorReactionMotion *)&p->base.tracking.target, 8);
}

/* Dog_RestoreRoute: unless flags2D4 bit 0x100 is set, restores after a delay, records the follow-up control (chase when flags2D4 & 0x80, else the entry target) and routes back toward the chased object via the route point (provisional name) */

static inline void SaveDelay(u8 *saved, void *target, u32 behavior, u32 delay)
{
    *(u32 *)(saved + 0xC) = delay;
    *(void **)(saved + 0x10) = target;
    saved[8] = 0;
    saved[9] = 0;
    saved[10] = behavior;
}

// confidence=low: dog cohort variant of the shared delayed restoration routine.

void Dog_RestoreRoute(struct ChaseActor *actor)
{
    u8 *group = (u8 *)&actor->group.common;
    u16 *flags;
    u32 chasing;

    {
        unsigned short mask = 0x100;
        flags = &actor->base.motionFlags;
        if ((u16)((*flags) & mask)) {
            return;
        }
    }
    Chase_SetState(actor, 3, 1, 0);
    Chase_SetCtl(actor, (void *)SceneActorReaction_RestoreAfterDelay, 0xE);
    chasing = 0x80;
    if (*flags & chasing) {
        actor->base.field2C2 = 0x20;
        SaveDelay(group + 0x8C, *(void **)(group + 0x48), 0xC, 0x14);
        if (*flags & chasing) {
            u16 *time = (u16 *)((u8 *)actor + 0x418);
            *time += 0x12C;
            if (*time > actor->group.common.gate.duration)
                *time = actor->group.common.gate.duration;
        }
        {
            u16 *field = (u16 *)(group + 0x40);
            u32 mask = 4;
            u32 value = *field;
            mask |= value;
            *field = mask;
        }
    } else {
        {
            u32 mask = 0x10;
            actor->base.behaviorFlags |= mask;
        }
        actor->base.field2C2 = 0x20;
        SaveDelay(group + 0x8C, actor->base.onAlternateEntry, 7, 0x3C);
    }
    Chase_SnapToRoute(*(struct ContactPosition **)(group + 0x200), group + 0x74);
    (*(struct ContactPosition **)(group + 0x200))->pad = 0;
    if (!Chase_TryRoute(actor, *(void **)(group + 0x1FC), &actor->base.body.position,
                        &((struct ChaseTarget *)actor->base.tracking.target)->position))
        Callback_InvokeActor(actor, *(void **)(group + 0x224));
}

// confidence=low: chooses response 0x75/0x82/0x87 from target range, heading, and action timers.

#define FIELD(p, n, T) (*(T *)((u8 *)(p) + (n)))

extern const u8 gUnk_0824E2A8[];

static inline u8 Dog_HasAwarenessFlag(struct Dog *context, u32 mask)
{
    u32 value = context->flags;
    if (value & mask)
        return 1;
    return 0;
}

static inline u32 ActorFlag(struct ChaseActor *actor, u32 mask)
{
    return actor->base.motionFlags & mask;
}

static inline void Response(struct ChaseActor *actor, u8 *nav, u32 behavior, u32 response)
{
    Chase_SetState(actor, 0, 2, behavior);
    Chase_SetCtl(actor, (void *)SceneActorControl_OnIdle, 0x10);
    SceneActorReaction_ResetResponseState(nav + 0x8C, gUnk_0824E2A8, response);
}

void Dog_SelectTargetAction(struct ChaseActor *actor)
{
    struct Dog *context = actor->base.context;
    u8 *nav = (u8 *)&actor->group.common;
    if (ActorFlag(actor, 0x100))
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
            u32 mode = FIELD(actor, 0x1E6, u16);
            if (mode == 0 || mode == 2) {
                if (actor->base.tracking.distance < context->speed && !context->nearTimer) {
                    Response(actor, nav, 0, 0x75);
                    return;
                }

                if (Dog_HasAwarenessFlag(context, 8))
                    return;
                goto move;
            } else {
                if (actor->base.tracking.distance < context->speed && !context->nearTimer) {
                    Response(actor, nav, 0, 0x82);
                    return;
                }
                if (actor->base.tracking.distance < context->alternateSpeed && angle <= 5 &&
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
    ((u8 *)a)[0x3CD] = bits;
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

struct IvyContext {
    u8 unknown0[0x62C];
    u32 flags;
    u8 unknown630[4];
    u16 field634;
};

struct IvyPlacement {
    u32 xy, z;
};

// confidence=low: clears action gates before resolving the route and stepping its drawable.

void SceneActorMovement_ResolveRoutePosition(void *);

void Dog_ResetActionState(struct ChaseActor *actor)
{
    struct Dog *context = actor->base.context;
    context->flags = 0;
    context->timer690 = 0;
    context->destinationCell = 0xFFFF;
    context->timer6A4 = 0;
    context->state6A6 = 0;
    context->emitted = 0;
    Memory_Clear(context->slots, 8);
    SceneActorMovement_ResolveRoutePosition(actor);
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    {
        struct SceneActorControlMotionDrawable *drawable = actor->base.drawable;
        DisplaySequence_StepState(drawable, &drawable->sequence);
    }
}

struct S {
    u8 filler[0x2d0];
    u32 flags;
};

void Dog_OnClearActivityFlag(struct S *s)
{
    u32 *p = &s->flags;
    u32 m = ~2;
    *p = *p & m;
}

extern const s16 gUnk_086149C4[];

extern const u16 gUnk_086061E4[][8];

void SpriteEffectController_Create(void *, const void *, s32, u32, u32, u32, u32, u32, u32, u32,
                                   u32, u32, s32, s32, s32, s32, s32, s32);

static inline u32 NextRandomSample(const u16 *table)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return table[gRandomIndex];
}

static inline u16 TestAnimationFlags(struct ChaseActor *actor, u16 mask)
{
    return actor->base.motionFlags & mask;
}

static inline void AddPosition(struct ContactPosition *out, const struct ContactPosition *a,
                               const struct ContactPosition *b)
{
    out->x = b->x + a->x;
    out->y = b->y + a->y;
    out->z = b->z + a->z;
}

static inline s32 TruncateFixed(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}

static inline void SetDirectionOffset(struct ContactPosition *position, u32 angle, s32 distance)
{
    position->x = TruncateFixed(gUnk_086149C4[(angle + 0x40) & 0xFF] * distance);
    position->y = 0;
    position->z = TruncateFixed(gUnk_086149C4[angle] * distance);
}

static inline void SetStatusFlags(void *object, u32 flags)
{
    struct ChaseActor *actor = object;
    actor->base.statusFlags = flags | actor->base.statusFlags;
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
        alternate = TestAnimationFlags(actor, 0x80) != 0;
        SetStatusFlags(actor, 0x10000);
        palette = ReadPaletteOffset(&actor->base.paletteOffset);
        paletteOffset = &actor->base.paletteOffset;
        if (palette == 3 || palette == 1) {
            if (sequence->index == 7 || sequence->index == 5 || sequence->index == 1) {
                position.packed.x = (NextRandomSample(gRandomTable) & 63) - 48;
                position.packed.z = (NextRandomSample(gRandomTable) & 63) - 48;
                AddPosition(&position.position, &position.position, &actor->base.body.position);
                SetDirectionOffset(&offset, (actor->base.body.direction + 0x80) & 0xFF, 0x50);
                AddPosition(&position.position, &position.position, &offset);
                position.position.y = actor->base.body.position.y;
                ZeroFill8(&velocity);
                if (*paletteOffset == 3) {
                    s32 lifetime = (NextRandomSample((const u16 *)RANDOM_TABLE_ADDR) & 63) + 20;
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
        u16 mask = 0x280;

        u16 v = actor->base.motionFlags & mask;

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
        u16 mask = 0x200;

        if (actor->base.motionFlags & mask)
            actor->base.field2C2 = 0x20;
    }
}

struct P;

void Dog_OnBodyCollision(struct P *p, s32 a, s32 b);

void Dog_OnSensorContact(u8 *unused, u8 *obj, u8 *ctx);

void Dog_OnAttackContact(u8 *collider, u8 *other, struct ChaseActor *actor);

void SceneActorCollision_InitBody(struct ChaseActor *, void *, void *, void *, void *);

void SceneActorCollision_InitAttack(struct ChaseActor *, void *, void *, void *, u32, void *);

void SceneActorCollision_InitExternalAttack(struct ChaseActor *, struct ContactCollider *, void *,
                                            void *, void *, u32, void *);

void SceneActorCollision_InitGateColliders(struct ChaseActor *actor)
{
    struct ContactPackedVector bodySize, bodyOffset, attackSize, attackOffset, externalSize,
        externalOffset;
    u8 *context = actor->base.context;
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
    SceneActorCollision_InitExternalAttack(actor, (struct ContactCollider *)(context + 0x624),
                                           &externalSize, &externalOffset, Dog_OnAttackContact, 1,
                                           actor);
    {
        u32 mask = 0x100;
        struct ContactCollider *collider = (struct ContactCollider *)(context + 0x624);
        mask |= collider->flags;
        collider->flags = mask;
    }
}

// confidence=low: places forward collider and selects direction-dependent contact bounds.

void Collider_SetPosition(void *, void *, u32);

static inline u8 SuppressCollision(struct ChaseActor *a)
{
    if (a) {
        u32 mask = 0x40;
        if (!(a->base.motionFlags & mask)) {
            u32 mask = 0x20000;
            if (!(a->base.statusFlags & mask)) {
                if (a->base.health > 0)
                    return 0;
            }
        }
        return 1;
    }
    return 0;
}

static inline u8 Test(u32 *field, u32 mask)
{
    if (*field & mask)
        return 1;
    return 0;
}

static inline u8 TestContext(u8 *context, u32 mask, u32 **out)
{
    u32 *field = (u32 *)(context + 0x678);
    u32 value = *field & mask;
    *out = field;
    if (value)
        return 1;
    return 0;
}

static inline s32 Scale(s32 value)
{
    return value >= 0 ? value >> 12 : -(-value >> 12);
}

static inline s32 Direction(s32 value)
{
    return value >= 0 ? value >> 6 : -(-value >> 6);
}

void Dog_UpdateCollisionBounds(struct ChaseActor *a)
{
    union SceneActorRegistryPackedPosition position;
    union SceneActorRegistryPackedPosition wide;
    union SceneActorRegistryPackedPosition narrow;
    u8 *context = a->base.context;
    s32 heading;
    u32 *flags;
    if (SuppressCollision(a))
        return;
    position.position = a->base.body.position;
    heading = Direction((a->base.body.direction + 32) & 255) << 6;
    {
        if (TestContext(context, 2, &flags)) {
            position.packed.x += Scale(gUnk_086149C4[(heading + 64) & 255] << 9);
            position.packed.z += Scale(gUnk_086149C4[heading & 255] << 9);
        } else {
            position.packed.x += Scale(gUnk_086149C4[(heading + 64) & 255] << 7);
            position.packed.z += Scale(gUnk_086149C4[heading & 255] << 7);
        }
    }
    Collider_SetPosition(context + 0x624, &position, 0);
    context[0x666] = heading;
    position.position = a->base.body.position;
    switch (Direction(heading)) {
        case 0:
        case 2:
            if (Test(flags, 2)) {
                wide.packed.x = 240;
                wide.packed.z = 128;
            }
            narrow.packed.x = 120;
            narrow.packed.z = 64;
            break;
        case 1:
        case 3:
            if (Test(flags, 2)) {
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
    if (Test(flags, 2))
        ((struct ContactCollider *)(context + 0x624))->bounds = *(struct ContactPosition *)&wide;
    else
        ((struct ContactCollider *)(context + 0x624))->bounds = *(struct ContactPosition *)&narrow;
}

struct T {
    u8 filler[0x698];
    u16 h698;
    u8 f2[0x69a - 0x69a];
    u16 h69a;
};

struct SUpdateActionTimers {
    u8 filler[0x3d0];
    struct T *t;
};

void Dog_UpdateActionTimers(struct SUpdateActionTimers *s)
{
    struct T *t = s->t;
    u16 *p = &t->h698;
    if (*p != 0)
        *p = *p - 1;
    p = &t->h69a;
    if (*p != 0)
        *p = *p - 1;
}

struct TUpdateActionCooldown {
    u8 filler[0x696];
    u16 h;
};

struct SUpdateActionCooldown {
    u8 filler[0x3d0];
    struct TUpdateActionCooldown *t;
};

void Dog_UpdateActionCooldown(struct SUpdateActionCooldown *s)
{
    u16 *p = &s->t->h;
    if (*p != 0)
        *p = *p - 1;
}

struct SceneActorActionCooldown {
    u8 unknown0[0x678];
    u32 flags;
    u8 unknown67C[0x14];
    u16 remaining, normalDelay, alternateDelay;
};

static inline u8 Unavailable(struct ChaseActor *actor)
{
    if (actor) {
        u32 mask = 0x40;
        if (actor->base.motionFlags & mask)
            return 1;
        mask = 0x20000;
        if (actor->base.statusFlags & mask)
            return 1;
        if (actor->base.health <= 0)
            return 1;
    }
    return 0;
}

static inline u8 HasPending(void **slot)
{
    u32 mask = 1;
    if (((struct SceneActorActionCooldown *)*slot)->flags & mask)
        return 1;
    return 0;
}

static inline u32 MaskFlags(u32 value, u32 *p)
{
    return *p & value;
}

static inline void QueueAction(struct ChaseActor *actor)
{
    u32 behavior = 15;
    u32 zero;
    u8 *q = &actor->base.state2A8;
    zero = 0;
    *q = zero;
    actor->base.phase2A9 = zero;
    actor->base.behavior2AA = behavior;
    actor->base.counter2AC = zero;
    actor->base.field2B1 = 1;
    {
        u32 *flags;
        s32 mask;
        struct SceneActorActionCooldown *ctx;
        ctx = actor->base.context;
        flags = &ctx->flags;
        mask = -2;
        *flags &= mask;
    }
}

void SceneActorControl_TickActionCooldown(struct ChaseActor *actor)
{
    struct SceneActorActionCooldown *cooldown = actor->base.context;
    u32 flags;
    if (Unavailable(actor))
        return;
    {
        u32 mask = 0xE33F;
        flags = actor->base.motionFlags;
        if (mask & flags)
            return;
    }
    if (MaskFlags(0x10, &actor->base.behaviorFlags))
        return;
    if (actor->base.controlKind > 1)
        return;
    if (cooldown->remaining) {
        cooldown->remaining--;
        return;
    }
    if (flags & 0x80)
        cooldown->remaining = cooldown->alternateDelay;
    else
        cooldown->remaining = cooldown->normalDelay;
    {
        u32 one = 1;
        u32 *p = &cooldown->flags;
        *p |= one;
    }
    if (HasPending(&actor->base.context)) {
        QueueAction(actor);
    }
}

struct SceneActorScriptContext {
    u8 unknown0[0x6A0];
    s16 mode, firstParameter, secondParameter;
};

void SceneActorControl_CallContextScript(struct RoomActor *actor)
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
    if (!actor->script1F4)
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
        values[7] = context->secondParameter;
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

struct DogRetreatContext {
    u8 unknown0[0x6A6];
    u8 turnClockwise;
    u8 blockedRoutes;
};

static inline s8 TurnClockwise(void *object)
{
    struct DogRetreatContext *context = object;
    s16 offset = (u32) & ((struct DogRetreatContext *)0)->turnClockwise;
    return *((u8 *)context + offset);
}

#define SetTurnClockwise(object_, value_)                                                          \
    do {                                                                                           \
        struct DogRetreatContext *context_ = (object_);                                            \
        s16 offset_ = (u32) & ((struct DogRetreatContext *)0)->turnClockwise;                      \
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

static inline s32 FacingQuarter(s32 heading)
{
    if (heading >= 0)
        return heading >> 6;
    return -(-heading >> 6);
}

static inline s32 MapWidth(struct EngineMap **terrain)
{
    return ((struct SceneActorChaseTerrainGrid *)(*terrain)->cells)->width;
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
    struct DogRetreatContext *context = actor->base.context;
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
            switch (FacingQuarter(actor->base.body.direction + 32) & 3) {
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
                    next = cell + MapWidth(terrain);
                    i = 0;
                    completion = (struct EngineMap **)0x030052F4;
                    do {
                        if (!SceneActorTerrain_CanTraverseCell(cell, next, height, shape))
                            goto finishSouth;
                        next += MapWidth(terrain);
                        i++;
                    } while (i < steps);
                    chosen = next - MapWidth(completion);
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
                    next = cell - MapWidth(terrain);
                    i = 0;
                    completion = (struct EngineMap **)0x030052F4;
                    do {
                        if (!SceneActorTerrain_CanTraverseCell(cell, next, height, shape))
                            goto finishNorth;
                        next -= MapWidth(terrain);
                        i++;
                    } while (i < steps);
                    chosen = next + MapWidth(completion);
                    goto scanDone;
                finishSouth:
                    chosen = next - MapWidth(terrain);
                    goto scanDone;
                finishNorth:
                    chosen = next + MapWidth(terrain);
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
            context->blockedRoutes++;
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

struct SIsActionTimerExpired {
    u8 filler[0x1e6];
    u16 st;
    u8 f2[0x3d0 - 0x1e8];
    struct T *t;
};

u8 Dog_IsActionTimerExpired(struct ChaseActor *sIn)
{
    struct SIsActionTimerExpired *s = (struct SIsActionTimerExpired *)sIn;
    struct T *t = s->t;
    u16 v;
    if (s->st == 0 || s->st == 2)
        v = t->h698;
    else {
        if (t->h698 == 0)
            return 1;
        v = t->h69a;
    }
    if (v != 0)
        return 0;
    return 1;
}

struct SceneActorTargetLink {
    u8 *target;
    u8 unknownE0[0xB];
    u8 flags;
};

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
    s32 *pendingId = (void *)((u8 *)owner + 0x304);
    struct SceneActorTargetLink *state;
    if (*pendingId > 0) {
        struct Delta {
            s16 x, y, z, pad;
        } delta;
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
        state = (void *)((u8 *)owner + 0xDC);
        {
            u32 mask = 0x10;
            state->flags = (*(&mask)) | state->flags;
        }
        {
            u8 *object = SceneActorContacts_FindObjectById(*pendingId);
            state->target = object;
            x = ((struct ContactPosition *)(object + 0xC))->x >> 8;
            z = ((struct ContactPosition *)(object + 0xC))->z >> 8;
        }
        if ((((x < 0) || (z < 0)) || (((u32)x) >= gTerrainWidth)) || (((u32)z) >= gTerrainHeight)) {
            cell = 0;
        } else {
            cell = gTerrainMap->rows[z] + x;
        }
        SceneActorContact_SaveTargetCell(owner, cell);
        {
            struct Delta *out = &delta;
            struct ContactPosition *source = &owner->base.body.position;
            target = (void *)(state->target + 0xC);
            {
                u32 xDelta = (u16)target->x;
                struct Delta *first;
                xDelta -= (u16)source->x;
                first = &delta;
                first->x = xDelta;
            }
            out->y = (u16)target->y - (u16)source->y;
            out->z = (u16)target->z - (u16)source->z;
            {
                struct Delta *first = &delta;
                u32 direction;
                u8 *field;
                direction = Math_Atan2(first->x, out->z);
                field = &owner->base.body.direction;
                *field = direction;
            }
        }
        {
            u32 *flags = (u32 *)(*(u8 **)((u8 *)owner + 0x3D0) + 0x678);
            u32 mask = ~0xC;
            *flags &= mask;
        }
        return 1;
    } else {
        struct SceneActorTargetLink *t = (void *)((u8 *)owner + 0xDC);
        u32 m2 = 2;
        u32 v = t->flags & m2;
        state = t;
        if (BoolOf(v)) {
            u32 mask = ~0x10;
            mask &= (*(&state))->flags;
            (*(&state))->flags = mask;
        }
        return 0;
    }
}

u8 Dog_SelectRetreatDirection(struct ChaseActor *actor, u32 priorDirection);

u8 SceneActorTarget_TrySelectNearestParticipant(struct ChaseActor *);

void SceneActorMotion_TestTargetTransition(struct ChaseActor *a);

void Dog_SelectRetreatOrChase(struct ChaseActor *a)
{
    u8 *context = a->base.context;
    u8 *group = (u8 *)&a->group.common;
    SceneActorReaction_ResetResponseState(group + 0x8C, gUnk_0824E2A8, 0x2AA);
    if (FIELD(context, 0x6A7, u8) <= 3) {
        if (a->base.tracking.distance < FIELD(context, 0x688, u32) ||
            !Dog_IsActionTimerExpired(a)) {
            if (FIELD(context, 0x6A7, u8) <= 3) {
                u32 saved = a->base.body.direction;
                a->base.body.direction = a->base.tracking.heading + 128;
                if (Dog_SelectRetreatDirection(a, saved)) {
                    Chase_SetCtl(a, SceneActorControl_OnIdle, 0x10);
                    Chase_SetBehavior(a, 17);
                }
                return;
            }
        }
        if (a->base.tracking.distance > FIELD(context, 0x68C, u32)) {
            struct ChaseTarget *target;
            if (a->base.controlKind == 0x1A)
                return;
            if ((*(u32 *)&a->base.state2A8 & 0xFFFFFF) == 0xC0000)
                return;
            if (Chase_Test(gEngineFlags, 0x4800)) {
                if (!SceneActorTarget_TrySelectNearestParticipant(a))
                    return;
            } else
                a->base.tracking.target = (u32)((u8 *)gPlayers[0] + 0x24);
            {
                void *route = *(void **)(group + 0x1FC);
                struct ContactPosition *from = &a->base.body.position;
                struct ContactPosition *to;
                target = (void *)a->base.tracking.target;
                to = &target->position;
                if (!Chase_TryRoute(a, route, from, to)) {
                    Callback_InvokeActor(a, *(void **)(group + 0x224));
                    return;
                }
            }
            Chase_SetCtl(a, *(void **)(group + 0x48), 0x1A);
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

struct STickCooldown {
    u8 filler[0x6a8];
    u8 b;
};

void Dog_TickCooldown(struct STickCooldown *s)
{
    u8 *p = &s->b;
    if (*p != 0)
        *p = *p - 1;
}

struct DogItemContext {
    u8 unknown0[0x6A0];
    s16 slot, kind, parameter;
    u8 turnClockwise, blockedRoutes, unknown6A8, healthPercent;
};

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
    struct DogItemContext *context;
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
    if (context->slot >= 0)
        return;
    if (palette != 0 && palette != 2)
        return;
    position =
        *(struct SceneActorEffectsOffset *)&((struct SceneActorChasePlayer *)gPlayers[0])
             ->target.position;
    position.y += 200;
    if (!InventorySlot_PickRandom(&context->slot, &context->kind, &context->parameter))
        return;
    spawned = FloatingItemEffect_Spawn(0, context->kind, 3, 0, &position,
                                       &actor->base.body.position, 24, actor);
    actorPosition = &actor->base.body.position;
    if (spawned < 0) {
        SceneActorChase_SetFlagComponent((struct SceneActorChaseFlags16 *)&context->slot, 0xFFFF);
        return;
    }
    Inventory_ClearItemSlot(context->slot);
    switch (context->kind) {
        case 2:
        case 5:
        case 14:
            break;
        default:
            return;
    }
    responseValue = Div((u16)actor->base.defaultHealth * context->healthPercent, 100);
    {
        health = actor->base.health;
        if (health - responseValue <= 0)
            actor->base.reactionSoundVariant = health;
        else
            actor->base.reactionSoundVariant = responseValue;
    }
    context->slot = -1;
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
        actor->base.tracking.target =
            &((struct SceneActorChasePlayer *)gPlayers[0])->target;
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

static inline u32 DogTrail_NextSample(const u16 *table)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return table[gRandomIndex];
}

static inline s32 DogTrail_Truncate(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}

void SceneActorAnimation_SpawnTrailAtPosition(void *position, u32 palette)
{
    s16 velocity[3];
    s16 *planar;
    u32 randomOrigin;
    const u16 *earlyTable;
    u32 mask, sampleOrigin;
    s32 heading;
    s32 speed, vertical;
    s16 *result;

    /* The recovered bank origin cancels when converting a sample address to its table offset. */
    randomOrigin = (u32)&gHeapHead + RESOURCE_REGISTRY_ENTRIES_ADDR;
    earlyTable = (const u16 *)RANDOM_TABLE_ADDR;
    mask = 255;
    heading = DogTrail_NextSample(earlyTable) & mask;
    {
        s32 raw = DogTrail_NextSample(earlyTable);
        s32 speedMask = 31;
        planar = velocity;
        raw &= speedMask;
        speed = raw;
    }
    {
        s32 x, zero, rawX;
        rawX = gUnk_086149C4[(heading + 64) & mask] * speed;
        sampleOrigin = randomOrigin;
        x = DogTrail_Truncate(rawX);
        zero = 0;
        planar[0] = x;
        planar[1] = zero;
        planar[2] = DogTrail_Truncate(gUnk_086149C4[heading] * speed);
    }
    vertical = ParticleRandom_ReadRelativeSample(sampleOrigin, (const u16 *)RANDOM_TABLE_ADDR);
    vertical &= 63;
    vertical -= 48;
    result = velocity;
    result[1] = vertical;
    if (palette == 3)
        SceneMovingParticle_CreateWithOptions(position, result, 60, 3, 3);
    else
        SceneMovingParticle_CreateWithOptions(position, result, 60, 4, 3);
}

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

void Dog_OnSensorContact(u8 *unused, u8 *obj, u8 *ctx)
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

inline static u8 blockedFirst(u8 *c)
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

static inline void Delta(struct ContactPosition *out, struct ContactPosition *to,
                         struct ContactPosition *from)
{
    out->x = to->x - from->x;
    out->y = to->y - from->y;
    out->z = to->z - from->z;
}

void Dog_OnAttackContact(u8 *collider, u8 *other, struct ChaseActor *actor)
{
    union SceneActorRegistryPackedPosition position;
    struct ContactPosition delta;
    s32 direction;
    struct ChaseActor *facingActor = actor;
    if (blockedFirst((u8 *)actor))
        return;
    position.position = *(struct ContactPosition *)(collider + 0x24);
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
    {
        struct ContactPosition *out = &delta;
        struct ContactPosition *from = &position.position;
        struct ContactPosition *to = (struct ContactPosition *)(other + 0x24);
        Delta(out, to, from);
    }
    collider[0x42] = Math_Atan2(delta.x, delta.z);
    if (!blocked2048((u8 *)actor) && (*(u16 *)(other + 0xA) & 0xF)) {
        u32 *flags;
        s32 mask;
        actor->base.counter304 = *(u16 *)(other + 4);
        flags = &actor->base.statusFlags;
        mask = 0xFEFFFFFF;
        *flags &= mask;
    }
}

struct P {
    u8 f[6];
    u16 flags;
};

void SceneActorReaction_HandleColliderContact(struct P *, s32, s32);

static inline u32 tst(u16 *a, u32 m)
{
    return *a & m;
}

void Dog_OnBodyCollision(struct P *p, s32 a, s32 b)
{
    if (tst(&p->flags, 0x800) == 0)
        SceneActorReaction_HandleColliderContact(p, a, b);
}

void SceneActorAnimation_SelectDirectionalEntry(struct RoomActor *, s32);

struct SSelectAnimationOne {
    u8 filler[0x2b2];
    u8 f;
};

void Dog_SelectAnimationOne(struct SSelectAnimationOne *s)
{
    if (s->f != 0)
        s->f = 0;
    SceneActorAnimation_SelectDirectionalEntry(s, 1);
}

struct SSelectAnimationZero {
    u8 filler[0x2b2];
    u8 f;
};

void Dog_SelectAnimationZero(struct SSelectAnimationZero *s)
{
    if (s->f != 0)
        s->f = 0;
    SceneActorAnimation_SelectDirectionalEntry(s, 0);
}

struct DogTurnMotion {
    u8 unknown0[9];
    u8 direction;
    u8 unknownA[0x1E];
    void *route;
};

struct DogTurnBody {
    u8 unknown0[0x21C];
    void *resumeControl;
};

struct DogTurnState {
    void *object;
    void *drawable;
    u8 drawableKind;
    u8 unknown9[0x47];
    struct DogTurnMotion motion;
    u8 unknown7C[0x88];
    u32 motionCounter;
    u8 unknown108[0xC];
    u32 motionFlags;
    u8 unknown118[0x104];
    void *control;
    u8 unknown220[4];
    void *turnControl;
    u8 unknown228[0x80];
    u8 state, phase, behavior, unknown2AB;
    u32 counter;
    u8 ready, phaseReady, prepared, restart;
    u8 unknown2B4, controlKind, unknown2B6, selection;
    u8 unknown2B8[0xA];
    u16 duration;
    u32 profileMode;
    u16 controlTimer, unknown2CA;
    u32 flags;
    u32 unknown2D0;
    u16 flags2D4;
    u8 unknown2D6[0xF7];
    u8 targetDirection;
    u8 unknown3CE[6];
    struct DogTurnBody body;
};

u32 Route_GetCurrentPointFacing(void *);

void Anim_SetDurationScale(void *, u32);

void Anim_SetFrameDurationScale(void *, u32);

static inline s32 DogTurn_Cardinal(u8 direction)
{
    return ((direction + 0x60) >> 6) & 3;
}

struct DogTurnSequence {
    u8 unknown0[0xE];
    u16 elapsed;
};

static inline u16 DogTurn_Elapsed(void *sequence)
{
    return ((struct DogTurnSequence *)sequence)->elapsed;
}

static inline u32 DogTurn_ActorFlags(struct DogTurnState *a, u32 mask)
{
    return a->flags & mask;
}

static inline void DogTurn_ApplyDuration(struct DogTurnState *a, u16 *motionFlags)
{
    u16 *scale;
    u32 mask = 0xA02F;
    if (*motionFlags & mask) {
        u16 *p = &a->duration;
        *p = 64;
        scale = p;
    } else {
        u16 *q = &a->duration;
        u16 value = *q;
        scale = q;
        if (!value)
            *scale = 64;
    }
    if (!a->drawableKind)
        Anim_SetDurationScale(((struct SceneActorAnimationSequenceDrawable *)a->drawable)->sequence,
                              *scale);
    else
        Anim_SetFrameDurationScale(&((struct SceneActorAnimationTileDrawable *)a->drawable)->tile,
                                   *scale);
}

static inline u8 DogTurn_TakePrepared(struct DogTurnState *a)
{
    if (a->prepared) {
        a->prepared = 0;
        return 1;
    }
    return 0;
}

static inline s32 DogTurn_Half(s32 value)
{
    if (value >= 0)
        return value >> 1;
    return -((-value) >> 1);
}

static inline u32 DogTurn_Flags(u16 *field, u32 mask)
{
    return *field & mask;
}

static inline void DogTurn_SetBits(u32 bits, u32 *field)
{
    *field |= bits;
}

static inline void DogTurn_ClearBits(u32 *field, u32 bits)
{
    *field &= ~bits;
}

static inline void DogTurn_SetState(struct DogTurnState *a, u32 state, u32 phase, u32 behavior)
{
    a->state = state;
    a->phase = phase;
    a->behavior = behavior;
    a->counter = 0;
    a->phaseReady = 1;
}

static inline void DogTurn_SetControl(struct DogTurnState *a, void *control, u32 kind)
{
    a->prepared = 1;
    a->ready = 0;
    a->controlKind = kind;
    a->control = control;
    a->controlTimer = 0;
    {
        u32 *field = &a->motionFlags;
        u32 mask = ~1;
        *field &= mask;
    }
    a->motionCounter = 0;
}

void Dog_UpdateCardinalTurn(struct DogTurnState *a)
{
    struct DogTurnBody *body = &a->body;
    struct DogTurnMotion *motion = &a->motion;
    u16 *motionFlags;
    u8 prepared = DogTurn_TakePrepared(a);
    s32 direction, target, delta;
    u32 oldDuration;

    motionFlags = &a->flags2D4;
    if (prepared) {
        a->motion.direction = (DogTurn_Cardinal(a->motion.direction) << 6) - 0x40;
        direction = DogTurn_Cardinal(a->motion.direction);
        if (a->behavior != 6 && !(DogTurn_Flags(motionFlags, 0x480))) {
            target = (((u16)Route_GetCurrentPointFacing(motion->route) + 0x70) >> 5) & 7;
            a->targetDirection = target;
        } else {
            target = a->targetDirection;
        }
        if (direction == DogTurn_Half(target)) {
            a->ready = 1;
            if (DogTurn_Flags(motionFlags, 0x400)) {
                SceneActorAnimation_SelectDirectionalEntry((void *)a, 0);
                DogTurn_SetState(a, 3, 1, 0);
                DogTurn_SetControl(a, body->resumeControl, 0x1B);
            } else {
                SceneActorAnimation_SelectDirectionalEntry((void *)a, 0);
            }
            return;
        }
        switch (direction) {
            case 0:
                if ((u32)(target - 2) <= 3) {
                    SceneActorTurn_SelectAnimation((void *)a, 13, 1, 0);
                    DogTurn_SetBits(0x10000, &a->flags);
                } else {
                    SceneActorTurn_SelectAnimation((void *)a, 13, 0, 1);
                    DogTurn_ClearBits(&a->flags, 0x10000);
                }
                break;
            case 1:
                if ((u32)(target - 4) <= 3) {
                    SceneActorTurn_SelectAnimation((void *)a, 13, 2, 0);
                    DogTurn_SetBits(0x10000, &a->flags);
                } else {
                    SceneActorTurn_SelectAnimation((void *)a, 13, 3, 0);
                    DogTurn_ClearBits(&a->flags, 0x10000);
                }
                break;
            case 2:
                if ((u32)(target - 1) > 4) {
                    SceneActorTurn_SelectAnimation((void *)a, 13, 3, 1);
                    DogTurn_SetBits(0x10000, &a->flags);
                } else {
                    SceneActorTurn_SelectAnimation((void *)a, 13, 2, 1);
                    DogTurn_ClearBits(&a->flags, 0x10000);
                }
                break;
            case 3:
                if ((u32)target <= 3) {
                    SceneActorTurn_SelectAnimation((void *)a, 13, 0, 0);
                    DogTurn_SetBits(0x10000, &a->flags);
                } else {
                    SceneActorTurn_SelectAnimation((void *)a, 13, 1, 1);
                    DogTurn_ClearBits(&a->flags, 0x10000);
                }
                break;
        }
        a->restart = 0;
    }
    if (DogTurn_Elapsed(((struct SceneActorAnimationSequenceDrawable *)a->drawable)->sequence) ==
        1) {
        if (DogTurn_ActorFlags(a, 0x10000))
            motion->direction += 0x20;
        else
            motion->direction -= 0x20;
    }
    oldDuration = a->duration;
    if (DogTurn_Flags(motionFlags, 0x80))
        a->duration = Div(0x40, 3);
    DogTurn_ApplyDuration(a, motionFlags);
    a->duration = oldDuration;
    if (a->restart) {
        s32 current = ((a->motion.direction + 0x70) >> 5) & 7;
        delta = a->targetDirection - current;
        if (delta < 0)
            delta = -delta;
        if (delta <= 0) {
            a->ready = 1;
            a->motion.direction = (DogTurn_Cardinal(a->motion.direction) << 6) - 0x40;
            SceneActorAnimation_SelectDirectionalEntry((void *)a, 0);
            if (DogTurn_Flags(motionFlags, 0x400)) {
                DogTurn_SetState(a, 3, 1, 0);
                DogTurn_SetControl(a, body->resumeControl, 0x1B);
            }
        } else {
            DogTurn_SetControl(a, a->turnControl, 2);
        }
    }
}

void Dog_OnReactionCleanup(void)
{
}

s32 Mod(s32, s32);

void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);

struct SUpdateDefeatFinish;

void Dog_UpdateDefeatFinish(struct SUpdateDefeatFinish *p, s32 n);

inline static u8 TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return 1;
    }
    return 0;
}

void Dog_UpdateDefeatDelay(struct ChaseActor *actor, s32 frame)
{
    struct SceneActorChaseGroup *group = &actor->group;
    u32 mask;
    if (TakeFlag(&actor->base.field2B2)) {
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

static inline u32 orr(u32 k, u16 *a)
{
    return k | *a;
}

struct SUpdateDefeatFinish {
    u8 f0[0x2b0];
    u8 b0;
    u8 f1;
    u8 b2;
    u8 b3;
    u8 f2[0x20];
    u16 h4;
    u16 h6;
};

static inline u8 tk(u8 *a)
{
    if (*a) {
        *a = 0;
        return 1;
    }
    return 0;
}

void Dog_UpdateDefeatFinish(struct SUpdateDefeatFinish *p, s32 n)
{
    if (tk(&p->b2)) {
        s32 m;
        u16 *a;
        SceneActorAnimation_SelectDirectionalEntry(p, 7);
        a = &p->h4;
        m = -2;
        *a = m & *a;
    }
    if (n == 0x28) {
        u16 k = 0x10;
        p->h6 = k | p->h6;
    }
    if (p->b3 && n > 0x78)
        p->b0 = 1;
}

// confidence=low: animation 11 is the route-arrival reaction selected by Dog_ResumeRoute.

static inline u8 TakeEntry(struct ChaseActor *a)
{
    if (a->base.field2B2) {
        a->base.field2B2 = 0;
        return 1;
    }
    return 0;
}

static inline u32 Test_UpdateArrivalAnimation(u16 *p, u32 mask)
{
    return *p & mask;
}

void Dog_UpdateArrivalAnimation(struct ChaseActor *a)
{
    u16 saved;
    u16 *flags;
    u16 *scale;
    u16 *originalScale;
    unsigned long long originalOffset;
    if (TakeEntry(a)) {
        SceneActorAnimation_SelectDirectionalEntry(a, 11);
        a->base.restart = 0;
    }
    originalOffset = 0x2C2;
    originalScale = (u16 *)((u8 *)a + originalOffset);
    saved = *originalScale;
    {
        u32 mask = 0x80;
        flags = &a->base.motionFlags;
        if (Test_UpdateArrivalAnimation(flags, mask)) {
            *originalScale = Div(0x40, 3);
        }
    }

    if (Test_UpdateArrivalAnimation(flags, 0xA02F)) {
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
        Anim_SetDurationScale((u8 *)a->base.drawable + 0x48, *scale);
    else
        Anim_SetFrameDurationScale((u8 *)a->base.drawable + 0x20, *scale);
    a->base.field2C2 = saved;
    if (a->base.restart) {
        a->base.stepDone = 1;
    }
}

// confidence=low: animation completion resumes group movement or idle.

void Dog_ResumeAfterReaction(struct ChaseActor *a)
{
    u8 *group;
    u16 flags;
    if (TakeEntry(a)) {
        SceneActorAnimation_SelectDirectionalEntry(a, 11);
        a->base.restart = 0;
    }
    if (!a->base.restart) {
        return;
    }
    group = (u8 *)&a->group.common;
    a->base.stepDone = 1;
    {
        u32 mask = 0x80;
        flags = a->base.motionFlags;
        flags = a->base.motionFlags & mask;
    }
    if (flags) {
        SceneActor_SetPositionXZ((u8 *)a + 0x50, *(s16 *)(*(u8 **)((u8 *)a + 0x5D0) + 0x1C),
                                 *(s16 *)(*(u8 **)((u8 *)a + 0x5D0) + 0x20));
        SceneActorAnimation_SelectDirectionalEntry(a, 0);
        Chase_SetCtl(a, *(void **)(group + 0x48), 0x1A);
        Chase_SetBehavior(a, 12);
        {
            u16 *field = (u16 *)((u8 *)a + 0x414);
            u32 mask = 4;
            *field = (*(&mask)) | (*field);
        }
        a->base.field2C2 = 0x20;
    } else {
        SceneActorAnimation_SelectDirectionalEntry(a, 0);
        Chase_SetCtl(a, SceneActorControl_OnIdle, 0x10);
        Chase_SetBehavior(a, 7);
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
    position->x = ({
        s32 grid_coordinate_ = (x);
        (s16)(((u32)grid_coordinate_ << 8) + 0x80);
    });
    {
        u32 attributes = SceneActorChase_CellProperties(cell)->attributes;
        u32 height = 15;
        height &= attributes;
        position->y = SceneActorChase_TerrainLevelHeight(height);
    }
    position->z = ({
        s32 grid_coordinate_ = (z);
        (s16)(((u32)grid_coordinate_ << 8) + 0x80);
    });
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

static inline u8 TakePrepared(struct ChaseActor *a)
{
    if (a->base.field2B2) {
        a->base.field2B2 = 0;
        return 1;
    }
    return 0;
}

void SceneActorAnimation_EmitDirectionalTrail(struct ChaseActor *a, s32 frame)
{
    union SceneActorRegistryPackedPosition p;
    if (TakePrepared(a))
        SceneActorAnimation_SelectDirectionalEntry(&a->base, 17);
    {
        struct DisplaySequenceState *sequence = (void *)((u8 *)a->base.drawable + 0x48);
        sequence->index = 0;
        sequence = (void *)((u8 *)a->base.drawable + 0x48);
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
        void *callback = SceneActorAnimation_RefreshTimedAttackCollider;
        u8 kind = 38;
        u8 *prepared = &a->base.field2B2;
        u32 zero = 0;
        *prepared = 1;
        a->base.stepDone = zero;
        a->base.controlKind = kind;
        a->base.target = callback;
        a->base.field2C8 = zero;
        {
            u32 *flags = &a->base.pendingControlFlags;
            s32 mask = -2;
            *flags &= mask;
        }
        a->base.pendingControl = zero;
    }
}

struct SceneActorTimedAttackContext {
    u8 unknown0[0x624];
    struct ContactCollider collider;
};

void FeatherEffectPool_CreateDirectional(struct ContactPosition *, u32, u32, u32);

void SceneActorAnimation_RefreshTimedAttackCollider(struct ChaseActor *a, u32 frame)
{
    struct SceneActorTimedAttackContext *context = a->base.context;
    if (TakePrepared(a)) {
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

static inline u8 TakePending(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return 1;
    }
    return 0;
}

static inline void BeginControl(struct ChaseActor *actor, void *control, u8 kind)
{
    actor->base.field2B2 = 1;
    actor->base.stepDone = 0;
    actor->base.controlKind = kind;
    actor->base.target = control;
    actor->base.field2C8 = 0;
    {
        u32 *flags = &actor->base.pendingControlFlags;
        s32 mask = -2;
        *flags &= mask;
    }
    actor->base.pendingControl = 0;
}

void Dog_UpdateLeapWindup(struct ChaseActor *actor)
{
    u8 *context = actor->base.context;
    if (TakePending(&actor->base.field2B2)) {
        SceneActorAnimation_SelectDirectionalEntry(actor, 0xF);
        *(struct ContactPosition *)(context + 0x6D8) =
            *(struct ContactPosition *)(*(u8 **)((u8 *)actor + 0xDC) + 0xC);
    }
    if (actor->base.restart)
        BeginControl(actor, Dog_LeapTowardTarget, 0x23);
}

/* Dog_LeapTowardTarget: on entry turns the leap velocity toward the context's target position (snapping to it within 16, else turning 16 at a time) and plays sound 0x3A4; halves the velocity while the motion's bit 0 is set, and hands over to the landing reaction (kind 0x24) when flags2D0 & 0x10 (provisional name) */

void Dog_UpdateAnimationSixteenReaction(u8 *p);

static inline s32 Scale12(s32 n)
{
    if (n >= 0)
        return n >> 12;
    return -((-n) >> 12);
}

static inline s32 Half(s32 n)
{
    if (n >= 0)
        return n >> 1;
    return -((-n) >> 1);
}

static inline void Polar(struct ContactPosition *out, u32 angle, s32 speed)
{
    out->x = Scale12(gUnk_086149C4[(angle + 0x40) & 0xFF] * speed);
    out->y = 0;
    out->z = Scale12(gUnk_086149C4[angle & 0xFF] * speed);
}

static inline void Delta_reaction(struct ContactPosition *out, const struct ContactPosition *from,
                                  const struct ContactPosition *to)
{
    out->x = to->x - from->x;
    out->y = to->y - from->y;
    out->z = to->z - from->z;
}

void Dog_LeapTowardTarget(struct ChaseActor *actor)
{
    u8 *context = actor->base.context;
    struct ContactPosition velocity;
    if (TakeFlag(&actor->base.field2B2)) {
        s32 angle, facing, turn;
        context = actor->base.context;
        Delta_reaction(&velocity, &actor->base.body.position,
                       (struct ContactPosition *)(context + 0x6D8));
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
    Collider_Refresh(context + 0x624);
    if (**(u8 **)((u8 *)actor + 0x6C) & 1) {
        actor->base.secondaryMotion.x = Half(actor->base.secondaryMotion.x);
        actor->base.secondaryMotion.z = Half(actor->base.secondaryMotion.z);
    }
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 5);
    {
        u32 mask = 0x2000;
        actor->base.behaviorFlags |= mask;
    }
    {
        u32 mask = 0x10;
        if (actor->base.statusFlags & mask) {
            void *callback = (void *)Dog_UpdateAnimationSixteenReaction;
            u32 kind = 0x24;
            actor->base.field2B2 = 1;
            actor->base.stepDone = 0;
            actor->base.controlKind = kind;
            actor->base.target = callback;
            actor->base.field2C8 = 0;
            {
                u32 *flags = &actor->base.pendingControlFlags;
                u32 mask = ~1;
                *flags &= mask;
            }
            actor->base.pendingControl = 0;
        }
    }
}

void Dog_UpdateAnimationSixteenReaction(u8 *p)
{
    if (TakeFlag(p + 0x2B2))
        SceneActorAnimation_SelectDirectionalEntry(p, 0x10);
    if (*(u8 *)(p + 0x2B3))
        *(u8 *)(p + 0x2B0) = 1;
}

void Dog_UpdateAnimationFourteenReaction(u8 *p)
{
    if (TakeFlag(p + 0x2B2))
        SceneActorAnimation_SelectDirectionalEntry(p, 0xE);
    if (*(u8 *)(p + 0x2B3))
        *(u8 *)(p + 0x2B0) = 1;
}

static inline void Dog_SetTurnFlags(struct ChaseActor *actor, u16 mask)
{
    actor->base.motionFlags = mask | actor->base.motionFlags;
}

static inline void Dog_ResetAnimation(struct ChaseActor *actor, u32 state, u32 phase)
{
    actor->base.state2A8 = state;
    actor->base.phase2A9 = phase;
    actor->base.behavior2AA = 0;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}

static inline void Dog_SetControl(struct ChaseActor *actor, u32 kind, void *control)
{
    u32 *flags;
    u32 mask;
    actor->base.field2B2 = 1;
    actor->base.stepDone = 0;
    actor->base.controlKind = kind;
    actor->base.target = control;
    actor->base.field2C8 = 0;
    flags = &actor->base.pendingControlFlags;
    mask = ~1;
    *flags &= mask;
    actor->base.pendingControl = 0;
}

void Dog_UpdateTurnSequence(struct ChaseActor *actor, s32 frame)
{
    struct SceneActorChaseGroup *group = &actor->group;
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        Dog_SetTurnFlags(actor, 0x400);
        actor->base.field2C2 = 32;
    }
    if (group->turn.turnsRemaining > 0) {
        if (frame > 20) {
            /* data holds the entry control, then the random table. */
            const void *data;
            u32 encoded;
            Dog_ResetAnimation(actor, 3, 1);
            data = actor->base.onEntry;
            Dog_SetControl(actor, 2, (void *)data);
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
            Dog_SetControl(actor, kind, control);
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

static inline u8 TestMovement(struct ChaseActor *a)
{
    void *collider = a->group.route;
    struct ContactPosition *position = &a->base.body.position;
    struct ContactPosition *destination = &a->base.tracking.target->position;
    if (!Nav_IsPositionInMesh(collider, position, destination)) {
        u16 *flags = &a->base.motionFlags;
        u32 mask = ~0x200;
        *flags &= mask;
        return 0;
    } else {
        s32 result = Terrain_ApplyBlocked(collider, position, destination);
        if (result) {
            if (result != 2)
                Memory_Clear(&a->base.body.motion, 8);
            return 1;
        } else {
            u16 *flags = &a->base.motionFlags;
            u32 mask = ~0x200;
            *flags &= mask;
            return 0;
        }
    }
}

void SceneActorMotion_TestTargetTransition(struct ChaseActor *a)
{
    struct Dog *context = a->base.context;
    if (!TakePrepared(a)) {
        SceneActorAnimation_SelectDirectionalEntry(&a->base, 11);
        if ((u8)Dog_IsActionTimerExpired(a)) {
            u8 *tail = (u8 *)a + 0x3D4;
            SceneActorAnimation_SelectDirectionalEntry(&a->base, 0);
            if (!TestMovement(a))
                Callback_InvokeActor(a, *(void **)(tail + 0x224));
            else {
                u32 *flags = &context->flags;
                s32 mask = -5;
                *flags &= mask;
                {
                    u32 next = 17;
                    u8 *state = &a->base.state2A8;
                    u32 zero = 0;
                    *state = zero;
                    *((u8 *)a + 0x2A9) = zero;
                    a->base.behavior2AA = next;
                    a->base.counter2AC = zero;
                    a->base.field2B1 = 1;
                }
            }
        }
    }
}

struct SceneActorTargetMotionContext {
    u8 unknown0[0x678];
    u32 flags;
    u8 unknown67C[0x10];
    u32 timeLimit;
    u8 unknown690[0x17];
    u8 phase;
};

static inline u8 IsActive(u32 flags)
{
    if (flags)
        return 1;
    return 0;
}

static inline void SetPhase(struct ChaseActor *a, u32 phase)
{
    u8 *state = &a->base.state2A8;
    u32 zero = 0;
    *state = zero;
    a->base.phase2A9 = zero;
    a->base.behavior2AA = phase;
    a->base.counter2AC = zero;
    a->base.field2B1 = 1;
}

void SceneActorMotion_UpdateTargetTransition(struct ChaseActor *a)
{
    u32 *flags;
    struct SceneActorTargetMotionContext *c = a->base.context;
    if (!TakePrepared(a)) {
        u32 four = 4;
        u32 *address = &c->flags;
        u32 value = *address & four;
        flags = address;
        if (IsActive(value)) {
            if (a->base.tracking.distance > c->timeLimit) {
                SetPhase(a, 19);
                {
                    s32 angle = a->base.tracking.heading;
                    if (angle >= 0)
                        angle >>= 5;
                    else
                        angle = -(-angle >> 5);
                    a->base.nextDirection = (angle + 3) & 7;
                }
            } else {
                s32 mask = -5;
                *flags &= mask;
                if (c->phase <= 3) {
                    if ((u8)Dog_SelectRetreatDirection(a, a->base.body.direction))
                        SetPhase(a, 17);
                } else {
                    SetPhase(a, 19);
                    {
                        s32 angle = a->base.tracking.heading;
                        if (angle >= 0)
                            angle >>= 5;
                        else
                            angle = -(-angle >> 5);
                        a->base.nextDirection = (angle + 3) & 7;
                    }
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

static inline void SetAnimation(struct ChaseActor *actor, u8 state, u8 phase, u8 behavior)
{
    actor->base.state2A8 = state;
    actor->base.phase2A9 = phase;
    actor->base.behavior2AA = behavior;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}

static inline void Hide(u8 *sprite, u32 mask)
{
    *(u32 *)(sprite + 8) |= mask;
}

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
            u32 fieldOffset = 0x2CC;
            u32 *field;
            u32 mask;
            field = (void *)((u8 *)actor + fieldOffset);
            mask = ~0x141C0;
            *field &= mask;
            control->counter = 0;
            control->field14 = 0;
            control->timer = 0;
        }
        {
            u16 *colliderFlags = (u16 *)((u8 *)actor + 0x13A);
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
                u32 mask = 0x4000;
                u16 value = *flags;
                u16 busy = value & mask;
                if (!busy) {
                    void *callback = (void *)SceneActorControl_OnIdle;
                    u32 kind;
                    kind = 16;
                    actor->base.field2B2 = 1;
                    actor->base.stepDone = 0;
                    actor->base.controlKind = kind;
                    actor->base.target = callback;
                    actor->base.field2C8 = 0;
                    {
                        u32 *flags = &actor->base.pendingControlFlags;
                        u32 mask = ~1;
                        *flags &= mask;
                    }
                    actor->base.pendingControl = 0;
                    SetAnimation(actor, 1, 0, 2);
                }
            }
            ActorShadow_Disable((u8 *)actor + 0x320);
            {
                u32 mask = 1;
                if (!actor->base.drawableKind)
                    *(u32 *)actor->base.drawable |= mask;
                else
                    Hide((u8 *)actor->base.drawable + 0x20, mask);
            }
        } else {
            void *callback = (void *)SceneActorControl_CompleteBlinkOut;
            u32 kind;
            kind = 0x27;
            actor->base.field2B2 = 1;
            actor->base.stepDone = instant;
            actor->base.controlKind = kind;
            actor->base.target = callback;
            actor->base.field2C8 = instant;
            {
                u32 *flags = &actor->base.pendingControlFlags;
                u32 mask = ~1;
                *flags &= mask;
            }
            actor->base.pendingControl = instant;
        }
    }
}

// confidence=low: finishes the blink, runs defeat scripts and hides the drawable and shadow.

/* SceneActorControl_CompleteBlinkOut: keeps terrain timers and height updated while the drawable blinks (shadow follows visibility); when the blink expires runs the context and defeat scripts, sets the defeat event flag, goes idle unless flags2D4 & 0x4000, and hides the drawable and shadow (provisional name) */

static inline u8 DrawableVisible(struct ChaseActor *actor)
{
    u32 mask = 1;
    if (!actor->base.drawableKind) {
        if (*(u32 *)actor->base.drawable & mask)
            goto visible;
        goto hidden;
    }
    if (((struct SceneActorRegistryTileDrawable *)actor->base.drawable)->flags & mask)
    visible:
        return 1;
hidden:
    return 0;
}

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
            u32 mask = 0x4000;
            u16 value = *flags;
            u16 busy = value & mask;
            if (!busy) {
                void *callback = (void *)SceneActorControl_OnIdle;
                u32 kind;
                kind = 16;
                actor->base.field2B2 = 1;
                actor->base.stepDone = 0;
                actor->base.controlKind = kind;
                actor->base.target = callback;
                actor->base.field2C8 = 0;
                {
                    u32 *flags = &actor->base.pendingControlFlags;
                    u32 mask = ~1;
                    *flags &= mask;
                }
                actor->base.pendingControl = 0;
                SetAnimation(actor, 1, 0, 2);
            }
        }
        ActorShadow_Disable((u8 *)actor + 0x320);
        {
            u32 mask = 1;
            if (!actor->base.drawableKind)
                *(u32 *)actor->base.drawable |= mask;
            else
                Hide((u8 *)actor->base.drawable + 0x20, mask);
        }
    } else if (DrawableVisible(actor))
        ActorShadow_Disable((u8 *)actor + 0x320);
    else
        ActorShadow_Enable((u8 *)actor + 0x320);
}

/* This class's own copies of the shared chase inlines it compiles differently. */

/* The reaction fields overlap the canonical actor's species storage. */

union DogReactionContext {
    struct ChaseActor base;
    struct Dog reaction;
};

static inline s32 Dog_HeadingOctant(s32 angle)
{
    if (angle >= 0)
        return angle >> 5;
    return -(-angle >> 5);
}

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
            s32 angle = Dog_HeadingOctant((a->base.tracking.heading + 0xA0) & 255);
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

struct Sscripted {
    u8 pad0[0x59];
    u8 at59;
    u8 pad1[0x98 - 0x5A];
    u8 at98;
    u8 pad2[0xE9 - 0x99];
    u8 atE9;
    u8 pad3[0x104 - 0xEA];
    u32 at104;
    u8 pad4[0x114 - 0x108];
    u32 flags114;
    u8 pad5[0x21C - 0x118];
    void (*callback)(void);
    u8 pad6[0x2AC - 0x220];
    u32 counter;
    u8 done;
    u8 queued;
    u8 taken;
    u8 pad7[2];
    u8 code;
    u8 pad8[0x2C8 - 0x2B6];
    u16 at2C8;
    u8 pad9[0x2D4 - 0x2CA];
    u16 flags2D4;
    u8 pad10[0x414 - 0x2D6];
    u16 flags414;
    u16 at416;
    u16 at418;
};

void Dog_UpdateScriptedAction(struct Sscripted *obj)
{
    if (TakeFlag(((u8 *)obj) + 0x2B1)) {
        u16 *h;
        u16 value;
        s32 off;
        SceneActorChase_SetControl(obj, Dog_UpdateArrivalAnimation, 10);
        {
            u32 byte = obj->atE9;
            obj->at59 = byte;
        }
        SceneActorAttachment_ReplaceResponseSlot((u8 *)obj, 0, 0, obj->at98);
        SceneActorBehavior_CallActionScript((u8 *)obj);
        h = (u16 *)(((u8 *)obj) + 0x414);
        {
            u32 flag = 4;
            flag |= *h;
            *h = flag;
        }
        {
            u32 flag = 0x80;
            u16 *flags = (u16 *)(((u8 *)obj) + 0x2D4);
            flag |= *flags;
            *flags = flag;
        }
        off = 0x416;
        value = *((u16 *)(((u8 *)obj) + off));
        off += 2;
        *((u16 *)(((u8 *)obj) - (-off))) = value;
    }
    if (obj->done) {
        Dog_SelectTargetAction((u8 *)obj);
    } else {
        ++(*((u32 *)(((u8 *)obj) + 0x2AC)));
    }
}

static inline u32 Dog_RouteFlags(struct ChaseActor *actor, u32 mask)
{
    return actor->base.behaviorFlags & mask;
}

/* Queue pursuit, a home route, or the short waiting response after the delay. */

void Dog_UpdateDelayedPursuit(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *group;
    u32 zero;
    SceneActorControl_TakeStateEntry(actor);
    if ((u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque))
        return;
    zero = (u8)sub_0812F3CC(actor);
    if (zero)
        return;
    if ((s32)actor->base.counter2AC <= 60)
        goto waiting;
    group = &actor->group;
    SceneActorAttachment_ReplaceResponseSlot((u8 *)actor, 0, 2, actor->base.category);
    SceneActorChase_SetState(actor, 3, 1, zero);
    SceneActorChase_SetControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
    if (SceneActorChase_MotionFlags(actor, 0x80)) {
        SceneActorDelay_Prepare((void *)&group->saved, group->common.gate.control, 12, 30);
        SceneActorChase_Arrive(actor);
    } else if (Dog_RouteFlags(actor, 0x10)) {
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

extern const u8 gUnk_08606204[];

struct SSetPrimaryBehaviorPreset {
    u8 filler[648];
    const u8 *p;
};

void Dog_SetPrimaryBehaviorPreset(struct SSetPrimaryBehaviorPreset *s)
{
    s->p = gUnk_08606204;
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

void Dog_UpdateActionTimers(struct SUpdateActionTimers *s);

void Dog_UpdateActionCooldown(struct SUpdateActionCooldown *s);

void Dog_TickCooldown(struct STickCooldown *s);

s32 Dog_FinishFrame(void *p)
{
    Dog_UpdateCollisionBounds(p);
    Dog_UpdateActionTimers(p);
    Dog_UpdateActionCooldown(p);
    Dog_TickCooldown(p);
    return 1;
}

void Dog_UpdateLeapWindup(struct ChaseActor *actor);

static inline void Dog_MaskAttackFlags(u32 *field, u32 mask)
{
    *field &= mask;
}

static inline void Dog_ClearAttackFlags(struct Dog *ctx, u32 bits)
{
    Dog_MaskAttackFlags(&ctx->flags, ~bits);
}

void Dog_UpdateLeapAttack(struct ChaseActor *a)
{
    struct Dog *context = a->base.context;
    if (Chase_TakeArrived(a)) {
        And32(&context->flags, ~8);
        SceneActorReaction_EnableMotionFlags(0x100, &a->base.motionFlags);
        {
            void *callback = Dog_UpdateLeapWindup;
            u32 kind = 34;
            a->base.field2B2 = 1;
            a->base.stepDone = 0;
            a->base.controlKind = kind;
            a->base.target = callback;
            a->base.field2C8 = 0;
            {
                u32 *flags = &a->base.pendingControlFlags;
                s32 mask = -2;
                *flags &= mask;
            }
            a->base.pendingControl = 0;
        }
        Dog_ClearAttackFlags(a->base.context, 2);
        {
            void *collider = &context->collider;
            u32 damage = a->base.attacks.first[1];
            u32 response = a->base.attacks.second[1];
            u32 power = a->base.attacks.flags[1];
            u32 extra = a->base.attacks.parameter[1];
            Collider_SetAttackProperties(collider, damage, response, 0, power, extra);
        }
        context->emitted = 0;
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
    if (Chase_TakeArrived(a)) {
        And32(&context->flags, ~8);
        SceneActorReaction_EnableMotionFlags(0x100, &a->base.motionFlags);
        {
            void *callback = SceneActorAnimation_EmitDirectionalTrail;
            u32 kind = 37;
            a->base.field2B2 = 1;
            a->base.stepDone = 0;
            a->base.controlKind = kind;
            a->base.target = callback;
            a->base.field2C8 = 0;
            {
                u32 *flags = &a->base.pendingControlFlags;
                s32 mask = -2;
                *flags &= mask;
            }
            a->base.pendingControl = 0;
        }
        Dog_SetContextFlags(a->base.context, 2);
        {
            void *collider = &context->collider;
            u32 damage = a->base.attacks.first[2];
            u32 response = a->base.attacks.second[2];
            u8 *p = (u8 *)a + 0xD4;
            u32 power = *(u32 *)p;
            u32 extra;
            p -= 12;
            extra = *(u16 *)p;
            Collider_SetAttackProperties(collider, damage, response, 0, power, extra);
        }
        context->emitted = 0;
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

extern const u8 gUnk_0860620C[];

struct SSetSecondaryBehaviorPreset {
    u8 filler[652];
    const u8 *p;
};

void Dog_SetSecondaryBehaviorPreset(struct SSetSecondaryBehaviorPreset *s)
{
    s->p = gUnk_0860620C;
}

// confidence=low: tracks player zero and switches from the chase animation to a bite in range.

/* Dog_ChasePlayer: on entry starts the chase animation (kind 0x28); bites when a bite frame of the run cycle ends with player 0 in range (records the player's position, extends the chase timer), otherwise once the control finishes either follows the group's chase (flags2D4 & 0x80) or routes back to its route point (provisional name) */

void Dog_UpdateAnimationFourteenReaction(u8 *p);

u8 sub_0812F3CC(struct ChaseActor *);

struct DogAnim {
    u8 unknown0[7];
    u8 rate;
    u16 frame;
    u8 unknownA[4];
    u16 timer;
};

static inline u16 LoadMotionFlags(struct ChaseActor *actor, u32 mask)
{
    u16 value = actor->base.motionFlags;
    return value & mask;
}

static inline void Delta_scripted(struct ContactPosition *out, const struct ContactPosition *from,
                                  const struct ContactPosition *to)
{
    out->x = to->x - from->x;
    out->y = to->y - from->y;
    out->z = to->z - from->z;
}

void Dog_ChasePlayer(struct ChaseActor *actor)
{
    u8 *context = actor->base.context;
    u8 *group = (u8 *)&actor->group.common;
    struct ContactPosition target;
    if (Chase_TakeArrived(actor))
        Chase_SetCtl(actor, Dog_UpdateAnimationFourteenReaction, 0x28);
    if ((u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque))
        return;
    if (sub_0812F3CC(actor))
        return;
    {
        struct DogAnim *anim;
        s32 distance;
        Delta_scripted(&target, &actor->base.body.position,
                       (struct ContactPosition *)((u8 *)gPlayers[0] + 0x30));
        distance = target.x * target.x + target.z * target.z;
        anim = (struct DogAnim *)((u8 *)actor->base.drawable + 0x48);
        if ((anim->frame == 6 || anim->frame == 0xC || anim->frame == 0x13 ||
             anim->frame == 0x19) &&
            anim->timer == anim->rate - 1 && (u32)distance < *(u32 *)(context + 0x684)) {
            SceneActorAttachment_ReplaceResponseSlot(actor, 0, 1, actor->base.category);
            Chase_SetBehavior(actor, 0x10);
            *(struct ContactPosition *)(context + 0x6D8) =
                *(struct ContactPosition *)((u8 *)gPlayers[0] + 0x30);
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
        if (LoadMotionFlags(actor, 0x80)) {
            Chase_SetBehavior(actor, 0xC);
            Chase_SetCtl(actor, *(void **)(group + 0x48), 0x1A);
            {
                u16 *field = (u16 *)(group + 0x40);
                u32 mask = 4;
                mask |= *field;
                *field = mask;
            }
            return;
        }
        Chase_SnapToRoute(&target, group + 0x74);
        Chase_SetCtl(actor, actor->base.onAlternateEntry, 1);
        Chase_SetBehavior(actor, 2);
        if (!Chase_TryRoute(actor, *(void **)(group + 0x1FC), &actor->base.body.position, &target))
            Callback_InvokeActor(actor, *(void **)(group + 0x224));
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

static inline void Dog_SetMovementBits(struct ChaseActor *actor, u32 mask)
{
    u32 *flags = &actor->base.behaviorFlags;
    *flags |= mask;
}

void Dog_ResumeRoute(struct ChaseActor *a)
{
    u8 *group = (u8 *)&a->group.common;
    u8 *context = a->base.context;
    if (Chase_TakeArrived(a))
        Chase_SetCtl(a, Dog_UpdateArrivalAnimation, 10);
    if ((u8)Callback_InvokeActor(a, a->base.contactQuery.opaque))
        return;
    if (sub_0812F3CC(a))
        return;
    if (a->base.stepDone) {
        if (!Chase_TryRoute(a, *(void **)(group + 0x1FC), &a->base.body.position,
                            (struct ContactPosition *)(context + 0x6D8))) {
            Callback_InvokeActor(a, *(void **)(group + 0x224));
            return;
        }
        if (Dog_SetMotionBits(a, 0x200)) {
            Chase_SetBehavior(a, 12);
            Chase_SetCtl(a, *(void **)(group + 0x48), 0x1A);
            {
                u16 *flags = (u16 *)(group + 0x40);
                u32 mask = 4;
                mask |= *flags;
                *flags = mask;
            }
        } else {
            struct ContactPosition *p = *(struct ContactPosition **)(group + 0x200);
            Chase_SnapToRoute(p, group + 0x74);
            (*(struct ContactPosition **)(group + 0x200))->pad = 0;
            Dog_SetMovementBits(a, 0x10);
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
    u32 zero;
    if (SceneActorControl_TakeStateEntry(actor)) {
        SceneActorChase_SetControl(actor, actor->base.onStanding, 0);
        SceneActorChase_ClearWordFlags(&actor->base.behaviorFlags, 0x80);
        SceneActorChase_ClearMotionFlagComponent(actor, 0x200);
    }
    if ((u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque))
        return;
    zero = (u8)sub_0812F3CC(actor);
    if (zero)
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

struct Sapproach {
    u8 filler[644];
    const u8 *p;
};

void Dog_SetControlPreset(struct Sapproach *s)
{
    s->p = gUnk_08606214;
}

s32 Dog_PrepareFrame(void *p);

void SceneActorControl_DispatchControl(void *);

s32 Dog_FinishFrame(void *p);

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

s32 Dog_Update(struct SUpdate *s);

u16 Dog_UpdateItemDrop(struct ChaseActor *actor);

void Dog_UpdateCardinalTurn(struct DogTurnState *a);

void Dog_UpdateContactRecovery(struct ChaseActor *actor);

void Dog_SelectAnimationOne(struct SSelectAnimationOne *s);

void Dog_SelectAnimationZero(struct SSelectAnimationZero *s);

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

void Dog_OnClearActivityFlag(struct S *s);

void Dog_RestoreRoute(struct ChaseActor *actor);

void Dog_UpdateTurnSequence(struct ChaseActor *actor, s32 frame);

void Dog_SetControlPreset(struct Sapproach *s);

void Dog_SetPrimaryBehaviorPreset(struct SSetPrimaryBehaviorPreset *s);

void Dog_SetSecondaryBehaviorPreset(struct SSetSecondaryBehaviorPreset *s);

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

static inline u8 TakeEntry_approach(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return 1;
    }
    return 0;
}

void SceneActor_UpdateAlternateEntryPhase(struct ChaseActor *actor)
{
    if (TakeEntry_approach(&actor->base.field2B1)) {
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
