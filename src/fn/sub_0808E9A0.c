#include "scene_actor_chase.h"
void Slime_TickWaitTimer(void);
/* SceneActorAnimation_SelectProfileEntry with the entry's variant and flags supplied by the caller. */
static inline void SelectAnimation(struct SceneActorAnimationView *actor, s32 index, u32 variant,
                                   u32 inputFlags)
{
    struct SceneActorProfileEntry *entry =
        (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, index);
    u32 selection = entry->value;
    u32 flags = inputFlags;
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
        Anim_SetSequence(drawable->sequence, (void *)drawable->resource, selection, variant, flags);
        restart = &actor->restart;
    } else {
        struct SceneActorAnimationTile *tile =
            &((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile;
        u8 *selectionAddress;
        u32 previous;
        u8 *savedSelection;
        selection += variant;
        selectionAddress = &actor->selection;
        previous = *selectionAddress;
        savedSelection = selectionAddress;
        restart = &actor->restart;
        if (!force && previous == selection && !*restart) {
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
                              actor->drawable, (u16)selection, 2, mode);
            if (flags & 1)
                tile->flags |= mask;
            else
                tile->flags &= ~4;
        }
    }
    *restart = 0;
}
void Slime_PlayWaitAnimation(struct SceneActorRegistryResetView *actor)
{
    if (SceneActorReaction_TakePrepared(actor))
        SelectAnimation((void *)actor, 15, 1, 0);
    if (actor->field2B3)
        Chase_SetCtl(actor, Slime_TickWaitTimer, 0x24);
}
