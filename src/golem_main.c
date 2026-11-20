#include "global.h"
#include "scene_particle_burst_api.h"
#include "engine.h"
#include "contact_actor.h"
#include "scene_actor_reset.h"
#include "scene_actor_chase.h"
#include "scene_actor_route.h"
#include "scene_actor_terrain.h"
#include "scene_actor_adjacent_cell.h"
#include "player.h"
#include "scene_actor_restoration.h"
#include "scene_actor_control_motion.h"
#include "scene_actor_contact_response.h"
#include "display_sequence_inline.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_reaction.h"
#include "scene_actor_turn.h"
#include "display_sequence.h"
#include "scene_actor_animation.h"
#include "scene_object.h"
#include "golem_charge.h"
#include "monster_particle_random.h"
#include "scene_actor_delayed_response.h"

void Golem_CenterPositionInCell(void *ptr)
{
    u16 *p = ptr;
    long long mask = 0xFFFFFF00;

    *p = (*p & mask) + 0x80;
    p[1] = (mask & p[1]) + 0x80;
    p[2] = (mask & p[2]) + 0x80;
}

// Golem settle step, called with the actor context.

struct Unk080C47DCObject {
    u8 unknown0[0x1C];
    struct ContactPosition position;
};

struct Unk080C47DCBody {
    u8 unknown0[0xC];
    struct ContactPosition position;
};

struct Unk080C47DCDelta {
    s16 x, y, z, pad;
};

struct Unk080C47DCLink {
    u8 unknown0[0x1FC];
    struct Unk080C47DCObject *target;
};

s32 Terrain_ApplyBlocked(void *, struct ContactPosition *, struct ContactPosition *);

static inline u32 Cell(s32 x, s32 z)
{
    if ((((x < 0) || (z < 0)) || (((u32)x) >= gTerrainWidth)) || (((u32)z) >= gTerrainHeight))
        return 0;
    return gTerrainMap->rows[z] + x;
}

s32 Golem_AlignChargeTarget(u8 *actor)
{
    u8 *owner = actor;
    struct Unk080C47DCLink *link = (struct Unk080C47DCLink *)(actor + 0x3D4);
    struct Unk080C47DCBody *body = (struct Unk080C47DCBody *)(actor + 0x50);
    struct Unk080C47DCObject **target = (struct Unk080C47DCObject **)(actor + 0x5D0);
    s32 found = Terrain_ApplyBlocked(*target, actor + 0x5C, actor + 0x69C);
    if (found != 0) {
        s32 dx = (*target)->position.x - body->position.x;
        s32 dz = (*target)->position.z - body->position.z;
        u8 *direction;
        struct Unk080C47DCDelta delta;
        struct Unk080C47DCObject **goalTarget;
        if (Cell((*target)->position.x >> 8, (*target)->position.z >> 8) !=
            Cell(body->position.x >> 8, body->position.z >> 8)) {
            if (((dx < 0) ? (-dx) : (dx)) < ((dz < 0) ? (-dz) : (dz)))
                link->target->position.x = (body->position.x & (~0xFF)) + 0x80;
            else {
                struct ContactPosition *position = &body->position;
                link->target->position.z = (position->z & (~0xFF)) + 0x80;
            }
        }
        goalTarget = &link->target;
        dx = (*goalTarget)->position.x - body->position.x;
        dz = (*goalTarget)->position.z - body->position.z;
        {
            u8 angle = Math_Atan2(dx, dz);
            direction = actor + 0x68E;
            *direction = angle;
        }
        *direction = ((((*direction) + 0x20) & 0xFF) >> 6) << 6;
        {
            struct Unk080C47DCDelta *out = &delta;
            struct ContactPosition *from = (struct ContactPosition *)(owner + 0x5C);
            struct ContactPosition *goal = &(*goalTarget)->position;
            s32 quadrant;
            {
                struct Unk080C47DCDelta *first = &delta;
                first->x = ((u16)goal->x) - ((u16)from->x);
            }
            out->y = ((u16)goal->y) - ((u16)from->y);
            out->z = ((u16)goal->z) - ((u16)from->z);
            quadrant = Math_Atan2(delta.x, out->z);
            quadrant = ((quadrant + 0x20) & 0xFF) >> 6;
            if ((delta.x != (-out->z)) && (quadrant != ((*direction) >> 6)))
                Memory_Clear(owner + 0x64, 8);
        }
    } else {
        *((u16 *)(actor + 0x680)) = found;
    }
    return 1;
}

void Golem_ResetIdleControl(struct ChaseActor *actor);

void SceneActorControl_OnIdle(u8 *);

static inline u32 TestFlags(u16 *flags, u32 mask)
{
    return *flags & mask;
}

void SceneActorControl_RefreshMotionRoute(struct ChaseActor *actor)
{
    u32 active = SceneActorChase_TrackingFlags(&actor->base.tracking, 8);
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
            u16 *flags = &actor->base.motionFlags;
            u32 value = *flags;
            mask |= value;
            *flags = mask;
        }
        actor->group.common.gate.counter = actor->group.common.gate.duration;
        if (TestFlags(&actor->base.motionFlags, 256))
            return;
        Golem_ResetIdleControl(actor);
    } else {
        Chase_SetState(actor, 0, 1, 0);
        Chase_SetCtl(actor, SceneActorControl_OnIdle, 16);
    }
    {
        struct SceneActorChaseTracking *tracking = &actor->base.tracking;
        u32 mask = 8;
        u32 value = tracking->flags;
        mask |= value;
        tracking->flags = mask;
    }
}

/* Golem_RestoreRoute: unless flags2D4 bit 0x100 is set, restores after a delay, records the follow-up control (chase when flags2D4 & 0x80, else the entry target) and routes back toward the chased object via the route point (provisional name) */

void Golem_RestoreRoute(struct ChaseActor *actor)
{
    u8 *group = (u8 *)&actor->group.common;
    void *flags;

    if (SceneActorContact_QueryMotionField(actor, 0x100, &flags))
        return;
    Chase_SetState(actor, 3, 1, 0);
    Chase_SetCtl(actor, (void *)SceneActorReaction_RestoreAfterDelay, 0xE);
    if (TestFlags(flags, 0x80)) {
        actor->base.field2C2 = 0x20;
        SceneActorChase_GroupSetFlags(&actor->group, 4);
        SceneActorDelay_Prepare((void *)&actor->group.saved, *(void **)(group + 0x48), 0xC, 0x1E);
        if (TestFlags(flags, 0x80)) {
            u16 *time = &actor->group.common.gate.counter;
            *time += 0x12C;
            if (*time > actor->group.common.gate.duration)
                *time = actor->group.common.gate.duration;
        }
    } else {
        SceneActorChase_SetActorFlags(actor, 0x10);
        actor->base.field2C2 = 0x20;
        SceneActorDelay_Prepare((void *)&actor->group.saved, actor->base.onAlternateEntry, 7, 0x3C);
    }
    Chase_SnapToRoute(*(struct ContactPosition **)(group + 0x200), group + 0x74);
    (*(struct ContactPosition **)(group + 0x200))->pad = 0;
    if (!Chase_TryRoute(actor, *(void **)(group + 0x1FC), &actor->base.body.position,
                        &actor->base.tracking.target->position))
        Callback_InvokeActor(actor, *(void **)(group + 0x224));
}

s32 Nav_IsPositionInMesh(void *, struct ContactPosition *, struct ContactPosition *);

void Golem_CenterPositionInCell(void *ptr);

void SceneActorRoute_CommitReachableDestination(struct ChaseActor *actor)
{
    struct SceneActorRouteContext *context = actor->base.context;
    struct SceneActorChaseGroup *group = &actor->group;
    struct SceneActorChaseTarget *target = actor->base.tracking.target;
    s32 cell = SceneActorTerrain_GetCellIndex(&target->position);
    u32 flags = SceneActor_CellAttributes(cell);
    if (!(flags & 0x43)) {
        if (Nav_IsPositionInMesh(group->route, &actor->base.body.position,
                                 &actor->base.tracking.target->position)) {
            context->destination = actor->base.tracking.target->position;
            Golem_CenterPositionInCell(&context->destination);
        }
    }
}

static inline void SceneActorRoute_SetContextFlags(struct SceneActorRouteContext *context,
                                                   u32 flags)
{
    context->flags628 |= flags;
}

void SceneActorRoute_SetReachableDestination(struct ChaseActor *actor,
                                             struct ContactPosition *destination)
{
    struct SceneActorRouteContext *context = actor->base.context;
    struct SceneActorChaseGroup *group = &actor->group;
    s32 cell = SceneActorTerrain_GetCellIndex(destination);
    u32 flags = SceneActor_CellAttributes(cell);
    if (!(flags & 0x43)) {
        if (Nav_IsPositionInMesh(group->route, &actor->base.body.position, destination)) {
            context->destination = *destination;
            Golem_CenterPositionInCell(&context->destination);
            SceneActorRoute_SetContextFlags(context, 0x80000);
        }
    }
}

void SceneActorRoute_CommitReachableDestination(struct ChaseActor *actor);

static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}

struct SkeletonLink {
    u8 filler[0x628];
    u32 h628;
};

