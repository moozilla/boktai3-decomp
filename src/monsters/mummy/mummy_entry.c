#include "global.h"
#include "scene/actor/scene_actor_damage_flash.h"
#include "scene/actor/scene_actor_drawable_flags.h"
#include "scene/actor/scene_actor_reaction.h"
#include "scene/motion/scene_actor_availability.h"
#include "npc/manager/scene_actor_animation.h"
#include "scene/motion/scene_actor_chase_reaction.h"
#include "scene/collision/scene_actor_delayed_response.h"
#include "engine/video/display_sequence.h"
#include "scene/motion/scene_actor_displacement.h"
#include "effects/particles/spark_particle.h"
#include "script/script_utility.h"
#include "engine/engine.h"
#include "scene/actor/scene_actor_part_burst.h"
#include "scene/motion/scene_actor_chase.h"
#include "scene/collision/scene_actor_stimulus.h"
#include "scene/actor/scene_actor_control_phase.h"
#include "debug.h"
#include "monsters/mummy/mummy.h"
#include "monsters/skeleton/enemy_defeat_statistics.h"
#include "scene/motion/scene_actor_turn.h"
#include "scene/actor/scene_actor_reset.h"
#include "scene/motion/scene_actor_chase_direct_arrival.h"
#include "scene/motion/scene_actor_cardinal_path.h"
#include "scene/motion/scene_actor_terrain.h"
#include "player/player.h"
#include "scene/motion/scene_actor_straight_route.h"
#include "scene/collision/scene_actor_contacts.h"
#include "scene/motion/scene_actor_chase_shared.h"
#include "scene/areas/area_effect_link.h"
#include "engine/video/render.h"
#include "engine/runtime.h"
#include "scene/motion/scene_actor_adjacent_cell.h"
#include "engine/ram.h"
#include "scene/actor/scene_actor_control_motion.h"
#include "engine/video/display_sequence_inline.h"
#include "scene/actor/scene_actor_registry.h"
#include "engine/engine_globals.h"
#include "monsters/scene_actor_init_primitives.h"
#include "monsters/scene_actor_init_group.h"
#include "scene/actor/scene_actor_init_dispatch.h"
#include "monsters/scene_actor_init_graphics.h"
#include "monsters/scene_actor_init_body.h"
#include "scene/collision/scene_actor_contact_response.h"
#include "engine/word_flags.h"
#include "engine/math/movement_curve.h"
#include "scene/collision/scene_actor_damage_visibility.h"
#include "bosses/vanargand/vanargand_parts.h"

void SceneActorAnimation_SelectDirectionalEntry(struct RoomActor *, s32);

