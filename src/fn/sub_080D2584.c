#include "scene_actor_displacement.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_reaction.h"
#include "spark_particle.h"

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

static inline u32 Mummy_Height(struct ContactPosition *position)
{
    return Mummy_HeightAt(SceneActorChase_CellAt(position), position);
}

static inline void Mummy_SnapToRoute(struct ContactPosition *position,
                                     struct SceneActorRegistryRoute *route)
{
    position->x = route->point->x;
    position->z = route->point->z;
    position->y = Mummy_Height(position);
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
        cell = ((struct SceneActorChasePlayer *)gUnknown_02000580[player])->cell->index;
    } else {
        struct SceneActorChaseTarget *t = tracking->target;
        cell = SceneActorChase_CellAt(&(t)->position);
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
        SceneActorChase_SetBehavior(actor, 12);
    } else {
        SceneActorChase_SetControl(actor, actor->base.onAlternateEntry, 1);
        SceneActorChase_SetBehavior(actor, 5);
        Mummy_SnapToRoute(group->destination, &group->homeRoute);
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
        (struct DisplaySequenceState *)((struct SceneActorAnimationSequenceDrawable *)
                                            actor->base.drawable)
            ->sequence;
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
