#include "global.h"
#include "dog.h"
#include "scene_actor_reset.h"
#include "scene_actor_chase.h"
#include "rotation_phase.h"
#include "scene_actor_profile.h"
#include "scene_actor_palette.h"
#include "scene_actor_control_motion.h"
#include "scene_actor_animation.h"
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

u8 Dog_IsActionTimerExpired(struct ChaseActor *);

void Dog_SelectRetreatOrChase(struct ChaseActor *a);

void Dog_SelectTargetAction(struct ChaseActor *);

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

u8 Dog_SelectRetreatDirection(struct ChaseActor *, u32);

u8 SceneActorTarget_TrySelectNearestParticipant(struct ChaseActor *);

void SceneActorMotion_TestTargetTransition(void);

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
        *(struct SceneActorEffectsOffset *)&((struct SceneActorChasePlayer *)gUnknown_02000580[0])
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
    actor->base.tracking.target = &((struct SceneActorChasePlayer *)gUnknown_02000580[0])->target;
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
            &((struct SceneActorChasePlayer *)gUnknown_02000580[0])->target;
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

static inline void DogTurn_SelectAnimation(struct DogTurnState *a, u32 selection, u32 variant,
                                           u32 flags)
{
    u32 force;
    if (flags & 0x10) {
        force = 0x10;
        flags &= ~0x10;
    } else
        force = 0;
    if (!a->drawableKind) {
        struct SceneActorAnimationSequenceDrawable *drawable;
        a->selection = selection;
        drawable = a->drawable;
        Anim_SetSequence(drawable->sequence, (void *)drawable->resource, (u16)selection,
                         (u8)variant, (u8)flags);
    } else {
        struct SceneActorAnimationTile *tile =
            &((struct SceneActorAnimationTileDrawable *)a->drawable)->tile;
        u8 *savedSelection;
        selection += variant;
        {
            u8 *selectionAddress = &a->selection;
            u32 previous = *selectionAddress;
            savedSelection = selectionAddress;
            if (!force && previous == selection && !a->restart) {
                if (flags == 1 && (tile->flags & 4))
                    return;
                if (flags == 0 && !(tile->flags & 4))
                    return;
            }
        }
        *savedSelection = selection;
        {
            u32 mask = 4;
            u32 mode;
            if (flags & mask)
                mode = 6;
            else
                mode = 4;
            Anim_SetKeyframes(&((struct SceneActorAnimationTileDrawable *)a->drawable)->tile,
                              a->drawable, (u16)selection, 2, (u16)mode);
            if (flags & 1)
                tile->flags |= mask;
            else
                tile->flags &= ~4;
        }
    }
    a->restart = 0;
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
                    DogTurn_SelectAnimation(a, 13, 1, 0);
                    DogTurn_SetBits(0x10000, &a->flags);
                } else {
                    DogTurn_SelectAnimation(a, 13, 0, 1);
                    DogTurn_ClearBits(&a->flags, 0x10000);
                }
                break;
            case 1:
                if ((u32)(target - 4) <= 3) {
                    DogTurn_SelectAnimation(a, 13, 2, 0);
                    DogTurn_SetBits(0x10000, &a->flags);
                } else {
                    DogTurn_SelectAnimation(a, 13, 3, 0);
                    DogTurn_ClearBits(&a->flags, 0x10000);
                }
                break;
            case 2:
                if ((u32)(target - 1) > 4) {
                    DogTurn_SelectAnimation(a, 13, 3, 1);
                    DogTurn_SetBits(0x10000, &a->flags);
                } else {
                    DogTurn_SelectAnimation(a, 13, 2, 1);
                    DogTurn_ClearBits(&a->flags, 0x10000);
                }
                break;
            case 3:
                if ((u32)target <= 3) {
                    DogTurn_SelectAnimation(a, 13, 0, 0);
                    DogTurn_SetBits(0x10000, &a->flags);
                } else {
                    DogTurn_SelectAnimation(a, 13, 1, 1);
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

void SceneActorAttachment_ReplaceResponseSlot(u8 *, s32, s32, u32);

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

void Dog_UpdateDefeatDelay(u8 *s, s32 arg)
{
    u8 *part = s + 0x3D4;
    s32 zero;
    u32 mask;
    s32 off;
    if (TakeFlag(s + 0x2B2)) {
        u32 test;
        u16 *flags;
        u32 value;
        SceneActorAnimation_SelectDirectionalEntry(s, 6);
        test = 2;
        flags = (u16 *)(s + 0x2D4);
        value = *flags;
        test &= value;
        if (test == 0) {
            test = 1;
            test |= value;
            *flags = test;
        }
    }
    if (Mod(arg, 60) == 0) {
        SceneActorAttachment_ReplaceResponseSlot(s, 2, 0, s[0x98]);
    }
    off = 0xF9;
    off <<= 1;
    if ((*((s16 *)(part + off))) <= 0) {
        mask = 0x80;
        mask <<= 5;
        off += 0xDE;
        zero = (*((u32 *)(s - (-off)))) & mask;
        if (zero == 0) {
            void (*fn)(void) = Dog_UpdateDefeatFinish;
            u32 code = 6;
            u32 *flags;
            u32 clear;
            s[0x2B2] = 1;
            s[0x2B0] = zero;
            s[0x2B5] = code;
            *((void (**)(void))(s + 0x21C)) = fn;
            *((u16 *)(s + 0x2C8)) = zero;
            flags = (u32 *)(s + 0x114);
            do {
                clear = ~1;
                *flags &= clear;
                *((u32 *)(s + 0x104)) = zero;
            } while (0);
        }
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