struct SkeletonActor {
    u8 filler0[0xDC];
    u32 h0DC;
    u8 filler1[0x2D4 - 0xE0];
    u16 flags;
    u8 filler2[0x3D0 - 0x2D6];
    struct SkeletonLink *t;
};

void Skeleton_TryTriggerAction(struct SkeletonActor *s)
{
    struct SkeletonLink *t = s->t;
    u32 mask;

    mask = 0x80000;
    if (Test(t->h628, mask))
        return;

    mask = 0x180;
    if (!(mask & s->flags))
        return;

    {
        struct Player *player = gPlayers[0];
        mask = 0x20000;
        if (player->statusFlags & mask)
            return;
    }

    if (s->h0DC == 0)
        return;

    SceneActorRoute_CommitReachableDestination(s);
}

extern const u8 gUnk_0824E274[];

void Golem_ResetIdleControl(struct ChaseActor *actor)
{
    SceneActorReaction_ResetResponseState(&actor->group.saved.control, gUnk_0824E274, 0x22F);
    SceneActorChase_SelectState(actor, 0, 2, 0);
    SceneActorChase_ResetStateEntry(actor);
    Chase_SetCtl(actor, actor->base.onAlternateEntry, 1);
}

s32 SceneActorProfiles_FindOrAdd(u32, u32);

void Golem_InitAnimationProfile(u8 *p)
{
    *(u32 *)(p + 0x308) = SceneActorProfiles_FindOrAdd(3, 0);
}

struct GolemCombatResetContext {
    u8 pad0[0x624];
    u8 b624;
    u8 pad625[3];
    u32 w628;
    u8 pad62c[0x50];
    u32 w67c;
    u16 h680, h682, h684, h686, h688, h68a;
    u8 b68c, b68d, b68e, b68f, b690, b691;
    u8 pad692[2];
    u8 a694[8];
    u8 a69c[8];
    u8 a6a4[8];
    u16 h6ac, h6ae;
};

struct GolemResetActor {
    u8 pad0[4];
    void *spr;
};

void Memory_Clear(void *, int);

void SceneActorMovement_ResolveRoutePosition(void *);

// confidence=low: resets golem combat counters and restarts the idle animation.

void Golem_ResetCombatState(u8 *p)
{
    struct GolemCombatResetContext *in = *(struct GolemCombatResetContext **)(p + 0x3d0);
    in->b624 = 0;
    in->w628 = 0;
    in->w67c = 0;
    in->h682 = 0;
    in->h680 = 0;
    in->h684 = 0;
    in->h686 = 0;
    in->h688 = 0;
    in->h68a = 0;
    in->b68c = 0;
    in->b68d = 0;
    in->b68e = 0;
    in->b68f = 0;
    in->b690 = 0;
    in->b691 = 0;
    Memory_Clear(in->a694, 8);
    Memory_Clear(in->a69c, 8);
    Memory_Clear(in->a6a4, 8);
    Memory_Clear((u8 *)in + 0x674, 8);
    in->h6ac = 0;
    in->h6ae = 0;
    SceneActorMovement_ResolveRoutePosition(p);
    SceneActorAnimation_SelectDirectionalEntry((void *)p, 0);
    {
        struct SceneActorControlMotionDrawable *drawable = ((struct GolemResetActor *)p)->spr;
        DisplaySequence_StepState(drawable, &drawable->sequence);
    }
}

void Golem_ClearControlFlag(struct ChaseActor *actor)
{
    SceneActorChase_ClearWordFlags(&actor->base.statusFlags, 2);
}

struct ActorMotion {
    u8 pad0[0x682];
    s16 speed;
};

u8 *TerrainModifiers_FindCellOverride(s32, s32);

void SceneActor_AccelerateForTerrain(struct ChaseActor *actor)
{
    struct ActorMotion *motion = actor->base.context;
    u16 type = actor->base.terrainCell;
    u8 *entry = TerrainModifiers_FindCellOverride(type, 1);
    s32 maximum, acceleration;
    if (entry)
        entry += 4;
    else
        entry = (type * 4 + 12) + gTerrainMap->cells;
    if (*entry >> 4) {
        switch (actor->base.paletteOffset) {
            case 1:
            case 3:
            case 5:
                maximum = 320;
                acceleration = 11;
                break;
            case 0:
                maximum = 160;
                acceleration = 5;
                break;
            default:
                maximum = 256;
                acceleration = 8;
                break;
        }
    } else {
        switch (actor->base.paletteOffset) {
            case 1:
            case 3:
            case 5:
                maximum = 448;
                acceleration = 22;
                break;
            case 0:
                maximum = 320;
                acceleration = 10;
                break;
            default:
                maximum = 384;
                acceleration = 16;
                break;
        }
    }
    if (motion->speed < maximum)
        motion->speed += acceleration;
    else
        motion->speed = maximum;
}

s32 Callback_InvokeActor(void *, void *);

u8 Golem_InvokeEnabledBehavior(struct ChaseActor *actor)
{
    u8 r;
    if (SceneActorChase_QueryMotionFlags(actor, 0x100))
        r = Callback_InvokeActor(actor, actor->base.targetQuery);
    else
        r = 0;
    return r;
}

struct Seq {
    u8 pad0[8];
    u16 h8;
    u8 padA[4];
    u16 hE;
};

struct AnimationLookupActor {
    u8 pad0[4];
    u8 *inner;
    u8 pad8[0x2D0 - 8];
    u32 flags;
};

extern u16 gUnk_08606120[];

u16 Skeleton_LookupAnimationEntry(struct AnimationLookupActor *p)
{
    struct Seq *s = (struct Seq *)(p->inner + 0x48);
    struct Seq *t = s;

    if (s->hE == 0) {
        u32 m = 0x10000;
        p->flags |= m;
        if (s->h8 <= 7)
            return gUnk_08606120[s->h8];
        if ((u16)(s->h8 - 8) <= 7)
            return gUnk_08606120[t->h8 - 8];
    }
    return 0;
}

struct SkeletonActorClearPendingFlag {
    u8 filler0[0xDC];
    u32 h0DC;
    u8 filler1[0x2A8 - 0xE0];
    u32 f2A8;
    u8 filler2[0x2B5 - 0x2AC];
    u8 f2B5;
    u8 filler3[0x2D4 - 0x2B6];
    u16 flags;
    u8 filler4[0x3D0 - 0x2D6];
    struct SkeletonLink *t;
};

void Skeleton_ClearPendingFlag(struct SkeletonActorClearPendingFlag *s)
{
    u32 mask;

    if (s->f2B5 == 9)
        return;
    mask = 0x100;
    if (mask & s->flags)
        return;
    if ((s->f2A8 & 0x00FFFFFF) == 0x200)
        return;
    {
        u32 *p = &s->t->h628;
        u32 finalMask = 0xFFF7FFFF;
        *p = finalMask & *p;
    }
}

struct H {
    u32 lo : 16;
    u32 hi : 16;
};

struct Tail {
    u32 lo : 16;
    u32 : 16;
};

struct Z {
    struct H a;
    struct Tail b;
};

void SceneActorCollision_InitBody(u8 *, struct Z *, struct Z *, void (*)(void), u8 *);

void SceneActorCollision_InitAttack(u8 *, struct Z *, struct Z *, void (*)(void), u32, u8 *);

void Golem_OnBodyCollision(void);

void Golem_OnAttackCollision(struct ContactCollider *self, struct ContactCollider *other,
                             struct ChaseActor *a);

void Golem_InitColliders(u8 *p)
{
    struct Z s1, s2, s3, s4;
    s1.a.lo = 0x40;
    s1.a.hi = 0x80;
    s1.b.lo = 0x40;
    s2.a.lo = 0;
    s2.a.hi = 0x80;
    s2.b.lo = 0;
    SceneActorCollision_InitBody(p, &s1, &s2, Golem_OnBodyCollision, p);
    s3.a.lo = 0x40;
    s3.a.hi = 0x78;
    s3.b.lo = 0x40;
    s4.a.lo = 0;
    s4.a.hi = 0x80;
    s4.b.lo = 0;
    SceneActorCollision_InitAttack(p, &s3, &s4, Golem_OnAttackCollision, 0, p);
}

void Collider_SetAttackProperties(void *, u32, u32, u32, u32, u32);

s32 Div(s32, s32);

static inline u32 Flag(struct ChaseActor *a, u32 m, u32 off)
{
    return off == 0x2D4 ? *(u16 *)((u8 *)a + off) & m : *(u32 *)((u8 *)a + off) & m;
}

static inline u8 Unavailable(struct ChaseActor *a)
{
    if (a && (Flag(a, 0x40, 0x2D4) || Flag(a, 0x20000, 0x2D0) || a->base.health <= 0))
        return 1;
    return 0;
}

