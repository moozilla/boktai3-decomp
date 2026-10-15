// MATCHING_CONCESSION: register-pinning
#include "scene_actor_animation.h"
#define SceneActorAnimation_SelectExplicitVariant sub_080765F8
void SceneActorAnimation_SelectExplicitVariant(struct SceneActorAnimationView *actor, s32 index,
                                               u32 variant, u32 flags)
{
    struct SceneActorProfileEntry *entry =
        (void *)sub_08073CB0(actor->profileId, actor->profileMode, index);
    u32 selection = entry->value;
    u32 force;
    u8 *restart;
    if (flags & 0x10) {
        force = 0x10;
        flags &= ~0x10;
    } else
        force = 0;
    asm("" : : "r"(flags), "r"(flags), "r"(flags));
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
            if (flags & 1)
                tile->flags |= mask;
            else
                tile->flags &= ~4;
        }
    }
    *restart = 0;
}
