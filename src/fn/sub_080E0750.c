#include "scene_actor_chase_reaction.h"
#include "scene_actor_reaction.h"

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
void sub_080E0750(struct ChaseActor *actor)
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
