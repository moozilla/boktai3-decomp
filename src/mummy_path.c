#include "global.h"
#include "scene_actor_terrain.h"
#include "player.h"
#include "scene_actor_reaction.h"
#include "scene_actor_straight_route.h"
#include "mummy.h"
#include "scene_actor_reset.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_contacts.h"
#include "scene_actor_chase_shared.h"
#include "area_effect_link.h"
#include "render.h"
#include "runtime.h"
#include "scene_actor_adjacent_cell.h"
#include "engine.h"
#include "scene_actor_control_phase.h"
#include "ram.h"
#include "scene_actor_control_motion.h"
#include "scene_actor_animation.h"
#include "display_sequence_inline.h"
#include "scene_actor_registry.h"

/* SceneActorPath_FaceAndCheckStraightPath: faces the actor toward its target and checks that the grid cells along the row/column from its cell to the goal cell are passable (SceneActorTerrain_CanTraverseCell); restores the facing on failure (provisional) */

u8 SceneActorTerrain_CanTraverseCell(s32, s32, s32, u32);

s32 Math_Atan2(s32, s32);

struct Unk080D6DF8Goal {
    u8 unknown0[6];
    u16 cell;
};

struct Unk080D6DF8Target {
    u8 unknown0[0xC];
    struct ContactPosition position;
    u8 unknown14[8];
    struct Unk080D6DF8Goal *goal;
};

struct Unk080D6DF8Actor {
    u8 unknown0[0x59];
    u8 direction;
    u8 unknown5A[2];
    struct ContactPosition position;
    u8 unknown64[0x78];
    struct Unk080D6DF8Target *target;
    u8 unknownE0[0x214];
    u16 terrainCell;
};

struct Unk080D6DF8Delta {
    s16 x, y, z;
};

static inline s32 GridWidth(void)
{
    return *(s16 *)(gTerrainMap->cells + 4);
}

static inline s32 GridHeight(void)
{
    return *(s16 *)(gTerrainMap->cells + 6);
}

static inline u8 CanReach(s32 cell, s32 goal, s32 y, u32 kind, s32 total)
{
    s32 i, col;
    if (cell < goal) {
        i = cell + 1;
        for (col = Mod(i, GridWidth());
             col < GridWidth() && SceneActorTerrain_CanTraverseCell(cell, i, y, kind); i++, col++) {
            if (i == goal)
                return 1;
        }
        for (i = cell + GridWidth();
             total > i && SceneActorTerrain_CanTraverseCell(cell, i, y, kind); i += GridWidth()) {
            if (i == goal)
                return 1;
        }
    } else {
        i = cell - 1;
        for (col = Mod(i, GridWidth());
             col >= 0 && SceneActorTerrain_CanTraverseCell(cell, i, y, kind); i--, col--) {
            if (i == goal)
                return 1;
        }
        for (i = cell - GridWidth(); i >= 0 && SceneActorTerrain_CanTraverseCell(cell, i, y, kind);
             i -= GridWidth()) {
            if (i == goal)
                return 1;
        }
    }
    return 0;
}

static inline u8 FindPath_FaceAndCheckStraightPath(struct Unk080D6DF8Actor *actor, s32 goal)
{
    s32 cell = actor->terrainCell;
    s32 y = actor->position.y;
    u32 kind = *SceneActorTerrain_GetCell(cell) >> 4;
    s32 total = GridWidth() * GridHeight();
    return CanReach(cell, goal, y, kind, total);
}

u8 SceneActorPath_FaceAndCheckStraightPath(struct Unk080D6DF8Actor *actor)
{
    struct Unk080D6DF8Target *target = actor->target;
    s32 goal = target->goal->cell;
    u8 saved = actor->direction;
    struct Unk080D6DF8Delta delta;
    u8 result;

    {
        struct Unk080D6DF8Delta *d = &delta;
        struct ContactPosition *from = &actor->position;
        struct ContactPosition *to = &target->position;
        d->x = to->x - from->x;
        d->y = to->y - from->y;
        d->z = to->z - from->z;
    }
    actor->direction = Math_Atan2(delta.x, delta.z);
    if (goal == actor->terrainCell)
        result = 1;
    else if (!FindPath_FaceAndCheckStraightPath(actor, goal)) {
        actor->direction = saved;
        result = 0;
    } else
        result = 1;
    return result;
}

bool32 Mummy_IsPlayerFlag8Set(void)
{
    u8 *player = gPlayers[0];
    u32 mask = 8;
    if (*(u32 *)(player + 0x20) & mask)
        return TRUE;
    return FALSE;
}

#define FIELD(p, off, T) (*(T *)((u8 *)(p) + (off)))

static inline void SetControl_RequestStraightRoute(struct ChaseActor *actor)
{
    void *callback = SceneActorControl_OnIdle;
    u8 kind = 0;
    actor->base.field2B2 = 1;
    actor->base.stepDone = 0;
    actor->base.controlKind = kind;
    actor->base.target = callback;
    actor->base.field2C8 = 0;
    {
        u32 *field = &actor->base.pendingControlFlags;
        u32 mask = ~1;
        *field &= mask;
    }
    actor->base.pendingControl = 0;
}

static inline void SetAnimation(struct ChaseActor *actor, u8 state, u8 phase, u8 behavior)
{
    actor->base.state2A8 = state;
    actor->base.phase2A9 = phase;
    actor->base.behavior2AA = behavior;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}

// confidence=low: scans to the last open cell in the current facing and starts a route.

u8 Mummy_RequestStraightRoute(struct ChaseActor *actor)
{
    u8 *state = actor->base.context;
    u8 *nav = (u8 *)actor + 0x3D4;
    s32 dest;
    s32 dir;
    struct ContactPosition pos;
    u32 result;

    if (FIELD(state, 0x6D2, u16))
        return 0;
    dir = ((actor->base.body.direction + 0x20) >> 6) & 3;
    dest = SceneActorTerrain_LastOpenCell(actor, dir);
    if (dest == actor->base.terrainCell)
        return 0;
    SceneActorTerrain_GetCenter(dest, &pos);
    pos.y = actor->base.body.position.y;
    result = SceneActorTerrain_RequestPath(actor, FIELD(nav, 0x1FC, void *),
                                           &actor->base.body.position, &pos);
    if (!result) {
        SetControl_RequestStraightRoute(actor);
        SetAnimation(actor, 3, 0, 0);
        return 0;
    }
    FIELD(state, 0x6D2, u16) = 0xF0;
    return 1;
}

