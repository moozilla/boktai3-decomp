#include "global.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_reaction.h"
#include "scene_actor_control_phase.h"
#include "scene_actor_terrain.h"
#include "scene_actor_turn.h"
#include "scene_actor_init_primitives.h"

void SceneActorReaction_PlayAnimationBUntilComplete(void);
void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
void SceneActorBehavior_CallActionScript(void *);

static inline u32 Skeleton_ProfileId(const struct SceneActorRegistryResetView *a)
{
    return a->profileId;
}

static inline u32 Skeleton_ProfileMode(const struct SceneActorRegistryResetView *a)
{
    return a->profileMode;
}

static inline struct SceneActorAnimationSequenceDrawable *
Skeleton_SequenceDrawable(struct SceneActorRegistryResetView *a)
{
    return a->drawable;
}

static inline void *Skeleton_Sequence(struct SceneActorAnimationSequenceDrawable *d)
{
    return d->sequence;
}

static inline struct SceneActorAnimationTile *Skeleton_TileOf(struct SceneActorRegistryResetView *a)
{
    return &((struct SceneActorAnimationTileDrawable *)a->drawable)->tile;
}

static inline u32 Skeleton_TileMirror(struct SceneActorAnimationTile *tile)
{
    return tile->flags & 4;
}

static inline struct SceneActorAnimationView *
Skeleton_Playback(struct SceneActorRegistryResetView *a)
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
    struct SceneActorRegistryResetView *actor = object;
    struct SceneActorAnimationSequenceDrawable *drawable;
    actor->animationIndex = selection;
    drawable = Skeleton_SequenceDrawable(actor);
    Anim_SetSequence(Skeleton_Sequence(drawable), (void *)drawable->resource, (u16)selection,
                     (u8)variant, (u8)flags);
    *restartOut = &Skeleton_Playback(actor)->restart;
}