void Golem_UpdateAttackProperties(struct ChaseActor *a)
{
    u8 *state = a->base.context;
    u8 *group = (u8 *)&a->group.common;
    if (Unavailable(a))
        return;
    if (Flag(a, 0xA027, 0x2D4))
        return;
    if (Flag(a, 0x10, 0x2D4)) {
        struct ContactCollider *c = (void *)((u8 *)a + 0x188);
        c->contactFlags = 0x20;
        c->flags = 0x2001;
        {
            u16 power = a->base.attacks.first[2];
            u16 response = a->base.attacks.second[2];
            u32 reaction = a->base.attacks.flags[2] | 0x200004;
            u16 value = a->base.attacks.parameter[2];
            Collider_SetAttackProperties(c, power, response, 0, reaction, value);
        }
        goto active;
    } else if (Flag(a, 0x100, 0x2D4)) {
        s32 speed = *(s16 *)(state + 0x682);
        s32 scale;
        s32 power, response;
        struct ContactCollider *c;
        if (speed >= 0)
            speed >>= 5;
        else
            speed = -(-speed >> 5);
        scale = speed;
        if (scale > 20)
            scale = 20;
        else if (scale < 10)
            scale = 10;
        power = Div(a->base.attacks.first[1] * scale, 10);
        response = Div(a->base.attacks.second[1] * (scale >> 1), 10);
        c = (void *)((u8 *)a + 0x188);
        {
            u32 flags = 0x2301;
            c->flags = flags;
            c->contactFlags = 0;
        }
        {
            u32 reaction = a->base.attacks.flags[1] | 0x40000;
            u16 value = a->base.attacks.parameter[1];
            Collider_SetAttackProperties(c, power, response, 0x400, reaction, value);
        }
        {
            struct Group {
                u32 a, flags;
            };
            struct Group *g = (void *)(group + 0x30);
            {
                u32 mask = 1;
                g->flags = mask | g->flags;
            }
        }
    active: {
        u32 mask = 0x4000;
        a->base.reactionFlags = mask | a->base.reactionFlags;
    }
    } else {
        struct ContactCollider *c = (void *)((u8 *)a + 0x188);
        c->flags = 0x2001;
        c->contactFlags = 0x10;
        {
            u16 power = a->base.attacks.first[0];
            u16 response = a->base.attacks.second[0];
            u32 reaction = a->base.attacks.flags[0];
            u16 value = a->base.attacks.parameter[0];
            Collider_SetAttackProperties(c, power, response, 0, reaction, value);
        }
        return;
    }
}

s32 Math_Atan2(s32, s32);

static inline void RecordContact(struct ChaseActor *a, struct ContactCollider *other)
{
    if (!Unavailable(a) && (other->contactFlags & 15)) {
        a->base.counter304 = other->collisionId;
        {
            u32 *p = &a->base.statusFlags;
            s32 mask = 0xFEFFFFFF;
            *p &= mask;
        }
    }
}

static inline u8 IsFrontal(u8 direction, struct ContactCollider *other)
{
    s32 a, b, delta;
    {
        u8 *owner = other->owner;
        u32 mask = 8;
        if (!(*(u32 *)(owner + 0x20) & mask))
            return 1;
    }
    a = (direction + 0x20) & 255;
    a = a >= 0 ? a >> 6 : -(-a >> 6);
    b = (other->direction + 0x20) & 255;
    b = b >= 0 ? b >> 6 : -(-b >> 6);
    delta = a - b;
    if (delta < 0)
        delta = -delta;
    if (delta == 2)
        return 1;
    return 0;
}

static inline u32 PlayerFlag(u8 *p, u32 mask)
{
    return *(u32 *)(p + 0x20) & mask;
}

static inline u8 Coop(void)
{
    if (gEngineFlags & 0x4800)
        return 1;
    return 0;
}

static inline void AddImpact(u8 *state)
{
    u16 *p = (void *)(state + 0x688);
    *p += 200;
    if (*p > 400)
        *p = 400;
}

static inline void Delta(struct ContactPosition *out, const struct ContactPosition *b,
                         const struct ContactPosition *a)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}

void Golem_OnAttackCollision(struct ContactCollider *self, struct ContactCollider *other,
                             struct ChaseActor *a)
{
    struct ChaseActor *base = a;
    u8 *state = a->base.context;
    union {
        struct ContactPackedVector packed;
        struct ContactPosition pos;
    } origin;
    struct ContactPosition delta;
    if (Unavailable(a))
        return;
    origin.pos = *(struct ContactPosition *)&self->x;
    switch ((s32)((base->base.body.direction + 0x20) & 255) >> 6) {
        case 0:
            origin.packed.x -= 256;
            break;
        case 1:
            origin.packed.z -= 256;
            break;
        case 2:
            origin.packed.x += 256;
            break;
        case 3:
            origin.packed.z += 256;
            break;
    }
    Delta(&delta, &origin.pos, (void *)&other->x);
    self->direction = Math_Atan2(delta.x, delta.z);
    if (other->contactFlags & 15) {
        if (Flag(base, 0x100, 0x2D4)) {
            if (base->base.controlKind == 0x29 || IsFrontal(base->base.body.direction, other)) {
                RecordContact(a, other);
                AddImpact(state);
                {
                    u32 mask = 1;
                    other->responseFlags = mask | other->responseFlags;
                }
                {
                    u16 id = other->collisionId;
                    *(u16 *)(state + 0x634) = id;
                }
            } else {
                u8 *player;
                if (Coop()) {
                    s32 index;
                    if (other->contactFlags & 1)
                        index = 0;
                    else if (other->contactFlags & 2)
                        index = 1;
                    else if (other->contactFlags & 4)
                        index = 2;
                    else
                        index = 3;
                    player = gPlayers[index];
                } else
                    player = gPlayers[0];
                if (!PlayerFlag(player, 0x20000)) {
                    u16 *p = (void *)(state + 0x688);
                    u32 value = 400;
                    *p = value;
                }
            }
        } else {
            RecordContact(a, other);
            AddImpact(state);
        }
    } else
        RecordContact(a, other);
}

void SceneActorReaction_HandleColliderContact(void);

void Golem_OnBodyCollision(void)
{
    SceneActorReaction_HandleColliderContact();
}

void SceneActorAnimation_SelectDirectionalEntry(struct RoomActor *, s32);

void Golem_UpdateIdleState(u8 *p)
{
    if (p[0x2b2] != 0)
        p[0x2b2] = 0;
    SceneActorAnimation_SelectDirectionalEntry(p, 1);
}

void Golem_UpdateIdle(u8 *p)
{
    if (p[0x2b2] != 0)
        p[0x2b2] = 0;
    SceneActorAnimation_SelectDirectionalEntry(p, 0);
}

u32 Route_GetCurrentPointFacing(void *);

static inline u16 GolemTurn_Elapsed(void *sequence)
{
    return ((struct DisplaySequenceState *)sequence)->elapsed;
}

static inline s32 GolemTurn_Half(s32 value)
{
    if (value >= 0)
        return value >> 1;
    return -((-value) >> 1);
}

/* Turn toward the route-facing octant, then restore the waiting controller. */