struct T {
    u8 filler[0x6d0];
    u16 h6d0;
    u8 f2[0x6d8 - 0x6d2];
    u32 v6d8;
};

struct SCanPassMovementThreshold {
    u8 filler[0xf0];
    u32 v;
    u8 f2[0x3d0 - 0xf4];
    struct T *t;
};

s32 Mummy_CanPassMovementThreshold(struct SCanPassMovementThreshold *s)
{
    struct T *t = s->t;
    if (s->v >= t->v6d8 && t->h6d0 != 0)
        return 0;
    return 1;
}

void Mummy_EnterIdleControl(struct ChaseActor *actor);

void SceneActorControl_OnIdle(u8 *);

static inline u8 MotionStarted(struct SceneActorReactionMotion *motion)
{
    u32 mask = 8;
    if (motion->flags & mask)
        return 1;
    return 0;
}

void SceneActor_EnterMotionPhase(struct ChaseActor *actor)
{
    u32 *flags = &actor->base.statusFlags;
    s32 mask = -5;
    struct Mummy *parent;
    u32 started;
    *flags &= mask;
    parent = actor->base.context;
    {
        u16 *p = &parent->flags6F2;
        u32 mask = 0xFFFFEBFF;
        *p = mask & *p;
    }
    started = MotionStarted((void *)&actor->base.tracking.target);
    if (started) {
        u16 *motionFlags = &actor->group.common.gate.flags;
        {
            u32 mask = 4;
            u32 value = *motionFlags;
            mask |= value;
            *motionFlags = mask;
        }
        {
            u32 mask = 0x80;
            motionFlags = &actor->base.motionFlags;
            {
                u32 value = *motionFlags;
                mask |= value;
                *motionFlags = mask;
            }
        }
        {
            u32 mask;
            actor->group.common.gate.counter = actor->group.common.gate.duration;
            mask = 0x100;
            if (*motionFlags & mask)
                return;
        }
        Mummy_EnterIdleControl(actor);
    } else {
        u32 one = 1;
        actor->base.state2A8 = 0;
        actor->base.phase2A9 = one;
        actor->base.behavior2AA = 0;
        actor->base.counter2AC = 0;
        actor->base.field2B1 = one;
        {
            void *control = SceneActorControl_OnIdle;
            u32 kind = 0x10;
            actor->base.field2B2 = one;
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
    }
    {
        struct SceneActorReactionMotion *motion = (void *)&actor->base.tracking.target;
        u32 mask = 8;
        {
            u32 value = motion->flags;
            mask |= value;
            motion->flags = mask;
        }
    }
}

u8 Mummy_RequestStraightRoute(struct ChaseActor *actor);

struct SOnMovementUpdate {
    u8 filler[0x2d4];
    u16 f;
};

void Mummy_OnMovementUpdate(struct SOnMovementUpdate *s)
{
    u32 m = 0x100;
    if (!(s->f & m))
        Mummy_RequestStraightRoute(s);
}

struct MummyContactTarget {
    struct SceneActorChaseTarget target;
    u8 unknown14[8];
    struct SceneActorChasePlayerCell *cell;
};

s32 Mummy_CanPassMovementThreshold(struct SCanPassMovementThreshold *s);

u8 Mummy_TryDirectPursuit(struct ChaseActor *actor);

void SceneActorTarget_ResetContactResponse(void *);

void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);

static inline u8 Mummy_TestContext(struct Mummy *context, u32 mask)
{
    if (context->flags6F2 & mask)
        return 1;
    return 0;
}

static inline u8 Mummy_TestContextOwned(struct Mummy *context, u32 mask,
                                        struct SceneActorChaseFlags16 **field)
{
    struct SceneActorChaseFlags16 *flags = (void *)&context->flags6F2;
    u32 value = flags->value;
    value &= mask;
    *field = flags;
    if (value)
        return 1;
    return 0;
}

static inline u32 Mummy_MotionFlags_TryEngageTarget(struct ChaseActor *actor, u32 mask)
{
    return actor->base.motionFlags & mask;
}

static inline void Mummy_SetTrackingFlags(struct SceneActorChaseTracking *tracking, u8 bits)
{
    tracking->flags |= bits;
}

static inline u32 Mummy_StatusFlags(struct ChaseActor *actor, u32 mask)
{
    return actor->base.statusFlags & mask;
}

static inline u32 Mummy_ActorFlags_TryEngageTarget(struct ChaseActor *actor, u32 mask)
{
    return actor->base.behaviorFlags & mask;
}

static inline void Mummy_SetBehavior(struct ChaseActor *actor, u32 behavior)
{
    actor->base.state2A8 = 0;
    actor->base.phase2A9 = 0;
    actor->base.behavior2AA = behavior;
    SceneActorChase_ResetStateEntry(actor);
}

static inline u8 Mummy_Arrive(void *obj, struct SceneActorChaseTracking *tracking)
{
    struct ContactPosition target;
    struct ChaseActor *actor = obj;
    struct SceneActorChaseGroup *group = SceneActorChase_GroupOf(obj);
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
            Callback_InvokeActor(actor, group->fallback);
            return 0;
        }
        cell = ((struct SceneActorChasePlayer *)gUnknown_02000580[player])->cell->index;
    } else {
        struct SceneActorChaseTarget *t = tracking->target;
        cell = SceneActorChase_CellAt(&t->position);
        player = 0;
    }
    if (cell != actor->base.terrainCell &&
        !SceneActorChase_Bits32(&((struct SceneActorChasePlayer *)gUnknown_02000580[player])->flags,
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
        Mummy_SetBehavior(actor, 12);
    } else {
        SceneActorChase_SetControl(actor, actor->base.onAlternateEntry, 1);
        Mummy_SetBehavior(actor, 5);
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
    SceneActorChase_CellCenter(cell, &target);
    if (SceneActorChase_TryRoute(obj, group->route, &actor->base.body.position, &target))
        goto success;
}
    callback = &group->fallback;
    Callback_InvokeActor(actor, *callback);
    return 0;
success:
    return 1;
}