static inline void Skeleton_CommitTile(void *obj, struct SceneActorAnimationTile *tile,
                                       u8 *savedSelection, u32 selection, u32 flags)
{
    struct SceneActorRegistryResetView *actor = obj;
    *savedSelection = selection;
    {
        const u32 mask = 4;
        u32 mode;
        if (flags & mask)
            mode = 6;
        else
            mode = 4;
        Anim_SetKeyframes(Skeleton_TileOf(actor), actor->drawable, (u16)selection, 2, (u16)mode);
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
            quadrant = SceneActorTurn_Cardinal(actor->body.direction);                             \
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
        if (!actor->drawableKind) {                                                                \
                                                                                                   \
            Skeleton_LaunchSequence(actor, selection, variant, flags, &restart);                   \
                                                                                                   \
        } else {                                                                                   \
                                                                                                   \
            struct SceneActorAnimationTile *tile = Skeleton_TileOf(actor);                         \
                                                                                                   \
            selection = selection + variant;                                                       \
            savedSelection = &actor->animationIndex;                                               \
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

static inline void Skeleton_SetRiseControl(struct SceneActorRegistryResetView *actor, void *cb,
                                           u32 controlKind, u8 ready)
{
    void *callback = cb;
    u32 kind = controlKind;
    actor->field2B2 = 1;
    actor->field2B0 = ready;
    actor->field2B5 = kind;
    actor->target = callback;
    actor->field2C8 = 0;
    {
        u32 *field = &actor->flags114;
        u32 mask = ~1;
        *field &= mask;
    }
    actor->word104 = 0;
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
    struct SceneActorRegistryResetView *actor = obj;
    struct SceneActorChaseGroup *group = SceneActorChase_GroupOf(actor);
    if (!SceneActorChase_QueryMotionFlagComponent(actor, 0x400) &&
        !SceneActorChase_TestField((u16 *)&group->common.gate.flags, 4)) {
        group->turn.direction =
            SceneActorChase_RandomDirectionExcept(actor->body.direction, randomSamples);
        {
            struct SceneActorChaseSavedControl *savedControl = &group->saved;
            savedControl->control = actor->target;
            savedControl->kind = actor->field2B5;
            savedControl->state = actor->state2A8;
            savedControl->phase = actor->phase2A9;
            savedControl->behavior = actor->behavior2AA;
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
    struct SceneActorRegistryResetView *actor = obj;
    struct SceneActorChaseGroup *group = SceneActorChase_GroupOf(actor);
    struct SceneActorChaseTracking *tracking = &actor->tracking;
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
        cell = ((struct SceneActorChasePlayer *)gUnknown_02000580[player])->cell->index;
    } else {
        struct SceneActorChaseTarget *t = tracking->target;
        cell = SceneActorChase_CellAt(&t->position);
        player = 0;
    }
    if (cell != actor->terrainCell &&
        !SceneActorChase_Bits32(&((struct SceneActorChasePlayer *)gUnknown_02000580[player])->flags,
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
        SceneActorChase_SetControl(actor, actor->entryTarget228, 1);
        SceneActorChase_SetTypedState(actor, 0, 0, 5);
        Skeleton_SnapDelayedRoute(group->destination, &group->homeRoute);
        (group->destination)->y = MonsterInit_TerrainHeight(
            SceneActorChase_CellAt(group->destination), group->destination);
        (group->destination)->pad = 0;
    }
    {
        u8 saved = actor->body.direction;
        Skeleton_BeginDelayedTurn(obj, saved, randomSamples);
    }
    SceneActorTerrain_RequestRetreatRoute(obj);
    actor->field2C2 = 0x20;
    return;
reroute:
    SceneActorTerrain_GetCenter(cell, &target);
    if (SceneActorChase_TryRoute(obj, group->route, &actor->body.position, &target))
        return;
    callback = &group->fallback;
invoke:
    Callback_InvokeActor(actor, *callback);
}

/* Recover from the directional rise, then resume or redirect the route. */
void Skeleton_UpdateRiseRecovery(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    if (SceneActorControl_TakeStateEntry(actor)) {
        u8 *savedSelection;
        s32 quadrant;
        u8 *restart;
        u32 variant;
        u16 *flags;
        actor->body.direction = actor->tracking.heading;
        if (actor->category == 5) {
            u32 mask = 0x200;
            u32 blocked = actor->flags2D4 & mask;
            flags = &actor->flags2D4;
            if (blocked || actor->animationIndex == 0x63) {
                SKELETON_PLAY_RISE(0x63, done5, quadrant);
            done5:
                actor->field2B0 = 1;
            } else
                Skeleton_SetRiseControl(actor, SceneActorReaction_PlayAnimationBUntilComplete, 10,
                                        0);
        } else if (actor->category == 6) {
            u32 mask = 0x200;
            u32 blocked = actor->flags2D4 & mask;
            flags = &actor->flags2D4;
            if (blocked || actor->animationIndex == 0x65) {
                SKELETON_PLAY_RISE(0x65, done6, quadrant);
            done6:
                actor->field2B0 = 1;
            } else
                Skeleton_SetRiseControl(actor, SceneActorReaction_PlayAnimationBUntilComplete, 10,
                                        0);
        } else {
            flags = &actor->flags2D4;
            if (actor->category == 7) {
                u32 mask = 0x200;
                u16 blocked = SceneActorChase_FlagComponentBits(
                    (struct SceneActorChaseFlags16 *)&actor->flags2D4, mask);
                if (blocked || actor->animationIndex == 0x67) {
                    s32 finalQuadrant;
                    SKELETON_PLAY_RISE(0x67, done7, finalQuadrant);
                done7:
                    actor->field2B0 = 1;
                } else
                    Skeleton_SetRiseControl(actor, SceneActorReaction_PlayAnimationBUntilComplete,
                                            10, 0);
            }
        }
        SceneActorAttachment_ReplaceResponseSlot(actor, 0, 0, actor->category);
        SceneActorBehavior_CallActionScript(actor);
        Skeleton_EnableFlags(&SceneActorChase_GroupOf(actor)->common.gate.flags, 4);
        SceneActorChase_SetFlagComponent((struct SceneActorChaseFlags16 *)flags, 0x80);
        SceneActorChase_GroupOf(actor)->common.gate.counter =
            SceneActorChase_GroupOf(actor)->common.gate.duration;
    }
    if (actor->field2B0) {
        SceneActorChase_SelectState(actor, 0, 0, 12);
        SceneActorChase_ResetStateEntry(actor);
        SceneActorChase_SetControl(actor, group->common.gate.control, 0x1A);
        Skeleton_ArriveAfterDelay(actor);
    } else
        actor->counter2AC++;
}
