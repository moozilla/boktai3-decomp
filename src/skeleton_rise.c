#include "global.h"
#include "player.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_reaction.h"
#include "scene_actor_control_phase.h"
#include "scene_actor_terrain.h"
#include "scene_actor_turn.h"
#include "scene_actor_init_primitives.h"
#include "scene_actor_delayed_response.h"

void SceneActorReaction_PlayAnimationBUntilComplete(void);

void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);

void SceneActorBehavior_CallActionScript(void *);

static inline u32 Skeleton_ProfileId(const struct ChaseActor *a)
{
    return a->base.profileId;
}

static inline u32 Skeleton_ProfileMode(const struct ChaseActor *a)
{
    return a->base.profileMode;
}

static inline struct SceneActorAnimationSequenceDrawable *
Skeleton_SequenceDrawable(struct ChaseActor *a)
{
    return a->base.drawable;
}

static inline void *Skeleton_Sequence(struct SceneActorAnimationSequenceDrawable *d)
{
    return d->sequence;
}

static inline struct SceneActorAnimationTile *Skeleton_TileOf(struct ChaseActor *a)
{
    return &((struct SceneActorAnimationTileDrawable *)a->base.drawable)->tile;
}

static inline u32 Skeleton_TileMirror(struct SceneActorAnimationTile *tile)
{
    return tile->flags & 4;
}

static inline struct RoomActor *Skeleton_Playback(struct ChaseActor *a)
{
    return (void *)a;
}

static inline u32 *Skeleton_SpriteBits(struct SceneActorAnimationTile *t)
{
    return &t->flags;
}

static inline void Skeleton_ResolvePlayback(u8 mirrorFlags, u8 entryFlags, u32 *playbackFlags,
                                            u32 *forceRestart)
{
    *playbackFlags = mirrorFlags | entryFlags;
    if (*playbackFlags & 0x10) {
        *forceRestart = 0x10;
        *playbackFlags &= ~0x10;
    } else
        *forceRestart = 0;
}

static inline void Skeleton_LaunchSequence(void *object, u32 selection, u8 variant, u8 flags,
                                           u8 **restartOut)
{
    struct ChaseActor *actor = object;
    struct SceneActorAnimationSequenceDrawable *drawable;
    actor->base.animationIndex = selection;
    drawable = Skeleton_SequenceDrawable(actor);
    Anim_SetSequence(Skeleton_Sequence(drawable), (void *)drawable->resource, (u16)selection,
                     (u8)variant, (u8)flags);
    *restartOut = &Skeleton_Playback(actor)->restart;
}

static inline void Skeleton_CommitTile(void *obj, struct SceneActorAnimationTile *tile,
                                       u8 *savedSelection, u32 selection, u32 flags)
{
    struct ChaseActor *actor = obj;
    *savedSelection = selection;
    {
        const u32 mask = 4;
        u32 mode;
        if (flags & mask)
            mode = 6;
        else
            mode = 4;
        Anim_SetKeyframes(Skeleton_TileOf(actor), actor->base.drawable, (u16)selection, 2,
                          (u16)mode);
        flags &= 1;
        if (flags)
            *Skeleton_SpriteBits(tile) |= mask;
        else
            *Skeleton_SpriteBits(tile) &= ~4;
    }
}

static inline u32 Skeleton_ReadFrame(u32 value, u32 *selectedFrame, u8 *variantReload)
{
    *selectedFrame = value;
    return *variantReload;
}

/* Sequence drawables take facing as a row. Tile drawables add it to the base frame. */