static inline struct SceneActorChaseTarget **
Mummy_BeginContactTracking(struct SceneActorChaseTracking *tracking, u8 bits)
{
    tracking->flags |= bits;
    return &tracking->target;
}

u8 Mummy_TryEngageTarget(struct ChaseActor *actor)
{
    struct Mummy *context = actor->base.context;
    struct SceneActorChaseGroup *group = &actor->group;
    struct SceneActorChaseTarget **target;
    struct SceneActorContactNode *contact;
    u16 cell;
    struct SceneActorChaseFlags16 *contextFlags;
    if (Mummy_MotionFlags_TryEngageTarget(actor, 0x10))
        return 0;
    if ((u8)Callback_InvokeActor(actor, actor->base.targetQuery))
        return 1;
    if (Mummy_MotionFlags_TryEngageTarget(actor, 0x100))
        return 0;
    if (!Mummy_MotionFlags_TryEngageTarget(actor, 0x200))
        return 0;
    if (!Mummy_StatusFlags(actor, 0x1000000))
        return 0;
    if (group->common.control.counter != gTrackedPlayerId[0])
        return 0;
    target = Mummy_BeginContactTracking(&actor->base.tracking, 2);
    if (!actor->base.tracking.limit)
        goto restore;
    contact = SceneActorContacts_SelectForActor((void *)actor, 2);
    if (!contact)
        goto restore;
    (*target) = contact->object;
    cell = ((struct MummyContactTarget *)(*target))->cell->index;
    if (*(u32 *)actor->base.unknown100) {
        if (SceneActorChase_TestField(&actor->base.placementFlags, 2))
            (*target) = &((struct SceneActorChasePlayer *)gUnknown_02000580[0])->target;
        else if (!((*target)))
            goto threshold;
        {
            s32 x = ((*target))->position.x - actor->base.body.position.x;
            s32 z = ((*target))->position.z - actor->base.body.position.z;
            actor->base.tracking.heading = Math_Atan2(x, z);
            actor->base.tracking.distance = (x * x) + (z * z);
        }
    threshold:
        if (!((u8)Mummy_CanPassMovementThreshold(actor))) {
            if (Mummy_TestContext(context, 0x1400))
                return 0;
            Mummy_Arrive(actor, (void *)target);
            return 0;
        }

        if (!Mummy_TryDirectPursuit(actor))
            return 0;
        if (SceneActorChase_TrackingFlags((void *)target, 8)) {
            if ((actor->base.savedTargetCell) != cell) {
                SceneActorChase_TryRoute(actor, group->route, &actor->base.body.position,
                                         &((*target))->position);
                actor->base.savedTargetCell = cell;
            }
        } else
            SceneActorTarget_ResetContactResponse(actor);
        SceneActorChase_SetReactionFlags(actor, 0x80);
        Mummy_SetTrackingFlags((void *)target, 0x10);
        return 1;
    }
    if (Mummy_ActorFlags_TryEngageTarget(actor, 0x80) && (!Mummy_MotionFlags_TryEngageTarget(actor, 0x400)))
        goto restore;
    if (Mummy_TestContext(context, 0x1400))
        goto restore;
    if (actor->base.field2C2 != 64)
        return 0;
    if (group->common.control.counter != gTrackedPlayerId[0])
        return 0;
    SceneActorChase_SetReactionFlags(actor, 0x100);
restore:
    if (Mummy_TestContextOwned(context, 0x4000, &contextFlags)) {
        SceneActorAttachment_ReplaceResponseSlot(actor, 0, 1, actor->base.category);
        SceneActorChase_ClearFlagComponent(contextFlags, 0x4000);
    }

    return 0;
}

struct H {
    u32 lo : 16;
    u32 hi : 16;
};

struct Z {
    struct H a, b;
};

void SceneActorCollision_InitBody(u8 *, struct Z *, struct Z *, void (*)(void), u8 *);

void SceneActorCollision_InitAttack(u8 *, struct Z *, struct Z *, void (*)(void), u32, u8 *);

void Mummy_OnBodyCollision(void);

void Mummy_OnAttackCollision(void);

void Mummy_InitCollision(u8 *p)
{
    struct Z s1, s2, s3, s4;
    s1.a.lo = 0x20;
    s1.a.hi = 0x80;
    s1.b.lo = 0x20;
    *(u32 *)&s2.a = 0x800000;
    *(u32 *)&s2.b &= 0xFFFF0000;
    SceneActorCollision_InitBody(p, &s1, &s2, Mummy_OnBodyCollision, p);
    s3.a.lo = 0x20;
    s3.a.hi = 0x64;
    s3.b.lo = 0x20;
    *(u32 *)&s4.a = 0x800000;
    *(u32 *)&s4.b &= 0xFFFF0000;
    SceneActorCollision_InitAttack(p, &s3, &s4, Mummy_OnAttackCollision, 0, p);
}

s32 SceneActorProfiles_FindOrAdd(u32, u32);

struct SInitProfile {
    u8 filler[0x308];
    s32 v;
};

void Mummy_InitProfile(struct SInitProfile *s)
{
    s->v = SceneActorProfiles_FindOrAdd(1, 0);
}

// confidence=low: selects attack bounds and properties from mummy movement flags.

void Collider_SetAttackProperties(struct ContactCollider *, u32, u32, u32, u32, u32);

static inline u8 Unavailable(struct ChaseActor *actor)
{

    u32 mask;
    if (!actor)
        goto no;
    mask = 0x40;
    if (actor->base.motionFlags & mask)
        goto yes;
    mask = 0x20000;
    if (actor->base.statusFlags & mask)
        goto yes;
    if (actor->base.health > 0)
        goto no;
yes:
    return 1;
no:
    return 0;
}

struct BoundsBits {
    u32 x : 16;
    u32 y : 16;
    u32 z : 16;
    u32 pad : 16;
};

static inline void ApplyAttack(struct ChaseActor *actor, struct ContactCollider *collider,
                               u32 alternate, u32 mask)
{

    u32 damage;
    u32 element;
    u32 response;
    u32 flags;
    if (alternate) {
        damage = actor->base.attacks.first[1];
        element = actor->base.attacks.second[1];
        flags = actor->base.attacks.flags[1];
        {
            u32 extra = mask;
            flags |= extra;
        }
        response = actor->base.attacks.parameter[1];
    } else {
        damage = actor->base.attacks.first[0];
        element = actor->base.attacks.second[0];
        flags = actor->base.attacks.flags[0];
        response = actor->base.attacks.parameter[0];
    }
    Collider_SetAttackProperties(collider, damage, element, 0, flags, response);
}