void Golem_TurnTowardRouteFacing(struct ChaseActor *a)
{
    struct SceneActorChaseGroup *group = &a->group;
    struct SceneActorBody *motion = &a->base.body;
    u16 *motionFlags;
    u8 prepared = SceneActorReaction_TakePrepared(a);
    s32 direction, target, delta;

    motionFlags = &a->base.motionFlags;
    if (prepared) {
        a->base.body.direction = (SceneActorTurn_Cardinal(a->base.body.direction) << 6) - 0x40;
        direction = SceneActorTurn_Cardinal(a->base.body.direction);
        if (a->base.behavior2AA != 6 && !(SceneActorTurn_Flags(motionFlags, 0x480))) {
            target = (((u16)Route_GetCurrentPointFacing(motion->currentRoute) + 0x70) >> 5) & 7;
            a->base.nextDirection = target;
        } else {
            target = a->base.nextDirection;
        }
        if (direction == GolemTurn_Half(target)) {
            a->base.stepDone = 1;
            if (SceneActorTurn_Flags(motionFlags, 0x400)) {
                SceneActorAnimation_SelectDirectionalEntry((void *)a, 0);
                SceneActorChase_SetState(a, 3, 1, 0);
                SceneActorChase_SetControl(a, group->turnControl, 0x1B);
            } else {
                SceneActorAnimation_SelectDirectionalEntry((void *)a, 0);
            }
            return;
        }
        switch (direction) {
            case 0:
                if ((u32)(target - 2) <= 3) {
                    SceneActorTurn_SelectAnimation(a, 2, 1, 0);
                    SceneActorChase_SetActorFlags(a, 0x10000);
                } else {
                    SceneActorTurn_SelectAnimation(a, 2, 0, 4);
                    SceneActorChase_ClearWordFlags(&a->base.behaviorFlags, 0x10000);
                }
                break;
            case 1:
                if ((u32)(target - 4) <= 3) {
                    SceneActorTurn_SelectAnimation(a, 2, 2, 0);
                    SceneActorChase_SetActorFlags(a, 0x10000);
                } else {
                    SceneActorTurn_SelectAnimation(a, 2, 1, 4);
                    SceneActorChase_ClearWordFlags(&a->base.behaviorFlags, 0x10000);
                }
                break;
            case 2:
                if ((u32)(target - 1) > 4) {
                    SceneActorTurn_SelectAnimation(a, 2, 1, 5);
                    SceneActorChase_SetActorFlags(a, 0x10000);
                } else {
                    SceneActorTurn_SelectAnimation(a, 2, 2, 4);
                    SceneActorChase_ClearWordFlags(&a->base.behaviorFlags, 0x10000);
                }
                break;
            case 3:
                if ((u32)target <= 3) {
                    SceneActorTurn_SelectAnimation(a, 2, 0, 0);
                    SceneActorChase_SetActorFlags(a, 0x10000);
                } else {
                    SceneActorTurn_SelectAnimation(a, 2, 1, 1);
                    SceneActorChase_ClearWordFlags(&a->base.behaviorFlags, 0x10000);
                }
                break;
        }
        a->base.restart = 0;
    }
    if (GolemTurn_Elapsed(
            &((struct SceneActorAnimationSequenceDrawable *)a->base.drawable)->sequence) == 0) {
        s32 targetDirection = SceneActorTurn_OctantDirection(a->base.nextDirection);
        s32 difference = motion->direction - targetDirection;
        if (difference < 0)
            difference = -difference;
        if (difference <= 0x16)
            motion->direction = targetDirection;
        else if (SceneActorTurn_ActorFlags(a, 0x10000))
            motion->direction += 0x16;
        else
            motion->direction -= 0x16;
    }
    SceneActorTurn_ApplyDuration(a, motionFlags);
    if (a->base.restart) {
        s32 current = ((a->base.body.direction + 0x70) >> 5) & 7;
        delta = a->base.nextDirection - current;
        if (delta < 0)
            delta = -delta;
        if (delta <= 0) {
            a->base.stepDone = 1;
            a->base.body.direction = (SceneActorTurn_Cardinal(a->base.body.direction) << 6) - 0x40;
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

extern void ActorShadow_Disable(void *);

struct GolemRevivalActor;

void Golem_UpdateRevival(struct GolemRevivalActor *s);

static inline u8 take6030(u8 *flag, u8 **held)
{
    u32 value = *flag;
    *held = flag;
    if (value) {
        *flag = 0;
        return 1;
    }
    return 0;
}

struct Obj6030 {
    u32 flags;
    u8 pad[0x1C];
};

struct Part6030 {
    u8 pad[8];
    u32 flags;
};

void Golem_UpdateRevivalReturn(u8 *s, s32 arg, u32 unused)
{
    u32 queued;
    u8 *flag;
    {
        u32 mask = 1;
        if (s[8] == 0) {
            struct Obj6030 *obj = *(struct Obj6030 **)(s + 4);
            u32 value = obj->flags;
            obj->flags = value | mask;
        } else {
            struct Obj6030 *obj = *(struct Obj6030 **)(s + 4);
            struct Part6030 *part = (struct Part6030 *)((u8 *)obj + 0x20);
            u32 value = part->flags;
            part->flags = value | mask;
        }
    }
    queued = take6030(s + 0x2B2, &flag);
    if (queued) {
        ActorShadow_Disable(s + 0x320);
    } else if (arg > 0x1A) {
        void (*fn)(void);
        u32 code;
        u32 *flags;
        u32 mask;
        SceneActorAnimation_SelectDirectionalEntry(s, 7);
        fn = Golem_UpdateRevival;
        code = 0xD;
        *flag = 1;
        s[0x2B0] = queued;
        s[0x2B5] = code;
        *(void (**)(void))(s + 0x21C) = fn;
        *(u16 *)(s + 0x2C8) = queued;
        flags = (u32 *)(s + 0x114);
        mask = ~1;
        *flags &= mask;
        *(u32 *)(s + 0x104) = queued;
    } else {
        s[0x2B3] = queued;
    }
}

/* Observed in this callback: owner flags at +0 and inner +0x20/+8.
 * The snapshot fields below are copied and reset at the listed offsets. */

typedef struct Inner {
    u8 pad0[8];
    u32 unk8;
} Inner;

typedef struct Owner {
    u32 unk0;
    u8 pad4[0x1c];
    Inner inner;
} Owner;

typedef struct GolemRevivalActor {
    u8 pad0[0x4];
    Owner *unk4;
    u8 unk8;
    u8 pad9[0x50];
    u8 unk59;
    u8 pad5A[0x42];
    u16 unk9C;
    u8 pad9E[0x12];
    s16 unkB0;
    u8 padB2[0x2A];
    u32 unkDC;
    u8 padE0[0x38];
    u32 unk118;
    u32 unk11C;
    u8 unk120;
    u8 unk121;
    u8 unk122;
    u8 unk123;
    u32 unk124;
    u8 unk128;
    u8 pad129[0x3];
    u16 unk12C;
    u16 unk12E;
    u8 pad130[0x48];
    u16 unk178;
    u8 pad17A[0xA2];
    u32 unk21C;
    u8 pad220[0x88];
    u8 unk2A8;
    u8 unk2A9;
    u8 unk2AA;
    u8 pad2AB[0x5];
    u8 unk2B0;
    u8 unk2B1;
    u8 unk2B2;
    u8 unk2B3;
    u8 pad2B4[0x1];
    u8 unk2B5;
    u8 unk2B6;
    u8 pad2B7[0x9];
    u16 unk2C0;
    u16 unk2C2;
    u8 pad2C4[0x8];
    u32 unk2CC;
    u32 unk2D0;
    u16 unk2D4;
    u16 unk2D6;
    u32 unk2D8;
    u32 unk2DC;
    u32 unk2E0;
    u8 pad2E4[0x2];
    u16 unk2E6;
    u8 pad2E8[0x2];
    u16 unk2EA;
    u8 pad2EC[0x18];
    u32 unk304;
    u8 pad308[0x18];
    u8 unk320[0xac];
    u8 unk3CC;
} Obj;

void ActorShadow_Enable(void *);

void Golem_ResetCombatState(u8 *p);

void SceneActorPalette_ClearCompletedTransitions(void);

static inline u8 take_flag(Obj *s)
{
    if (s->unk2B2) {
        s->unk2B2 = 0;
        return 1;
    }
    return 0;
}

/* Keep this byte predicate local; its address is not retained by the body. */

static inline u8 peek_flag(Obj *s)
{
    return s->unk2B3;
}

void Golem_UpdateRevival(struct GolemRevivalActor *s)
{
    s32 n;
    if (take_flag(s)) {
        ActorShadow_Enable(s->unk320);
        if (!s->unk8) {
            u32 *q = &s->unk4->unk0;
            u32 mask = ~1;
            *q = mask & *q;
        } else {
            Inner *q = &s->unk4->inner;
            u32 mask = ~1;
            q->unk8 = mask & q->unk8;
        }
        {
            u16 one = 1;
            s->unk9C = one;
        }
        s->unk178 = 60;
    }
    if (!peek_flag(s))
        return;
    {
        Golem_ResetCombatState(s);
        s->unk2CC = s->unk118;
        s->unk2D0 = s->unk11C;
        s->unk59 = s->unk120;
        s->unk2A8 = s->unk121;
        s->unk2A9 = s->unk122;
        s->unk2AA = s->unk123;
        s->unk21C = s->unk124;
        s->unk2B5 = s->unk128;
        s->unk2C0 = s->unk12E;
        s->unk2EA = s->unk12C;
        n = s->unkB0;
        if (n <= 0)
            n = 0;
        s->unk9C = n;
        s->unk2D6 = 0;
        s->unk2D4 = 0;
        s->unk2B0 = 0;
        s->unk2B1 = 1;
        s->unk2B2 = 1;
        s->unk2B3 = 0;
        s->unk304 = 0;
        s->unk2B6 = 0;
        s->unk2C2 = 64;
        s->unk2D8 = 0;
        s->unk2DC = 0;
        s->unk2E0 = 0;
        s->unk2E6 = 0;
        s->unk3CC = 0;
        s->unkDC = 0;
        SceneActorPalette_ClearCompletedTransitions();
    }
}

s32 Mod(s32, s32);

void SceneActorAttachment_ReplaceResponseSlot(u8 *, s32, s32, u32);

struct S;

void Golem_UpdateDeath(struct S *p, s32 n);

inline static u8 TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return 1;
    }
    return 0;
}

void Golem_UpdateDying(struct ChaseActor *actor, s32 frame)
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
            SceneActorChase_SetControl(actor, Golem_UpdateDeath, 6);
    }
}

static inline u32 orr(u32 k, u16 *a)
{
    return k | *a;
}

struct S {
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

void Golem_UpdateDeath(struct S *p, s32 n)
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

void Skeleton_ClearDirectionalEntryFlag(struct RoomActor *s)
{
    struct Seq *seq = (struct Seq *)((u8 *)s->drawable + 0x48);

    if (TakeFlag(&s->field2B2)) {
        SceneActorAnimation_SelectDirectionalEntry(s, 0xE);
        s->restart = 0;
    }
    if (seq->h8 == 1)
        seq->hE = 0;
}

void Golem_UpdateChargeBuildup(struct ChaseActor *actor, s32 frame);

void Golem_BeginChargeBuildup(struct ChaseActor *actor)
{
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectDirectionalEntry(actor, 0xE);
        {
            u32 mask = 0x100;
            actor->base.motionFlags = mask | actor->base.motionFlags;
        }
    }
    if (actor->base.restart)
        Chase_SetCtl(actor, Golem_UpdateChargeBuildup, 0x22);
}

