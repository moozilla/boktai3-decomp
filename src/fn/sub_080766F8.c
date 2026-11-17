#include "scene_actor_animation.h"
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

void SceneActorAnimation_SelectDirectionalFlags(struct RoomActor *actor, s32 index)
{
    struct SceneActorProfileEntry *entry =
        (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, index);
    u8 directionVariant, mirror;
    u32 selection;
    u32 flags;
    u8 merged;
    u16 variant;
    u32 force;
    u8 *restart;
    SceneActorAnimation_GetDirectionFrame(actor, &directionVariant, &mirror);
    variant = 0;
    merged = mirror | entry->flags;
    selection = entry->value;
    flags = merged;
    if (flags & 0x10) {
        force = 0x10;
        flags &= ~0x10;
    } else
        force = 0;
    if (!actor->drawableKind) {
        struct SceneActorAnimationSequenceDrawable *drawable;
        actor->animationIndex = selection;
        drawable = actor->drawable;
        {
            void *sequence = &drawable->sequence;
            void *resource = (void *)drawable->resource;
            u32 selected = (u16)selection;
            Anim_SetSequence(sequence, resource, selected, (u8)variant, flags);
        }
        restart = &actor->restart;
    } else {
        struct SceneInputWidget *tile =
            &((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile;
        u8 *savedSelection;

        savedSelection = &actor->animationIndex;
        restart = &actor->restart;
        if (!force && *savedSelection == selection && !*restart) {
            if (flags == 1 && (tile->flags & 4))
                return;
            if (flags == 0 && !(tile->flags & 4))
                return;
        }
        *savedSelection = selection;
        {
            u32 mask = 4;
            u32 mode;
            if (flags & mask)
                mode = 6;
            else
                mode = 4;
            Anim_SetKeyframes(&((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile,
                              actor->drawable, (u16)selection, 2, (u16)mode);
            flags &= 1;
            if (flags)
                tile->flags |= mask;
            else
                tile->flags &= ~4;
        }
    }
    *restart = 0;
}