void Mummy_UpdateAttackCollider(struct ChaseActor *actor)
{
    struct BoundsBits bounds;

    u32 flags;
    u32 attackBit;

    if (Unavailable(actor))
        return;
    if (actor->base.colliderA.responseValue)
        return;
    {
        u32 mask = 0x8000;
        flags = actor->base.motionFlags;
        if (mask & flags)
            return;
    }
    attackBit = 16;
    if (flags & attackBit) {
        struct ContactCollider *collider;

        bounds.x = 64;
        bounds.y = 100;
        bounds.z = 64;
        collider = &actor->base.colliderB;
        collider->bounds = *(struct ContactPosition *)&bounds;
        collider->contactFlags = 32;
        collider->flags = 0x2201;
        ApplyAttack(actor, collider, 1, 0x200004);
        SceneActorChase_SetReactionFlags(actor, 0x4000);
    } else if (flags & 0x100) {
        struct ContactCollider *collider;
        ((struct ContactPosition *)&bounds)->x = 32;
        ((struct ContactPosition *)&bounds)->y = 100;
        ((struct ContactPosition *)&bounds)->z = 32;
        collider = &actor->base.colliderB;
        collider->bounds = *(struct ContactPosition *)&bounds;
        collider->contactFlags = attackBit;
        collider->flags = 0x2201;
        ApplyAttack(actor, collider, 1, 0x20000);
    } else {
        struct ContactCollider *collider;
        ((struct ContactPosition *)&bounds)->x = 32;
        ((struct ContactPosition *)&bounds)->y = 100;
        ((struct ContactPosition *)&bounds)->z = 32;
        collider = &actor->base.colliderB;
        collider->bounds = *(struct ContactPosition *)&bounds;
        collider->contactFlags = attackBit;
        collider->flags = 0x2201;
        ApplyAttack(actor, collider, 0, 0);
    }
}

struct TUpdateMovementTimers {
    u8 filler[0x6d0];
    u16 h6d0;
    u8 f2[0x6d6 - 0x6d2];
    u16 h6d6;
};

struct SUpdateMovementTimers {
    u8 filler[0x2d4];
    u16 flags;
    u8 f2[0x3d0 - 0x2d6];
    struct TUpdateMovementTimers *t;
};

void Mummy_UpdateMovementTimers(struct SUpdateMovementTimers *s)
{
    struct TUpdateMovementTimers *t = s->t;
    u32 m = 0x40;
    if (!(s->flags & m)) {
        u16 *p = &t->h6d0;
        if (*p != 0)
            *p = *p - 1;
        p = &t->h6d6;
        if (*p != 0)
            *p = *p - 1;
    }
}

void SceneActorRecord_RecordPair(u16, u16);

void Player_NotifyFirstSelectedSlot(u32, u32);

void Player_RefreshLifeState(u32, u32);

void sub_08159164(u32);

void SceneActorEffects_AdvanceSteps(void *, u32);

void Sound_PlaySong(u32);

void AnimData_SelectPaletteBank(void *, s32);

void SceneHud_SetTimedState(u32, s32, s32, s32, s32, s32);

void CpuSet(const void *, void *, u32);

void DamageNumberDisplay_Show(void *, s32, u32, void *, void *, u32);

struct BurnState {
    u8 unknown0[0xAA];
    u8 intensity;
    u8 unknownAB[0x1E4 - 0xAB];
    u16 durations[4];
    u16 count, active, timer;
};

#define STATS ((u8 *)gGameState)

#define H(p, n) (*(s16 *)((p) + (n)))

struct GroupLink {
    u32 unknown0;
    u32 flags;
};

static inline s32 clamp(s32 v)
{
    if (v > 9999)
        v = 9999;
    else if (v < 0)
        v = 0;
    return v;
}

/* Saturating defeat counters in the game-state block. */

#define STAT_INC(n) Count(&H(STATS, n))

#define STAT_ADD(n) H(STATS, n) = clamp(H(STATS, n) + 1)

#define KILL_KIND(flags)                                                                           \
    {                                                                                              \
        if ((flags) & 0x80)                                                                        \
            STAT_ADD(0x58A);                                                                       \
        else if ((flags) & 0x100)                                                                  \
            STAT_ADD(0x58C);                                                                       \
        else if ((flags) & 0x200)                                                                  \
            STAT_ADD(0x58E);                                                                       \
        else if ((flags) & 0x400)                                                                  \
            STAT_ADD(0x590);                                                                       \
        else if ((flags) &= 0x800)                                                                 \
            STAT_ADD(0x592);                                                                       \
    }

#define CASE_BODY(n)                                                                               \
    {                                                                                              \
        u32 flags = *(u32 *)(hit + 0x38);                                                          \
        STAT_INC(n);                                                                               \
        KILL_KIND(flags);                                                                          \
    }

static inline void Count(s16 *p)
{
    if (*p < 9999)
        *p = *p + 1;
    else
        *p = 9999;
}

static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}

static inline u32 Mask(u16 *flags, u32 maskInput)
{
    u32 mask = maskInput;
    return *flags & mask;
}

static inline u8 Test2(u16 **out, struct ChaseActor *actor, u32 maskInput)
{
    u32 mask = maskInput;
    u16 *flags = &actor->base.placementFlags;
    u32 value = *flags & mask;
    *out = flags;
    if (value)
        return 1;
    return 0;
}

static inline u32 Flag2D0(struct ChaseActor *actor, u32 maskInput)
{
    u32 mask = maskInput;
    return actor->base.statusFlags & mask;
}

static inline void Reaction_SetControl(struct ChaseActor *actor, u32 kind, void *control)
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

static inline u8 Blocked(struct ChaseActor *s)
{
    if (!s)
        goto zero;
    {
        u32 mask = 0x40;
        if (s->base.motionFlags & mask)
            goto one;
    }
    {
        u32 mask = 0x20000;
        if (s->base.statusFlags & mask)
            goto one;
    }
    if (s->base.health > 0)
        goto zero;
one:
    return 1;
zero:
    return 0;
}

