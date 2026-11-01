// confidence=low: existing Golem state interpretation retained.
#include "global.h"
// confidence=low: neighboring Golem controls establish the state role.
#include "scene_actor_reset.h"
#include "scene_actor_animation.h"
#include "scene_object.h"
extern const u16 gUnk_08606130[];
void Anim_SetDurationScale(void *, u32);
void Anim_SetFrameDurationScale(void *, u32);
#define A(a) ((struct SceneActorAnimationView *)(a))
static inline u8 TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return 1;
    }
    return 0;
}
static inline void Golem_GetDirectionVariant(struct SceneActorRegistryResetView *actor, u8 *variant,
                                             u8 *mirror)
{
    s32 quadrant = ((actor->body.direction + 0x60) >> 6) & 3;
    if (quadrant > 1) {
        *mirror = 1;
        *variant = 3 - quadrant;
    } else {
        *mirror = 0;
        *variant = quadrant;
    }
}

static inline void Golem_SelectAnimation(struct SceneActorRegistryResetView *actor, u32 selection,
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

// confidence=low: charge and slide behaviors install this completion control.
void Golem_UpdateChargeStop(struct SceneActorRegistryResetView *actor)
{
    if (TakeFlag(&actor->field2B2)) {
        u8 directionVariant, mirror;
        Golem_GetDirectionVariant(actor, &directionVariant, &mirror);
        Golem_SelectAnimation(actor, 7, directionVariant, mirror | 4);
        actor->field2B3 = 0;
    }
    if (actor->field2B3) {
        {
            u16 *field = &actor->flags2D4;
            u32 mask = ~0x100;
            u32 value = *field;
            mask &= value;
            *field = mask;
        }
        actor->field2B0 = 1;
    }
}
