#include "global.h"
#include "dog.h"
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

u8 Dog_IsActionTimerExpired(struct ChaseActor *);
s32 Dog_SelectRetreatDirection(struct ChaseActor *, u32);

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