static inline s32 LocalPlayer(void)
{
    if (!gLinkSession)
        return -1;
    return gLinkSession->localPlayer;
}

static inline u8 ShouldShowDamage(struct ChaseActor *actor)
{
    if (Test(gEngineFlags, 0x4800)) {
        u32 mask = actor->base.hitOwners;
        if (!(mask & 0x20) && mask) {
            switch (LocalPlayer()) {
                case 0:
                    if (actor->base.hitOwners != 1)
                        return 0;
                    break;
                case 1:
                    if (actor->base.hitOwners != 2)
                        return 0;
                    break;
                case 2:
                    if (actor->base.hitOwners != 4)
                        return 0;
                    break;
                case 3:
                    if (actor->base.hitOwners != 8)
                        return 0;
                    break;
            }
        }
    }
    return 1;
}

static inline void Fill(u32 *zero, void *dst)
{
    *zero = 0;
    CpuSet(zero, dst, 0x05000002);
}

static inline s32 CurrentHealth(struct ChaseActor *actor)
{
    s16 *health = (void *)&actor->base.health;
    return *health;
}

static inline void OrStatus(u16 status, u16 *flags)
{
    *flags |= status;
}

// confidence=low: timer-driven damage and defeat recording; the status is tentatively burn.


void Mummy_UpdateBurnDamage(struct ChaseActor *actor)
{
    struct BurnState *burn = (void *)&actor->group.common;
    u16 *flags2C0;
    u8 *category;
    u8 *collider;
    u8 *hit;
    u32 kind;
    s32 i;
    u32 *damageFlags;
    if (Blocked(actor))
        return;
    if (!burn->timer)
        return;
    if (actor->base.controlKind == 0x15) {
        {
            u32 n = 0x20;
            actor->base.paletteTimer = n;
            actor->base.motionFlags &= n - 0x31;
        }
        actor->base.field2C2 = 0x40;
        SceneActorAttachment_ReplaceResponseSlot(actor, 0, 127, 0);
        SceneActorAttachment_ReplaceResponseSlot(actor, 1, 127, 0);
        SceneActorAttachment_ReplaceResponseSlot(actor, 3, 127, 0);
        SceneActorEffects_AdvanceSteps(actor, 4);
        burn->timer = 0;
        return;
    }
    if (burn->intensity > 2) {
        if (Mod(burn->timer, 16) == 0) {
            Sound_PlaySong(0x121);
            OrStatus(0x10, &actor->base.reactionFlags);
        }
    } else {
        if (Mod(burn->timer, 48) == 0) {
            Sound_PlaySong(0x121);
            OrStatus(0x20, &actor->base.reactionFlags);
        }
    }
    if (burn->count) {
        for (i = 0; i < burn->count; i++) {
            u32 value = burn->durations[i];
            if (!value) {
                SceneActorEffects_AdvanceSteps(actor, 1);
                burn->durations[i] = value;
                burn->active &= ~(1 << i);
                burn->count--;
            }
            burn->durations[i]--;
        }
    }
    burn->timer--;
    if (Mod(burn->timer, 40) == 0) {
        {
            struct ContactCollider *initial = &actor->base.colliderA;
            u32 *flags;
            u32 masked;
            {
                u32 cost = 1 << burn->intensity;
                initial->cost = cost;
                actor->base.reactionSoundVariant = cost;
            }
            {
                u32 amount = initial->cost;
                u16 *health;
                health = (void *)&actor->base.health;
                *health -= amount;
            }
            flags = &actor->base.hitAttributes;
            masked = *flags & 0x20000;
            collider = (u8 *)initial;
            damageFlags = flags;
            if (!masked) {
                u16 bank = actor->base.paletteAlternateBank;
                {
                    struct ChaseActor *p = actor;
                    if (p->base.drawableKind == 0)
                        AnimData_SelectPaletteBank((u8 *)p->base.drawable + 0x2C, bank);
                    else {
                        struct ChaseActor *q = actor;
                        *(u8 **)((u8 *)q->base.drawable + 0x68) = gPaletteBankData + bank * 32;
                    }
                }
                {
                    u32 n = 8;
                    actor->base.flashTimer = n;
                }
                {
                    u32 bit = 0x8000;
                    actor->base.behaviorFlags |= bit;
                }
                if (ShouldShowDamage(actor)) {
                    u32 zero;
                    u32 buffer[2];
                    s32 health = CurrentHealth(actor);
                    u32 max = (u16)actor->base.defaultHealth;
                    s32 type = (s16)actor->base.hudKind;
                    s32 param = (s16)actor->base.hudLabel;
                    SceneHud_SetTimedState(actor->base.body.id, health, max, type, 0, param);
                    Fill(&zero, buffer);
                    if (actor->base.reactionSoundVariant || !(*damageFlags & 0x20000))
                        DamageNumberDisplay_Show((struct ContactCollider *)collider,
                                                 actor->base.reactionSoundVariant, 0,
                                                 &actor->base.body.position, buffer, 1);
                }
            }
        }
    }
    if (actor->base.health > 0)
        return;
    {
        u32 state = 3;
        actor->base.state2A8 = state;
    }
    actor->base.phase2A9 = 0;
    actor->base.behavior2AA = 0;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
    {
        void *control = *(void **)((u8 *)actor + 0x234);
        Reaction_SetControl(actor, 17, control);
    }
    if (Test(gEngineFlags, 0x4800)) {
        Player_NotifyFirstSelectedSlot(actor->base.hitOwners, actor->base.hitAttributes);
        goto done;
    }
    {
        u8 *address = &actor->base.category;
        u32 owner;
        kind = *address;
        owner = actor->base.hitOwners & 1;
        collider = (u8 *)&actor->base.colliderA;
        category = address;
        flags2C0 = &actor->base.placementFlags;
        if (!owner)
            goto record;
    }
    {
        u8 blocked = Test(Mask(flags2C0, 0x80), ~0);
        collider = (u8 *)&actor->base.colliderA;
        if (blocked)
            goto record;
    }
    {
        if (kind != 5 && kind != 7 && kind != 6) {
            STAT_INC(0x87C);
            STAT_INC(0x538);
            {
                u32 offset = 0x134;
                u8 *pinned;
                pinned = (u8 *)actor + offset;
                hit = pinned;
            }
            {
                u32 index = *category - 1; /* category 1..25 */
                collider = hit;
                switch (index) {
                    case 3:
                        CASE_BODY(0X53A);
                        break;
                    case 0:
                        CASE_BODY(0X53C);
                        break;
                    case 1:
                        CASE_BODY(0X53E);
                        break;
                    case 2:
                        CASE_BODY(0X540);
                        break;
                    case 13:
                        CASE_BODY(0X54C);
                        break;
                    case 23:
                        CASE_BODY(0X55C);
                        break;
                    case 9:
                        CASE_BODY(0X546);
                        break;
                    case 4:
                    case 5:
                    case 6:
                        CASE_BODY(0X542);
                        break;
                    case 20:
                        CASE_BODY(0X554);
                        break;
                    case 17:
                        CASE_BODY(0X550);
                        break;
                    case 18:
                        CASE_BODY(0X552);
                        break;
                    case 16:
                        CASE_BODY(0X54E);
                        break;
                    case 21:
                        CASE_BODY(0X556);
                        break;
                    case 19:
                        CASE_BODY(0X558);
                        break;
                    case 8:
                        CASE_BODY(0X544);
                        break;
                    case 22:
                        CASE_BODY(0X55A);
                        break;
                    case 11:
                    case 12:
                        CASE_BODY(0X54A);
                        break;
                    case 24:
                        CASE_BODY(0X560);
                        break;
                }
            }
        }
        sub_08159164(actor->base.experience);
        Player_RefreshLifeState(actor->base.hitAttributes, *((u8 *)actor + 0xA1));
    }

record:
    if (!Test(Mask(flags2C0, 0x80), ~0) && !Test(gEngineFlags, 0x4800) && !Flag2D0(actor, 0x800))
        SceneActorRecord_RecordPair(gScriptEntryProcedure, actor->base.body.id);
    ((struct ContactCollider *)collider)->flags |= 4;
done:
    burn->timer = 0;
    actor->base.hitLockTimer = 0;
}