struct RoomActor;

void SceneActorPlacement_PlayVariantEventSound(struct RoomActor *, u32);

struct GolemWaitContext {
    u8 unknown[0x628];
    u32 flags;
};

static inline u8 TestBody(struct GolemWaitContext *body, u32 mask)
{
    return Test(body->flags, mask);
}

static inline u32 Random(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return ((u16 *)RANDOM_TABLE_ADDR)[gRandomIndex];
}

static inline s32 WaitTime(struct GolemWaitContext *context, u32 mask, s32 shift, s32 base)
{
    u32 bits = mask;
    u32 *flags;
    u8 tested;
    u32 masked;
    u32 *clear;
    flags = &context->flags;
    masked = *flags & bits;
    clear = flags;
    tested = Test(masked, ~0);
    if (tested) {
        *clear &= ~mask;
        return 0;
    }
    return ((Random() & 3) << shift) + base;
}

// confidence=low: ramps speed and waits for a species-dependent windup duration.

void Golem_UpdateChargeBuildup(struct ChaseActor *actor, s32 frame)
{
    u8 *body = actor->base.context;
    if (TakeFlag(&actor->base.field2B2)) {
        struct GolemWaitContext *context = actor->base.context;
        s32 duration;
        switch (actor->base.paletteOffset) {
            case 1:
            case 3:
            case 5:
                duration = WaitTime(context, 0x800, 2, 30);
                break;
            case 0:
                duration = WaitTime(context, 0x800, 4, 50);
                break;
            default:
                duration = 0;
                break;
        }
        *(u16 *)(body + 0x68A) = duration;
    }
    {
        u8 directionVariant, mirror;
        Golem_GetDirectionVariant(actor, &directionVariant, &mirror);
        Golem_SelectAnimation(actor, 8, directionVariant, Golem_MirrorFlags(mirror));
    }
    Golem_SetMotionFlags(actor, 0x100);
    if (*(s16 *)(body + 0x682) <= 0x17F)
        *(s16 *)(body + 0x682) += 16;
    actor->base.field2C2 = 0x40 >> (*(s16 *)(body + 0x682) >> 7);
    Golem_ApplyDuration(actor);
    if (frame > *(u16 *)(body + 0x68A)) {
        actor->base.stepDone = 1;
        SceneActorPlacement_PlayVariantEventSound((void *)actor, 4);
    }
}

// confidence=low: neighboring Golem controls establish the state role.

// confidence=low: return-home behaviors use this control while moving to a cell center.

void Golem_UpdateRouteMotion(struct ChaseActor *actor, u32 frame)
{
    u8 *body = actor->base.context;
    struct SceneActorBody *motion = &actor->base.body;
    if (TakeFlag(&actor->base.field2B2))
        actor->base.restart = 0;
    Golem_SetMotionFlags(actor, 0x100);
    {
        u8 directionVariant, mirror;
        Golem_GetDirectionVariant(actor, &directionVariant, &mirror);
        Golem_SelectAnimation(actor, 8, directionVariant, Golem_MirrorFlags(mirror));
    }
    actor->base.field2C2 = 0x40 >> (*(s16 *)(body + 0x682) >> 7);
    Golem_ApplyDuration(actor);
    if (!(frame & 3)) {
        struct ContactPosition velocity, size;
        velocity.x = -motion->motion.x;
        velocity.y = 0;
        velocity.z = -motion->motion.z;
        size.x = 32;
        size.y = 0;
        size.z = 32;
        SceneParticleBurst_CreateWithPause(&motion->position, &velocity, &size, 1, 3, 4);
    }
}

// confidence=low: existing Golem state interpretation retained.

// confidence=low: neighboring Golem controls establish the state role.

#define A(a) ((struct RoomActor *)(a))

// confidence=low: charge and slide behaviors install this completion control.

void Golem_UpdateChargeStop(struct ChaseActor *actor)
{
    if (TakeFlag(&actor->base.field2B2)) {
        u8 directionVariant, mirror;
        Golem_GetDirectionVariant(actor, &directionVariant, &mirror);
        Golem_SelectAnimation(actor, 7, directionVariant, mirror | 4);
        actor->base.restart = 0;
    }
    if (actor->base.restart) {
        {
            u16 *field = &actor->base.motionFlags;
            u32 mask = ~0x100;
            u32 value = *field;
            mask &= value;
            *field = mask;
        }
        actor->base.stepDone = 1;
    }
}

s32 Golem_AlignChargeTarget(u8 *actor);

/* Decelerate, approach the current cell center, then restore route motion. */

void Golem_UpdateChargeRecovery(struct ChaseActor *actor)
{
    struct GolemChargeContext *context = actor->base.context;
    struct SceneActorBody *body = &actor->base.body;
    u8 directionVariant, mirror;
    if (SceneActorReaction_TakePrepared(actor)) {
        actor->base.stepDone = 0;
        actor->base.restart = 0;
        context->charge.phase = 0;
        {
            u32 angle = (context->charge.savedDirection + 0x60) >> 6;
            u32 quadrant = 3;
            quadrant &= angle;
            context->charge.quadrant = quadrant;
        }
        context->charge.stopPosition = body->position;
        Golem_CenterPositionInCell(&context->charge.stopPosition);
    }
    Golem_SetMotionFlags(actor, 0x100);
    Golem_GetDirectionVariant(actor, &directionVariant, &mirror);
    Golem_SelectAnimation(actor, 8, directionVariant, Golem_MirrorFlags(mirror));
    if (context->charge.phase == 0) {
        s16 *speed = &context->charge.speed;
        *speed -= 0x28;
        if (*speed < 0) {
            context->charge.phase = 1;
            *speed = 0;
            return;
        }
        {
            struct ContactPackedVector step;
            union SceneActorRegistryPackedPosition candidate;
            s32 cell;
            step.x =
                (*speed * gUnk_086149C4[(context->charge.motionDirection + 0x40) & 0xFF]) >> 16;
            step.z = (*speed * gUnk_086149C4[context->charge.motionDirection]) >> 16;
            candidate.position = actor->base.body.position;
            candidate.packed.x += step.x;
            candidate.packed.z += step.z;
            cell = SceneActorTerrain_GetCellIndex(&candidate.position);
            if (actor->base.terrainCell != cell) {
                context->charge.speed = 0;
                context->charge.phase = 1;
            } else {
                actor->base.body.motion.x += step.x;
                actor->base.body.motion.z += step.z;
            }
        }
    } else if (context->charge.phase == 1) {
        struct ContactPosition delta, center;
        SceneActorTerrain_GetCenter(actor->base.terrainCell, &center);
        SceneActorTerrain_Subtract(&delta, &center, &actor->base.body.position);
        switch (context->charge.motionDirection >> 6) {
            case 0:
            case 2:
                delta.z = 0;
                if (delta.x > 8)
                    body->motion.x = 8;
                else if (delta.x < -8)
                    body->motion.x = -8;
                else
                    body->motion.x = delta.x;
                break;
            case 1:
            case 3:
                delta.x = 0;
                if (delta.z > 8)
                    body->motion.z = 8;
                else if (delta.z < -8)
                    body->motion.z = -8;
                else
                    body->motion.z = delta.z;
                break;
        }
        if (delta.x + delta.z == 0) {
            context->charge.phase = 2;
            context->charge.settleFrames = 0;
            return;
        }
    } else {
        u8 *counter = &context->charge.settleFrames;
        u32 value = *counter + 1;
        u32 zero = 0;
        *counter = value;
        if ((u8)value > 15) {
            context->charge.stopPosition = context->charge.routePosition;
            Golem_AlignChargeTarget(context);
            context->charge.savedDirection = context->charge.routeDirection;
            actor->base.body.direction = context->charge.routeDirection;
            actor->base.stepDone = 1;
            *counter = zero;
            context->charge.speed = zero;
            return;
        }
    }
    actor->base.field2C2 = 0x40 >> (context->charge.durationSpeed >> 7);
    Golem_ApplyDuration(actor);
}

static inline s32 Scale8(s16 n)
{
    return n >= 0 ? n >> 8 : -((-n) >> 8);
}

/* Charge windup: reset the pending-hit count, emit dust, and ramp the stored speed. */

// confidence=low: charge controls 080CDF14 and 080CEDF0 install this preparatory state.

