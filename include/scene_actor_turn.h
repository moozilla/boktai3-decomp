#ifndef GUARD_SCENE_ACTOR_TURN_H
#define GUARD_SCENE_ACTOR_TURN_H

#include "scene_actor_reset.h"
#include "scene_actor_animation.h"

void Anim_SetDurationScale(void *, u32);
void Anim_SetFrameDurationScale(void *, u32);

static inline s32 SceneActorTurn_Cardinal(u8 direction)
{
    return ((direction + 0x60) >> 6) & 3;
}

static inline u32 SceneActorTurn_ActorFlags(struct ChaseActor *a, u32 mask)
{
    return a->base.behaviorFlags & mask;
}

static inline void SceneActorTurn_ApplyDuration(struct ChaseActor *a, u16 *motionFlags)
{
    u16 *scale;
    u32 mask = 0xA02F;
    if (*motionFlags & mask) {
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

/* The same, testing the actor's own motion flags. */
static inline void SceneActorTurn_ApplyActorDuration(struct ChaseActor *a)
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

static inline u32 SceneActorTurn_Flags(u16 *field, u32 mask)
{
    return *field & mask;
}

static inline void SceneActorTurn_SetAnimationIndex(struct ChaseActor *a, u32 selection)
{
    a->base.animationIndex = selection;
}

static inline void SceneActorTurn_SelectAnimation(struct ChaseActor *a, u32 selection, u32 variant,
                                                  u32 flags)
{
    u32 force;
    if (flags & 0x10) {
        force = 0x10;
        flags &= ~0x10;
    } else
        force = 0;
    if (!a->base.drawableKind) {
        struct SceneActorAnimationSequenceDrawable *drawable;
        a->base.animationIndex = selection;
        drawable = a->base.drawable;
        Anim_SetSequence(&drawable->sequence, (void *)drawable->resource, (u16)selection,
                         (u8)variant, (u8)flags);
    } else {
        struct SceneInputWidget *tile =
            &((struct SceneActorAnimationTileDrawable *)a->base.drawable)->tile;
        selection += variant;
        {
            u8 *selectionAddress = &a->base.animationIndex;
            u32 previous = *selectionAddress;
            if (!force && previous == selection && !a->base.restart) {
                if (flags == 1 && (tile->flags & 4))
                    return;
                if (flags == 0 && !(tile->flags & 4))
                    return;
            }
        }
        SceneActorTurn_SetAnimationIndex(a, selection);
        {
            u32 mask = 4;
            u32 mode;
            if (flags & mask)
                mode = 6;
            else
                mode = 4;
            Anim_SetKeyframes(&((struct SceneActorAnimationTileDrawable *)a->base.drawable)->tile,
                              a->base.drawable, (u16)selection, 2, (u16)mode);
            if (flags & 1)
                tile->flags |= mask;
            else
                tile->flags &= ~4;
        }
    }
    a->base.restart = 0;
}

static inline void SceneActorTurn_SelectProfileAnimation(struct RoomActor *actor, s32 index,
                                                         u32 variant, u32 flags)
{
    struct SceneActorProfileEntry *entry =
        (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, index);
    SceneActorTurn_SelectAnimation((void *)actor, entry->value, variant, flags);
}

static inline u8 SceneActorTurn_OctantDirection(u32 octant)
{
    return ((octant + 5) & 7) << 5;
}

#endif