s32 Mod(s32, s32);

extern const u16 gUnk_086061B8[];

struct Q {
    u8 filler[8];
    u16 h8;
    u8 f2[4];
    u16 hE;
};

struct R {
    u8 f[4];
    u8 *p;
    u8 f2[0x2c2 - 8];
    u16 h2c2;
    u8 f3[0x2d0 - 0x2c4];
    u32 flags;
};

s32 Mummy_GetAnimationStepOffset(struct R *s)
{
    struct Q *q = (struct Q *)(s->p + 0x48);
    if (q->hE == 0) {
        s32 m = Mod(q->h8, 4);
        u32 c = 0x10000;
        s->flags |= c;
        if (m != 0) {
            u16 h = s->h2c2;
            s32 k = 1;
            if (h <= 0x1f)
                k = 2;
            return gUnk_086061B8[m] * k;
        }
    }
    return 0;
}

struct TUpdateMovementCooldown {
    u8 filler[0x6d2];
    u16 h6d2;
};

struct SUpdateMovementCooldown {
    u8 filler[0x2d4];
    u16 flags;
    u8 f2[0x3d0 - 0x2d6];
    struct TUpdateMovementCooldown *t;
};

void Mummy_UpdateMovementCooldown(struct SUpdateMovementCooldown *s)
{
    struct TUpdateMovementCooldown *t = s->t;
    u32 m = 0x40;
    if (!(s->flags & m)) {
        u16 *p = &t->h6d2;
        if (*p != 0)
            *p = *p - 1;
    }
}

// confidence=low: retains the current cell unless its requested neighbour passes the height and flag checks.

static inline s32 SceneActor_NextRowCell(const u16 *cell)
{
    s32 width = SceneActorTerrain_Width();
    return *cell + width;
}

static inline s32 SceneActor_PreviousRowCell(s32 cell)
{
    return cell - SceneActorTerrain_Width();
}

s32 SceneActor_FindDirectionalCell(struct ChaseActor *actor, s32 direction)
{
    s32 result = actor->base.terrainCell;
    switch (direction) {
        case 0:
            if (SceneActor_IsAdjacentCellOpen(actor, result + 1))
                result = actor->base.terrainCell + 1;
            break;

        case 1:
            if (SceneActor_IsAdjacentCellOpen(actor, result + SceneActorTerrain_Width()))
                result = SceneActor_NextRowCell(&actor->base.terrainCell);
            break;

        case 2:
            if (SceneActor_IsAdjacentCellOpen(actor, result - 1))
                result = actor->base.terrainCell - 1;
            break;

        case 3:
            if (SceneActor_IsAdjacentCellOpen(actor, result - SceneActorTerrain_Width()))
                result = SceneActor_PreviousRowCell(actor->base.terrainCell);
            break;
    }

    return result;
}

#define gCurrentTerrainMap gTerrainMap

s32 Div(s32, s32);

s32 Nav_IsPositionInMesh(void *, struct ContactPosition *, struct ContactPosition *);

s32 Terrain_ApplyBlocked(void *, struct ContactPosition *, struct ContactPosition *);

s32 SceneActor_FindDirectionalCell(struct ChaseActor *actor, s32 direction);

s32 SceneActor_StartAdjacentCellRoute(struct Mummy *actor)
{
    struct ContactPosition target;
    struct Mummy *parent = actor->chase.base.context;
    u8 *group = (void *)&actor->chase.group.common;
    s32 direction = (((actor->chase.base.body.direction + 0x20) >> 6) + 2) & 3;
    s32 cell = 0;
    s32 count = 0;
    struct ContactPosition *targetPointer;
    s32 cellX, cellZ;
    u8 *terrain;
    void *route;
    struct ContactPosition *position;
    s32 result;
    u16 *flagsPointer;
    for (; count <= 3; count++) {
        cell = SceneActor_FindDirectionalCell(&actor->chase, direction);
        direction = (direction + 1) & 3;
        if (cell != actor->chase.base.terrainCell)
            break;
    }
    targetPointer = &target;
    cellX = Mod(cell, *(s16 *)(gCurrentTerrainMap->cells + 4));
    cellZ = Div(cell, *(s16 *)(gCurrentTerrainMap->cells + 4));
    target.x = (cellX << 8) + 0x80;
    terrain = TerrainModifiers_FindCellOverride(cell, 1);
    if (terrain)
        terrain += 4;
    else
        terrain = gCurrentTerrainMap->cells + (cell * 4 + 12);
    targetPointer->y = (*terrain & 15) << 8;
    targetPointer->z = (cellZ << 8) + 0x80;
    target.y = actor->chase.base.body.position.y;
    route = *(void **)(group + 0x1FC);
    position = &actor->chase.base.body.position;
    result = Nav_IsPositionInMesh(route, position, &target);
    if (!result) {
        u16 *flags = &actor->chase.base.motionFlags;
        u32 mask = 0xFFFFFDFF;
        u32 value = *flags;
        mask &= value;
        *flags = mask;
    } else {
        result = Terrain_ApplyBlocked(route, position, &target);
        if (result) {
            if (result != 2)
                Memory_Clear(&actor->chase.base.body.motion, 8);
        } else {
            u16 *flags = &actor->chase.base.motionFlags;
            u32 mask = 0xFFFFFDFF;
            u32 value = *flags;
            mask &= value;
            *flags = mask;
        }
    }
    parent->flags6D4 = 30;
    return 1;
}

