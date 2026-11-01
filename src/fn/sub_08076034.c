// MATCHING_CONCESSION: register-pinning
#include "scene_actor_animation.h"
void SceneActorAnimation_SelectDirectionalEntry(struct SceneActorAnimationView *actor, s32 index)
{
    struct SceneActorProfileEntry *entry =
        (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, index);
    u8 directionVariant, mirror;
    u32 selection;
    u8 *mirrorPointer;
    u8 *variantPointer = &directionVariant;
    u32 flags;
    u32 mirrorValue;
    u8 variant;
    u32 entryFlags;
    u8 *variantReload;
    u32 force;
    u8 *restart;
    {
        u8 *mirrorAddress = &mirror;
        s32 quadrant = (((s32)actor->direction + 0x60) >> 6) & 3;
        mirrorPointer = mirrorAddress;
        if (quadrant > 1) {
            *mirrorPointer = 1;
            *variantPointer = 3 - quadrant;
        } else {
            *mirrorPointer = 0;
            *variantPointer = quadrant;
        }
    }
    variantReload = &directionVariant;
    index = *mirrorPointer;
    entryFlags = entry->flags;
    DEBUG_NOP();
    selection = entry->value;
    variant = *variantReload;
    flags = index | entryFlags;
    if (flags & 0x10) {
        force = 0x10;
        flags &= ~0x10;
    } else {
        force = 0;
    }
    if (!actor->drawableKind) {
        struct SceneActorAnimationSequenceDrawable *drawable;
        actor->selection = selection;
        drawable = actor->drawable;
        {
            void *sequence = drawable->sequence;
            void *resource = (void *)drawable->resource;
            u32 selected = (u16)selection;
            u32 row;
            row = variant;
            Anim_SetSequence(sequence, resource, selected, (char)row, flags);
        }
        restart = &actor->restart;
    } else {
        struct SceneActorAnimationTile *tile =
            &((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile;
        u8 *savedSelection;

        selection += variant;
        savedSelection = &actor->selection;
        restart = &actor->restart;
        if (!force && *savedSelection == selection && !*restart) {
            if ((flags == 1) && (tile->flags & 4)) {
                return;
            }
            if ((flags == 0) && (!(tile->flags & 4))) {
                return;
            }
        }
        *savedSelection = selection;
        {
            u32 mask = 4;
            u32 mode;
            if (flags & mask) {
                mode = 6;
            } else {
                mode = 4;
            }
            Anim_SetKeyframes(&((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile,
                              actor->drawable, (u16)selection, 2, (u16)mode);
            flags &= 1;
            if (flags) {
                tile->flags |= mask;
            } else {
                tile->flags &= ~4;
            }
        }
    }
    *restart = 0;
}
