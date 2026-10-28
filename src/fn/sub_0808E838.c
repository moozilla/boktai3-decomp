// MATCHING_CONCESSION: register-pinning
#include "scene_actor_chase.h"
// confidence=low: plays profile animation 15 with a random duration, then enters the next Slime control.
void Slime_PlayWaitAnimation(void);
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
        Anim_SetSequence(drawable->sequence, (void *)drawable->resource, selection, 0, flags);
        restart = &actor->restart;
    } else {
        void *drawable = actor->drawable;
        register struct SceneActorAnimationTile *tile asm("r4");
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
                register u32 savedMask asm("r8");
                u32 mode;
                savedMask = mask;
                asm("" : : "r"(savedMask));
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
}
void Slime_UpdateRandomWait(struct SceneActorRegistryResetView *actor)
{
    if (SceneActorReaction_TakePrepared(actor)) {
        u32 index;
        SelectAnimation((void *)actor, 15, 0);
        index = (gRandomIndex + 1) & 1023;
        gRandomIndex = index;
        {
            u32 random = *(u16 *)(index * 2 + (u32)gRandomTable);
            actor->field2C2 = random & 63;
        }
        if (!actor->field2C2)
            actor->field2C2 = 64;
        Anim_SetDurationScale((u8 *)actor->drawable + 0x48, actor->field2C2);
    }
    if (actor->field2B3)
        Chase_SetCtl(actor, Slime_PlayWaitAnimation, 0x23);
}