// confidence=low: refreshes the nearby route heading after the context cooldown expires.

s32 BouncingObject_GetNearbyHeading(const struct ContactPosition *);

s32 SceneActor_StartAdjacentCellRoute(struct Mummy *actor);

u8 BouncingObject_RefreshNearbyHeading(struct ChaseActor *arg)
{
    struct ChaseActor *actor = arg;
    struct SceneActorControlState **slot = &((struct SceneActorControlPhaseView *)actor)->control;
    u8 *context = (u8 *)*slot;
    u16 *counter;
    u32 v;
    u32 saved;
    counter = (u16 *)(context + 0x6D4);
    v = *counter;
    saved = (unsigned short)v;
    if (saved) {
        *counter = v - 1;
        return 0;
    }
    if (BouncingObject_GetNearbyHeading(&actor->base.body.position) < 0) {
        u16 *flags = (u16 *)((u8 *)*slot + 0x6F2);
        u32 mask = ~0x400;
        *flags &= mask;
        *counter = saved;
        return 0;
    }
    {
        u8 *control = (u8 *)*slot;
        u32 mask = 0x400;
        u16 *flags = (u16 *)(control + 0x6F2);
        *flags = mask | *flags;
    }
    SceneActor_StartAdjacentCellRoute(actor);
    return 1;
}

u8 SceneActorPath_FaceAndCheckStraightPath(struct Unk080D6DF8Actor *actor);

struct SceneActorTrackingTarget {
    u8 unknown0[12];
    struct ContactPosition position;
};

struct SceneActorTrackingParent {
    u8 unknown0[0x6F2];
    u16 flags;
};

static inline u8 IsPlayerTracked(struct ChaseActor *actor)
{
    u32 mask = 2;
    if (actor->base.placementFlags & mask)
        return 1;
    return 0;
}

static inline void SubtractPosition(struct ContactPosition *out, struct ContactPosition *a,
                                    struct ContactPosition *b)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}

u8 SceneActor_TryTrackNearbyPlayer(struct ChaseActor *actor, struct ContactPosition *input,
                                   u32 playerId)
{
    u8 *group = (void *)&actor->group.common;
    struct ContactPosition delta;
    struct ContactPosition *deltaPointer;
    struct ContactPosition *position;
    u32 distance;
    struct SceneActorReactionMotion *motion;
    if (playerId != gTrackedPlayerId[0])
        goto fail;
    {
        struct Player *player = gPlayers[0];
        u32 mask = 8;
        if (!(player->statusFlags & mask))
            goto fail;
    }
    deltaPointer = &delta;
    SubtractPosition(deltaPointer, input, &actor->base.body.position);
    distance = delta.x * delta.x + deltaPointer->z * deltaPointer->z;
    {
        u32 range = *(u32 *)(group + 0x50);
        position = &actor->base.body.position;
        if (distance >= range)
            goto fail;
    }
    if (!(u8)SceneActorPath_FaceAndCheckStraightPath(actor))
        goto fail;
    {
        u32 *flags = &actor->base.behaviorFlags;
        s32 mask = -0x81;
        *flags &= mask;
    }
    {
        struct SceneActorTrackingParent *parent = actor->base.context;
        u32 mask = 0x800;
        u16 *flags = &parent->flags;
        u32 value = *flags;
        mask |= value;
        *flags = mask;
    }
    motion = (void *)&actor->base.tracking.target;
    motion->fieldDC = (u32)((u8 *)gPlayers[0] + 0x24);
    if (IsPlayerTracked(actor))
        motion->fieldDC = (u32)((u8 *)gPlayers[0] + 0x24);
    else if (!motion->fieldDC)
        goto ready;
    {
        struct SceneActorTrackingTarget *target = (void *)motion->fieldDC;
        s32 dx = target->position.x - position->x;
        s32 dz = target->position.z - actor->base.body.position.z;
        actor->base.tracking.heading = Math_Atan2(dx, dz);
        actor->base.tracking.distance = dx * dx + dz * dz;
    }
ready: {
    u32 mask = 0x80;
    u16 *flags = &actor->base.reactionFlags;
    u32 value = *flags;
    mask |= value;
    *flags = mask;
}
    return 1;
fail:
    return 0;
}

#define gSceneGridMap gTerrainMap

#define gSceneGridWidth gTerrainWidth

#define gSceneGridHeight gTerrainHeight

void SceneGrid_SnapPositionToCellCenter(struct ContactPosition *position, u32 cell)
{
    s32 x = position->x >> 8;
    s32 z = position->z >> 8;
    s32 index;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        index = 0;
    else
        index = gTerrainMap->rows[z] + x;
    SceneActorTerrain_GetCenter(index, position);
}

void Memory_Clear(void *dst, s32 bytes); /* zero fill (bytes, then CpuSet words) */

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

struct S2 {
    u8 filler[0x5c0];
    u16 h5c0;
    u16 h5c2;
};

void Mummy_ResetMotionState(struct S2 *s)
{
    SceneActorEffects_AdvanceSteps(s, 4);
    Memory_Clear((u8 *)s + 0x5b8, 8);
    s->h5c2 = 0;
    s->h5c0 = 0;
}

// confidence=low: clears movement response flags and restores the response speed and sentinel.