void Golem_UpdateChargeWindup(struct ChaseActor *actor, u32 frame)
{
    struct GolemChargeContext *context = actor->base.context;
    struct SceneActorBody *body = &actor->base.body;
    u8 directionVariant, mirror;
    s16 *speed;
    if (SceneActorReaction_TakePrepared(actor)) {
        actor->base.restart = 0;
        context->pendingHits = 0;
    }
    Golem_GetDirectionVariant(actor, &directionVariant, &mirror);
    Golem_SelectAnimation(actor, 8, directionVariant, Golem_MirrorFlags(mirror));
    {
        u32 mask = 0x20;
        mask |= actor->base.reactionFlags;
        actor->base.reactionFlags = mask;
    }
    Golem_SetMotionFlags(actor, 0x100);
    if ((frame & 3) == 0) {
        struct ContactPosition velocity, size;
        u32 origin, savedOrigin, direction, count;
        PARTICLE_RANDOM_CAPTURE_ORIGIN(origin);
        direction = ParticleRandom_ReadDirectSample((const u16 *)RANDOM_TABLE_ADDR) & 0xFF;
        velocity.x = Scale8(gUnk_086149C4[(direction + 0x40) & 0xFF]);
        velocity.y = 0;
        PARTICLE_RANDOM_SAVE_ORIGIN(savedOrigin, origin);
        velocity.z = Scale8(gUnk_086149C4[(direction + 0x40) & 0xFF]);
        size.x = 48;
        size.y = 0;
        size.z = 48;
        count =
            (ParticleRandom_ReadRelativeSample(savedOrigin, (const u16 *)RANDOM_TABLE_ADDR) & 1) +
            2;
        SceneParticleBurst_CreateWithPause(&body->position, &velocity, &size, 2, count, 4);
    }
    speed = &context->charge.speed;
    if (*speed <= 383)
        *speed += 16;
    actor->base.field2C2 = 0x40 >> (*speed >> 7);
    Golem_ApplyDuration(actor);
}

u32 ActorMode_SelectOddStateVariant(struct ChaseActor *);

void sub_08014634(u32, u32, struct SceneExtent *, struct SceneExtent *, struct SceneExtent *, u32,
                  u32);

struct GolemSlideContext {
    u8 unknown0[0x634];
    u16 recovery;
    u8 unknown636[0x4A];
    u16 state;
    s16 speed;
    u8 unknown684[0x28];
    u16 impactDelay, colliderDelay;
};

static inline u32 GolemSlide_Random(const u16 *table)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return table[gRandomIndex];
}

static inline s32 GolemSlide_Quarter(s16 value)
{
    if (value >= 0)
        return value >> 2;
    return -((-value) >> 2);
}

static inline void GolemSlide_SetMotionFlags(struct ChaseActor *actor, u16 mask)
{
    actor->base.motionFlags = mask | actor->base.motionFlags;
}

static inline void GolemSlide_ClearColliderFlags(struct ContactCollider *collider, u16 bits)
{
    u32 mask = ~bits;
    collider->flags = mask & collider->flags;
}

static inline void GolemSlide_SetDelay(struct ChaseActor *actor, u16 delay)
{
    actor->base.attachmentTimer = delay;
}

static inline void GolemSlide_SetStatusFlags(struct ChaseActor *actor, u32 mask)
{
    actor->base.statusFlags |= mask;
}

void Golem_UpdateSlideImpact(struct ChaseActor *actor, u32 frame)
{
    struct GolemSlideContext *context = actor->base.context;
    struct SceneActorChaseGroup *group = &actor->group;
    struct SceneActorBody *body = &actor->base.body;
    if (SceneActorReaction_TakePrepared(actor))
        SceneActorPlacement_PlayVariantEventSound((void *)actor, 4);
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 15);
    SceneActorChase_SetReactionFlags(actor, 0x20);
    GolemSlide_SetMotionFlags(actor, 0x100);
    SceneActorChase_SetActorFlags(actor, 0x2000);
    actor->base.body.motion.x +=
        (context->speed * gUnk_086149C4[(body->direction + 64) & 255]) >> 16;
    actor->base.body.motion.z += (context->speed * gUnk_086149C4[body->direction]) >> 16;
    if ((frame & 3) == 0) {
        struct ContactPosition velocity, spread;
        velocity.x = -body->motion.x;
        velocity.y = 0;
        velocity.z = -body->motion.z;
        spread.x = 32;
        spread.y = 0;
        spread.z = 32;
        SceneParticleBurst_CreateWithPause(&body->position, &velocity, &spread, 1, 3, 4);
    }
    if (actor->base.unknown9[0x40 - 9] & 1) {
        struct SceneExtent velocity, spread;
        u32 count;
        u32 randomOrigin;
        context->impactDelay = 30;
        context->colliderDelay = 20;
        velocity.x = GolemSlide_Quarter(body->motion.z);
        /* The recovered origin cancels when a later sample address becomes a table offset. */
        PARTICLE_RANDOM_CAPTURE_ORIGIN(randomOrigin);
        velocity.y = GolemSlide_Random((const u16 *)RANDOM_TABLE_ADDR) & 7;
        velocity.z = GolemSlide_Quarter(body->motion.x);
        spread.x = (ParticleRandom_ReadRelativeSample(randomOrigin, gRandomTable) & 3) + 15;
        spread.y = (ParticleRandom_ReadRelativeSample(randomOrigin, gRandomTable) & 7) + 15;
        spread.z = (ParticleRandom_ReadRelativeSample(randomOrigin, gRandomTable) & 3) + 15;
        count = (ParticleRandom_ReadRelativeSample(randomOrigin, gRandomTable) & 3) + 4;
        sub_08014634(count, ActorMode_SelectOddStateVariant(actor), (void *)&body->position,
                     &velocity, &spread, 45, 31);
        SceneActorPlacement_PlayVariantEventSound((void *)actor, 5);
        if (context->recovery) {
            actor->base.stepDone = 1;
        } else {
            GolemSlide_SetDelay(actor, 180);
            GolemSlide_SetStatusFlags(actor, 1);
            SceneActorChase_SetState(actor, 3, 0, 0);
            SceneActorChase_SetControl(actor, group->contactControl, 7);
            GolemSlide_ClearColliderFlags(&actor->base.colliderA, 0x100);
        }
    }
}

// confidence=low: neighboring Golem controls establish the state role.

/* Decelerate recoil, converge on the saved position, then signal completion. */

// confidence=low: inferred from the three movement phases and neighboring Golem controls.

struct Unk080C75B8Motion {
    u8 unknown0[0xC];
    struct ContactPosition position;
    s16 x, unknown16, z;
};

void Golem_UpdateRecoil(struct ChaseActor *actor)
{
    u8 *body = actor->base.context;
    struct Unk080C75B8Motion *motion = (struct Unk080C75B8Motion *)((u8 *)actor + 0x50);
    s16 *speed;

    if (TakeFlag(&actor->base.field2B2)) {
        actor->base.restart = 0;
        body[0x68D] = 0;
        {
            u32 angle = (body[0x68E] + 0x60) >> 6;
            u32 quadrant = 3;
            quadrant &= angle;
            body[0x68C] = quadrant;
        }
        *(struct ContactPosition *)(body + 0x69C) = motion->position;
        Golem_CenterPositionInCell((struct ContactPosition *)(body + 0x69C));
    }
    Golem_SetMotionFlags(actor, 0x100);
    {
        u32 mask = 0x2000;
        actor->base.behaviorFlags |= mask;
    }
    {
        u8 directionVariant, mirror;
        Golem_GetDirectionVariant(actor, &directionVariant, &mirror);
        Golem_SelectAnimation(actor, 8, directionVariant, Golem_MirrorFlags(mirror));
    }
    if (body[0x68D] == 0) {
        switch (actor->base.paletteOffset) {
            case 1:
            case 3:
            case 5:
                *(s16 *)(body + 0x682) -= 0xC0;
                break;
            case 0:
                *(s16 *)(body + 0x682) -= 0x40;
                break;
            default:
                *(s16 *)(body + 0x682) -= 0x80;
                break;
        }
        speed = (s16 *)(body + 0x682);
        if (*speed < 0) {
            u8 *phase = body + 0x68D;
            u32 zero = 0;
            *phase = 1;
            *speed = zero;
            return;
        }
        actor->base.body.motion.x += (*speed * gUnk_086149C4[(body[0x68E] + 0x40) & 0xFF]) >> 16;
        actor->base.body.motion.z += (*speed * gUnk_086149C4[body[0x68E]]) >> 16;
    } else if (body[0x68D] == 1) {
        struct ContactPosition delta;
        Delta(&delta, &motion->position, (struct ContactPosition *)(body + 0x69C));
        switch (body[0x691] >> 6) {
            case 0:
            case 2:
                delta.z = 0;
                if (delta.x > 8)
                    motion->x = 8;
                else if (delta.x < -8)
                    motion->x = -8;
                else
                    motion->x = delta.x;
                break;
            case 1:
            case 3:
                delta.x = 0;
                if (delta.z > 8)
                    motion->z = 8;
                else if (delta.z < -8)
                    motion->z = -8;
                else
                    motion->z = delta.z;
                break;
        }
        if (delta.x + delta.z == 0) {
            body[0x68D] = 2;
            body[0x68F] = 0;
            return;
        }
    } else {
        u8 *counter = body + 0x68F;
        u32 value = *counter + 1;
        u32 zero = 0;
        *counter = value;
        if ((u8)value > 7) {
            actor->base.stepDone = 1;
            *counter = zero;
            *(u16 *)(body + 0x682) = zero;
            return;
        }
    }
    actor->base.field2C2 = 0x40 >> (*(u16 *)(body + 0x684) >> 7);
    Golem_ApplyDuration(actor);
}

/* SceneActorReaction_SlideBack: on entry plays sequence 8 facing the context direction (+0x691); slides along the dominant axis of that direction at the context speed (+0x682), decaying it by 0x10 per frame, and flags completion (+0x2B0) 12 frames after it stops (provisional name) */

