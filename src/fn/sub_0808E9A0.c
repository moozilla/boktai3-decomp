// MATCHING_CONCESSION: register-pinning
#include "scene_actor_chase.h"
void Slime_TickWaitTimer(void);
static inline void SelectAnimation(struct SceneActorAnimationView *actor, s32 index, u32 inputFlags)
{
    struct SceneActorProfileEntry *entry =
        (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, index);
    u32 selection = entry->value;
    u8 *restart;
    u32 flags = inputFlags;
    if (!actor->drawableKind) {
        struct SceneActorAnimationSequenceDrawable *drawable;
        actor->selection = selection;
        drawable = actor->drawable;
        Anim_SetSequence(drawable->sequence, (void *)drawable->resource, selection, 1, flags);
        restart = &actor->restart;
    } else {
        void *drawable = actor->drawable;
        register struct SceneActorAnimationTile *tile asm("r4");
        u8 *selectionAddress;
        u32 previous;
        u32 next;
        u8 *savedSelection;
        asm("" : "+r"(drawable));
        tile = &((struct SceneActorAnimationTileDrawable *)drawable)->tile;
        next = selection + 1;
        asm("" : "+r"(next));
        selectionAddress = &actor->selection;
        previous = *selectionAddress;
        savedSelection = selectionAddress;
        restart = &actor->restart;
        if (previous != next || *restart || (tile->flags & 4)) {
            *savedSelection = next;
            {
                register u32 mask asm("r1") = 4;
                register u32 savedMask asm("r8");
                u32 mode;
                asm("" : : "r"(savedMask));
                if (flags & mask)
                    mode = 6;
                else
                    mode = 4;
                Anim_SetKeyframes(
                    &((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile,
                    actor->drawable, (u16)next, 2, mode);
                tile->flags &= ~4;
            }
        } else
            return;
    }
    *restart = 0;
}
#define Slime_PlayWaitAnimation sub_0808E9A0
void Slime_PlayWaitAnimation(struct SceneActorRegistryResetView *actor)
{
    if (SceneActorReaction_TakePrepared(actor))
        SelectAnimation((void *)actor, 15, 0);
    if (actor->field2B3)
        Chase_SetCtl(actor, Slime_TickWaitTimer, 0x24);
}
