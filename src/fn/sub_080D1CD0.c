#include "global.h"
// confidence=low: hidden recovery disables the shadow and waits to enter revival.
#include "scene_actor_animation.h"
#include "scene_actor_reaction.h"
static inline void Mummy_GetDirectionVariant(struct SceneActorRegistryResetView *actor, u8 *variant,
                                             u8 *mirror, u32 zero)
{
    s32 quadrant = ((actor->body.direction + 0x60) >> 6) & 3;
    if (quadrant > 1) {
        *mirror = 1;
        *variant = 3 - quadrant;
    } else {
        *mirror = zero;
        *variant = quadrant;
    }
}

static inline void Mummy_SelectAnimation(struct SceneActorRegistryResetView *actor, u32 selection,
                                         u32 variant, u32 flags)
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
        actor->animationIndex = selection;
        drawable = actor->drawable;
        Anim_SetSequence(drawable->sequence, (void *)drawable->resource, (u16)selection,
                         (u8)variant, (u8)flags);
        restart = &actor->field2B3;
    } else {
        struct SceneActorAnimationTile *tile =
            &((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile;
        u8 *savedSelection;
        selection += variant;
        savedSelection = &actor->animationIndex;
        restart = &actor->field2B3;
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

static inline u8 TakeSaved(u8 *flag, u8 **saved)
{
    u32 value = *flag;
    *saved = flag;
    if (value) {
        **saved = 0;
        return 1;
    }
    return 0;
}
void Mummy_UpdateRevival(void *);
void Mummy_UpdateHiddenRecovery(struct SceneActorRegistryResetView *actor, s32 frame)
{
    u32 queued;
    u8 *prepared;
    u32 mask = 1;
    if (!actor->drawableKind) {
        u32 *flags = actor->drawable;
        *flags |= mask;
    } else {
        struct SceneActorAnimationTile *tile = (void *)((u8 *)actor->drawable + 0x20);
        tile->flags |= mask;
    }
    queued = TakeSaved(&actor->field2B2, &prepared);
    if (queued) {
        ActorShadow_Disable((u8 *)actor + 0x320);
    } else if (frame > 0x1A) {
        void *callback;
        u32 kind, zero;
        {
            u8 directionVariant, mirror;
            Mummy_GetDirectionVariant(actor, &directionVariant, &mirror, queued);
            Mummy_SelectAnimation(actor, 8, directionVariant, mirror | 4);
        }
        callback = Mummy_UpdateRevival;
        kind = 13;
        zero = 0;
        *prepared = 1;
        actor->field2B0 = zero;
        actor->field2B5 = kind;
        actor->target = callback;
        actor->field2C8 = zero;
        {
            u32 *flags = &actor->flags114;
            s32 mask = ~1;
            *flags &= mask;
        }
        actor->word104 = zero;
    } else
        actor->field2B3 = queued;
}
