#include "scene_actor_animation.h"
#define SceneActorAnimation_SelectEntryFlags sub_080763D0
void SceneActorAnimation_SelectEntryFlags(struct SceneActorAnimationView *actor, s32 index,
                                          u32 extraFlags)
{
    struct SceneActorProfileEntry *entry =
        (void *)sub_08073CB0(actor->profileId, actor->profileMode, index);
    u32 entryFlags = entry->flags;
    u32 selection = entry->value;
    u32 variant = 0;
    u32 flags = extraFlags | entryFlags;
    u32 force;
    u8 *restart;
    if (flags & 0x10) {
        force = 0x10;
        flags &= ~0x10;
    } else
        force = 0;
    if (!actor->drawableKind) {
        struct SceneActorAnimationSequenceDrawable *drawable;
        actor->selection = selection;
        drawable = actor->drawable;
        Anim_SetSequence(drawable->sequence, (void *)drawable->resource, (u16)selection,
                         (u8)variant, (u8)flags);
        restart = &actor->restart;
    } else {
        struct SceneActorAnimationTile *tile =
            &((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile;
        u8 *savedSelection;

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
            Anim_SetKeyframes(&((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile,
                              actor->drawable, (u16)selection, 2, (u16)mode);
            if (flags & 1)
                tile->flags |= mask;
            else
                tile->flags &= ~4;
        }
    }
    *restart = 0;
}
