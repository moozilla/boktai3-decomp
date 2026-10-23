// MATCHING_CONCESSION: register-pinning
#include "scene_actor_chase.h"
static inline void SelectAnimation(struct SceneActorAnimationView *actor, s32 index, u32 inputFlags)
{
    struct SceneActorProfileEntry *entry =
        (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, index);
    u32 selection = entry->value;
    u8 *restart;
    u32 flags = inputFlags;
    register u32 spare asm("r8");
    asm volatile("" : "=r"(spare));
    asm volatile("" : "+r"(flags));
    if (!actor->drawableKind) {
        struct SceneActorAnimationSequenceDrawable *drawable;
        actor->selection = selection;
        drawable = actor->drawable;
        Anim_SetSequence(drawable->sequence, (void *)drawable->resource, selection, 0, flags);
        restart = &actor->restart;
    } else {
        void *drawable = actor->drawable;
        register struct SceneActorAnimationTile *tile asm("r5");
        u8 *selectionAddress;
        u32 previous;
        u8 *savedSelection;
        asm("" : "+r"(drawable));
        tile = &((struct SceneActorAnimationTileDrawable *)drawable)->tile;
        selectionAddress = &actor->selection;
        previous = *selectionAddress;
        savedSelection = selectionAddress;
        restart = &actor->restart;
        if (previous != selection || *restart || (tile->flags & 4)) {
            *savedSelection = selection;
            {
                register u32 mask asm("r1") = 4;
                u32 mode;
                if (flags & mask)
                    mode = 6;
                else
                    mode = 4;
                Anim_SetKeyframes(
                    &((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile,
                    actor->drawable, selection, 2, mode);
                tile->flags &= ~4;
            }
        } else
            return;
    }
    *restart = 0;
    asm volatile("" : : "r"(flags));
}
// confidence=low: plays profile entry two when the action begins.
#define Crow_PlayEntry2 sub_0809EF50
void Crow_PlayEntry2(struct SceneActorRegistryResetView *actor)
{
    if (SceneActorReaction_TakePrepared(actor))
        SelectAnimation((void *)actor, 2, 0);
}