extern const u16 gUnk_08606130[];

struct Unk080C79A8Motion {
    u8 unknown0[0x14];
    s16 x, unknown16, z;
};

static inline s32 Scale16(s32 n)
{
    return n >= 0 ? n >> 16 : -((-n) >> 16);
}

void SceneActorReaction_SlideBack(struct ChaseActor *actor)
{
    u8 *body = actor->base.context;
    struct Unk080C79A8Motion *motion = (struct Unk080C79A8Motion *)((u8 *)actor + 0x50);
    s32 dx, dz;
    s16 *speed;

    if (TakeFlag(&actor->base.field2B2)) {
        u8 *direction = &actor->base.body.direction;
        u8 saved = *direction;
        u8 directionVariant, mirror;
        u8 *mirrorPointer;
        u8 *variantPointer;
        u32 flags, variant, selection;
        u8 *variantReload;
        u32 force;
        u8 *restart;

        *direction = body[0x691];
        variantPointer = &directionVariant;
        {
            u8 *mirrorAddress = &mirror;
            s32 quadrant = (((s32)*direction + 0x60) >> 6) & 3;
            mirrorPointer = mirrorAddress;
            if (quadrant > 1) {
                *mirrorPointer = 1;
                *variantPointer = 3 - quadrant;
            } else {
                *mirrorPointer = 0;
                *variantPointer = quadrant;
            }
        }
        *(&actor->base.body.direction) = saved;
        variantReload = &directionVariant;
        {
            u32 mirrorValue = *mirrorPointer;
            flags = gUnk_08606130[mirrorValue];
            selection = 8;
            variant = *variantReload;
            flags |= mirrorValue;
        }
        if (flags & 0x10) {
            force = 0x10;
            flags &= ~0x10;
        } else {
            force = 0;
        }
        if (!actor->base.drawableKind) {
            struct SceneActorAnimationSequenceDrawable *drawable;
            actor->base.animationIndex = selection;
            drawable = actor->base.drawable;
            Anim_SetSequence(&drawable->sequence, (void *)drawable->resource, (u16)selection,
                             (u8)variant, (u8)flags);
            restart = &actor->base.restart;
        } else {
            struct SceneInputWidget *tile =
                &((struct SceneActorAnimationTileDrawable *)actor->base.drawable)->tile;
            u8 *savedSelection;

            selection += variant;
            savedSelection = &actor->base.animationIndex;
            restart = &actor->base.restart;
            if (!force && *savedSelection == selection && !*restart) {
                if ((flags == 1) && (tile->flags & 4)) {
                    goto done;
                }
                if ((flags == 0) && (!(tile->flags & 4))) {
                    goto done;
                }
            }
            *savedSelection = selection;
            {
                u32 mask = 4;
                u32 mode;
                if (flags & mask) {
                    mode = 6;
                } else {
                    mode = 4;
                }
                Anim_SetKeyframes(
                    &((struct SceneActorAnimationTileDrawable *)(&actor->base)->drawable)->tile,
                    actor->base.drawable, (u16)selection, 2, (u16)mode);
                flags &= 1;
                if (flags) {
                    tile->flags |= mask;
                } else {
                    tile->flags &= ~4;
                }
            }
        }
        *restart = 0;
    done:
        actor->base.restart = 0;
        body[0x68F] = 0;
    }
    {
        u32 mask = 0x100;
        actor->base.motionFlags |= mask;
    }
    {
        u32 mask = 0x2000;
        actor->base.behaviorFlags |= mask;
    }
    dx = Scale16(gUnk_086149C4[(body[0x691] + 0x40) & 0xFF] * *(s16 *)(body + 0x682));
    dz = Scale16(gUnk_086149C4[body[0x691]] * *(s16 *)(body + 0x682));
    if (((dx < 0) ? (-dx) : (dx)) < ((dz < 0) ? (-dz) : (dz))) {
        motion->z = dz;
    } else {
        motion->x = dx;
    }
    speed = (s16 *)(body + 0x682);
    if (*speed <= 0) {
        s16 zero = 0;
        *speed = zero;
        if (++body[0x68F] > 11) {
            actor->base.stepDone = 1;
            *speed = zero;
        }
    } else {
        *speed -= 0x10;
    }
}

/* Sibling of Bat_ResumeSavedTarget (Bat_ResumeSavedTarget): after 60 frames it either resumes group
 * +0x48 (kind 0x1A, flag 0x80 set) or goes idle (kind 0x10) and routes to group +0x58;
 * before that it rewinds a mode-2 animation. */

struct BatRoutePoint {
    u8 unknown0[0x1C];
    s16 x;
    u8 unknown1E[2];
    s16 z;
};

struct BatAnim {
    u8 unknown0[7];
    u8 loop;
    u16 mode;
    u8 unknownA[4];
    u16 frame;
};

struct BatDrawable {
    u8 unknown0[0x48];
    struct BatAnim anim;
};

void SceneActor_SetPositionXZ(void *, s32, s32);

static inline u32 Bat_MaskFlags(struct ChaseActor *actor, u32 mask)
{
    return actor->base.motionFlags & mask;
}

static inline void Bat_SetControl(struct ChaseActor *actor, u32 kind, void *control)
{
    actor->base.field2B2 = 1;
    actor->base.stepDone = 0;
    actor->base.controlKind = kind;
    actor->base.target = control;
    actor->base.field2C8 = 0;
    {
        u32 *flags = &actor->base.pendingControlFlags;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->base.pendingControl = 0;
}

void SceneActorRoute_SetReachableDestination(struct ChaseActor *actor,
                                             struct ContactPosition *destination);

void Golem_ResumeSavedTarget(struct ChaseActor *actor, s32 frame)
{
    u8 *group = (u8 *)&actor->group.common;
    struct BatAnim *anim = &((struct BatDrawable *)actor->base.drawable)->anim;
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        actor->base.restart = 0;
    }
    if (frame > 60) {
        u16 blocked;
        if (!actor->base.restart)
            return;
        actor->base.stepDone = 1;
        blocked = Bat_MaskFlags(actor, 128);
        if (blocked) {
            void *target;
            SceneActor_SetPositionXZ(&actor->base.body.id,
                                     (*(struct BatRoutePoint **)(group + 0x1FC))->x,
                                     (*(struct BatRoutePoint **)(group + 0x1FC))->z);
            SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
            target = *(void **)(group + 0x48);
            Bat_SetControl(actor, 0x1A, target);
            {
                u32 behavior = 12;
                actor->base.state2A8 = 0;
                actor->base.phase2A9 = 0;
                actor->base.behavior2AA = behavior;
                actor->base.counter2AC = 0;
                actor->base.field2B1 = 1;
            }
            {
                u16 *flags = (u16 *)(group + 0x40);
                u16 mask = 4;
                *flags = mask | *flags;
            }
            actor->base.field2C2 = 0x20;
        } else {
            SceneActor_SetPositionXZ(&actor->base.body.id,
                                     (*(struct BatRoutePoint **)(group + 0x1FC))->x,
                                     (*(struct BatRoutePoint **)(group + 0x1FC))->z);
            SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
            {
                void *target = SceneActorControl_OnIdle;
                Bat_SetControl(actor, 0x10, target);
            }
            {
                u32 phase = 2;
                actor->base.state2A8 = 0;
                actor->base.phase2A9 = phase;
                actor->base.behavior2AA = 0;
                actor->base.counter2AC = 0;
                actor->base.field2B1 = 1;
            }

            actor->base.behaviorFlags |= 16;
            SceneActorRoute_SetReachableDestination(actor,
                                                    (struct ContactPosition *)(group + 0x58));
        }
    } else {
        if (anim->mode == 2) {
            u16 f = anim->frame;
            u8 loop = anim->loop;
            if (f == (loop != 0) && loop != 1)
                anim->frame = 0;
        }
    }
}

static inline void Golem_SnapDelayedRoute(struct ContactPosition *position,
                                          struct SceneActorRegistryRoute *route)
{
    position->x = route->point->x;
    position->z = route->point->z;
    position->y = SceneActorChase_HeightAt(SceneActorChase_CellAt(position), position);
}

static inline void Golem_BeginDelayedTurn(void *obj, u32 saved, const u16 *randomSamples)
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