void SceneActorAnimation_UpdateDirectionalEntry(struct ChaseActor *actor, s32 frame)
{
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectDirectionalEntry(&actor->base, 0);
        actor->base.restart = 0;
    }
    if (frame > 0x10 && actor->base.restart && actor->base.stepDone != 1) {
        actor->base.stepDone = 1;
        SceneActorAnimation_SelectDirectionalEntry(&actor->base, 0);
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

static inline void Mummy_GetDirectionVariant(struct ChaseActor *actor, u8 *variant, u8 *mirror)
{
    s32 quadrant = ((actor->base.body.direction + 0x60) >> 6) & 3;
    if (quadrant > 1) {
        *mirror = 1;
        *variant = 3 - quadrant;
    } else {
        *mirror = 0;
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
        struct AnimSprite *tile =
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

void Mummy_UpdateRevival(void *);

void Mummy_UpdateHiddenRecovery(struct ChaseActor *actor, s32 frame)
{
    u32 queued;
    ActorStatus_SetDrawableFlags(actor, 1);
    queued = SceneActorReaction_TakePrepared(actor);
    if (queued) {
        ActorShadow_Disable(actor->base.shadow);
    } else if (frame > 0x1A) {
        {
            u8 directionVariant, mirror;
            Mummy_GetDirectionVariant(actor, &directionVariant, &mirror);
            Mummy_SelectAnimation(actor, 8, directionVariant, mirror | 4);
        }
        SceneActorChase_SetControl(actor, Mummy_UpdateRevival, 13);
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
        ActorStatus_ClearDrawableFlags(actor, 1);
        SceneActorReaction_BeginRevival(&actor->base, 1);
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

void Mummy_UpdateDying(struct ChaseActor *actor, s32 frame)
{
    struct SceneActorControlMotionDrawable *drawable = actor->base.drawable;
    struct DisplaySequenceState *sequence = &drawable->sequence;
    struct SceneActorChaseGroup *group = &actor->group;
    u32 mask;
    if (SceneActorReaction_TakePrepared(actor)) {
        u32 test;
        u16 *flags;
        u32 value;
        SceneActorAnimation_SelectDirectionalEntry(&actor->base, 8);
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
    if (Mod(frame, 60) == 0) {
        SceneActorAttachment_ReplaceResponseSlot(actor, 2, 0, actor->base.category);
    }
    if ((s16)group->field5C6 <= 0) {
        mask = 0x80;
        mask <<= 5;
        if (!(actor->base.statusFlags & mask)) {
            SceneActorChase_SetControl(actor, Mummy_UpdateDeath, 6);
            return;
        }
    }
    if (sequence->index == 2)
        sequence->elapsed = 0;
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
        &((struct SceneActorAnimationSequenceDrawable *)actor->base.drawable)->sequence;
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
        if (!Mummy_DisplayFlags(actor, 0x20000))
            ActorStatus_ClearDrawableFlags(actor, 1);
        {
            struct SceneActorRegistryControl *control = &group->common.control;
            SceneActorChase_ClearWordFlags(&actor->base.behaviorFlags, 0x141C0);
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
            if (!SceneActorTurn_Flags(motionFlags, 0xA007)) {
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
    ActorStatus_SetDrawableFlags(actor, 1);
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

static inline void Turn(u8 saved, void *ctx, const u16 *randomSamples)
{
    struct ChaseActor *actor = ctx;
    struct SceneActorChaseGroup *group = &actor->group;
    {
        u32 mask = 0x400;
        if (actor->base.motionFlags & mask)
            return;
    }
    if (SceneActorChase_TestField((u16 *)&group->common.gate.flags, 4))
        return;
    group->turn.direction =
        SceneActorChase_RandomDirectionExcept(actor->base.body.direction, randomSamples);
    {
        struct SceneActorChaseSavedControl *save = &group->saved;
        save->control = actor->base.target;
        save->kind = actor->base.controlKind;
        save->state = actor->base.state2A8;
        save->phase = actor->base.phase2A9;
        save->behavior = actor->base.behavior2AA;
    }
    group->turn.savedDirection = saved;
    Chase_SetCtl(actor, group->turnControl, 0x1B);
    Chase_SetState(actor, 3, 1, 0);
    group->turn.turnsRemaining = 6;
}

static inline u8 Arrive(void *obj)
{
    struct ContactPosition target;
    struct ChaseActor *actor = obj;
    struct SceneActorChaseGroup *group = &actor->group;
    struct SceneActorChaseTarget **slot = &actor->base.tracking.target;
    s32 cell;
    s32 player;
    const u16 *randomSamples = gRandomTable;
    Chase_SnapToRoute(actor->group.destination, (u8 *)&group->homeRoute);
    if (Chase_Test(gEngineFlags, 0x4800)) {
        struct SceneActorChaseTarget *t = *slot;
        if (!t)
            goto invoke;
        player = SceneActorChase_PlayerSlot(t);
        if (player < 0) {
            goto invoke;
        }
        cell = ((struct SceneActorChasePlayerCell *)gPlayers[player]->followTarget)->index;
    } else {
        struct SceneActorChaseTarget *t = *slot;
        cell = Chase_CellIndex(t->position.x >> 8, t->position.z >> 8);
        player = 0;
    }
    if (cell != actor->base.terrainCell && !Chase_Bits32(&gPlayers[player]->statusFlags, 0x20000))
        goto reroute;
    if (MotionFlags(actor, 0x400))
        return 0;
    {
        struct ChaseFlags40 *flags = (void *)&group->common.gate.flags;
        s32 mask = ~4;
        flags->bits = mask & flags->bits;
    }
    if (MotionFlags(actor, 0x80)) {
        Chase_SetCtl(actor, group->common.gate.control, 0x1A);
        Chase_SetBehavior(actor, 12);
    } else {
        Chase_SetCtl(actor, actor->base.onAlternateEntry, 1);
        Chase_SetBehavior(actor, 5);
        Chase_SnapToRoute(group->destination, (u8 *)&group->homeRoute);
        (group->destination)->y =
            Chase_HeightAt(Chase_CellAt(group->destination),
                           group->destination);
        (group->destination)->pad = 0;
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
    if (Chase_TryRoute(obj, group->route, &actor->base.body.position, &target))
        goto moved;
    goto invoke;
}
invoke:
    Callback_InvokeActor(actor, group->fallback);
    return 0;
moved:
    return 1;
}

// confidence=low: control callback reacquires the player and resumes a route.

u8 BouncingObject_RefreshNearbyHeading(struct ChaseActor *);

void Mummy_UpdatePlayerRoute(struct ChaseActor *actor)
{
    struct Mummy *body = actor->base.context;
    struct SceneActorChaseGroup *group = &actor->group;
    if (SceneActorReaction_TakePrepared(actor)) {
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
    if (!SceneActorTurn_ActorFlags(a, 0x200000))
        return 1;
    if (motion & 4)
        return 1;
    if ((a->base.behaviorFlags & 0x100) || ActorHalfBits(a, 0xE13F) ||
        SceneActorChase_TrackingFlags(&a->base.tracking, 16))
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
    struct SceneActorChaseGroup *group;
    struct Mummy *body;
    s8 tag;
    u32 started;
    u16 *motionFlags;
    if (Mummy_ShouldSkipStimulus((u8 *)a))
        goto failed;
    group = &a->group;
    group->common.control.mode = 0;
    started = 0;
    body = a->base.context;
    tag = SceneStimulus_FindOverlap(
        a->base.body.position.x, a->base.body.position.z,
        group->common.control.radius,
        group->common.control.active, (u16)a->base.body.id);
    if (tag < 0)
        goto failed;
    if (!group->common.control.timer) {
        group->common.control.timer = 180;
        SceneActorChase_SetControl(a, group->common.field43C, 9);
        SceneActorChase_SetBehavior(a, 11);
        {
            u32 mask = 0x4000;
            body->flags6F2 = mask | (body->flags6F2);
        }
        started = 1;
    }
    if (SceneActorStimulus_CaptureMotionFlags(a, 0x80, &motionFlags)) {
        SceneActorStimulus_ExtendGate(a);
    }
    SceneActorPosition_ResolveTaggedPosition(&group->common.control, &a->base.body.id, group->route, tag);
    if (SceneActor_TryTrackNearbyPlayer(a, &group->common.control.targetPosition, group->common.control.counter))
        goto success;
    SceneActorChase_SnapToRoute(group->destination, &group->homeRoute);
    (group->destination)->y = a->base.body.position.y;
    (group->destination)->pad = 0;
    SceneGrid_SnapPositionToCellCenter(&group->common.control.targetPosition, group->common.control.counter);
    if (!Mummy_TryRoute_defeat(motionFlags, 0x200, group->route, &a->base.body.position,
                               &group->common.control.targetPosition)) {
        Callback_InvokeActor(a, group->fallback);
    success:
        return 1;
    }
    SceneActorChase_SetActorFlags(a, 16);
    a->base.field2C2 = Div(64, 3);
    SceneActorChase_ClearActorFlags(a, 0x800000);
    body->interactionFrames = 4;
    SceneActorChase_SetFlagComponent((void *)motionFlags, 0x200);
    {
        u32 mask = 0x1000000;
        a->base.statusFlags |= mask;
    }
    SceneActorAttachment_RetargetNearestPlayer(a, &group->common.control.targetPosition);
    if (started)
        goto success;
failed:
    return 0;
}

void Mummy_UpdateAttackCollider(struct ChaseActor *actor);

void Mummy_UpdateMovementTimers(struct ChaseActor *s);

void Mummy_UpdateMovementCooldown(struct ChaseActor *s);

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

void Mummy_ClearInactiveActionFlags(struct ChaseActor *s);

s32 Mummy_UpdateSpecialFlags(void *actor)
{
    Mummy_ClearInactiveActionFlags(actor);
    return 1;
}

void SceneActorBehavior_CallActionScript(struct RoomActor *actor);

void Mummy_EnterIdleControl(struct ChaseActor *actor);

void Mummy_UpdateScriptedAction(struct ChaseActor *obj)
{
    if (SceneActorControl_TakeStateEntry(obj)) {
        u16 *h;
        SceneActorChase_SetControl(obj, SceneActorAnimation_UpdateDirectionalEntry, 10);
        obj->base.body.direction = obj->base.tracking.heading;
        SceneActorAttachment_ReplaceResponseSlot(obj, 0, 0, obj->base.category);
        SceneActorBehavior_CallActionScript(&obj->base);
        h = (u16 *)&obj->group.common.gate.flags;
        {
            u32 flag = 4;
            flag |= *h;
            *h = flag;
        }
        {
            u32 flag = 0x80;
            u16 *flags = &obj->base.motionFlags;
            flag |= *flags;
            *flags = flag;
        }
        obj->group.common.gate.counter = obj->group.common.gate.duration;
    } else if (obj->base.stepDone) {
        Mummy_EnterIdleControl(obj);
    } else {
        ++obj->base.counter2AC;
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
        actor->base.behaviorFlags |= 0x80;
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

void Mummy_InitActionHandlers(struct ChaseActor *s)
{
    s->base.alternateBehaviorCallbacks = (void **)gUnk_08606164;
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
    if (!SceneActorContact_MotionFlags(actor, 0x400) &&
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
    SceneActorTurn_ApplyActorDuration(actor);
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
                ((struct SceneActorContactTarget *)actor->base.tracking.target)->cell->index) {
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
    SceneActorTurn_ApplyActorDuration(a);
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
    if (!(u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque))
        ++actor->base.counter2AC;
}

extern const u8 gUnk_0860616C[];

void Mummy_SetControlPreset(struct ChaseActor *s)
{
    s->base.callbacks284 = (void **)gUnk_0860616C;
}

// confidence=low: animation checkpoint emits aimed shots with rising speed and falling strength.

s32 Div(s32, s32);

struct BouncingObject;
struct BouncingObject *BouncingObject_Spawn(struct ContactPosition *source,
                                           struct ContactPosition *delta,
                                           struct ContactPosition *target, u32 shot,
                                           u32 category, u32 strength, u32 id);

extern const s16 gUnk_086149C4[];

static inline s32 Scale(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -(-value >> 12);
}

static inline s32 Abs(s32 value)
{
    return value < 0 ? -value : value;
}

void Mummy_FireAcceleratingVolley(struct ChaseActor *actor)
{
    struct Mummy *body = actor->base.context;
    struct ContactPosition source = actor->base.body.position;
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
    MovementCurve_SubtractPositions(&delta, &actor->base.body.position, &actor->base.tracking.target->position);
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
    if (speed > body->field6CC)
        speed = body->field6CC;
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
    strength = body->currentTick;
    for (; i != 0; i--) {
        velocity = &delta;
        velocity->x = Scale(gUnk_086149C4[(direction + 64) & 255] * speed);
        velocity->y = 0;
        velocity->z = Scale(gUnk_086149C4[direction & 255] * speed);
        {
            u32 shot = actor->base.attacks.first[2];
            u32 category = actor->base.attacks.second[2];
            struct ContactPosition *target = &actor->base.tracking.target->position;
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
    s32 goal = ((struct SceneActorContactTarget *)actor->base.tracking.target)->cell->index;
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

static inline u8 CanReach(s32 cell, s32 goal, s32 y, u32 kind, s32 total)
{
    s32 i, col;
    if (cell < goal) {
        i = cell + 1;
        for (col = Mod(i, SceneActorTerrain_Width());
             col < SceneActorTerrain_Width() && SceneActorTerrain_CanTraverseCell(cell, i, y, kind); i++, col++) {
            if (i == goal)
                return 1;
        }
        for (i = cell + SceneActorTerrain_Width();
             total > i && SceneActorTerrain_CanTraverseCell(cell, i, y, kind); i += SceneActorTerrain_Width()) {
            if (i == goal)
                return 1;
        }
    } else {
        i = cell - 1;
        for (col = Mod(i, SceneActorTerrain_Width());
             col >= 0 && SceneActorTerrain_CanTraverseCell(cell, i, y, kind); i--, col--) {
            if (i == goal)
                return 1;
        }
        for (i = cell - SceneActorTerrain_Width(); i >= 0 && SceneActorTerrain_CanTraverseCell(cell, i, y, kind);
             i -= SceneActorTerrain_Width()) {
            if (i == goal)
                return 1;
        }
    }
    return 0;
}

static inline u8 FindPath_FaceAndCheckStraightPath(struct ChaseActor *actor, s32 goal)
{
    s32 cell = actor->base.terrainCell;
    s32 y = actor->base.body.position.y;
    u32 kind = *SceneActorTerrain_GetCell(cell) >> 4;
    s32 total = SceneActorTerrain_Width() * SceneActorTerrain_Height();
    return CanReach(cell, goal, y, kind, total);
}

u8 SceneActorPath_FaceAndCheckStraightPath(struct ChaseActor *actor)
{
    struct SceneActorContactTarget *target = (void *)actor->base.tracking.target;
    s32 goal = target->cell->index;
    u8 saved = actor->base.body.direction;
    struct ContactPosition delta;
    u8 result;

    {
        struct ContactPosition *d = &delta;
        struct ContactPosition *from = &actor->base.body.position;
        struct ContactPosition *to = &target->target.position;
        d->x = to->x - from->x;
        d->y = to->y - from->y;
        d->z = to->z - from->z;
    }
    actor->base.body.direction = Math_Atan2(delta.x, delta.z);
    if (goal == actor->base.terrainCell)
        result = 1;
    else if (!FindPath_FaceAndCheckStraightPath(actor, goal)) {
        actor->base.body.direction = saved;
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

s32 Mummy_CanPassMovementThreshold(struct ChaseActor *s)
{
    struct Mummy *context = s->base.context;
    if (s->base.tracking.distance >= context->limit && context->targetTick != 0)
        return 0;
    return 1;
}

void SceneActor_EnterMotionPhase(struct ChaseActor *actor)
{
    struct Mummy *parent;
    u32 started;
    SceneActorChase_ClearWordFlags(&actor->base.statusFlags, 4);
    parent = actor->base.context;
    SceneActorChase_ClearFlagComponent((void *)&parent->flags6F2, 0x1400);
    started = SceneActorChase_TrackingFlags(&actor->base.tracking, 8);
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
    if (!(SceneActorContact_MotionFlags(actor, 0x100)))
        Mummy_RequestStraightRoute(actor);
}

s32 Mummy_CanPassMovementThreshold(struct ChaseActor *s);

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

static inline void Mummy_SetTrackingFlags(struct SceneActorChaseTracking *tracking, u8 bits)
{
    tracking->flags |= bits;
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
    if (SceneActorContact_MotionFlags(actor, 0x10))
        return 0;
    if ((u8)Callback_InvokeActor(actor, actor->base.targetQuery))
        return 1;
    if (SceneActorContact_MotionFlags(actor, 0x100))
        return 0;
    if (!SceneActorContact_MotionFlags(actor, 0x200))
        return 0;
    if (!SceneActorChase_StatusFlags(actor, 0x1000000))
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
    cell = ((struct SceneActorContactTarget *)(*target))->cell->index;
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
    if (SceneActorTurn_ActorFlags(actor, 0x80) &&
        (!SceneActorContact_MotionFlags(actor, 0x400)))
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

void SceneActorCollision_InitBody(struct ChaseActor *, void *, void *, void *, void *);

void SceneActorCollision_InitAttack(struct ChaseActor *, void *, void *, void *, u32, void *);

void Mummy_OnBodyCollision(void *, void *, void *);

void Mummy_OnAttackCollision(u8 *, struct ContactCollider *, u8 *);

void Mummy_InitCollision(struct ChaseActor *actor)
{
    struct ContactPosition bodySize, bodyOffset, attackSize, attackOffset;
    bodySize.x = 0x20;
    bodySize.y = 0x80;
    bodySize.z = 0x20;
    bodyOffset.x = 0;
    bodyOffset.y = 0x80;
    bodyOffset.z = 0;
    SceneActorCollision_InitBody(actor, &bodySize, &bodyOffset, Mummy_OnBodyCollision, actor);
    attackSize.x = 0x20;
    attackSize.y = 0x64;
    attackSize.z = 0x20;
    attackOffset.x = 0;
    attackOffset.y = 0x80;
    attackOffset.z = 0;
    SceneActorCollision_InitAttack(actor, &attackSize, &attackOffset, Mummy_OnAttackCollision, 0, actor);
}

s32 SceneActorProfiles_FindOrAdd(u32, u32);

void Mummy_InitProfile(struct ChaseActor *actor)
{
    actor->base.profileId = SceneActorProfiles_FindOrAdd(1, 0);
}

// confidence=low: selects attack bounds and properties from mummy movement flags.

void Collider_SetAttackProperties(struct ContactCollider *, u32, u32, u32, u32, u32);

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

    if (SceneActor_ActionsBlocked(actor))
        return;
    if (actor->base.colliderA.responseValue)
        return;
    {
        u32 mask = 0x8000;
        flags = actor->base.motionFlags;
        if (mask & flags)
            return;
    }
    if (flags & 16) {
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
        collider->contactFlags = 16;
        collider->flags = 0x2201;
        ApplyAttack(actor, collider, 1, 0x20000);
    } else {
        struct ContactCollider *collider;
        ((struct ContactPosition *)&bounds)->x = 32;
        ((struct ContactPosition *)&bounds)->y = 100;
        ((struct ContactPosition *)&bounds)->z = 32;
        collider = &actor->base.colliderB;
        collider->bounds = *(struct ContactPosition *)&bounds;
        collider->contactFlags = 16;
        collider->flags = 0x2201;
        ApplyAttack(actor, collider, 0, 0);
    }
}

void Mummy_UpdateMovementTimers(struct ChaseActor *s)
{
    struct Mummy *t = s->base.context;
    u32 m = 0x40;
    if (!(s->base.motionFlags & m)) {
        if (t->targetTick != 0)
            --t->targetTick;
        if (t->savedTick != 0)
            --t->savedTick;
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

#define STATS ((struct EnemyDefeatStatistics *)gGameState)

static inline s32 clamp(s32 v)
{
    if (v > 9999)
        v = 9999;
    else if (v < 0)
        v = 0;
    return v;
}

/* Saturating defeat counters in the game-state block. */

#define STAT_INC(n) Count(&STATS->n)

#define STAT_ADD(n) STATS->n = clamp(STATS->n + 1)

#define KILL_KIND(flags)                                                                           \
    {                                                                                              \
        if ((flags) & 0x80)                                                                        \
            STAT_ADD(reactionCounts[0]);                                                                       \
        else if ((flags) & 0x100)                                                                  \
            STAT_ADD(reactionCounts[1]);                                                                       \
        else if ((flags) & 0x200)                                                                  \
            STAT_ADD(reactionCounts[2]);                                                                       \
        else if ((flags) & 0x400)                                                                  \
            STAT_ADD(reactionCounts[3]);                                                                       \
        else if ((flags) & 0x800)                                                                 \
            STAT_ADD(reactionCounts[4]);                                                                       \
    }

#define CASE_BODY(n)                                                                               \
    {                                                                                              \
        u32 flags = hit->reactionFlags;                                                          \
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

static inline u8 ShouldShowDamage(struct ChaseActor *actor)
{
    if (Test(gEngineFlags, 0x4800)) {
        u32 mask = actor->base.hitOwners;
        if (!(mask & 0x20) && mask) {
            switch (SceneActorDamage_LocalPlayer()) {
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

// confidence=low: timer-driven damage and defeat recording; the status is tentatively burn.

void Mummy_UpdateBurnDamage(struct ChaseActor *actor)
{
    struct BurnState *burn = (void *)&actor->group.common;
    u16 *flags2C0;
    u8 *category;
    struct ContactCollider *collider;
    struct ContactCollider *hit;
    u32 kind;
    s32 i;
    u32 *damageFlags;
    if (SceneActor_ActionsBlocked(actor))
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
            HalfwordFlags_Set(0x10, &actor->base.reactionFlags);
        }
    } else {
        if (Mod(burn->timer, 48) == 0) {
            Sound_PlaySong(0x121);
            HalfwordFlags_Set(0x20, &actor->base.reactionFlags);
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
            collider = initial;
            damageFlags = flags;
            if (!masked) {
                SceneActorDamage_Flash(actor);
                SceneActorChase_SetActorFlags(actor, 0x8000);
                if (ShouldShowDamage(actor)) {
                    // concession-ok(const-local): CpuSet reads this fixed word twice to clear the damage buffer.
                    u32 zero;
                    u32 buffer[2];
                    s32 health = actor->base.health;
                    u32 max = (u16)actor->base.defaultHealth;
                    s32 type = (s16)actor->base.hudKind;
                    s32 param = (s16)actor->base.hudLabel;
                    SceneHud_SetTimedState(actor->base.body.id, health, max, type, 0, param);
                    Fill(&zero, buffer);
                    if (actor->base.reactionSoundVariant || !(*damageFlags & 0x20000))
                        DamageNumberDisplay_Show(collider,
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
    SceneActorChase_SetControl(actor, actor->base.onDefeat, 17);
    if (Test(gEngineFlags, 0x4800)) {
        Player_NotifyFirstSelectedSlot(actor->base.hitOwners, actor->base.hitAttributes);
        goto done;
    }
    {
        u8 *address = &actor->base.category;
        u32 owner;
        kind = *address;
        owner = actor->base.hitOwners & 1;
        collider = &actor->base.colliderA;
        category = address;
        flags2C0 = &actor->base.placementFlags;
        if (!owner)
            goto record;
    }
    {
        u8 blocked = Test(Mask(flags2C0, 0x80), ~0);
        collider = &actor->base.colliderA;
        if (blocked)
            goto record;
    }
    {
        if (kind != 5 && kind != 7 && kind != 6) {
            STAT_INC(lifetimeTotal);
            STAT_INC(total);
            hit = &actor->base.colliderA;
            {
                u32 index = *category - 1; /* category 1..25 */
                collider = hit;
                switch (index) {
                    case 3:
                        CASE_BODY(category4);
                        break;
                    case 0:
                        CASE_BODY(category1);
                        break;
                    case 1:
                        CASE_BODY(category2);
                        break;
                    case 2:
                        CASE_BODY(category3);
                        break;
                    case 13:
                        CASE_BODY(category14);
                        break;
                    case 23:
                        CASE_BODY(category24);
                        break;
                    case 9:
                        CASE_BODY(category10);
                        break;
                    case 4:
                    case 5:
                    case 6:
                        CASE_BODY(category5To7);
                        break;
                    case 20:
                        CASE_BODY(category21);
                        break;
                    case 17:
                        CASE_BODY(category18);
                        break;
                    case 18:
                        CASE_BODY(category19);
                        break;
                    case 16:
                        CASE_BODY(category17);
                        break;
                    case 21:
                        CASE_BODY(category22);
                        break;
                    case 19:
                        CASE_BODY(category20);
                        break;
                    case 8:
                        CASE_BODY(category9);
                        break;
                    case 22:
                        CASE_BODY(category23);
                        break;
                    case 11:
                    case 12:
                        CASE_BODY(category12And13);
                        break;
                    case 24:
                        CASE_BODY(category25);
                        break;
                }
            }
        }
        sub_08159164(actor->base.experience);
        Player_RefreshLifeState(actor->base.hitAttributes, actor->base.fieldA1);
    }

record:
    if (!Test(Mask(flags2C0, 0x80), ~0) && !Test(gEngineFlags, 0x4800) && !Flag2D0(actor, 0x800))
        SceneActorRecord_RecordPair(gScriptEntryProcedure, actor->base.body.id);
    (collider)->flags |= 4;
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

void Mummy_UpdateMovementCooldown(struct ChaseActor *s)
{
    struct Mummy *t = s->base.context;
    u32 m = 0x40;
    if (!(s->base.motionFlags & m)) {
        if (t->pursuitDelay)
            t->pursuitDelay--;
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
    struct Mummy *context = actor->base.context;
    if (context->flags6D4) {
        context->flags6D4--;
        return 0;
    }
    if (BouncingObject_GetNearbyHeading(&actor->base.body.position) < 0) {
        struct Mummy *control = actor->base.context;
        u16 *flags = &control->flags6F2;
        u32 mask = ~0x400;
        *flags &= mask;
        context->flags6D4 = 0;
        return 0;
    }
    {
        struct Mummy *control = actor->base.context;
        u32 mask = 0x400;
        u16 *flags = &control->flags6F2;
        *flags = mask | *flags;
    }
    SceneActor_StartAdjacentCellRoute(actor);
    return 1;
}

static inline u8 IsPlayerTracked(struct ChaseActor *actor)
{
    u32 mask = 2;
    if (actor->base.placementFlags & mask)
        return 1;
    return 0;
}

u8 SceneActor_TryTrackNearbyPlayer(struct ChaseActor *actor, struct ContactPosition *input,
                                   u32 playerId)
{
    struct SceneActorChaseGroup *group = &actor->group;
    struct ContactPosition delta;
    struct ContactPosition *deltaPointer;
    struct ContactPosition *position;
    u32 distance;
    struct SceneActorChaseTracking *tracking;
    if (playerId != gTrackedPlayerId[0])
        goto fail;
    {
        struct Player *player = gPlayers[0];
        u32 mask = 8;
        if (!(player->statusFlags & mask))
            goto fail;
    }
    deltaPointer = &delta;
    Vanargand_Difference(deltaPointer, input, &actor->base.body.position);
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
    tracking = &actor->base.tracking;
    tracking->target = &((struct SceneActorChasePlayer *)gPlayers[0])->target;
    if (IsPlayerTracked(actor))
        tracking->target = &((struct SceneActorChasePlayer *)gPlayers[0])->target;
    else if (!tracking->target)
        goto ready;
    {
        struct SceneActorChaseTarget *target = tracking->target;
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

void Mummy_ResetMotionState(struct ChaseActor *s)
{
    SceneActorEffects_AdvanceSteps(s, 4);
    Memory_Clear(&s->group.motion5B8, 8);
    s->group.field5C2 = 0;
    s->group.field5C0 = 0;
}

// confidence=low: clears movement response flags and restores the response speed and sentinel.

static inline void ClearHalf(u16 *p, u32 mask)
{
    *p = *p & mask;
}

static inline u32 WordFlag(struct ChaseActor *actor, u32 mask)
{
    return actor->base.behaviorFlags & mask;
}

void Mummy_ResetResponseState(struct ChaseActor *actor)
{
    struct SceneActorChaseTracking *body = &actor->base.tracking;
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
    if (WordFlag(actor, 0x200000)) {
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

void Mummy_ResetMotionState(struct ChaseActor *s);

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

void Mummy_OnClearActivityFlag(struct ChaseActor *s)
{
    u32 *p = &s->base.statusFlags;
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
    struct ContactPosition position;
    s32 i;
    position = emitter->base.body.position;
    position.y += 0x40;
    switch (emitter->base.paletteOffset) {
        case 0:
            ExplosionEffect_CreateDelayed(&position, 0, emitter->base.attacks.first[2],
                                          emitter->base.attacks.second[2], 0, 0);
            break;
        case 1:
            for (i = 0; i < 2; i++) {
                position = emitter->base.body.position;
                position.y += 0x40;
                position.x += gUnk_086061D4[i];
                position.z += gUnk_086061D8[i];
                ExplosionEffect_CreateDelayed(&position, 0, emitter->base.attacks.first[2],
                                              emitter->base.attacks.second[2], 0, gUnk_086061C0[i]);
            }
            break;
        case 2:
            for (i = 0; i < 4; i++) {
                position = emitter->base.body.position;
                position.y += 0x40;
                position.x += gUnk_086061C4[i];
                position.z += gUnk_086061CC[i];
                ExplosionEffect_CreateDelayed(&position, 0, emitter->base.attacks.first[2],
                                              emitter->base.attacks.second[2], 0, gUnk_086061C0[i]);
            }
            break;
    }
}

void Mummy_ClearInactiveActionFlags(struct ChaseActor *s)
{
    if (s->base.controlKind != 0x27) {
        struct Mummy *context = s->base.context;
        u16 *p = &context->flags6F2;
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
        SceneActorChase_ClearWordFlags(&actor->base.statusFlags, 4);
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
        SceneActorChase_ClearMotionFlagComponent(actor, 0x100);
        body->flags6F2 &= ~contextBits;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        Mummy_ClearCheckpointFlags((void *)&group->common.gate.flags, ~6);
        {
            u32 mask = 0x1000;
            body->flags6F2 = mask | body->flags6F2;
        }
        SceneActorChase_SetControl(actor, Mummy_UpdatePlayerRoute, 0x27);
        SceneActorChase_SetBehavior(actor, 0x10);
        if (!SceneActorChase_TryRoute(actor, group->route, &actor->base.body.position,
                            &actor->base.body.position)) {
            SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0);
            SceneActorChase_SetState(actor, 3, 0, 0);
        }
    }
}

extern const u8 gUnk_086061DC[];

void Mummy_SetBehaviorTable(struct ChaseActor *s)
{
    s->base.behaviorCallbacks = (void **)gUnk_086061DC;
}

s32 Mummy_UpdateBehavior(void *);

void SceneActorControl_DispatchControl(void *);

s32 Mummy_UpdateSpecialFlags(void *);

s32 Mummy_OnUpdate(struct ChaseActor *s)
{
    u32 m;
    Mummy_UpdateBehavior(s);
    m = 0x2000;
    if ((s->base.motionFlags & m) || s->base.hitLockTimer == 0) {
        Callback_InvokeActor(s, s->base.stateCallbacks[s->base.state2A8]);
        SceneActorControl_DispatchControl(s);
    }
    Mummy_UpdateSpecialFlags(s);
    return 1;
}

void SceneActor_UpdateBehaviorCallbacks(void *);

s32 Mummy_OnInteractionUpdate(void *actor)
{
    SceneActor_UpdateBehaviorCallbacks(actor);
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

void Mummy_InitProfile(struct ChaseActor *actor);

void Mummy_InitCollision(struct ChaseActor *actor);

void Mummy_InitActionHandlers(struct ChaseActor *s);

void Mummy_SetBehaviorTable(struct ChaseActor *s);

s32 Mummy_OnUpdate(struct ChaseActor *s);

void Mummy_TurnTowardRoute(void *);

void Mummy_UpdateWalking(void *);

void Mummy_UpdateIdle(void *);

void Mummy_UpdateDisplacementRecovery(struct ChaseActor *actor, s32 frame);

void Mummy_UpdateHiddenRecovery(struct ChaseActor *actor, s32 frame);

void SceneActorTarget_ResolvePendingObject(void);

void Mummy_ResumeSavedTarget(struct ChaseActor *actor, s32 frame);

void Mummy_OnUnusedState(void);

void SceneActor_EnterMotionPhase(struct ChaseActor *actor);

u8 Mummy_SelectMovementResponse(struct ChaseActor *actor);

void Mummy_OnClearActivityFlag(struct ChaseActor *s);

s32 Mummy_OnInteractionUpdate(void *);

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

void Mummy_SetControlPreset(struct ChaseActor *s);

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
    Mummy_InitProfile(&actor->chase);
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
    Mummy_InitCollision(&actor->chase);
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
    SceneActorAnimation_SelectDirectionalEntry(&actor->chase.base, 0);
    if (MonsterInit_GetDrawableKind(&actor->chase) == 0)
        DisplaySequence_Step(actor->chase.base.drawable);
    else {
        struct SceneActorAnimationTileDrawable *drawable = actor->chase.base.drawable;
        Anim_Step(&drawable->tile, drawable);
    }
    MonsterInit_ClearReceiver(&actor->chase);
    SceneActorBehavior_InstallCallbackTables(actor);
    Mummy_SetControlPreset(&actor->chase);
    Mummy_InitActionHandlers(&actor->chase);
    Mummy_SetBehaviorTable(&actor->chase);

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
