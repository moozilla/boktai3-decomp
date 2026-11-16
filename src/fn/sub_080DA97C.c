#include "scene_actor_chase_reaction.h"

/* This class's own copies of the shared chase inlines it compiles differently. */
static inline struct SceneActorChaseGroup *Mummy_GroupOf(struct ChaseActor *actor)
{
    return &actor->group;
}

static inline s32 Mummy_MapWidth(struct EngineMap *map)
{
    return ((struct SceneActorChaseTerrainGrid *)map->cells)->width;
}

static inline void Mummy_CellCenter(s32 cell, struct ContactPosition *position)
{
    s32 x = Mod(cell, Mummy_MapWidth(SceneActorChase_CurrentMap()));
    s32 z = Div(cell, Mummy_MapWidth(SceneActorChase_CurrentMap()));
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

static inline void Mummy_BeginTurn(void *obj, u32 saved, const u16 *randomSamples)
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

static inline u8 Mummy_Arrive(void *obj)
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
        if (!Mummy_Arrive(actor) && !Mummy_TryRoute(actor, group->route, &actor->base.body.position,
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
        if (!Mummy_Arrive(actor)) {
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
        if (!Mummy_Arrive(actor) && !Mummy_TryRoute(actor, group->route, &actor->base.body.position,
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