#define SKELETON_PLAY_RISE(value, done, quadrant)                                                  \
    {                                                                                              \
        struct SceneActorProfileEntry *entry;                                                      \
        u8 directionVariant, mirror;                                                               \
        u32 selection;                                                                             \
        u8 *mirrorPointer;                                                                         \
        u32 flags;                                                                                 \
        u8 entryFlags, mirrorValue;                                                                \
        u8 *variantReload;                                                                         \
        u32 force;                                                                                 \
                                                                                                   \
        {                                                                                          \
            u8 *variantAddress = &directionVariant;                                                \
            u8 *mirrorAddress = &mirror;                                                           \
            quadrant = SceneActorTurn_Cardinal(actor->base.body.direction);                        \
            mirrorPointer = mirrorAddress;                                                         \
            if (quadrant > 1) {                                                                    \
                *mirrorPointer = 1;                                                                \
                *variantAddress = 3 - quadrant;                                                    \
            } else {                                                                               \
                *mirrorPointer = 0;                                                                \
                *variantAddress = quadrant;                                                        \
            }                                                                                      \
        }                                                                                          \
        entry = (void *)SceneActorProfiles_GetEntryAddress(Skeleton_ProfileId(actor),              \
                                                           Skeleton_ProfileMode(actor), 0);        \
        variantReload = &directionVariant;                                                         \
        mirrorValue = mirror;                                                                      \
        do {                                                                                       \
            entryFlags = entry->flags;                                                             \
            variant = Skeleton_ReadFrame(value, &selection, variantReload);                        \
        } while (0);                                                                               \
        Skeleton_ResolvePlayback(mirrorValue, entryFlags, &flags, &force);                         \
        if (!actor->base.drawableKind) {                                                           \
                                                                                                   \
            Skeleton_LaunchSequence(actor, selection, variant, flags, &restart);                   \
                                                                                                   \
        } else {                                                                                   \
                                                                                                   \
            struct SceneActorAnimationTile *tile = Skeleton_TileOf(actor);                         \
                                                                                                   \
            selection = selection + variant;                                                       \
            savedSelection = &actor->base.animationIndex;                                          \
            restart = &Skeleton_Playback(actor)->restart;                                          \
            if (!force && *savedSelection == selection && !*restart) {                             \
                if (flags == 1 && Skeleton_TileMirror(tile))                                       \
                    goto done;                                                                     \
                if (flags == 0 && !Skeleton_TileMirror(tile))                                      \
                    goto done;                                                                     \
            }                                                                                      \
            Skeleton_CommitTile(actor, tile, savedSelection, selection, flags);                    \
        }                                                                                          \
                                                                                                   \
        *restart = 0;                                                                              \
    }

static inline void Skeleton_EnableFlags(u16 *p, u32 maskInput)
{
    u32 mask = maskInput;
    mask |= *p;
    *p = mask;
}

