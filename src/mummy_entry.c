#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_animation.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_delayed_response.h"
#include "display_sequence.h"
#include "scene_actor_displacement.h"
#include "spark_particle.h"
#include "script_utility.h"
#include "engine.h"
#include "scene_actor_part_burst.h"
#include "scene_actor_chase.h"
#include "scene_actor_stimulus.h"
#include "scene_actor_control_phase.h"
#include "debug.h"
#include "mummy.h"
#include "scene_actor_turn.h"
#include "scene_actor_reset.h"
#include "scene_actor_chase_direct_arrival.h"
#include "scene_actor_cardinal_path.h"
#include "scene_actor_terrain.h"
#include "player.h"
#include "scene_actor_straight_route.h"
#include "scene_actor_contacts.h"
#include "scene_actor_chase_shared.h"
#include "area_effect_link.h"
#include "render.h"
#include "runtime.h"
#include "scene_actor_adjacent_cell.h"
#include "ram.h"
#include "scene_actor_control_motion.h"
#include "display_sequence_inline.h"
#include "scene_actor_registry.h"
#include "engine_globals.h"
#include "scene_actor_init_primitives.h"
#include "scene_actor_init_group.h"
#include "scene_actor_init_dispatch.h"
#include "scene_actor_init_graphics.h"
#include "scene_actor_init_body.h"

void SceneActorAnimation_SelectDirectionalEntry(struct RoomActor *, s32);

static inline u8 TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return TRUE;
    }
    return FALSE;
}

void SceneActorAnimation_UpdateDirectionalEntry(u8 *p, s32 arg1)
{
    if (TakeFlag(p + 0x2B2)) {
        SceneActorAnimation_SelectDirectionalEntry(p, 0);
        *(u8 *)(p + 0x2B3) = 0;
    }
    if (arg1 > 0x10 && *(u8 *)(p + 0x2B3) && *(u8 *)(p + 0x2B0) != 1) {
        *(u8 *)(p + 0x2B0) = 1;
        SceneActorAnimation_SelectDirectionalEntry(p, 0);
    }
}

void SceneActor_SetPositionXZ(void *, s32, s32);

void Mummy_ResumeSavedTarget(struct ChaseActor *actor, s32 frame)
{
    struct DisplaySequenceState *anim =
        &((struct SceneActorAnimationSequenceDrawable *)actor->base.drawable)->sequence;
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        actor->base.restart = 0;
    }
    if (frame > 42) {
        struct SceneActorChaseGroup *group = &actor->group;
        u16 blocked;
        if (!actor->base.restart) {
            return;
        }
        actor->base.stepDone = 1;
        SceneActor_SetPositionXZ(&actor->base.body.id,
                                 ((struct ChaseRouteProbe *)group->route)->destination.x,
                                 ((struct ChaseRouteProbe *)group->route)->destination.z);
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        blocked = SceneActorChase_QueryMotionFlags(actor, 0x80);
        if (blocked) {
            void *target = group->common.gate.control;
            SceneActorChase_SetControl(actor, target, 0x25);
            SceneActorChase_SetBehavior(actor, 12);
            group->destination->pad = 1;
            SceneActorChase_GroupSetFlags(group, 4);
        } else {
            void *target;
            SceneActorChase_SetBehavior(actor, 7);
            target = actor->base.onAlternateEntry;
            SceneActorChase_SetControl(actor, target, 1);
            group->destination->pad = blocked;
            actor->base.field2C2 = Div(64, 3);
            SceneActorChase_SetActorFlags(actor, 16);
        }
    } else if (anim->index == 2) {
        u16 frame = anim->elapsed;
        u32 loop = anim->duration;
        if (loop) {
            if (frame == (loop - 1)) {
                goto rewind;
            }
            return;
        } else if (frame) {
            return;
        }
    rewind:
        anim->elapsed = 0;
    }
}

// confidence=low: hidden recovery disables the shadow and waits to enter revival.

static inline void Mummy_GetDirectionVariant(struct ChaseActor *actor, u8 *variant, u8 *mirror,
                                             u32 zero)
{
    s32 quadrant = ((actor->base.body.direction + 0x60) >> 6) & 3;
    if (quadrant > 1) {
        *mirror = 1;
        *variant = 3 - quadrant;
    } else {
        *mirror = zero;
        *variant = quadrant;
    }
}

static inline void Mummy_SelectAnimation(struct ChaseActor *actor, u32 selection, u32 variant,
                                         u32 flags)
{
    u32 force;
    u8 *restart;
    if (flags & 0x10) {
        force = 0x10;
        flags &= ~0x10;
    } else
        force = 0;
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
            if (flags == 1 && (tile->flags & 4))
                return;
            if (flags == 0 && !(tile->flags & 4))
                return;
        }
        *savedSelection = selection;
        {
            u32 mode;
            if (flags & 4)
                mode = 6;
            else
                mode = 4;
            Anim_SetKeyframes(
                &((struct SceneActorAnimationTileDrawable *)actor->base.drawable)->tile,
                actor->base.drawable, (u16)selection, 2, (u16)mode);
            flags &= 1;
            if (flags)
                tile->flags |= 4;
            else
                tile->flags &= ~4;
        }
    }
    *restart = 0;
}

static inline u8 TakeSaved(u8 *flag, u8 **saved)
{
    u32 value = *flag;
    *saved = flag;
    if (value) {
        **saved = 0;
        return 1;
    }
    return 0;
}

void Mummy_UpdateRevival(void *);

void Mummy_UpdateHiddenRecovery(struct ChaseActor *actor, s32 frame)
{
    u32 queued;
    u8 *prepared;
    u32 mask = 1;
    if (!actor->base.drawableKind) {
        u32 *flags = actor->base.drawable;
        *flags |= mask;
    } else {
        struct SceneInputWidget *tile = (void *)((u8 *)actor->base.drawable + 0x20);
        tile->flags |= mask;
    }
    queued = TakeSaved(&actor->base.field2B2, &prepared);
    if (queued) {
        ActorShadow_Disable((u8 *)actor + 0x320);
    } else if (frame > 0x1A) {
        void *callback;
        u32 kind, zero;
        {
            u8 directionVariant, mirror;
            Mummy_GetDirectionVariant(actor, &directionVariant, &mirror, queued);
            Mummy_SelectAnimation(actor, 8, directionVariant, mirror | 4);
        }
        callback = Mummy_UpdateRevival;
        kind = 13;
        zero = 0;
        *prepared = 1;
        actor->base.stepDone = zero;
        actor->base.controlKind = kind;
        actor->base.target = callback;
        actor->base.field2C8 = zero;
        {
            u32 *flags = &actor->base.pendingControlFlags;
            s32 mask = ~1;
            *flags &= mask;
        }
        actor->base.pendingControl = zero;
    } else
        actor->base.restart = queued;
}

void ActorShadow_Enable(void *);

void Mummy_ResetRevivalMotion(struct ChaseActor *actor);

void SceneActorPalette_ClearCompletedTransitions(void);

static inline u8 peek_flag(struct ChaseActor *actor)
{
    return actor->base.restart;
}

void Mummy_UpdateRevival(void *object)
{
    struct ChaseActor *actor = object;
    s32 n;
    if (SceneActorReaction_TakePrepared(actor)) {
        ActorShadow_Enable(actor->base.shadow);
        if (!actor->base.drawableKind) {
            struct SceneActorRegistrySequenceDrawable *drawable = actor->base.drawable;
            u32 *flags = &drawable->flags;
            u32 mask = ~1;
            *flags = mask & *flags;
        } else {
            struct SceneActorAnimationTileDrawable *drawable = actor->base.drawable;
            struct SceneInputWidget *tile = &drawable->tile;
            u32 mask = ~1;
            tile->flags = mask & tile->flags;
        }
        {
            u16 one = 1;
            actor->base.health = one;
        }
        actor->base.colliderA.responseValue = 60;
    }
    if (!peek_flag(actor))
        return;
    {
        Mummy_ResetRevivalMotion(actor);
        actor->base.behaviorFlags = actor->base.saved.behaviorFlags;
        actor->base.statusFlags = actor->base.saved.statusFlags;
        actor->base.body.direction = actor->base.saved.direction;
        actor->base.state2A8 = actor->base.saved.state;
        actor->base.phase2A9 = actor->base.saved.phase;
        actor->base.behavior2AA = actor->base.saved.behavior;
        actor->base.target = (void *)actor->base.saved.control;
        actor->base.controlKind = actor->base.saved.kind;
        actor->base.placementFlags = actor->base.saved.motionFlags;
        actor->base.heightOffset = actor->base.saved.heightOffset;
        n = actor->base.defaultHealth;
        if (n <= 0)
            n = 0;
        actor->base.health = n;
        actor->base.reactionFlags = 0;
        actor->base.motionFlags = 0;
        actor->base.stepDone = 0;
        actor->base.field2B1 = 1;
        actor->base.field2B2 = 1;
        actor->base.restart = 0;
        actor->base.counter304 = 0;
        actor->base.reactionDirection = 0;
        actor->base.field2C2 = 64;
        actor->base.hitAttributes = 0;
        actor->base.hitReactionFlags = 0;
        actor->base.hitOwners = 0;
        actor->base.field2E6 = 0;
        actor->base.field3CC = 0;
        actor->base.tracking.target = 0;
        SceneActorPalette_ClearCompletedTransitions();
    }
}

s32 Mod(s32, s32);

void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);

void Mummy_UpdateDeath(struct ChaseActor *actor, s32 frame);

struct Sub {
    u8 f0[8];
    u16 h8;
    u8 fA[4];
    u16 hE;
};

struct Ctx {
    u8 f0[0x48];
    struct Sub sub;
};

void Mummy_UpdateDying(u8 *s, s32 arg)
{
    struct Sub *c = &(*(struct Ctx **)(s + 4))->sub;
    u8 *part = s + 0x3D4;
    u32 mask;
    s32 off;
    if (TakeFlag(s + 0x2B2)) {
        u32 test;
        u16 *flags;
        u32 value;
        SceneActorAnimation_SelectDirectionalEntry(s, 8);
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
        if (!(*((u32 *)(s - (-off))) & mask)) {
            SceneActorChase_SetControl(s, Mummy_UpdateDeath, 6);
            return;
        }
    }
    if (c->h8 == 2)
        c->hE = 0;
}

/* Mummy poses after dying before signaling that the control finished. */

void Mummy_UpdateDeath(struct ChaseActor *actor, s32 frame)
{
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectDirectionalEntry(&actor->base, 9);
        SceneActorChase_ClearBits(&actor->base.motionFlags, 1);
    }
    if (frame == 40)
        SceneActorChase_SetReactionFlags(actor, 0x10);
    if (actor->base.restart) {
        SceneActorAnimation_SelectDirectionalEntry(&actor->base, 0);
        if (frame > 60)
            actor->base.stepDone = 1;
    }
}

/* This class's own copies of the shared chase inlines it compiles differently. */

static inline s32 Mummy_CellAt(struct ContactPosition *position)
{
    return SceneActorChase_CellIndex(position->x >> 8, position->z >> 8);
}

static inline u32 Mummy_Height(struct ContactPosition *position)
{
    return SceneActorChase_HeightAt(Mummy_CellAt(position), position);
}

static inline void Mummy_SnapToRoute(struct ContactPosition *position,
                                     struct SceneActorRegistryRoute *route)
{
    position->x = route->point->x;
    position->z = route->point->z;
    position->y = Mummy_Height(position);
}

