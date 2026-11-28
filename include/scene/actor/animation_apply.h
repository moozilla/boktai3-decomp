#ifndef GUARD_SCENE_ACTOR_ANIMATION_APPLY_H
#define GUARD_SCENE_ACTOR_ANIMATION_APPLY_H
#include "npc/manager/scene_actor_animation.h"

/* Splits a facing direction into a sprite variant and a horizontal mirror flag. */
static inline void SceneActorAnimation_GetDirectionFrame(struct RoomActor *actor, u8 *variant,
                                                         u8 *mirror)
{
    s32 quadrant = ((actor->body.direction + 0x60) >> 6) & 3;
    if (quadrant > 1) {
        *mirror = 1;
        *variant = 3 - quadrant;
    } else {
        *mirror = 0;
        *variant = quadrant;
    }
}

/* Starts a profile animation. Sequence drawables take the selection and variant separately;
 * tile drawables use selection + variant and skip the restart when nothing changed.
 * Flag 0x10 forces the restart. */
static inline void SceneActorAnimation_Apply(struct RoomActor *actor, u32 selection, u32 variant,
                                             u32 flags)
{
    u32 force;
    if (flags & 0x10) {
        force = 0x10;
        flags &= ~0x10;
    } else
        force = 0;
    if (!actor->drawableKind) {
        struct SceneActorAnimationSequenceDrawable *drawable;
        actor->animationIndex = selection;
        drawable = actor->drawable;
        Anim_SetSequence(&drawable->sequence, (void *)drawable->resource, (u16)selection,
                         (u8)variant, (u8)flags);
    } else {
        struct AnimSprite *tile =
            &((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile;
        u32 mode;
        selection += variant;
        if (!force && actor->animationIndex == selection && !actor->restart) {
            if (flags == 1 && (tile->flags & 4))
                return;
            if (flags == 0 && !(tile->flags & 4))
                return;
        }
        actor->animationIndex = selection;
        if (flags & 4)
            mode = 6;
        else
            mode = 4;
        Anim_SetKeyframes(&((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile,
                          actor->drawable, (u16)selection, 2, (u16)mode);
        if (flags & 1)
            tile->flags |= 4;
        else
            tile->flags &= ~4;
    }
    actor->restart = 0;
}

#endif