static inline void Skeleton_SetRiseControl(struct ChaseActor *actor, void *cb, u32 controlKind,
                                           u8 ready)
{
    void *callback = cb;
    u32 kind = controlKind;
    actor->base.field2B2 = 1;
    actor->base.stepDone = ready;
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

static inline void Skeleton_SnapDelayedRoute(struct ContactPosition *position,
                                             struct SceneActorRegistryRoute *route)
{
    position->x = route->point->x;
    position->z = route->point->z;
    position->y = SceneActorTerrain_GetHeight(SceneActorChase_CellAt(position), position);
}

static inline void Skeleton_BeginDelayedTurn(void *obj, u32 saved, const u16 *randomSamples)
{
    struct ChaseActor *actor = obj;
    struct SceneActorChaseGroup *group = SceneActorChase_GroupOf(actor);
    if (!SceneActorChase_QueryMotionFlagComponent(actor, 0x400) &&
        !SceneActorChase_TestField((u16 *)&group->common.gate.flags, 4)) {
        group->turn.direction =
            SceneActorChase_RandomDirectionExcept(actor->base.body.direction, randomSamples);
        {
            struct SceneActorChaseSavedControl *savedControl = &group->saved;
            savedControl->control = actor->base.target;
            savedControl->kind = actor->base.controlKind;
            savedControl->state = actor->base.state2A8;
            savedControl->phase = actor->base.phase2A9;
            savedControl->behavior = actor->base.behavior2AA;
        }
        group->turn.savedDirection = saved;
        SceneActorChase_SetControl(actor, group->turnControl, 0x1B);
        SceneActorChase_SetState(actor, 3, 1, 0);
        group->turn.turnsRemaining = 6;
    }
}

/* Resume the tracked route, or invoke the group fallback if rerouting fails. */

static inline void Skeleton_ArriveAfterDelay(void *obj)
{
    struct ContactPosition target;
    struct ChaseActor *actor = obj;
    struct SceneActorChaseGroup *group = SceneActorChase_GroupOf(actor);
    struct SceneActorChaseTracking *tracking = &actor->base.tracking;
    s32 cell, player;
    const u16 *randomSamples = gRandomTable;
    void **callback;
    Skeleton_SnapDelayedRoute(group->destination, &group->homeRoute);
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
        !SceneActorChase_Bits32(&gPlayers[player]->statusFlags,
                                0x20000))
        goto reroute;
    if (SceneActorChase_MotionFlags(actor, 0x400))
        return;
    {
        u32 bits = 4;
        group->common.gate.flags &= ~bits;
    }
    if (SceneActorChase_MotionFlags(actor, 0x80)) {
        SceneActorChase_SetControl(actor, group->common.gate.control, 0x1A);
        SceneActorChase_SetTypedState(actor, 0, 0, 12);
    } else {
        SceneActorChase_SetControl(actor, actor->base.onAlternateEntry, 1);
        SceneActorChase_SetTypedState(actor, 0, 0, 5);
        Skeleton_SnapDelayedRoute(group->destination, &group->homeRoute);
        (group->destination)->y = MonsterInit_TerrainHeight(
            SceneActorChase_CellAt(group->destination), group->destination);
        (group->destination)->pad = 0;
    }
    {
        u8 saved = actor->base.body.direction;
        Skeleton_BeginDelayedTurn(obj, saved, randomSamples);
    }
    SceneActorTerrain_RequestRetreatRoute(obj);
    actor->base.field2C2 = 0x20;
    return;
reroute:
    SceneActorTerrain_GetCenter(cell, &target);
    if (SceneActorChase_TryRoute(obj, group->route, &actor->base.body.position, &target))
        return;
    callback = &group->fallback;
invoke:
    Callback_InvokeActor(actor, *callback);
}

/* Recover from the directional rise, then resume or redirect the route. */

void Skeleton_UpdateRiseRecovery(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    if (SceneActorControl_TakeStateEntry(actor)) {
        u8 *savedSelection;
        s32 quadrant;
        u8 *restart;
        u32 variant;
        u16 *flags;
        actor->base.body.direction = actor->base.tracking.heading;
        if (actor->base.category == MONSTER_CATEGORY_SKELETON) {
            u32 mask = 0x200;
            u32 blocked = actor->base.motionFlags & mask;
            flags = &actor->base.motionFlags;
            if (blocked || actor->base.animationIndex == 0x63) {
                SKELETON_PLAY_RISE(0x63, done5, quadrant);
            done5:
                actor->base.stepDone = 1;
            } else
                Skeleton_SetRiseControl(actor, SceneActorReaction_PlayAnimationBUntilComplete, 10,
                                        0);
        } else if (actor->base.category == MONSTER_CATEGORY_SKELETON_GUARD) {
            u32 mask = 0x200;
            u32 blocked = actor->base.motionFlags & mask;
            flags = &actor->base.motionFlags;
            if (blocked || actor->base.animationIndex == 0x65) {
                SKELETON_PLAY_RISE(0x65, done6, quadrant);
            done6:
                actor->base.stepDone = 1;
            } else
                Skeleton_SetRiseControl(actor, SceneActorReaction_PlayAnimationBUntilComplete, 10,
                                        0);
        } else {
            flags = &actor->base.motionFlags;
            if (actor->base.category == MONSTER_CATEGORY_SKELETON_ARMED) {
                u32 mask = 0x200;
                u16 blocked = SceneActorChase_FlagComponentBits(
                    (struct SceneActorChaseFlags16 *)&actor->base.motionFlags, mask);
                if (blocked || actor->base.animationIndex == 0x67) {
                    s32 finalQuadrant;
                    SKELETON_PLAY_RISE(0x67, done7, finalQuadrant);
                done7:
                    actor->base.stepDone = 1;
                } else
                    Skeleton_SetRiseControl(actor, SceneActorReaction_PlayAnimationBUntilComplete,
                                            10, 0);
            }
        }
        SceneActorAttachment_ReplaceResponseSlot(actor, 0, 0, actor->base.category);
        SceneActorBehavior_CallActionScript(actor);
        Skeleton_EnableFlags(&SceneActorChase_GroupOf(actor)->common.gate.flags, 4);
        SceneActorChase_SetFlagComponent((struct SceneActorChaseFlags16 *)flags, 0x80);
        SceneActorChase_GroupOf(actor)->common.gate.counter =
            SceneActorChase_GroupOf(actor)->common.gate.duration;
    }
    if (actor->base.stepDone) {
        SceneActorChase_SelectState(actor, 0, 0, 12);
        SceneActorChase_ResetStateEntry(actor);
        SceneActorChase_SetControl(actor, group->common.gate.control, 0x1A);
        Skeleton_ArriveAfterDelay(actor);
    } else
        actor->base.counter2AC++;
}

