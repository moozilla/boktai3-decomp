// MATCHING_CONCESSION: register-pinning
#include "scene_actor_control_phase.h"

#ifdef NONMATCHING
// fuzzy: 92.4%
// Register allocation and inline animation restart-pointer lifetimes still differ.
static inline void Select(struct SceneActorAnimationView *actor, u32 selection, u32 variant,
                          u32 flags)
{
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

void TentacleBody_GetDirectionFrame(u8, u32 *, u32 *);
void TentacleBody_ResetDirectionalAnimations(void *, s32);
void TentacleBody_RefreshDirectionalAnimations(void *, s32);
void sub_080A9DB0(void *, s32, s32);
void TentacleBody_UpdateAttackRecovery(struct SceneActorRegistryResetView *actor, s32 frame)
{
    register u8 *entry asm("r10") = (void *)SceneActorProfiles_GetEntryAddress(
        ((struct SceneActorAnimationView *)actor)->profileId,
        ((struct SceneActorAnimationView *)actor)->profileMode, 0);
    u32 variant, flags;
    if (actor->field2B2)
        actor->field2B2 = 0;
    TentacleBody_GetDirectionFrame(actor->direction, &variant, &flags);
    Select((void *)actor, entry[0x44], variant, flags);
    TentacleBody_ResetDirectionalAnimations(actor->context, 18);
    sub_080A9DB0(actor, 20, 25600);
    if (frame > 40) {
        TentacleBody_GetDirectionFrame(actor->direction, &variant, &flags);
        Select((void *)actor, entry[0x38], variant, flags);
        TentacleBody_RefreshDirectionalAnimations(actor->context, 15);
        actor->field2B0 = 1;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", TentacleBody_UpdateAttackRecovery);
#endif