static inline u8 Golem_ArriveAfterDelay(void *obj)
{
    struct ContactPosition target;
    struct ChaseActor *actor = obj;
    struct SceneActorChaseGroup *group = SceneActorChase_GroupOf(obj);
    struct SceneActorChaseTracking *tracking = &actor->base.tracking;
    s32 cell, player;
    const u16 *randomSamples = gRandomTable;
    void **callback;
    Golem_SnapDelayedRoute(group->destination, &group->homeRoute);
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
        Golem_SnapDelayedRoute(group->destination, &group->homeRoute);
        (group->destination)->y = SceneActorChase_HeightAt(
            SceneActorChase_CellAt(group->destination), group->destination);
        (group->destination)->pad = 0;
    }
    {
        u8 saved = actor->base.body.direction;
        Golem_BeginDelayedTurn(obj, saved, randomSamples);
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

void Golem_UpdateReactionRecovery(struct ChaseActor *actor, s32 frame)
{
    struct DisplaySequenceState *sequence =
        &((struct SceneActorControlMotionDrawable *)actor->base.drawable)->sequence;
    struct SceneActorChaseGroup *group = SceneActorChase_GroupOf(actor);
    if (SceneActorReaction_TakePrepared(actor))
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 3);
    SceneActorChase_SetReactionFlags(actor, 0x10);
    if (frame <= 19 && !DisplaySequence_GetIndex(sequence) &&
        sequence->elapsed == (sequence->duration != 0) && sequence->duration != 1)
        sequence->elapsed = 0;
    if (!actor->base.restart)
        return;
    actor->base.stepDone = 1;
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    SceneActorChase_SetState(actor, 3, 1, 0);
    SceneActorChase_SetControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
    if (SceneActorChase_MotionFlags(actor, 0x80)) {
        if (Golem_ArriveAfterDelay(actor))
            SceneActorDelay_Prepare((void *)&group->saved, group->common.gate.control, 12, 30);
    } else {
        SceneActorDelay_Prepare((void *)&group->saved, actor->base.onAlternateEntry, 7, 40);
        SceneActorChase_SetActorFlags(actor, 0x10);
        actor->base.field2C2 = 64;
        Golem_SnapDelayedRoute(group->destination, &group->homeRoute);
        if (!SceneActorChase_TryRoute(actor, group->route, &actor->base.body.position,
                                      group->destination)) {
            SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0);
            SceneActorChase_SetState(actor, 3, 0, 0);
        } else
            group->destination->pad = 1;
    }
}

// confidence=low: existing Golem state interpretation retained.

// confidence=low: Golem initialization installs this displacement recovery callback.

/* The Golem's chase step looks cells up from scalar axes, reads the map width at full width and
 * saves the control directly; otherwise it is SceneActorChase_Arrive. */

static inline s32 Golem_CellIndex(s32 x, s32 z)
{
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return SceneActorChase_MapRowStart(SceneActorChase_CurrentMap(), z) + x;
}

static inline s32 Golem_CellAt(struct ContactPosition *position)
{
    return Golem_CellIndex(position->x >> 8, position->z >> 8);
}

static inline u32 Golem_Height(struct ContactPosition *position)
{
    return SceneActorChase_HeightAt(Golem_CellAt(position), position);
}

static inline void Golem_SnapToRoute(struct ContactPosition *position,
                                     struct SceneActorRegistryRoute *route)
{
    position->x = route->point->x;
    position->z = route->point->z;
    position->y = Golem_Height(position);
}

static inline s32 Golem_MapWidth(struct EngineMap *map)
{
    return ((struct SceneActorChaseTerrainGrid *)map->cells)->width;
}

static inline void Golem_CellCenter(s32 cell, struct ContactPosition *position)
{
    s32 x = Mod(cell, Golem_MapWidth(SceneActorChase_CurrentMap()));
    s32 z = Div(cell, Golem_MapWidth(SceneActorChase_CurrentMap()));
    position->x = SceneActorChase_CellCenterCoordinate(x);
    {
        u32 attributes = SceneActorChase_CellProperties(cell)->attributes;
        u32 height = 15;
        height &= attributes;
        position->y = SceneActorChase_TerrainLevelHeight(height);
    }
    position->z = SceneActorChase_CellCenterCoordinate(z);
}

static inline void Golem_SaveControl(struct SceneActorChaseSavedControl *packet, void *object)
{
    struct ChaseActor *actor = object;
    packet->control = actor->base.target;
    packet->kind = actor->base.controlKind;
    packet->state = actor->base.state2A8;
    packet->phase = actor->base.phase2A9;
    packet->behavior = actor->base.behavior2AA;
}

static inline void Golem_BeginTurn(void *obj, u32 saved, const u16 *randomSamples)
{
    struct ChaseActor *actor = obj;
    struct SceneActorChaseGroup *group = SceneActorChase_GroupOf(obj);
    if (!SceneActorChase_MotionFlags(actor, 0x400) &&
        !SceneActorChase_TestField((u16 *)&group->common.gate.flags, 4)) {
        group->turn.direction =
            SceneActorChase_RandomDirectionExcept(actor->base.body.direction, randomSamples);
        Golem_SaveControl(&group->saved, actor);
        group->turn.savedDirection = saved;
        SceneActorChase_SetControl(actor, group->turnControl, 0x1B);
        SceneActorChase_SetState(actor, 3, 1, 0);
        group->turn.turnsRemaining = 6;
    }
}

/* A successful cell-center reroute returns one. Other paths leave the
 * caller to try the separate partner-position route. */

static inline u8 Golem_Arrive(void *obj)
{
    struct ContactPosition target;
    struct ChaseActor *actor = obj;
    struct SceneActorChaseGroup *group = SceneActorChase_GroupOf(obj);
    struct SceneActorChaseTracking *tracking = &actor->base.tracking;
    s32 cell, player;
    const u16 *randomSamples = gRandomTable;
    void **callback;
    Golem_SnapToRoute(group->destination, &group->homeRoute);
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
        cell = Golem_CellIndex(t->position.x >> 8, t->position.z >> 8);
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
        Golem_SnapToRoute(group->destination, &group->homeRoute);
        (group->destination)->y =
            SceneActorChase_HeightAt(Golem_CellAt(group->destination), group->destination);
        (group->destination)->pad = 0;
    }
    {
        u8 saved = actor->base.body.direction;
        Golem_BeginTurn(obj, saved, randomSamples);
    }
    SceneActorTerrain_RequestRetreatRoute(obj);
    actor->base.field2C2 = 0x20;
    return 0;
reroute: {
    Golem_CellCenter(cell, &target);
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

static inline s32 Golem_ReadDesiredX(const struct ChaseActor *a)
{
    return a->base.reactionStart.x;
}

static inline s32 Golem_ReadDesiredZ(const struct ChaseActor *a)
{
    return a->base.reactionStart.z;
}

static inline s32 Golem_ScaleMagnitude(s32 value, u32 shift)
{
    return value >> shift;
}

static inline s32 Golem_QuarterDisplacement(const s32 *value)
{
    if (*value >= 0)
        return Golem_ScaleMagnitude(*value, 2);
    return -Golem_ScaleMagnitude(-*value, 2);
}

static inline u8 Golem_DisplacementStopped(s32 x, s32 z)
{
    if (!x && !z)
        return 1;
    return 0;
}

static inline s32 Golem_ReadDisplacementAxis(s32 desired, s16 *current, s16 **field)
{
    s32 difference = desired - *current;
    *field = current;
    return Golem_QuarterDisplacement(&difference);
}

static inline s32 Golem_ClampAxis(s32 value, s32 limit)
{
    if (value > limit)
        return limit;
    if (value < -limit)
        return -limit;
    return value;
}

/* Ease the displacement toward the saved reaction position; true once both axes are still. */

static inline u8 Golem_Drift(struct ChaseActor *actor)
{
    s16 *x, *z;
    s32 dx, dz;
    if (SceneActorReaction_TakePrepared(actor)) {
        u16 *flags;
        u32 mask;
        SceneActorAnimation_SelectDirectionalEntry(&actor->base, 3);
        if (actor->base.hitAttributes & 0x400)
            Sound_PlaySong(0x18B);
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
    dx = Golem_ReadDisplacementAxis(Golem_ReadDesiredX(actor), &actor->base.reactionEnd.x, &x);
    dz = Golem_ReadDisplacementAxis(Golem_ReadDesiredZ(actor), &actor->base.reactionEnd.z, &z);
    dx = Golem_ClampAxis(dx, 200);
    dz = Golem_ClampAxis(dz, 200);
    actor->base.body.motion.x += dx;
    actor->base.body.motion.z += dz;
    *x += dx;
    *z += dz;
    return Golem_DisplacementStopped(dx, dz);
}

/* Converge on the saved motion target; once at rest either hand control to the
 * contact callback, or (flag 0x80) chase the player / walk the route (tentative). */

void Golem_ResumeBehaviorAfterDisplacement(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    if (!Golem_Drift(actor))
        goto moving;
    if (!actor->base.restart)
        goto moving;
    actor->base.stepDone = 1;
    SceneActorChase_ClearMotionFlagComponent(actor, 0x20);
    if (actor->base.hitReactionFlags & 0x80000) {
        SceneActorChase_SetControl(actor, group->contactControl, 7);
        SceneActorChase_SetState(actor, 3, 0, 0);
        return;
    }
    if (SceneActorChase_MotionFlags(actor, 0x80)) {
        SceneActorChase_SetActorFlags(actor, 0x80);
        SceneActorChase_SetFlagComponent((struct SceneActorChaseFlags16 *)&group->common.gate.flags,
                                         4);
        SceneActorChase_TrackPlayer(actor);
        if (!Golem_Arrive(actor)) {
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
        SceneActorChase_SetActorFlags(actor, 0x90);
        group->destination->pad = 0;
        if (!Golem_Arrive(actor)) {
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
    return;
moving:
    SceneActorChase_SetActorFlags(actor, 0x2000);
}