u8 sub_0812F3CC(void *);

static inline void Skeleton_SnapDelayedRoute_UpdateDelayedArrival(struct ContactPosition *position,
                                                      struct SceneActorRegistryRoute *route)
{
    position->x = route->point->x;
    position->z = route->point->z;
    position->y = SceneActorChase_HeightAt(SceneActorChase_CellAt(position), position);
}

static inline void Skeleton_BeginDelayedTurn_UpdateDelayedArrival(void *obj, u32 saved,
                                                      const u16 *randomSamples)
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

static inline u8 Skeleton_ArriveAfterDelay_UpdateDelayedArrival(void *obj)
{
    struct ContactPosition target;
    struct ChaseActor *actor = obj;
    struct SceneActorChaseGroup *group = &actor->group;
    struct SceneActorChaseTracking *tracking = &actor->base.tracking;
    s32 cell, player;
    const u16 *randomSamples = gRandomTable;
    void **callback;
    Skeleton_SnapDelayedRoute_UpdateDelayedArrival(group->destination, &group->homeRoute);
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
        !SceneActorChase_Bits32(&gPlayers[player]->statusFlags,
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
        Skeleton_SnapDelayedRoute_UpdateDelayedArrival(group->destination, &group->homeRoute);
        (group->destination)->y = SceneActorChase_HeightAt(
            SceneActorChase_CellAt(group->destination), group->destination);
        (group->destination)->pad = 0;
    }
    {
        u8 saved = actor->base.body.direction;
        Skeleton_BeginDelayedTurn_UpdateDelayedArrival(obj, saved, randomSamples);
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

void Skeleton_UpdateDelayedArrival(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    if (SceneActorControl_TakeStateEntry(actor))
        actor->base.body.direction = actor->base.tracking.heading;
    if ((u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque))
        return;
    if (sub_0812F3CC(actor))
        return;
    if ((s32)actor->base.counter2AC > 60) {
        if (SceneActorChase_QueryMotionFlags(actor, 0x80)) {
            SceneActorChase_SetState(actor, 0, 0, 12);
            SceneActorChase_SetControl(actor, group->common.gate.control, 0x1A);
            Skeleton_ArriveAfterDelay_UpdateDelayedArrival(actor);
        } else {
            SceneActorChase_SetState(actor, 3, 1, 0);
            SceneActorChase_SetControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
            SceneActorAttachment_ReplaceResponseSlot(actor, 0, 2, actor->base.category);
            SceneActorDelay_Prepare((void *)&group->saved, actor->base.onAlternateEntry, 7, 30);
            Skeleton_SnapDelayedRoute_UpdateDelayedArrival(group->destination, &group->homeRoute);
            if (!SceneActorChase_TryRoute(actor, group->route, &actor->base.body.position,
                                          group->destination)) {
                SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0);
                SceneActorChase_SetState(actor, 3, 0, 0);
            } else
                group->destination->pad = 1;
        }
    } else
        actor->base.counter2AC++;
}

extern const u32 gUnk_08606098[];

void Skeleton_SetAttackControlTable(u8 *p)
{
    *(const u32 **)(p + 0x288) = gUnk_08606098;
}