static inline struct SceneActorChaseFlags16 *Mummy_BeginRecovery(struct ChaseActor *actor, u16 bits)
{
    struct SceneActorChaseFlags16 *flags = (void *)&actor->base.reactionFlags;
    SceneActorChase_SetFlagComponent(flags, bits);
    return flags;
}

void Mummy_ResumeRouteAfterRecovery(struct ChaseActor *actor, s32 frame)
{
    struct DisplaySequenceState *sequence =
        (void *)&((struct SceneActorAnimationSequenceDrawable *)actor->base.drawable)->sequence;
    struct SceneActorChaseFlags16 *reactionFlags;
    SceneActorReaction_TakePrepared(actor);
    reactionFlags = Mummy_BeginRecovery(actor, 0x10);
    if ((((frame <= 0x11) && (!sequence->index)) &&
         (sequence->elapsed == (sequence->duration != 0))) &&
        (sequence->duration != 1))
        sequence->elapsed = 0;
    if (actor->base.restart) {
        struct SceneActorChaseGroup *group = &actor->group;
        actor->base.stepDone = 1;
        if (SceneActorChase_QueryMotionFlags(actor, 2)) {
            SceneActorChase_SetState(actor, 3, 0, 0);
            SceneActorChase_SetControl(actor, actor->group.contactControl, 7);
            SceneActorChase_ClearFlagComponent(reactionFlags, 0x983);
            return;
        }
        if (SceneActorChase_QueryMotionFlags(actor, 0x80)) {
            SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
            SceneActorChase_SetBehavior(actor, 14);
            SceneActorChase_SetControl(actor, group->common.gate.control, 0x25);
            return;
        }
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        SceneActorChase_SetState(actor, 3, 1, 0);
        SceneActorChase_SetControl(actor, (void *)SceneActorReaction_RestoreAfterDelay, 0xE);
        SceneActorDelay_Prepare((void *)&group->saved, actor->base.onAlternateEntry, 7, 0x28);
        SceneActorChase_SetActorFlags(actor, 0x10);
        Mummy_SnapToRoute(actor->group.destination, &actor->group.homeRoute);
        if (!SceneActorChase_TryRoute(actor, group->route, &actor->base.body.position,
                                      group->destination)) {
            SceneActorChase_SetControl(actor, (void *)SceneActorControl_OnIdle, 0);
            SceneActorChase_SetState(actor, 3, 0, 0);
        } else
            group->destination->pad = 1;
    }
}

/* This class's own copies of the shared chase inlines it compiles differently. */

static inline void Mummy_SaveControl(struct SceneActorChaseSavedControl *packet, void *object)
{
    struct ChaseActor *actor = object;
    packet->control = actor->base.target;
    packet->kind = actor->base.controlKind;
    packet->state = actor->base.state2A8;
    packet->phase = actor->base.phase2A9;
    packet->behavior = actor->base.behavior2AA;
}

static inline s32 Mummy_MapWidth(struct EngineMap *map)
{
    return ((struct SceneActorChaseTerrainGrid *)map->cells)->width;
}

static inline s32 Mummy_CellCenterCoordinate(s32 coordinate)
{
    return ((u32)coordinate << 8) + 0x80;
}

static inline void Mummy_StoreCoordinate(s16 *field, s32 coordinate)
{
    *field = coordinate;
}

static inline void Mummy_CellCenter(s32 cell, struct ContactPosition *position)
{
    s32 x = Mod(cell, Mummy_MapWidth(SceneActorChase_CurrentMap()));
    s32 z = Div(cell, Mummy_MapWidth(SceneActorChase_CurrentMap()));
    Mummy_StoreCoordinate(&position->x, Mummy_CellCenterCoordinate(x));
    {
        u32 attributes = SceneActorChase_CellProperties(cell)->attributes;
        u32 height = 15;
        height &= attributes;
        position->y = SceneActorChase_TerrainLevelHeight(height);
    }
    Mummy_StoreCoordinate(&position->z, Mummy_CellCenterCoordinate(z));
}

static inline u8 Mummy_RouteCount(struct SceneActorChaseRouteSelection *route)
{
    return route->points->count;
}

static inline void Mummy_ChooseRoute(struct ChaseActor *actor)
{
    struct SceneActorChaseRouteSelection *route = actor->base.body.currentRoute;
    u8 choice;
    {
        u32 index = (gRandomIndex + 1) & 1023;
        s32 value;
        gRandomIndex = index;
        index *= 2;
        value = *(const u16 *)(0x0203B400 + index);
        value >>= 3;
        choice = Mod(value, Mummy_RouteCount(route));
    }
    if (choice == route->index)
        choice = Mod(choice + 1, Mummy_RouteCount(route));
    route->index = choice;
    route->phase = 0;
    {
        u8 *base = gTerrainMap->routes;
        base += route->points->offset;
        route->current = base + route->index * 8;
    }
}

static inline u8 Mummy_CoordinateFraction(s16 coordinate)
{
    return coordinate;
}

static inline u32 Mummy_HeightAt(s32 cell, struct ContactPosition *position)
{
    struct SceneActorChaseTerrainCell *data = SceneActorChase_CellProperties(cell);
    u32 kind, height;
    kind = data->attributes >> 4;
    height = (data->attributes & 15) << 8;
    switch (kind) {
        case 1:
            height -= Mummy_CoordinateFraction(position->z);
            break;
        case 2:
            height -= Mummy_CoordinateFraction(position->x);
            break;
    }
    return height;
}

static inline u8 Mummy_TryRoute(struct ChaseActor *actor, void *route,
                                struct ContactPosition *position, struct ContactPosition *target)
{
    s32 result;
    if (!Nav_IsPositionInMesh(route, position, target)) {
        SceneActorChase_ClearBits((&(actor)->base.motionFlags), 0x200);
        return 0;
    } else {
        result = Terrain_ApplyBlocked(route, position, target);
        if (result) {
            if (result != 2)
                Memory_Clear((&(actor)->base.body.motion), 8);
            return 1;
        } else {
            SceneActorChase_ClearBits((&(actor)->base.motionFlags), 0x200);
            return 0;
        }
    }
}

static inline u32 Mummy_Height_UpdateDisplacementRecovery(struct ContactPosition *position)
{
    return Mummy_HeightAt(SceneActorChase_CellAt(position), position);
}

static inline void
Mummy_SnapToRoute_UpdateDisplacementRecovery(struct ContactPosition *position,
                                             struct SceneActorRegistryRoute *route)
{
    position->x = route->point->x;
    position->z = route->point->z;
    position->y = Mummy_Height_UpdateDisplacementRecovery(position);
}

static inline void Mummy_BeginTurn(void *obj, u32 saved, const u16 *randomSamples)
{
    struct ChaseActor *actor = obj;
    struct SceneActorChaseGroup *group = SceneActorChase_GroupOf(obj);
    if (!SceneActorChase_MotionFlags(actor, 0x400) &&
        !SceneActorChase_TestField((u16 *)&group->common.gate.flags, 4)) {
        group->turn.direction =
            SceneActorChase_RandomDirectionExcept(actor->base.body.direction, randomSamples);
        Mummy_SaveControl(&group->saved, actor);
        group->turn.savedDirection = saved;
        SceneActorChase_SetControl(actor, group->turnControl, 0x1B);
        SceneActorChase_SetState(actor, 3, 1, 0);
        group->turn.turnsRemaining = 6;
    }
}

static inline u8 Mummy_Arrive(void *obj)
{
    struct ContactPosition target;
    struct ChaseActor *actor = obj;
    struct SceneActorChaseGroup *group = SceneActorChase_GroupOf(obj);
    struct SceneActorChaseTracking *tracking = &actor->base.tracking;
    s32 cell, player;
    const u16 *randomSamples = gRandomTable;
    void **callback;
    Mummy_SnapToRoute_UpdateDisplacementRecovery(group->destination, &group->homeRoute);
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
        Mummy_SnapToRoute_UpdateDisplacementRecovery(group->destination, &group->homeRoute);
        (group->destination)->y =
            Mummy_HeightAt(SceneActorChase_CellAt(group->destination), group->destination);
        (group->destination)->pad = 0;
    }
    {
        u8 saved = actor->base.body.direction;
        Mummy_BeginTurn(obj, saved, randomSamples);
    }
    SceneActorTerrain_RequestRetreatRoute(obj);
    actor->base.field2C2 = 0x20;
    return 0;
reroute: {
    Mummy_CellCenter(cell, &target);
    if (Mummy_TryRoute(obj, group->route, &actor->base.body.position, &target))
        goto success;
}
    callback = &group->fallback;
invoke:
    Callback_InvokeActor(actor, *callback);
    return 0;
success:
    return 1;
}

void SceneActorMovement_ResolveRoutePosition(void *);

void Mummy_ResumeRouteAfterRecovery(struct ChaseActor *, s32);

static inline u8 Mummy_DisplacementStopped(s32 x, s32 z)
{
    if (!x && !z)
        return 1;
    return 0;
}

/* Ease recoil displacement, then select route or pursuit recovery. */

