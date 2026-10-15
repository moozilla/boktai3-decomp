#include "scene_actor_animation.h"
#define SceneActorAnimation_SelectDirectionalEntryFlags sub_08076174
void SceneActorAnimation_SelectDirectionalEntryFlags(struct SceneActorAnimationView *actor,
                                                     s32 index, u32 extraFlags)
{
    struct SceneActorProfileEntry *entry =
        (void *)sub_08073CB0(actor->profileId, actor->profileMode, index);
    u8 directionVariant, mirror;
    u32 selection;
    u8 *mirrorPointer;
    u8 *variantPointer = &directionVariant;
    u32 flags, variant;
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
    flags = *mirrorPointer | extraFlags;
    entryFlags = entry->flags;
    selection = entry->value;
    variant = *variantReload;
    flags |= entryFlags;
    if (flags & 0x10) {
        force = 0x10;
        flags &= ~0x10;
    } else
        force = 0;
    if (!actor->drawableKind) {
        struct SceneActorAnimationSequenceDrawable *drawable;
        actor->selection = selection;
        drawable = actor->drawable;
        sub_08220C8C(drawable->sequence, drawable->resource, selection, variant, flags);
        restart = &actor->restart;
    } else {
        struct SceneActorAnimationTile *tile =
            &((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile;
        u8 *savedSelection;

        selection += variant;
        savedSelection = &actor->selection;
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
            sub_08220D78(&((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile,
                         actor->drawable, selection, 2, mode);
            flags &= 1;
            if (flags)
                tile->flags |= mask;
            else
                tile->flags &= ~4;
        }
    }
    *restart = 0;
}
