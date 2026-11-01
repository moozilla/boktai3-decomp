#include "global.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_reaction.h"
#include "scene_actor_delayed_response.h"
#include "scene_actor_control_phase.h"
#include "scene_actor_terrain.h"
/* Skeleton: on entry play the category's directional rise animation (0x63/0x65/0x67) or hand
 * over to SceneActorReaction_PlayAnimationBUntilComplete; once done, return to the route
 * point, then either resume the route (target cell reached) or walk toward the target cell
 * (tentative). */
void SceneActorReaction_PlayAnimationBUntilComplete(void);
void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
void SceneActorBehavior_CallActionScript(void *);

#define PLAY_DIRECTIONAL(value, done)                                                              \
    {                                                                                              \
        struct SceneActorProfileEntry *entry;                                                      \
        u8 directionVariant, mirror;                                                               \
        u32 selection;                                                                             \
        u8 *mirrorPointer;                                                                         \
        u8 *variantPointer = &directionVariant;                                                    \
        u32 flags, variant;                                                                        \
        u8 entryFlags, mirrorValue;                                                                \
        u8 *variantReload;                                                                         \
        u32 force;                                                                                 \
        u8 *restart;                                                                               \
        {                                                                                          \
            u8 *mirrorAddress = &mirror;                                                           \
            s32 quadrant = (((s32)actor->body.direction + 0x60) >> 6) & 3;                         \
            mirrorPointer = mirrorAddress;                                                         \
            if (quadrant > 1) {                                                                    \
                *mirrorPointer = 1;                                                                \
                *variantPointer = 3 - quadrant;                                                    \
            } else {                                                                               \
                *mirrorPointer = 0;                                                                \
                *variantPointer = quadrant;                                                        \
            }                                                                                      \
        }                                                                                          \
        entry =                                                                                    \
            (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, 0);   \
        variantReload = &directionVariant;                                                         \
        mirrorValue = *mirrorPointer;                                                              \
        entryFlags = entry->flags;                                                                 \
        DEBUG_NOP();                                                                               \
        selection = value;                                                                         \
        variant = *variantReload;                                                                  \
        flags = mirrorValue | entryFlags;                                                          \
        if (flags & 0x10) {                                                                        \
            force = 0x10;                                                                          \
            flags &= ~0x10;                                                                        \
        } else                                                                                     \
            force = 0;                                                                             \
        if (!actor->drawableKind) {                                                                \
            struct SceneActorAnimationSequenceDrawable *drawable;                                  \
            actor->animationIndex = selection;                                                     \
            drawable = actor->drawable;                                                            \
            Anim_SetSequence(drawable->sequence, (void *)drawable->resource, (u16)selection,       \
                             (u8)variant, (u8)flags);                                              \
            restart = &actor->field2B3;                                                            \
        } else {                                                                                   \
            struct SceneActorAnimationTile *tile =                                                 \
                &((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile;                \
            u8 *savedSelection;                                                                    \
                                                                                                   \
            selection += variant;                                                                  \
            savedSelection = &actor->animationIndex;                                               \
            restart = &actor->field2B3;                                                            \
            if (!force && *savedSelection == selection && !*restart) {                             \
                if (flags == 1 && (tile->flags & 4))                                               \
                    goto done;                                                                     \
                if (flags == 0 && !(tile->flags & 4))                                              \
                    goto done;                                                                     \
            }                                                                                      \
            *savedSelection = selection;                                                           \
            {                                                                                      \
                u32 mask = 4;                                                                      \
                u32 mode;                                                                          \
                if (flags & mask)                                                                  \
                    mode = 6;                                                                      \
                else                                                                               \
                    mode = 4;                                                                      \
                Anim_SetKeyframes(                                                                 \
                    &((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile,            \
                    actor->drawable, (u16)selection, 2, (u16)mode);                                \
                flags &= 1;                                                                        \
                if (flags)                                                                         \
                    tile->flags |= mask;                                                           \
                else                                                                               \
                    tile->flags &= ~4;                                                             \
            }                                                                                      \
        }                                                                                          \
        *restart = 0;                                                                              \
    }

static inline void Or16(u16 *p, u32 maskInput)
{
    u32 mask = maskInput;
    mask |= *p;
    *p = mask;
}
static inline void SetCtlZ(struct SceneActorRegistryResetView *actor, void *cb, u32 k, u8 zero)
{
    void *callback = cb;
    u32 kind = k;
    actor->field2B2 = 1;
    actor->field2B0 = zero;
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
    position->y = SceneActorChase_HeightAt(SceneActorChase_CellAt(position), position);
}

static inline void Skeleton_BeginDelayedTurn(void *obj, u32 saved, const u16 *randomSamples)
{
    struct SceneActorRegistryResetView *actor = obj;
    struct SceneActorChaseGroup *group = &actor->group;
    if (!SceneActorChase_MotionFlags(actor, 0x400) &&
        !SceneActorChase_TestField((u16 *)&group->common.gate.flags, 4)) {
        group->turn.direction =
            SceneActorChase_RandomDirectionExcept(actor->body.direction, randomSamples);
        SceneActorChase_SaveControl(&group->saved, actor);
        group->turn.savedDirection = saved;
        SceneActorChase_SetControl(actor, group->turnControl, 0x1B);
        SceneActorChase_SetState(actor, 3, 1, 0);
        group->turn.turnsRemaining = 6;
    }
}

/* A successful cell-center reroute returns one. Other paths leave the
 * caller to try the separate partner-position route. */

static inline u8 Skeleton_ArriveAfterDelay(void *obj)
{
    struct ContactPosition target;
    struct SceneActorRegistryResetView *actor = obj;
    struct SceneActorChaseGroup *group = &actor->group;
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
        Skeleton_SnapDelayedRoute(group->destination, &group->homeRoute);
        (group->destination)->y = SceneActorChase_HeightAt(
            SceneActorChase_CellAt(group->destination), group->destination);
        (group->destination)->pad = 0;
    }
    {
        u8 saved = actor->body.direction;
        Skeleton_BeginDelayedTurn(obj, saved, randomSamples);
    }
    SceneActorTerrain_RequestRetreatRoute(obj);
    actor->field2C2 = 0x20;
    return 0;
reroute: {
    SceneActorChase_CellCenter(cell, &target);
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

#ifdef NONMATCHING
// fuzzy: 74.6%
// scope shape 71 (differences once low registers are set aside)
void sub_080BF0E4(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    if (SceneActorControl_TakeStateEntry(actor)) {
        u16 *flags;
        actor->body.direction = actor->tracking.heading;
        if (actor->category == 5) {
            u32 mask = 0x200;
            u32 blocked = actor->flags2D4 & mask;
            flags = &actor->flags2D4;
            if (blocked || actor->animationIndex == 0x63) {
                PLAY_DIRECTIONAL(0x63, done5);
            done5:
                actor->field2B0 = 1;
            } else
                SetCtlZ(actor, SceneActorReaction_PlayAnimationBUntilComplete, 10, 0);
        } else if (actor->category == 6) {
            u32 mask = 0x200;
            u32 blocked = actor->flags2D4 & mask;
            flags = &actor->flags2D4;
            if (blocked || actor->animationIndex == 0x65) {
                PLAY_DIRECTIONAL(0x65, done6);
            done6:
                actor->field2B0 = 1;
            } else
                SetCtlZ(actor, SceneActorReaction_PlayAnimationBUntilComplete, 10, 0);
        } else {
            flags = &actor->flags2D4;
            if (actor->category == 7) {
                u32 mask = 0x200;
                u16 blocked = *flags & mask;
                if (blocked || actor->animationIndex == 0x67) {
                    PLAY_DIRECTIONAL(0x67, done7);
                done7:
                    actor->field2B0 = 1;
                } else
                    SetCtlZ(actor, SceneActorReaction_PlayAnimationBUntilComplete, 10, 0);
            }
        }
        SceneActorAttachment_ReplaceResponseSlot(actor, 0, 0, actor->category);
        SceneActorBehavior_CallActionScript(actor);
        Or16(&actor->group.common.gate.flags, 4);
        Or16(flags, 0x80);
        actor->group.common.gate.counter = actor->group.common.gate.duration;
    }
    if (actor->field2B0) {
        SceneActorChase_SelectState(actor, 0, 0, 12);
        SceneActorChase_ResetStateEntry(actor);
        SceneActorChase_SetControl(actor, group->common.gate.control, 0x1A);
        Skeleton_ArriveAfterDelay(actor);
    } else
        actor->counter2AC++;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080BF0E4);
#endif