struct ResponseState {
    u8 pad[8];
    u16 timer;
    u8 mode;
    u8 padB[13];
    u32 counter;
};

static inline void ClearHalf(u16 *p, u32 mask)
{
    *p = *p & mask;
}

static inline u32 WordFlag(u8 *p, u32 mask, u32 off)
{
    return *(u32 *)(p + off) & mask;
}

void Mummy_ResetResponseState(u8 *p)
{
    u8 *body = p + 0xDC;
    u32 mask = ~0x18;
    u32 zero;
    u32 *flags;
    u16 *motion;
    struct ResponseState *response;
    {
        u32 loaded = body[15];
        u32 value = mask;
        do {
            value &= loaded;
        } while (0);
        zero = 0;
        body[15] = value;
    }
    flags = (u32 *)(p + 0x2D0);
    {
        u32 mask = 0xFEFFFFFF;
        *flags &= mask;
    }
    *(u16 *)(p + 0x2C2) = 64;
    *(u16 *)(p + 0x414) = zero;
    *(u16 *)(p + 0x418) = zero;
    motion = (u16 *)(p + 0x2D4);
    ClearHalf(motion, 0xFFFFFC7F);
    body[15] &= mask;
    response = (void *)(p + 0x420);
    if (WordFlag(p, 0x200000, 0x2CC)) {
        {
            u32 mask = 0xFEFFFFFF;
            *flags &= mask;
        }
        ClearHalf(motion, 0xFFFFFDFF);
        response->mode = 0;
        response->counter = zero;
        response->timer = zero;
    }
    ClearHalf(motion, 0xFFFFFBFF);
    {
        u32 offset = 0x5DA;
        u8 *address = p + offset;
        *address = 255;
    }
}

// confidence=low: clears mummy motion counters and restarts the directional animation.

void Mummy_ResetMotionState(struct S2 *s);

void SceneActorMovement_ResolveRoutePosition(void *);

void Mummy_ResetRevivalMotion(struct ChaseActor *actor)
{
    struct Mummy *body = actor->base.context;
    body->targetTick = 0;
    body->pursuitDelay = 0;
    body->flags6D4 = 0;
    body->savedTick = 0;
    body->flags6F2 = 0;
    actor->group.field5C4 = 0;
    body->counter6EE = 0;
    Mummy_ResetMotionState(actor);
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    SceneActorMovement_ResolveRoutePosition(actor);
    {
        struct SceneActorControlMotionDrawable *drawable = actor->base.drawable;
        DisplaySequence_StepState(drawable, &drawable->sequence);
    }
}

struct SOnClearActivityFlag {
    u8 filler[0x2d0];
    u32 flags;
};

void Mummy_OnClearActivityFlag(struct SOnClearActivityFlag *s)
{
    u32 *p = &s->flags;
    u32 m = ~2;
    *p = *p & m;
}

struct TUpdateTemporaryFlagTimer {
    u8 filler[0x6ea];
    u8 cnt;
};

struct SUpdateTemporaryFlagTimer {
    u8 filler[0x2d0];
    u32 flags;
    u8 filler2[0x3d0 - 0x2d4];
    struct TUpdateTemporaryFlagTimer *t;
};

void Mummy_UpdateTemporaryFlagTimer(struct SUpdateTemporaryFlagTimer *s)
{
    u8 *p = &s->t->cnt;
    if (*p != 0)
        *p = *p - 1;
    else {
        u32 *q = &s->flags;
        u32 m = 0xFEFFFFFF;
        *q = *q & m;
    }
}

#define SceneEffect_CreateParticle ExplosionEffect_CreateDelayed

#define gSceneEffectPatternArguments gUnk_086061C0

#define gSceneEffectFourOffsetsX gUnk_086061C4

#define gSceneEffectFourOffsetsZ gUnk_086061CC

#define gSceneEffectTwoOffsetsX gUnk_086061D4

#define gSceneEffectTwoOffsetsZ gUnk_086061D8

extern const u16 gUnk_086061D4[], gUnk_086061D8[], gUnk_086061C4[], gUnk_086061CC[];

extern const u8 gUnk_086061C0[];

u8 *ExplosionEffect_CreateDelayed(void *, u32, s32, s32, s32, u32);

void SceneEffect_EmitPositionPattern(struct ChaseActor *emitter)
{
    struct ContactPackedVector position;
    s32 i;
    position = *(struct ContactPackedVector *)&emitter->base.body.position;
    position.y += 0x40;
    switch (emitter->base.paletteOffset) {
        case 0:
            ExplosionEffect_CreateDelayed(&position, 0, emitter->base.attacks.first[2],
                                          emitter->base.attacks.second[2], 0, 0);
            break;
        case 1:
            for (i = 0; i < 2; i++) {
                struct ContactPosition *p = (struct ContactPosition *)&position;
                position = *(struct ContactPackedVector *)&emitter->base.body.position;
                p->y += 0x40;
                p->x += gUnk_086061D4[i];
                p->z += gUnk_086061D8[i];
                ExplosionEffect_CreateDelayed(&position, 0, emitter->base.attacks.first[2],
                                              emitter->base.attacks.second[2], 0, gUnk_086061C0[i]);
            }
            break;
        case 2:
            for (i = 0; i < 4; i++) {
                struct ContactPosition *p = (struct ContactPosition *)&position;
                position = *(struct ContactPackedVector *)&emitter->base.body.position;
                p->y += 0x40;
                p->x += gUnk_086061C4[i];
                p->z += gUnk_086061CC[i];
                ExplosionEffect_CreateDelayed(&position, 0, emitter->base.attacks.first[2],
                                              emitter->base.attacks.second[2], 0, gUnk_086061C0[i]);
            }
            break;
    }
}

struct TClearInactiveActionFlags {
    u8 filler[0x6f2];
    u16 h;
};

struct SClearInactiveActionFlags {
    u8 filler[0x2b5];
    u8 b;
    u8 filler2[0x3d0 - 0x2b6];
    struct TClearInactiveActionFlags *t;
};

void Mummy_ClearInactiveActionFlags(struct SClearInactiveActionFlags *s)
{
    if (s->b != 0x27) {
        u16 *p = &s->t->h;
        u32 m = 0xFFFFEBFF;
        *p = *p & m;
    }
}