void Mummy_UpdateDisplacementRecovery(struct ChaseActor *actor, s32 frame)
{
    struct DisplaySequenceState *animation =
        &((struct SceneActorAnimationSequenceDrawable *)actor->base.drawable)->sequence;
    struct ContactPackedVector range;
    struct SceneActorChaseGroup *group;
    s16 *x, *z;
    s32 dx, dz;
    if (SceneActorReaction_TakePrepared(actor)) {
        actor->base.secondaryMotion.x =
            SceneActorDisplacement_Quarter(actor->base.secondaryMotion.x);
        actor->base.secondaryMotion.z =
            SceneActorDisplacement_Quarter(actor->base.secondaryMotion.z);
        SceneActorAnimation_SelectDirectionalEntry(&actor->base, 3);
        if (SceneActorChase_QueryMotionFlagComponent(actor, 8)) {
            Sound_PlaySong(0x12A);
            Sound_PlaySong(0xFD);
        }
        SceneActorChase_ClearMotionFlagComponent(actor, 1);
        actor->base.restart = 0;
    }
    SceneActorChase_SetActorFlags(actor, 0x2000);
    {
        u32 mask = 0x10000;
        actor->base.statusFlags |= mask;
    }
    if (frame <= 17 && animation->index == 0 && animation->elapsed == (animation->duration != 0) &&
        animation->duration != 1)
        animation->elapsed = 0;
    if (Mod(frame, 12) == 0 && SceneActorChase_QueryMotionFlagComponent(actor, 8)) {
        u32 index, sample;
        u8 count;
        range.x = 60;
        range.y = 60;
        range.z = 40;
        index = (gRandomIndex + 1) & 1023;
        gRandomIndex = index;
        index *= 2;
        sample = *(const u16 *)(RANDOM_TABLE_ADDR + index);
        sample &= 3;
        count = sample + 1;
        SparkParticle_Spawn(&actor->base.body.position, count,
                            (const struct ContactPosition *)&range,
                            actor->base.body.direction + 128, 10, 6, 2, 2);
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
    if (!Mummy_DisplacementStopped(dx, dz))
        return;
    if (!actor->base.restart)
        return;
    group = &actor->group;
    actor->base.stepDone = 1;
    SceneActorChase_ClearMotionFlagComponent(actor, 0x120);
    SceneActorChase_ClearActorFlags(actor, 0x80);
    if (SceneActorChase_QueryMotionFlagComponent(actor, 0x10)) {
        Mummy_ChooseRoute(actor);
        SceneActorMovement_ResolveRoutePosition(actor);
        SceneActorChase_SetBehavior(actor, 8);
        SceneActorChase_SetControl(actor, group->routeChoiceControl, 0x13);
        SceneActorChase_ClearMotionFlagComponent(actor, 8);
        return;
    }
    if (actor->base.hitReactionFlags & 0x80000) {
        SceneActorChase_SetReactionFlags(actor, 2);
        return;
    }
    if (SceneActorChase_QueryMotionFlagComponent(actor, 8)) {
        SceneActorChase_SetState(actor, 3, 0, 0);
        SceneActorChase_SetControl(actor, Mummy_ResumeRouteAfterRecovery, 0x14);
        SceneActorChase_ClearMotionFlagComponent(actor, 8);
        actor->base.field2C2 = 64;
        return;
    }
    if (SceneActorChase_QueryMotionFlagComponent(actor, 0x80)) {
        SceneActorChase_SetActorFlags(actor, 0x80);
        SceneActorChase_TrackPlayer(actor);
        if (!Mummy_Arrive(actor)) {
            if (!Mummy_TryRoute(actor, group->route, &actor->base.body.position,
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
    SceneActorChase_SetActorFlags(actor, 0x10);
    group->destination->pad = 0;
    SceneActorChase_TrackPlayer(actor);
    if (!Mummy_Arrive(actor)) {
        if (!Mummy_TryRoute(actor, group->route, &actor->base.body.position,
                            &actor->base.tracking.target->position)) {
            Callback_InvokeActor(actor, group->fallback);
            return;
        }
    }
    SceneActorChase_SetControl(actor, actor->base.onAlternateEntry, 1);
    SceneActorChase_SetBehavior(actor, 7);
    actor->base.field2C2 = Div(64, 3);
}

// confidence=low: defeat control emits effects, runs scripts, and returns the actor to idle.

void SceneActorMovement_UpdateAirborne(void *);

void SceneActorMovement_UpdateHeight(void *);

void SceneEffect_EmitPositionPattern(struct ChaseActor *emitter);

void SceneStimulus_AddUntagged(s32, s32, s32, s32, s32);

/* Mummy defeat: hides the drawable, emits defeat effects, runs the script and returns to idle. */

void SceneActorEffects_AdvanceSteps(void *, u32);

void *Bestiary_GetEntryForActor(u32, u32);

void Bestiary_UnlockEntry(void *);

void SceneActorBehavior_CallTerrainScript(void *);

void SceneActorPlacement_PlayCategoryEventSound(void *);

static inline u32 Mummy_MotionFlags(struct ChaseActor *actor, u32 maskInput)
{
    u32 mask = maskInput;
    return actor->base.motionFlags & mask;
}

static inline u32 Mummy_ActorFlags(struct ChaseActor *actor, u32 maskInput)
{
    u32 mask = maskInput;
    return actor->base.behaviorFlags & mask;
}

static inline u32 Mummy_DisplayFlags(struct ChaseActor *actor, u32 maskInput)
{
    u32 mask = maskInput;
    return actor->base.statusFlags & mask;
}

static inline u32 Mummy_HalfFlags(u16 *field, u32 mask)
{
    return *field & mask;
}

static inline void ShowTile(struct SceneInputWidget *tile, u32 mask)
{
    tile->flags |= mask;
}

static inline void HideTile(struct SceneInputWidget *tile)
{
    u32 mask = ~1;
    tile->flags = mask & tile->flags;
}

static inline u32 Random4(void)
{
    u32 index = (gRandomIndex + 1) & 1023;
    u32 v;
    gRandomIndex = index;
    index *= 2;
    v = *(const u16 *)((u32)((const u16 *)RANDOM_TABLE_ADDR) + index);
    v &= 3;
    return v + 1;
}

static inline void StoreSpeed(struct ChaseActor *a, u32 v)
{
    a->base.paletteTimer = v;
}

void Mummy_UpdateDefeat(struct ChaseActor *actor, s32 frame)
{
    s32 values[8];
    struct ContactPackedVector rangeA;
    struct ContactPackedVector rangeB;
    struct ScriptUtilityArguments arguments;
    struct SceneActorAnimationSequenceDrawable *drawable = actor->base.drawable;
    struct DisplaySequenceState *anim = &drawable->sequence;
    u16 script;
    u16 *motionFlags;
    struct ContactCollider *collider;
    if (SceneActorReaction_TakePrepared((void *)actor)) {
        struct SceneActorChaseGroup *group = &actor->group;
        if (!Mummy_DisplayFlags(actor, 0x20000)) {
            if (!actor->base.drawableKind) {
                u32 *flags = actor->base.drawable;
                s32 mask = ~1;
                *flags = mask & (*flags);
            } else
                HideTile(&((struct SceneActorAnimationTileDrawable *)actor->base.drawable)->tile);
        }
        {
            struct SceneActorRegistryControl *control = &group->common.control;
            u32 *flags = &actor->base.behaviorFlags;
            u32 mask = ~0x141C0;
            *flags &= mask;
            control->counter = 0;
            control->field14 = 0;
            control->timer = 0;
        }
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 10);
        actor->base.restart = 0;
        StoreSpeed(actor, 0x20);
        SceneActorAttachment_ReplaceResponseSlot(actor, 0, 127, 0);
        SceneActorAttachment_ReplaceResponseSlot(actor, 1, 127, 0);
        SceneActorAttachment_ReplaceResponseSlot(actor, 3, 127, 0);
        SceneActorEffects_AdvanceSteps(actor, 4);
        collider = &actor->base.colliderA;
        actor->base.colliderA.responseValue = 0;
        SceneActorPlacement_PlayCategoryEventSound(actor);
        if (Mummy_MotionFlags(actor, 8))
            Sound_PlaySong(0x12A);
        {
            collider->flags |= 4;
        }
    }
    if (Mod(frame, 12) == 0 && Mummy_MotionFlags(actor, 8)) {
        rangeA.x = 60;
        rangeA.y = 60;
        rangeA.z = 40;
        SparkParticle_Spawn(&actor->base.body.position, Random4(),
                            (const struct ContactPosition *)&rangeA,
                            actor->base.body.direction + 128, 10, 8, 2, 2);
    }
    if (frame <= 0x22)
        anim->elapsed = 0;
    SceneActorMovement_UpdateAirborne(actor);
    SceneActorMovement_UpdateHeight(actor);
    if (!actor->base.restart)
        return;
    actor->base.stepDone = 1;
    {
        u32 test = Mummy_ActorFlags(actor, 0x400);
        motionFlags = &actor->base.motionFlags;
        if (!test) {
            u8 category;
            if (!Mummy_HalfFlags(motionFlags, 0xA007)) {
                SceneEffect_EmitPositionPattern(actor);
                SceneStimulus_AddUntagged(actor->base.body.position.x, actor->base.body.position.z,
                                          0x400, 0, 1);
            } else {
                rangeB.x = 80;
                rangeB.y = 60;
                rangeB.z = 60;
                {
                    const u16 *table = gRandomTable;
                    SCENE_ACTOR_SPAWN_DEFEAT_SPARK_RING(
                        actor, (const struct ContactPosition *)&rangeB, table, 32);
                    SCENE_ACTOR_SPAWN_DEFEAT_SPARK_RING(
                        actor, (const struct ContactPosition *)&rangeB, table, 64);
                    SCENE_ACTOR_SPAWN_DEFEAT_SPARK_RING(
                        actor, (const struct ContactPosition *)&rangeB, table, 94);
                    SCENE_ACTOR_SPAWN_DEFEAT_SPARK_RING(
                        actor, (const struct ContactPosition *)&rangeB, table, 128);
                    SCENE_ACTOR_SPAWN_DEFEAT_SPARK_RING(
                        actor, (const struct ContactPosition *)&rangeB, table, 196);
                    SCENE_ACTOR_SPAWN_DEFEAT_SPARK_RING(
                        actor, (const struct ContactPosition *)&rangeB, table, 240);
                }
                category = actor->base.category;
                if (category == 4 || category == 1 || category == 5 || category == 7 ||
                    category == 6 || category == 0x18 || category == 0x19)
                    Sound_PlaySong(0xE5);
            }
        }
    }
    SceneActorBehavior_CallTerrainScript(actor);
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
    {
        if (!SceneActorChase_FlagComponentBits((void *)motionFlags, 0x4000) &&
            (u32)(actor->base.category - 11) > 2)
            Bestiary_UnlockEntry(
                Bestiary_GetEntryForActor(actor->base.category, actor->base.paletteOffset));
        actor->base.attachmentTimer = 0;
        SceneActorChase_SetTypedControl(actor, SceneActorControl_OnIdle, 16);
        SceneActorChase_SelectState(actor, 1, 0, 2);
        SceneActorChase_ResetStateEntry(actor);
    }
    if (!actor->base.drawableKind)
        *((u32 *)actor->base.drawable) |= 1;
    else
        ShowTile(&((struct SceneActorAnimationTileDrawable *)actor->base.drawable)->tile, 1);
    ActorShadow_Disable(actor->base.shadow);
}

// confidence=low: control reacquires a player route once the context delay clears.

static inline void SetHalf(u16 *field, u32 bits)
{
    s16 mask = bits;
    mask |= *field;
    *field = mask;
}

static inline u32 ClearWordValue(u32 *field, u32 bits)
{
    return *field & ~bits;
}

static inline u16 MotionFlags(struct ChaseActor *actor, u32 mask)
{
    u16 *field = &actor->base.motionFlags;
    u32 value = *field;
    return value & mask;
}

static inline u8 Test16(u16 *p, u32 mask)
{
    if (*p & mask)
        return 1;
    return 0;
}

struct MummyRandomTable {
    const u16 *samples;
};

static inline u32 PickTurn(u32 dir, const u16 *randomSamples)
{
    s32 i;
    struct MummyRandomTable table = {randomSamples};
    for (i = 0; i < 4; i++) {
        const u16 *samples = (&table)->samples;
        u32 index = (gRandomIndex + 1) & 1023;
        u32 d;
        gRandomIndex = index;
        d = samples[index];
        d &= 3;
        d <<= 6;
        if (d != dir)
            return d;
    }
    return dir;
}

struct MummyTurnBytes {
    u8 direction : 8;
    u8 saved : 8;
    u8 remaining : 8;
};

struct MummySavedControl {
    void *target;
    u8 state, phase, behavior, kind;
};

static inline void Turn(u8 saved, void *ctx, const u16 *randomSamples)
{
    struct ChaseActor *actor = ctx;
    u8 *group = (u8 *)&actor->group.common;
    {
        u32 mask = 0x400;
        if (((struct ChaseActor *)ctx)->base.motionFlags & mask)
            return;
    }
    if (Test16((u16 *)((u8 *)ctx + 0x414), 4))
        return;
    ((struct MummyTurnBytes *)(group + 0x204))->direction =
        PickTurn(actor->base.body.direction, randomSamples);
    {
        struct MummySavedControl *save = (struct MummySavedControl *)(group + 0x8C);
        save->target = actor->base.target;
        save->kind = actor->base.controlKind;
        save->state = actor->base.state2A8;
        save->phase = actor->base.phase2A9;
        save->behavior = actor->base.behavior2AA;
    }
    group[0x205] = saved;
    Chase_SetCtl(actor, *(void **)(group + 0x21C), 0x1B);
    Chase_SetState(actor, 3, 1, 0);
    group[0x206] = 6;
}

static inline s32 PlayerSlot(u8 *target)
{
    if (target[8] <= 3)
        return target[8];
    return -1;
}

static inline u8 Arrive(void *obj)
{
    struct ContactPosition target;
    struct ChaseActor *actor = obj;
    u8 *group = (u8 *)&actor->group.common;
    struct ChaseTarget **slot = (struct ChaseTarget **)((u8 *)actor + 0xDC);
    s32 cell;
    s32 player;
    const u16 *randomSamples = gRandomTable;
    Chase_SnapToRoute(*(struct ContactPosition **)((u8 *)actor + 0x5D4), (u8 *)actor + 0x448);
    if (Chase_Test(gEngineFlags, 0x4800)) {
        struct ChaseTarget *t = *slot;
        if (!t)
            goto invoke;
        player = PlayerSlot((u8 *)t);
        if (player < 0) {
            goto invoke;
        }
        cell = ((u16 *)gPlayers[player]->followTarget)[3];
    } else {
        struct ChaseTarget *t = *slot;
        cell = Chase_CellIndex(t->position.x >> 8, t->position.z >> 8);
        player = 0;
    }
    if (cell != actor->base.terrainCell && !Chase_Bits32(&gPlayers[player]->statusFlags, 0x20000))
        goto reroute;
    if (MotionFlags(actor, 0x400))
        return 0;
    {
        struct ChaseFlags40 *flags = (void *)(group + 0x40);
        s32 mask = ~4;
        flags->bits = mask & flags->bits;
    }
    if (MotionFlags(actor, 0x80)) {
        Chase_SetCtl(actor, *(void **)(group + 0x48), 0x1A);
        Chase_SetBehavior(actor, 12);
    } else {
        Chase_SetCtl(actor, actor->base.onAlternateEntry, 1);
        Chase_SetBehavior(actor, 5);
        Chase_SnapToRoute(*(struct ContactPosition **)(group + 0x200), group + 0x74);
        (*(struct ContactPosition **)(group + 0x200))->y =
            Chase_HeightAt(Chase_CellAt(*(struct ContactPosition **)(group + 0x200)),
                           *(struct ContactPosition **)(group + 0x200));
        (*(struct ContactPosition **)(group + 0x200))->pad = 0;
    }
    {
        u8 saved = actor->base.body.direction;
        Turn(saved, obj, randomSamples);
    }
    SceneActorTerrain_RequestRetreatRoute(obj);
    actor->base.field2C2 = 0x20;
    return 0;
reroute: {
    struct ContactPosition *targetPointer = &target;
    s32 x = Mod(cell, *(s16 *)(gTerrainMap->cells + 4));
    s32 z = Div(cell, *(s16 *)(gTerrainMap->cells + 4));
    u8 *data;
    targetPointer->x = (x << 8) + 0x80;
    data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data += 4;
    else
        data = gTerrainMap->cells + (cell * 4 + 12);
    targetPointer->y = (*data & 15) << 8;
    targetPointer->z = (z << 8) + 0x80;
    if (Chase_TryRoute(obj, *(void **)(group + 0x1FC), &actor->base.body.position, &target))
        goto moved;
    goto invoke;
}
invoke:
    Callback_InvokeActor(actor, *(void **)(group + 0x224));
    return 0;
moved:
    return 1;
}

// confidence=low: control callback reacquires the player and resumes a route.

u8 BouncingObject_RefreshNearbyHeading(struct ChaseActor *);

static inline u8 TakeControlEntry(struct ChaseActor *actor)
{
    if (actor->base.field2B2) {
        actor->base.field2B2 = 0;
        return 1;
    }
    return 0;
}

void Mummy_UpdatePlayerRoute(struct ChaseActor *actor)
{
    struct Mummy *body = actor->base.context;
    struct SceneActorChaseGroup *group = &actor->group;
    if (TakeControlEntry(actor)) {
        u32 mask;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        mask = 4;
        actor->base.statusFlags |= mask;
    }
    if (BouncingObject_RefreshNearbyHeading(actor))
        Chase_SetBehavior(actor, 16);
    if (actor->base.behavior2AA == 16)
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 1);
    else if (actor->base.behavior2AA == 18)
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    actor->base.field2C2 = 32;
    if (!body->savedTick) {
        SceneActorChase_TrackPlayer(actor);
        {
            SetHalf((u16 *)&group->common.gate.flags, 4);
        }
        actor->base.statusFlags = ClearWordValue(&actor->base.statusFlags, 4);
        {
            struct ChaseFlags40 *field = (void *)&body->flags6F2;
            s32 mask = ~0x1400;
            field->bits = mask & field->bits;
        }
        if (Arrive(actor)) {
            Chase_SetCtl(actor, group->common.gate.control, 0x25);
            Chase_SetBehavior(actor, 12);
        }
    } else
        actor->base.counter2AC++;
}

// confidence=low: consumes a nearby stimulus and attempts a tagged-position route.

u8 SceneStimulus_FindOverlap(s16, s16, u16, u8, u16);

void SceneActorPosition_ResolveTaggedPosition(void *, void *, void *, s8);

u8 SceneActor_TryTrackNearbyPlayer(struct ChaseActor *, struct ContactPosition *, u32);

void SceneGrid_SnapPositionToCellCenter(struct ContactPosition *, u32);

void SceneActorAttachment_RetargetNearestPlayer(void *, void *);

struct MummyStimulusInfo {
    u16 field4C, radius;
    u32 field50;
    u16 timer : 16;
    u8 phase[1], kind;
};

struct MummyStimulusRouteLink {
    u8 unknown0[8];
    u16 *point;
};

struct MummyStimulusGroup {
    u8 unknown0[0x40];
    u16 flags, timerLimit, movementTimer;
    u8 unknown46[6];
    struct MummyStimulusInfo stimulus;
    struct ContactPosition target;
    u8 unknown60[4];
    u32 cell;
    void *control;
    u8 unknown6C[8];
    struct MummyStimulusRouteLink link;
    u8 unknown80[0x17C];
    void *route;
    struct ContactPosition *destination;
    u8 unknown204[0x20];
    void *fallback;
};

struct MummyStimulusContext {
    u8 unknown0[0x6EA];
    u8 phase[1];
    u8 unknown6EB[7];
    u16 flags;
};

static inline u8 Mummy_TryRoute_defeat(u16 *flags, u32 mask, void *route,
                                       struct ContactPosition *from, struct ContactPosition *to)
{
    s32 result = Nav_IsPositionInMesh(route, from, to);
    if (!result) {
        *flags &= ~mask;
        return 0;
    }
    result = Terrain_ApplyBlocked(route, from, to);
    if (result) {
        if (result != 2)
            Memory_Clear(from + 1, 8);
        return 1;
    }
    *flags &= ~mask;
    return 0;
}

static inline u8 Mummy_TestContactFlags(struct SceneActorReactionMotion *motion, u32 mask)
{
    if (motion->flags & mask)
        return 1;
    return 0;
}

static inline u32 ActorWordBits(struct ChaseActor *a, u32 mask)
{
    return a->base.behaviorFlags & mask;
}

static inline u16 ActorHalfBits(struct ChaseActor *a, u32 mask)
{
    u16 value = a->base.motionFlags;
    value &= mask;
    return value;
}

static inline u32 ClearActivity(u32 *flags, u32 mask)
{
    u32 value = *flags;
    value &= mask;
    *flags = value;
    return value;
}

static inline u8 Mummy_ShouldSkipStimulus(u8 *object)
{
    struct ChaseActor *a = (void *)object;
    u32 motion;
    motion = ClearActivity(&a->base.statusFlags, 0xFEFFFFFF);
    if (!ActorWordBits(a, 0x200000))
        return 1;
    if (motion & 4)
        return 1;
    if (ActorWordBits(a, 0x100) || ActorHalfBits(a, 0xE13F) ||
        Mummy_TestContactFlags((void *)&a->base.tracking.target, 16))
        return 1;
    if (((struct ChaseActor *)object)->base.hitLockTimer)
        return 1;
    {
        u16 *timer = &((struct ChaseActor *)object)->base.colliderA.responseValue;
        object = (u8 *)timer;
        if (*timer)
            return 1;
    }
    return 0;
}

u32 Mummy_TryStimulusRoute(struct ChaseActor *a)
{
    struct MummyStimulusGroup *group;
    struct MummyStimulusContext *body;
    s8 tag;
    u32 started;
    u16 *motionFlags;
    if (Mummy_ShouldSkipStimulus((u8 *)a))
        goto failed;
    group = (void *)&a->group.common;
    ((struct MummyStimulusGroup *)&a->group.common)->stimulus.phase[0] = 0;
    started = 0;
    body = a->base.context;
    tag = SceneStimulus_FindOverlap(
        a->base.body.position.x, a->base.body.position.z,
        ((struct MummyStimulusGroup *)&a->group.common)->stimulus.radius,
        ((struct MummyStimulusGroup *)&a->group.common)->stimulus.kind, (u16)a->base.body.id);
    if (tag < 0)
        goto failed;
    if (!((struct MummyStimulusGroup *)&a->group.common)->stimulus.timer) {
        ((struct MummyStimulusGroup *)&a->group.common)->stimulus.timer = 180;
        SceneActorChase_SetControl(a, group->control, 9);
        SceneActorChase_SetBehavior(a, 11);
        {
            u32 mask = 0x4000;
            body->flags = mask | (body->flags);
        }
        started = 1;
    }
    if (SceneActorStimulus_CaptureMotionFlags(a, 0x80, &motionFlags)) {
        SceneActorStimulus_ExtendGate(a);
    }
    SceneActorPosition_ResolveTaggedPosition(&group->stimulus, &a->base.body.id, group->route, tag);
    if (SceneActor_TryTrackNearbyPlayer(a, &group->target, group->cell))
        goto success;
    Chase_SnapToRoute(group->destination, (u8 *)&group->link);
    (group->destination)->y = a->base.body.position.y;
    (group->destination)->pad = 0;
    SceneGrid_SnapPositionToCellCenter(&group->target, group->cell);
    if (!Mummy_TryRoute_defeat(motionFlags, 0x200, group->route, &a->base.body.position,
                               &group->target)) {
        Callback_InvokeActor(a, group->fallback);
    success:
        return 1;
    }
    SceneActorChase_SetActorFlags(a, 16);
    a->base.field2C2 = Div(64, 3);
    SceneActorChase_ClearActorFlags(a, 0x800000);
    body->phase[0] = 4;
    {
        u32 mask = 0x200;
        *motionFlags = mask | *motionFlags;
    }
    {
        u32 mask = 0x1000000;
        a->base.statusFlags |= mask;
    }
    SceneActorAttachment_RetargetNearestPlayer(a, &group->target);
    if (started)
        goto success;
failed:
    return 0;
}

void Mummy_UpdateAttackCollider(struct ChaseActor *actor);

struct SUpdateMovementTimers;

void Mummy_UpdateMovementTimers(struct SUpdateMovementTimers *s);

struct SUpdateMovementCooldown;

void Mummy_UpdateMovementCooldown(struct SUpdateMovementCooldown *s);

void Mummy_UpdateBurnDamage(struct ChaseActor *actor);

void Mummy_UpdateTemporaryFlagTimer(struct ChaseActor *actor);

s32 Mummy_UpdateBehavior(void *p)
{
    Mummy_UpdateAttackCollider(p);
    Mummy_UpdateMovementTimers(p);
    Mummy_UpdateMovementCooldown(p);
    Mummy_UpdateBurnDamage(p);
    Mummy_UpdateTemporaryFlagTimer(p);
    return 1;
}

struct SClearInactiveActionFlags;

void Mummy_ClearInactiveActionFlags(struct SClearInactiveActionFlags *s);

s32 Mummy_UpdateSpecialFlags(void *actor)
{
    Mummy_ClearInactiveActionFlags(actor);
    return 1;
}

void SceneActorAnimation_UpdateDirectionalEntry(u8 *p, s32 arg1);

void SceneActorBehavior_CallActionScript(u8 *);

void Mummy_EnterIdleControl(struct ChaseActor *actor);

struct SUpdateScriptedAction {
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

void Mummy_UpdateScriptedAction(struct SUpdateScriptedAction *obj)
{
    if (TakeFlag(((u8 *)obj) + 0x2B1)) {
        u16 *h;
        u16 value;
        s32 off;
        SceneActorChase_SetControl(obj, SceneActorAnimation_UpdateDirectionalEntry, 10);
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
    } else if (obj->done) {
        Mummy_EnterIdleControl((u8 *)obj);
    } else {
        ++(*((u32 *)(((u8 *)obj) + 0x2AC)));
    }
}

void Mummy_UpdateDelayedPursuit(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    SceneActorControl_TakeStateEntry(actor);
    if ((u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque))
        return;
    if ((s32)actor->base.counter2AC <= 60)
        goto waiting;
    SceneActorAttachment_ReplaceResponseSlot(actor, 0, 2, actor->base.category);
    SceneActorChase_SetState(actor, 3, 1, 0);
    SceneActorChase_SetControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
    if (SceneActorChase_MotionFlags(actor, 0x80)) {
        SceneActorChase_SetActorFlags(actor, 0x80);
        if (!SceneActorChase_Arrive(actor)) {
            if (!SceneActorChase_TryRoute(actor, group->route, &actor->base.body.position,
                                          &actor->base.tracking.target->position)) {
                Callback_InvokeActor(actor, group->fallback);
                return;
            }
        }
        SceneActorChase_SetFlagComponent(&group->common.gate.flags, 4);
        SceneActorDelay_Prepare((void *)&group->saved, group->common.gate.control, 12, 30);
    } else {
        SceneActorChase_SetActorFlags(actor, 0x10);
        SceneActorDelay_Prepare((void *)&group->saved, actor->base.onAlternateEntry, 7, 30);
        SceneActorChase_SnapToRoute(group->destination, &group->homeRoute);
        if (!SceneActorChase_TryRoute(actor, group->route, &actor->base.body.position,
                                      group->destination)) {
            SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0);
            SceneActorChase_SetState(actor, 3, 0, 0);
        } else
            group->destination->pad = 1;
    }
    return;
waiting:
    actor->base.counter2AC++;
}

extern const u8 gUnk_08606164[];

struct SInitActionHandlers {
    u8 filler[648];
    const u8 *p;
};

void Mummy_InitActionHandlers(struct SInitActionHandlers *s)
{
    s->p = gUnk_08606164;
}

// confidence=low: waits for a timer, then replaces response slot 0 and restarts the behavior.

void Mummy_WaitReplaceResponse(struct ChaseActor *actor)
{
    struct SceneActorRegistryControlGroup *group = &actor->group.common;
    if (Chase_TakeArrived(actor)) {
        DEBUG_PRINT();
    }
    if (!(u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque)) {
        if ((s32)actor->base.counter2AC >= group->gate.counter) {
            SceneActorAttachment_ReplaceResponseSlot(actor, 0, 2, actor->base.category);
            Chase_SetBehavior(actor, 2);
        } else {
            actor->base.counter2AC++;
        }
    }
}

// Tentative: consumes route probes and selects navigation or a descriptor turn.

s32 NavMesh_ConsumeProbeEvent(void *);

u8 SceneActorBehavior_RequestDescriptorTurn(void *, void *, s32, s32, s32);

u8 SceneActorBehavior_AccelerateTowardDescriptor(void *, void *, s32, s32, s32);

u8 SceneActorRoute_TryRecoveryDetour(void *);

u8 Mummy_RequestStraightRoute(struct ChaseActor *actor);

static inline u32 Mummy_MotionBits(struct ChaseActor *actor, u32 mask)
{
    return actor->base.motionFlags & mask;
}

static inline void Mummy_ApplyDuration(struct ChaseActor *a)
{
    u16 *scale;
    u32 mask = 0xA02F;
    if (a->base.motionFlags & mask) {
        u16 *p = &a->base.field2C2;
        *p = 64;
        scale = p;
    } else {
        u16 *q = &a->base.field2C2;
        u16 value = *q;
        scale = q;
        if (!value)
            *scale = 64;
    }
    if (!a->base.drawableKind)
        Anim_SetDurationScale(
            &((struct SceneActorAnimationSequenceDrawable *)a->base.drawable)->sequence, *scale);
    else
        Anim_SetFrameDurationScale(
            &((struct SceneActorAnimationTileDrawable *)a->base.drawable)->tile, *scale);
}

struct MummyProbeGroup {
    struct SceneActorChaseGroup *group;
};

static inline struct MummyProbeGroup Mummy_ProbeGroup(void *object)
{
    struct ChaseActor *actor = object;
    struct MummyProbeGroup owner;
    owner.group = SceneActorChase_GroupOf(actor);
    return owner;
}

static inline void Mummy_BeginProbeTurn(void *object, u32 saved, const u16 *randomSamples)
{
    struct ChaseActor *actor = object;
    struct MummyProbeGroup owner = Mummy_ProbeGroup(object);
    struct SceneActorChaseGroup *group = owner.group;
    if (!Mummy_MotionBits(actor, 0x400) &&
        !SceneActorChase_TestField((u16 *)&actor->group.common.gate.flags, 4)) {
        group->turn.direction =
            SceneActorChase_RandomDirectionExcept(actor->base.body.direction, randomSamples);
        SceneActorChase_SaveControl(&group->saved, actor);
        group->turn.savedDirection = saved;
        SceneActorChase_SetControl(actor, group->turnControl, 0x1B);
        SceneActorChase_SetState(actor, 3, 1, 0);
        group->turn.turnsRemaining = 2;
    }
}

static inline void Mummy_SetBehavior(struct ChaseActor *actor, u32 behavior)
{
    actor->base.state2A8 = 0;
    actor->base.phase2A9 = 0;
    actor->base.behavior2AA = behavior;
    SceneActorChase_ResetStateEntry(actor);
}

void Mummy_UpdateProbeNavigation(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    struct Mummy *body = actor->base.context;
    s32 speed;
    const u16 *randomSamples = gRandomTable;
    if (SceneActorControl_TakeStateEntry(actor)) {
        u16 *field = &body->flags6F2;
        u32 mask = ~0x1000;
        u32 value = *field;
        mask &= value;
        *field = mask;
    }
    if ((u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque))
        return;
    speed = Callback_InvokeActor(actor, actor->base.animationQuery.opaque);
    Mummy_ApplyDuration(actor);
    if (NavMesh_ConsumeProbeEvent(group->route)) {
        if (!SceneActorBehavior_RequestDescriptorTurn(actor, group->route, 32, 32, speed))
            goto increment;
        if (!SceneActorChase_TestField((u16 *)&group->common.gate.flags, 4)) {
            if (SceneActorRoute_TryRecoveryDetour(actor))
                return;
            if (SceneActorChase_TestField((u16 *)&group->common.gate.flags, 1)) {
                u32 saved = ((actor->base.nextDirection + 5) & 7) << 5;
                Mummy_BeginProbeTurn(actor, saved, randomSamples);
                return;
            }
        }
        Mummy_SetBehavior(actor, 13);
    } else if (SceneActorBehavior_AccelerateTowardDescriptor(actor, group->route, 32, 32, speed)) {
        if (SceneActorChase_Bits32(&gPlayers[0]->statusFlags, 8) && !body->pursuitDelay) {
            if (Mummy_RequestStraightRoute(actor))
                return;
        }
        Mummy_SetBehavior(actor, 14);
        return;
    }
increment:
    actor->base.counter2AC++;
}

/* This class's own copies of the shared chase inlines it compiles differently. */

static inline u8 Mummy_Arrive_pursuit(void *obj)
{
    struct ContactPosition target;
    struct ChaseActor *actor = obj;
    struct SceneActorChaseGroup *group = SceneActorChase_GroupOf(obj);
    struct SceneActorChaseTracking *tracking = &actor->base.tracking;
    s32 cell, player;
    const u16 *randomSamples = gRandomTable;
    void **callback;
    Mummy_SnapToRoute(group->destination, &group->homeRoute);
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
        Mummy_SnapToRoute(group->destination, &group->homeRoute);
        (group->destination)->y =
            SceneActorChase_HeightAt(Mummy_CellAt(group->destination), group->destination);
        (group->destination)->pad = 0;
    }
    {
        u8 saved = actor->base.body.direction;
        Mummy_BeginTurn(obj, saved, randomSamples);
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

struct MummyRouteTarget {
    struct SceneActorChaseTarget target;
    u8 unknown14[8];
    struct SceneActorChasePlayerCell *cell;
};

void Mummy_ResumeDelayedRoute(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    struct Mummy *body = actor->base.context;
    if (SceneActorControl_TakeStateEntry(actor)) {
        SceneActorChase_SetControl(actor, actor->base.onStanding, 0);
        SceneActorChase_ClearMotionFlagComponent(actor, 0x200);
        SceneActorChase_ClearWordFlags(&actor->base.behaviorFlags, 0x80);
        group->common.control.timer = 0;
    }
    if ((u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque))
        return;
    if ((s32)actor->base.counter2AC > 60) {
        SceneActorChase_ClearFlagComponent((void *)&group->common.gate.flags, 6);
        SceneActorChase_SetControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
        SceneActorDelay_Prepare((void *)&group->saved, group->common.gate.control, 12, 30);
        SceneActorChase_SetState(actor, 3, 1, 0);
        if (body->pursuitDelay &&
            actor->base.terrainCell ==
                ((struct MummyRouteTarget *)actor->base.tracking.target)->cell->index) {
            if (Mummy_RequestStraightRoute(actor))
                return;
            Mummy_ChooseRoute(actor);
            SceneActorMovement_ResolveRoutePosition(actor);
            SceneActorChase_SetFlagComponent(&group->common.gate.flags, 4);
        } else
            Mummy_Arrive_pursuit(actor);
    } else
        actor->base.counter2AC++;
}

// confidence=low: follows a navigation descriptor, switching between acceleration and turning.

static inline u32 TestFlags(struct ChaseActor *a, u32 mask)
{
    return a->base.motionFlags & mask;
}

void Mummy_UpdateDescriptorPursuit(struct ChaseActor *a)
{
    struct SceneActorChaseGroup *group = &a->group;
    s32 motion;
    s32 event;
    if (a->base.field2B1)
        a->base.field2B1 = 0;
    if ((u8)Callback_InvokeActor(a, a->base.contactQuery.opaque))
        return;
    motion = Callback_InvokeActor(a, a->base.animationQuery.opaque);
    Mummy_ApplyDuration(a);
    event = NavMesh_ConsumeProbeEvent(group->route);
    if (!event) {
        if (SceneActorBehavior_AccelerateTowardDescriptor(a, group->route, 32, 32, motion)) {
            SceneActorChase_SetBehavior(a, 0x12);
            return;
        }
    } else {
        if (SceneActorBehavior_RequestDescriptorTurn(a, group->route, 32, 32, motion))
            Chase_SetBehavior(a, 0x11);
    }
    a->base.counter2AC++;
}

s32 Callback_InvokeActor(void *, void *);

void Mummy_UpdatePlayerRoute(struct ChaseActor *actor);

void SceneActor_WaitForControlCompletion(struct ChaseActor *actor)
{
    u8 busy;
    if (SceneActorControl_TakeStateEntry(actor)) {
        void *control = actor->base.onEntry;
        SceneActorChase_SetControl(actor, control, 2);
    }
    busy = Callback_InvokeActor(actor, actor->base.contactQuery.opaque);
    if (!busy) {
        if (actor->base.stepDone) {
            SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
            SceneActorChase_SetBehavior(actor, 0x10);
            SceneActorChase_SetControl(actor, Mummy_UpdatePlayerRoute, 0x27);
        } else
            actor->base.counter2AC++;
    }
}

/* SceneActorControl_UpdatePersistentPhase_080D622C: SceneActorControl_UpdatePersistentPhase variant: control Mummy_UpdatePlayerRoute kind 39, no shadow toggle, update callback via Callback_InvokeActor (provisional name) */

void SceneActorControl_UpdatePersistentPhase_080D622C(struct ChaseActor *actor)
{
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        SceneActorChase_SetControl(actor, Mummy_UpdatePlayerRoute, 39);
    }
    if (!(u8)Callback_InvokeActor(actor, *(void **)((u8 *)actor + 0x270)))
        ++actor->base.counter2AC;
}

extern const u8 gUnk_0860616C[];

struct SSetControlPreset {
    u8 filler[644];
    const u8 *p;
};

void Mummy_SetControlPreset(struct SSetControlPreset *s)
{
    s->p = gUnk_0860616C;
}

// confidence=low: animation checkpoint emits aimed shots with rising speed and falling strength.

s32 Div(s32, s32);

void BouncingObject_Spawn(struct ContactPackedVector *, struct ContactPosition *,
                          struct ContactPosition *, u32, u32, u32, u32);

extern const s16 gUnk_086149C4[];

static inline s32 Scale(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -(-value >> 12);
}

static inline void Subtract(struct ContactPosition *out, struct ContactPosition *from,
                            struct ContactPosition *to)
{
    out->x = to->x - from->x;
    out->y = to->y - from->y;
    out->z = to->z - from->z;
}

static inline s32 Abs(s32 value)
{
    return value < 0 ? -value : value;
}

void Mummy_FireAcceleratingVolley(struct ChaseActor *actor)
{
    u8 *body = actor->base.context;
    struct ContactPackedVector source = *(struct ContactPackedVector *)&actor->base.body.position;
    struct ContactPosition delta;
    struct ContactPosition *velocity;
    s32 divisor, facing, difference;
    s32 heightDifference;
    s32 speed;
    s32 count, i;
    s32 direction;
    s32 strength;
    source.y += 200;
    heightDifference = source.y - actor->base.tracking.target->position.y;
    if (heightDifference <= 255)
        divisor = 15;
    else if (heightDifference <= 511)
        divisor = 20;
    else if (heightDifference <= 767)
        divisor = 23;
    else if (heightDifference <= 1023)
        divisor = 27;
    else
        divisor = 34;
    Subtract(&delta, &actor->base.body.position, &actor->base.tracking.target->position);
    {
        s32 x = delta.x, z;
        s32 ax = Abs(x), az;
        z = delta.z;
        az = Abs(z);
        if (ax > az)
            speed = Div(x, divisor);
        else
            speed = Div(z, divisor);
        speed = Abs(speed);
    }
    if (speed <= 10)
        speed = 10;
    if (speed > *(u16 *)(body + 0x6cc))
        speed = *(u16 *)(body + 0x6cc);
    switch (actor->base.paletteOffset) {
        case 0:
            count = 1;
            break;
        case 1:
            count = 2;
            break;
        default:
            count = 3;
            break;
    }
    facing = ((actor->base.body.direction + 32) & 255) >> 6 << 6;
    difference = actor->base.tracking.heading - facing;
    if (difference > 128)
        difference -= 256;
    else if (difference < -128)
        difference += 256;
    direction = difference;
    if (Abs(direction) > 28)
        direction = facing;
    else
        direction = actor->base.tracking.heading;
    i = count;
    strength = *(u16 *)(body + 0x6ce);
    for (; i != 0; i--) {
        velocity = &delta;
        velocity->x = Scale(gUnk_086149C4[(direction + 64) & 255] * speed);
        velocity->y = 0;
        velocity->z = Scale(gUnk_086149C4[direction & 255] * speed);
        {
            u32 shot = actor->base.attacks.first[2];
            u32 category = actor->base.attacks.second[2];
            void *target = &actor->base.tracking.target->position;
            BouncingObject_Spawn(&source, &delta, target, shot, category, strength,
                                 actor->base.body.id);
        }
        strength = Div(strength << 3, 10);
        speed += Div(speed << 2, 10);
    }
    Sound_PlaySong(0x101);
}

void SceneActorControl_OnIdle(u8 *);

void Mummy_EnterIdleControl(struct ChaseActor *actor)
{
    struct Mummy *context = actor->base.context;
    if (actor->base.tracking.distance < context->limit) {
        SceneActorChase_SetTypedControl(actor, SceneActorControl_OnIdle, 0x10);
        SceneActorChase_SelectState(actor, 0, 2, 0);
        SceneActorChase_ResetStateEntry(actor);
    } else {
        SceneActorChase_SetTypedControl(actor, SceneActorControl_OnIdle, 0x10);
        SceneActorChase_SelectState(actor, 0, 2, 1);
        SceneActorChase_ResetStateEntry(actor);
    }
}

/* This class's own copies of the shared chase inlines it compiles differently. */

static inline u8 Mummy_ArriveWithDirectFallback(void *obj)
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
            Callback_InvokeActor(actor, group->fallback);
            return 0;
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
    SceneActorChase_ClearFlagComponent((void *)&group->common.gate.flags, 4);
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
        Mummy_BeginTurn(obj, saved, randomSamples);
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

static inline u8 FindPath(struct ChaseActor *actor, s32 goal)
{
    s32 cell = actor->base.terrainCell;
    s32 y = actor->base.body.position.y;
    u32 kind = *SceneActorTerrain_GetCell(cell) >> 4;
    s32 total = SceneActorPath_Width() * SceneActorPath_Height();
    return SceneActorPath_CanReach(cell, goal, y, kind, total);
}

u8 Mummy_TryDirectPursuit(struct ChaseActor *actor)
{
    struct Mummy *body = actor->base.context;
    struct SceneActorChaseGroup *group = &actor->group;
    s32 goal = ((struct MummyRouteTarget *)actor->base.tracking.target)->cell->index;
    if (group->common.control.counter != gTrackedPlayerId[0])
        return 0;
    if (goal == actor->base.terrainCell)
        return 1;
    if (FindPath(actor, goal))
        return 1;
    if ((!body->pursuitDelay) && (actor->base.terrainCell == goal)) {
        Mummy_RequestStraightRoute(actor);
        return 0;
    }
    SceneActorChase_TrackPlayer(actor);
    Mummy_ArriveWithDirectFallback(actor);
    return 0;
}

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
    struct Player *player = gPlayers[0];
    u32 mask = 8;
    if (player->statusFlags & mask)
        return TRUE;
    return FALSE;
}

#define FIELD(p, off, T) (*(T *)((u8 *)(p) + (off)))

// confidence=low: scans to the last open cell in the current facing and starts a route.

u8 Mummy_RequestStraightRoute(struct ChaseActor *actor)
{
    struct Mummy *state = actor->base.context;
    struct SceneActorChaseGroup *nav = &actor->group;
    s32 dest;
    s32 dir;
    struct ContactPosition pos;
    u32 result;

    if (state->pursuitDelay)
        return 0;
    dir = ((actor->base.body.direction + 0x20) >> 6) & 3;
    dest = SceneActorTerrain_LastOpenCell(actor, dir);
    if (dest == actor->base.terrainCell)
        return 0;
    SceneActorTerrain_GetCenter(dest, &pos);
    pos.y = actor->base.body.position.y;
    result = SceneActorTerrain_RequestPath(actor, nav->route, &actor->base.body.position, &pos);
    if (!result) {
        SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0);
        SceneActorChase_SetState(actor, 3, 0, 0);
        return 0;
    }
    state->pursuitDelay = 0xF0;
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

static inline u8 MotionStarted(struct SceneActorReactionMotion *motion)
{
    u32 mask = 8;
    if (motion->flags & mask)
        return 1;
    return 0;
}

void SceneActor_EnterMotionPhase(struct ChaseActor *actor)
{
    struct Mummy *parent;
    u32 started;
    SceneActorChase_ClearWordFlags(&actor->base.statusFlags, 4);
    parent = actor->base.context;
    SceneActorChase_ClearFlagComponent((void *)&parent->flags6F2, 0x1400);
    started = MotionStarted((void *)&actor->base.tracking.target);
    if (started) {
        u16 *motionFlags;
        SceneActorChase_GroupSetFlags(&actor->group, 4);
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
        SceneActorChase_SetPhase(actor, 1, 0);
        SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0x10);
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

void Mummy_OnMovementUpdate(struct ChaseActor *actor)
{
    u32 m = 0x100;
    if (!(actor->base.motionFlags & m))
        Mummy_RequestStraightRoute(actor);
}

struct MummyContactTarget {
    struct SceneActorChaseTarget target;
    u8 unknown14[8];
    struct SceneActorChasePlayerCell *cell;
};

s32 Mummy_CanPassMovementThreshold(struct SCanPassMovementThreshold *s);

u8 Mummy_TryDirectPursuit(struct ChaseActor *actor);

void SceneActorTarget_ResetContactResponse(void *);

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

static inline u8 Mummy_Arrive_path(void *obj, struct SceneActorChaseTracking *tracking)
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
    SceneActorChase_ClearFlagComponent((void *)&group->common.gate.flags, 4);
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
            (*target) = &((struct SceneActorChasePlayer *)gPlayers[0])->target;
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
            Mummy_Arrive_path(actor, (void *)target);
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
    if (Mummy_ActorFlags_TryEngageTarget(actor, 0x80) &&
        (!Mummy_MotionFlags_TryEngageTarget(actor, 0x400)))
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
        if (t->h6d0)
            t->h6d0--;
        if (t->h6d6)
            t->h6d6--;
    }
}

void SceneActorRecord_RecordPair(u16, u16);

void Player_NotifyFirstSelectedSlot(u32, u32);

void Player_RefreshLifeState(u32, u32);

void sub_08159164(u32);

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
                SceneActorChase_SetActorFlags(actor, 0x8000);
                if (ShouldShowDamage(actor)) {
                    // concession-ok(const-local): CpuSet reads this fixed word twice to clear the damage buffer.
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
            hit = (u8 *)&actor->base.colliderA;
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

extern const u16 gUnk_086061B8[];

s32 Mummy_GetAnimationStepOffset(struct ChaseActor *actor)
{
    struct SceneActorAnimationSequenceDrawable *drawable = actor->base.drawable;
    struct DisplaySequenceState *q = &drawable->sequence;
    if (q->elapsed == 0) {
        s32 m = Mod(q->index, 4);
        u32 c = 0x10000;
        actor->base.statusFlags |= c;
        if (m != 0) {
            u16 h = actor->base.field2C2;
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
        if (t->h6d2)
            t->h6d2--;
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
    struct Mummy **slot = (void *)&actor->base.context;
    struct Mummy *context = *slot;
    u16 *counter;
    u32 v;
    u32 saved;
    counter = &context->flags6D4;
    v = *counter;
    saved = (unsigned short)v;
    if (saved) {
        *counter = v - 1;
        return 0;
    }
    if (BouncingObject_GetNearbyHeading(&actor->base.body.position) < 0) {
        u16 *flags = &(*slot)->flags6F2;
        u32 mask = ~0x400;
        *flags &= mask;
        *counter = saved;
        return 0;
    }
    {
        struct Mummy *control = *slot;
        u32 mask = 0x400;
        u16 *flags = &control->flags6F2;
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
    struct SceneActorChaseGroup *group = &actor->group;
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
        u32 range = group->common.control.radiusSquared;
        position = &actor->base.body.position;
        if (distance >= range)
            goto fail;
    }
    if (!(u8)SceneActorPath_FaceAndCheckStraightPath(actor))
        goto fail;
    SceneActorChase_ClearActorFlags(actor, 0x80);
    {
        struct Mummy *parent = actor->base.context;
        u32 mask = 0x800;
        u16 *flags = &parent->flags6F2;
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

static inline void ClearHalf(u16 *p, u32 mask)
{
    *p = *p & mask;
}

static inline u32 WordFlag(u8 *p, u32 mask, u32 off)
{
    return *(u32 *)(p + off) & mask;
}

void Mummy_ResetResponseState(struct ChaseActor *actor)
{
    u8 *p = (u8 *)actor;
    struct SceneActorChaseTracking *body = (void *)(p + 0xDC);
    u32 mask = ~0x18;
    u32 *flags;
    u16 *motion;
    struct SceneActorRegistryControl *response;
    body->flags &= mask;
    flags = &actor->base.statusFlags;
    SceneActorChase_ClearWordFlags(flags, ROOM_ACTOR_STATUS_INTERACTING);
    actor->base.field2C2 = 64;
    actor->group.common.gate.flags = 0;
    actor->group.common.gate.counter = 0;
    motion = &actor->base.motionFlags;
    ClearHalf(motion, 0xFFFFFC7F);
    body->flags &= mask;
    response = &actor->group.common.control;
    if (WordFlag(p, 0x200000, 0x2CC)) {
        SceneActorChase_ClearWordFlags(flags, ROOM_ACTOR_STATUS_INTERACTING);
        ClearHalf(motion, 0xFFFFFDFF);
        response->mode = 0;
        response->counter = 0;
        response->timer = 0;
    }
    ClearHalf(motion, 0xFFFFFBFF);
    actor->group.turn.turnsRemaining = -1;
}

// confidence=low: clears mummy motion counters and restarts the directional animation.

void Mummy_ResetMotionState(struct S2 *s);

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

void Mummy_UpdateTemporaryFlagTimer(struct ChaseActor *actor)
{
    struct Mummy *context = actor->base.context;
    u8 *frames = &context->interactionFrames;
    if (*frames)
        --*frames;
    else {
        u32 *flags = &actor->base.statusFlags;
        u32 m = 0xFEFFFFFF;
        *flags = *flags & m;
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

/* This class's own copies of the shared chase inlines it compiles differently. */

static inline struct SceneActorChaseGroup *Mummy_GroupOf(struct ChaseActor *actor)
{
    return &actor->group;
}

static inline void Mummy_CellCenter_path(s32 cell, struct ContactPosition *position)
{
    s32 x = Mod(cell, Mummy_MapWidth(SceneActorChase_CurrentMap()));
    s32 z = Div(cell, Mummy_MapWidth(SceneActorChase_CurrentMap()));
    position->x = SceneActorChase_CellCenterCoordinate(x);
    {
        u32 attributes = SceneActorChase_CellProperties(cell)->attributes;
        u32 height = 15;
        height &= attributes;
        position->y = SceneActorChase_TerrainLevelHeight(height);
    }
    position->z = SceneActorChase_CellCenterCoordinate(z);
}

static inline void Mummy_BeginTurn_path(void *obj, u32 saved, const u16 *randomSamples)
{
    struct ChaseActor *actor = obj;
    struct SceneActorChaseGroup *group = Mummy_GroupOf(obj);
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

static inline u8 Mummy_Arrive_SelectMovementResponse(void *obj)
{
    struct ContactPosition target;
    struct ChaseActor *actor = obj;
    struct SceneActorChaseGroup *group = Mummy_GroupOf(obj);
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
        Mummy_BeginTurn_path(obj, saved, randomSamples);
    }
    SceneActorTerrain_RequestRetreatRoute(obj);
    actor->base.field2C2 = 0x20;
    return 0;
reroute: {
    Mummy_CellCenter_path(cell, &target);
    if (Mummy_TryRoute(obj, group->route, &actor->base.body.position, &target))
        goto success;
}
    callback = &group->fallback;
invoke:
    Callback_InvokeActor(actor, *callback);
    return 0;
success:
    return 1;
}

/* Choose a route, react to contact, or resume pursuit in motion-flag priority order. */

u8 Mummy_SelectMovementResponse(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    if (!actor->base.reactionSoundVariant)
        Sound_PlaySong(0x211);
    else
        Sound_PlaySong(0xFD);
    {
        u32 *word = &actor->base.statusFlags;
        s32 mask = ~4;
        *word &= mask;
    }
    if (SceneActorChase_QueryMotionFlags(actor, 0x10)) {
        SceneActorChase_ChooseRoute(actor);
        SceneActorMovement_ResolveRoutePosition(actor);
        SceneActorChase_SetBehavior(actor, 8);
        SceneActorChase_SetControl(actor, group->routeChoiceControl, 0x13);
        SceneActorChase_ClearMotionFlags(actor, 0x20);
        return 1;
    }
    if (SceneActorChase_QueryMotionFlags(actor, 0x2000)) {
        actor->base.body.direction = actor->base.reactionDirection + 0x80;
        SceneActorAnimation_SelectDirectionalEntry(&actor->base, 0);
        SceneActorChase_ClearMotionFlags(actor, 0x2020);
        SceneActorChase_SetActorFlags(actor, 0x10);
        group->destination->pad = 0;
        SceneActorChase_TrackPlayer(actor);
        if (!Mummy_Arrive_SelectMovementResponse(actor) &&
            !Mummy_TryRoute(actor, group->route, &actor->base.body.position,
                            &actor->base.tracking.target->position)) {
            Callback_InvokeActor(actor, group->fallback);
            return 0;
        }
        SceneActorChase_SetControl(actor, actor->base.onAlternateEntry, 1);
        SceneActorChase_SetBehavior(actor, 7);
        actor->base.field2C2 = Div(64, 3);
        return 1;
    }
    if (SceneActorChase_QueryMotionFlags(actor, 0x8002)) {
        SceneActorChase_ClearMotionFlags(actor, 0x20);
        return 0;
    }
    if (actor->base.hitReactionFlags & 0xC0000) {
        SceneActorChase_SetControl(actor, actor->base.onReaction, 0x12);
        SceneActorChase_SetState(actor, 3, 0, 0);
        SceneActorChase_ClearWordFlags(&actor->base.statusFlags, 0x1000000);
        return 1;
    }
    SceneActorChase_ClearMotionFlags(actor, 0x20);
    if (SceneActorChase_QueryMotionFlags(actor, 0x100))
        return 0;
    if (actor->base.hitLockTimer)
        return 0;
    if (SceneActorChase_QueryMotionFlags(actor, 0x80)) {
        SceneActorChase_BeginContactAnimation(actor);
        if (!actor->base.tracking.target)
            return 0;
        SceneActorChase_SetActorFlags(actor, 0x80);
        SceneActorChase_GroupSetFlags(group, 4);
        SceneActorChase_TrackPlayer(actor);
        if (!Mummy_Arrive_SelectMovementResponse(actor)) {
            if (!Mummy_TryRoute(actor, group->route, &actor->base.body.position,
                                &actor->base.tracking.target->position)) {
                Callback_InvokeActor(actor, group->fallback);
                return 0;
            }
        }
        SceneActorChase_SetControl(actor, group->common.gate.control, 0x1A);
        SceneActorChase_SetBehavior(actor, 12);
        return 1;
    } else {
        SceneActorChase_BeginContactAnimation(actor);
        if (!actor->base.tracking.target)
            return 0;
        SceneActorChase_SetActorFlags(actor, 0x10);
        group->destination->pad = 0;
        SceneActorChase_TrackPlayer(actor);
        if (!Mummy_Arrive_SelectMovementResponse(actor) &&
            !Mummy_TryRoute(actor, group->route, &actor->base.body.position,
                            &actor->base.tracking.target->position)) {
            Callback_InvokeActor(actor, group->fallback);
            return 0;
        }
        SceneActorChase_SetControl(actor, actor->base.onAlternateEntry, 1);
        SceneActorChase_SetBehavior(actor, 7);
        actor->base.field2C2 = Div(64, 3);
        return 1;
    }
}

/* This class's own copies of the shared chase inlines it compiles differently. */

static inline void Mummy_BeginTurn_ResumePlayerPursuit(void *obj, u32 saved,
                                                       const u16 *randomSamples)
{
    struct ChaseActor *actor = obj;
    struct SceneActorChaseGroup *group = SceneActorChase_GroupOf(obj);
    if (!SceneActorChase_MotionFlags(actor, 0x400) &&
        !SceneActorChase_TestField((u16 *)&group->common.gate.flags, 4)) {
        group->turn.direction =
            SceneActorChase_RandomDirectionExcept(actor->base.body.direction, randomSamples);
        Mummy_SaveControl(&group->saved, actor);
        group->turn.savedDirection = saved;
        SceneActorChase_SetControl(actor, group->turnControl, 0x1B);
        SceneActorChase_SetState(actor, 3, 1, 0);
        group->turn.turnsRemaining = 6;
    }
}

static inline u8 Mummy_Arrive_ResumePlayerPursuit(void *obj)
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
        Mummy_BeginTurn_ResumePlayerPursuit(obj, saved, randomSamples);
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

void SceneActor_UpdateDirectionalAnimation(void);

void Mummy_ResumePlayerPursuit(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    struct Mummy *body = actor->base.context;
    if (SceneActorControl_TakeStateEntry(actor)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        SceneActorChase_SetControl(actor, SceneActor_UpdateDirectionalAnimation, 3);
    }
    if (actor->base.stepDone) {
        SceneActorChase_ClearFlagComponent(&body->flags6F2, 0x800);
        SceneActorChase_TrackPlayer(actor);
        if (Mummy_Arrive_ResumePlayerPursuit(actor)) {
            SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
            SceneActorChase_SetControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
            SceneActorDelay_Prepare((void *)&group->saved, group->common.gate.control, 12, 30);
            SceneActorChase_SetState(actor, 3, 1, 0);
        }
    }
}

// confidence=low: checkpoint completion restores player-route control and retries the route.

void SceneActor_UpdateAnimationCheckpoint(void);

static inline void Mummy_ClearMotion(struct ChaseActor *actor, u32 bits)
{
    struct ChaseFlags40 *field = (void *)&actor->base.motionFlags;
    s32 mask = ~bits;
    field->bits &= mask;
}

static inline void Mummy_ClearCheckpointFlags(struct ChaseFlags40 *flags, s32 mask)
{
    flags->bits &= mask;
}

void Mummy_ResumeRouteAfterCheckpoint(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    struct Mummy *body = actor->base.context;
    if (Chase_TakeArrived(actor)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        SceneActorChase_SetControl(actor, SceneActor_UpdateAnimationCheckpoint, 0x26);
    }
    if (actor->base.stepDone) {
        s32 contextBits = 0x800;
        Mummy_ClearMotion(actor, 0x100);
        body->flags6F2 &= ~contextBits;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        Mummy_ClearCheckpointFlags((void *)&group->common.gate.flags, ~6);
        {
            u32 mask = 0x1000;
            body->flags6F2 = mask | body->flags6F2;
        }
        SceneActorChase_SetControl(actor, Mummy_UpdatePlayerRoute, 0x27);
        SceneActorChase_SetBehavior(actor, 0x10);
        if (!Chase_TryRoute(actor, group->route, &actor->base.body.position,
                            &actor->base.body.position)) {
            SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0);
            SceneActorChase_SetState(actor, 3, 0, 0);
        }
    }
}

extern const u8 gUnk_086061DC[];

struct SSetBehaviorTable {
    u8 filler[652];
    const u8 *p;
};

void Mummy_SetBehaviorTable(struct SSetBehaviorTable *s)
{
    s->p = gUnk_086061DC;
}

s32 Mummy_UpdateBehavior(void *);

void SceneActorControl_DispatchControl(void *);

s32 Mummy_UpdateSpecialFlags(void *);

struct SOnUpdate {
    u8 filler[0x27c];
    u32 *tbl;
    u8 filler2[0x2a8 - 0x280];
    u8 idx;
    u8 filler3[0x2b8 - 0x2a9];
    u16 h2b8;
    u8 filler4[0x2d4 - 0x2ba];
    u16 flags;
};

s32 Mummy_OnUpdate(struct SOnUpdate *s)
{
    u32 m;
    Mummy_UpdateBehavior(s);
    m = 0x2000;
    if ((s->flags & m) || s->h2b8 == 0) {
        Callback_InvokeActor(s, s->tbl[s->idx]);
        SceneActorControl_DispatchControl(s);
    }
    Mummy_UpdateSpecialFlags(s);
    return 1;
}

void SceneActor_UpdateBehaviorCallbacks(void);

s32 Mummy_OnInteractionUpdate(void)
{
    SceneActor_UpdateBehaviorCallbacks();
    return 0;
}

void SceneActorEffects_ReleaseResources(void *);

s32 Mummy_OnReleaseResources(u8 *p)
{
    SceneActorEffects_ReleaseResources(p);
    return 0;
}

void *Heap_Alloc(s32 size); /* first-fit allocation from gHeapHead; NULL on failure */

void SceneActorRegistry_SetObjectContext(void *, void *, s32, s32);

u32 SceneActorReaction_OnEnemyDefault(void);

void SceneActorReaction_ResolvePlacementDamage(void);

void SceneActorScript_OnCommand(void *actor, const u16 *command);

void SceneActorPlacement_HideActor(void *, s32);

s32 Anim_Step(void *, void *);

void SceneActorBehavior_InstallCallbackTables(void *);

void SceneActorTerrain_ReadScriptParameters(void *);

u8 SceneActorRegistry_LinkActor(void *);

void SceneActorBehavior_InitializeShadow(void *);

u8 Mummy_TryEngageTarget(struct ChaseActor *actor);

void SceneActorPlacement_ResetStateThree(void);

void SceneActorReaction_BeginTargetRecovery(void);

static inline void MummyInit_ResetFlags(void *object)
{
    struct Mummy *actor = object;
    actor->flags6F2 = 0;
}

void Mummy_InitProfile(struct SInitProfile *s);

void Mummy_InitCollision(u8 *p);

void Mummy_InitActionHandlers(struct SInitActionHandlers *s);

void Mummy_SetBehaviorTable(struct SSetBehaviorTable *s);

s32 Mummy_OnUpdate(struct SOnUpdate *s);

void Mummy_TurnTowardRoute(void *);

void Mummy_UpdateWalking(void *);

void Mummy_UpdateIdle(void *);

void Mummy_UpdateDisplacementRecovery(struct ChaseActor *actor, s32 frame);

void Mummy_UpdateHiddenRecovery(struct ChaseActor *actor, s32 frame);

void SceneActorTarget_ResolvePendingObject(void);

void Mummy_UpdateDying(u8 *s, s32 arg);

void Mummy_ResumeSavedTarget(struct ChaseActor *actor, s32 frame);

void Mummy_OnUnusedState(void);

void SceneActor_EnterMotionPhase(struct ChaseActor *actor);

u8 Mummy_SelectMovementResponse(struct ChaseActor *actor);

void Mummy_OnClearActivityFlag(struct SOnClearActivityFlag *s);

s32 Mummy_OnInteractionUpdate(void);

s32 Mummy_OnReleaseResources(u8 *);

static inline void MummyInit_EnableInitialFlags(struct Mummy *actor)
{
    MonsterInit_SetTargetingFlags(&actor->chase.base.tracking, 1);
    MonsterInit_SetBehaviorFlags(&actor->chase, 0x600000);
    MonsterInit_SetStatusFlags(&actor->chase, 0x8000000);
}

static inline struct Mummy *MummyInit_Context(struct Mummy *actor)
{
    return actor->chase.base.context;
}

static inline void MummyInit_InstallCompletionCallbacks(struct Mummy *arg0)
{
    arg0->chase.base.onMessage = SceneActorScript_OnCommand;
    arg0->chase.base.update = (SceneActorResultCallback)Mummy_OnInteractionUpdate;
    arg0->chase.base.destroy = (SceneActorResultCallback)Mummy_OnReleaseResources;
}

static inline void MummyInit_InitializePaletteRange(struct Mummy *actor, u32 selected,
                                                    u32 alternate, u32 current)
{
    actor->chase.base.alternateOffset = selected;
    actor->chase.base.alternatePalette = MonsterInit_GetPaletteBase(&actor->chase) + alternate;
    actor->chase.base.paletteBank = MonsterInit_GetPaletteBase(&actor->chase) + current;
}

void Mummy_SetControlPreset(struct SSetControlPreset *s);

void SceneActorEffects_InitAtActorPosition(void *);

void Mummy_UpdateRecoveryWait(void);

u32 Mummy_TryStimulusRoute(struct ChaseActor *a);

s32 Mummy_Init(void *object)
{
    struct Mummy *arg0 = object;
    struct Mummy *actor = object;
    struct SceneActorChaseGroup *group = &arg0->chase.group;
    // concession-ok(const-local): Both CpuSet fills read the same fixed source word.
    u32 zero;
    struct ContactPosition tmp;
    u32 flag;

    MummyInit_ResetFlags(arg0);
    SceneActorRegistry_SetObjectContext(actor, actor, 0, 1);
    MonsterInit_SetCategory(actor, MONSTER_CATEGORY_MUMMY);
    Mummy_InitProfile(actor);
    if (MonsterInit_GetDrawableKind(&actor->chase) == 0) {
        u8 *heap;
        actor->chase.base.drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->chase.base.drawable, 0x5C);
        heap = MonsterInit_Drawable(&actor->chase);
        MonsterInit_InitializeSprite(heap, 0x48DE, 0xA4B2);
        if (MonsterInit_GetCategory(actor) == 0x11 || MonsterInit_GetCategory(actor) == 0x17)
            heap[7] = 1;
        actor->chase.base.body.direction = 0;
    } else {
        u8 *heap;
        actor->chase.base.drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->chase.base.drawable, 0x80);
        heap = MonsterInit_Drawable(&actor->chase);
        MonsterInit_ClearPosition(&zero, &tmp);
        MonsterInit_InitializeWidget(heap, 0xCB05, 0x48DE, &tmp);
        actor->chase.base.animationIndex = 0xFF;
    }
    MonsterInit_InitializePursuitBody(arg0, &zero);
    MonsterInit_ReadPalette(&actor->chase, 0x23, 0x192);
    MummyInit_InitializePaletteRange(actor, MonsterInit_GetPaletteBase(&actor->chase) + 3, 4, 0);
    Mummy_InitCollision(actor);
    MonsterInit_InstallSequenceCallbacks(
        &actor->chase, Mummy_OnUpdate, Mummy_GetAnimationStepOffset, Mummy_TurnTowardRoute,
        Mummy_UpdateWalking, Mummy_UpdateIdle, Mummy_UpdateHiddenRecovery,
        Mummy_UpdateDisplacementRecovery, Mummy_UpdateDefeat, SceneActorReaction_OnEnemyDefault,
        Mummy_TryEngageTarget, SceneActorTarget_ResolvePendingObject,
        SceneActorReaction_ResolvePlacementDamage, SceneActorPlacement_ResetStateThree);
    MonsterInit_InitializeGroupWithBounds(
        group, &arg0->routeProbe, &arg0->destination, Mummy_UpdateDying, Mummy_ResumeSavedTarget,
        SceneActorReaction_BeginTargetRecovery, Mummy_OnUnusedState, 3, &actor->chase.base.body);
    SceneActorMovement_ResolveRoutePosition(arg0);
    MonsterInit_SetRouteReset(&actor->chase, Mummy_ResetRevivalMotion);
    MonsterInit_SetReactionGate(&actor->chase, SceneActor_EnterMotionPhase);
    MonsterInit_SetContactCallback(&actor->chase, Mummy_SelectMovementResponse);
    group->responseCallback = Mummy_OnClearActivityFlag;
    group->routeRestoreCallback = Mummy_OnMovementUpdate;
    group->routeChoiceControl = Mummy_UpdateRecoveryWait;
    group->common.fallback = Mummy_TryStimulusRoute;
    actor->chase.base.field2E8 = 1;
    MummyInit_EnableInitialFlags(actor);
    MonsterInit_MaskColliderFlags(&actor->chase.base.colliderA, ~1);
    SceneActorBehavior_InitializeShadow(actor);
    SceneActorEffects_InitAtActorPosition(arg0);
    SceneActorTerrain_ReadScriptParameters(actor);
    MonsterInit_ReadInitialMode(&arg0->chase);
    flag = 0;
    if (MonsterInit_TestMotionFlags(&actor->chase, 4)) {
        MonsterInit_SetStatusFlags(&actor->chase, 0x2000);
        flag = 1;
    }
    if (flag != 1) {
        MonsterInit_SetState(&actor->chase, 0, 0, 0);
        MonsterInit_SetControl(&actor->chase, MonsterInit_GetStandingControl(&actor->chase), 0);
    }
    MonsterInit_InitializeRegion(&actor->chase);
    if (MonsterInit_TestMotionFlags(&actor->chase, 0x40))
        MonsterInit_MaskColliderFlags(&actor->chase.base.colliderA, ~0x100);
    if (Script_GetKeywordValue(0x53, 0) != 0 || !MonsterInit_IsInCurrentRegion(&actor->chase))
        SceneActorPlacement_HideActor(actor, 0);
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    if (MonsterInit_GetDrawableKind(&actor->chase) == 0)
        DisplaySequence_Step(actor->chase.base.drawable);
    else
        Anim_Step(actor->chase.base.drawable + 0x20, actor->chase.base.drawable);
    MonsterInit_ClearReceiver(&actor->chase);
    SceneActorBehavior_InstallCallbackTables(actor);
    Mummy_SetControlPreset(actor);
    Mummy_InitActionHandlers(actor);
    Mummy_SetBehaviorTable(actor);

    arg0->field6E8 = actor->chase.base.tracking.x;
    {
        struct Mummy *context = MummyInit_Context(actor);
        if (Script_SeekToKeyword(0x42)) {
            context->field6CC = MonsterInit_ReadHalfword(0x14);
            context->currentTick = MonsterInit_ReadHalfword(0xB4);
        } else {
            context->field6CC = 0x14;
            context->currentTick = 0xB4;
        }
    }
    {
        struct Mummy *context = MummyInit_Context(actor);
        context->limit = Div(actor->chase.base.tracking.range * 3, 10);
        context->field6DC = actor->chase.base.tracking.range;
    }

    MonsterInit_SaveControl(MonsterInit_SavedControlOf(&actor->chase), &actor->chase);
    MummyInit_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}

s32 Mummy_Init(void *object);

void Heap_Free(void *);

void Mummy_Create(void)
{
    u8 *p = Heap_Alloc(0x720);
    if (p != 0) {
        Memory_Clear(p, 0x720);
        if (Mummy_Init(p) < 0) {
            Mummy_OnReleaseResources(p);
            Heap_Free(p);
        }
    }
}
