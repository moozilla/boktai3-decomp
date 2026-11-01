// confidence=low: existing Golem state interpretation retained.
#include "global.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_reaction.h"
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
    struct SceneActorRegistryResetView *actor = object;
    packet->control = actor->target;
    packet->kind = actor->field2B5;
    packet->state = actor->state2A8;
    packet->phase = actor->phase2A9;
    packet->behavior = actor->behavior2AA;
}

static inline void Golem_BeginTurn(void *obj, u32 saved, const u16 *randomSamples)
{
    struct SceneActorRegistryResetView *actor = obj;
    struct SceneActorChaseGroup *group = SceneActorChase_GroupOf(obj);
    if (!SceneActorChase_MotionFlags(actor, 0x400) &&
        !SceneActorChase_TestField((u16 *)&group->common.gate.flags, 4)) {
        group->turn.direction =
            SceneActorChase_RandomDirectionExcept(actor->body.direction, randomSamples);
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
    struct SceneActorRegistryResetView *actor = obj;
    struct SceneActorChaseGroup *group = SceneActorChase_GroupOf(obj);
    struct SceneActorChaseTracking *tracking = &actor->tracking;
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
        cell = ((struct SceneActorChasePlayer *)gUnknown_02000580[player])->cell->index;
    } else {
        struct SceneActorChaseTarget *t = tracking->target;
        cell = Golem_CellIndex(t->position.x >> 8, t->position.z >> 8);
        player = 0;
    }
    if (cell != actor->terrainCell &&
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
        SceneActorChase_SetControl(actor, actor->entryTarget228, 1);
        SceneActorChase_SetBehavior(actor, 5);
        Golem_SnapToRoute(group->destination, &group->homeRoute);
        (group->destination)->y =
            SceneActorChase_HeightAt(Golem_CellAt(group->destination), group->destination);
        (group->destination)->pad = 0;
    }
    {
        u8 saved = actor->body.direction;
        Golem_BeginTurn(obj, saved, randomSamples);
    }
    sub_0807E334(obj);
    actor->field2C2 = 0x20;
    return 0;
reroute: {
    Golem_CellCenter(cell, &target);
    if (SceneActorChase_TryRoute(obj, group->route, &actor->body.position, &target))
        goto success;
}
    callback = &group->fallback;
invoke:
    Callback_InvokeActor(actor, *callback);
    return 0;
success:
    return 1;
}

static inline s32 Golem_ReadDesiredX(const struct SceneActorRegistryResetView *a)
{
    return a->reactionStart.x;
}
static inline s32 Golem_ReadDesiredZ(const struct SceneActorRegistryResetView *a)
{
    return a->reactionStart.z;
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
static inline u8 Golem_Drift(struct SceneActorRegistryResetView *actor)
{
    s16 *x, *z;
    s32 dx, dz;
    if (SceneActorReaction_TakePrepared(actor)) {
        u16 *flags;
        u32 mask;
        SceneActorAnimation_SelectDirectionalEntry((struct SceneActorAnimationView *)actor, 3);
        if (actor->field2D8 & 0x400)
            Sound_PlaySong(0x18B);
        mask = 0x20;
        flags = &actor->flags2D4;
        mask |= *flags;
        mask &= ~0x100;
        *flags = mask;
    }
    {
        u32 mask = 0x10000;
        actor->flags2D0 |= mask;
    }
    dx = Golem_ReadDisplacementAxis(Golem_ReadDesiredX(actor), &actor->reactionEnd.x, &x);
    dz = Golem_ReadDisplacementAxis(Golem_ReadDesiredZ(actor), &actor->reactionEnd.z, &z);
    dx = Golem_ClampAxis(dx, 200);
    dz = Golem_ClampAxis(dz, 200);
    actor->body.motion.x += dx;
    actor->body.motion.z += dz;
    *x += dx;
    *z += dz;
    return Golem_DisplacementStopped(dx, dz);
}

/* Converge on the saved motion target; once at rest either hand control to the
 * contact callback, or (flag 0x80) chase the player / walk the route (tentative). */
void Golem_ResumeBehaviorAfterDisplacement(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    if (!Golem_Drift(actor))
        goto moving;
    if (!actor->field2B3)
        goto moving;
    actor->field2B0 = 1;
    SceneActorChase_ClearMotionFlagComponent(actor, 0x20);
    if (actor->field2DC & 0x80000) {
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
            if (!SceneActorChase_TryRoute(actor, group->route, &actor->body.position,
                                          &actor->tracking.target->position)) {
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
            if (!SceneActorChase_TryRoute(actor, group->route, &actor->body.position,
                                          &actor->tracking.target->position)) {
                Callback_InvokeActor(actor, group->fallback);
                return;
            }
        }
        SceneActorChase_SetControl(actor, actor->entryTarget228, 1);
        SceneActorChase_SetState(actor, 0, 0, 7);
        actor->field2C2 = 0x20;
    }
    return;
moving:
    SceneActorChase_SetActorFlags(actor, 0x2000);
}
