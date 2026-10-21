#define TentacleBody_UpdateAttackWindup sub_080A5C50
#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_animation.h"
void TentacleBody_GetDirectionFrame(u8, u32 *, u32 *);
void TentacleBody_ResetDirectionalAnimations(void *, s32);
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
            if (flags & 1)
                tile->flags |= mask;
            else
                tile->flags &= ~4;
        }
    }
    *restart = 0;
}
void TentacleBody_RefreshDirectionalAnimations(void *, s32);
static inline u8 Take(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}
void TentacleActor_AssignSegmentPhases(void *);

void TentacleBody_UpdateAttackWindup(struct SceneActorAnimationView *actor)
{
    u8 *entry = (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, 0);
    u32 variant, flags;
    if (Take(&actor->directionalEntryFlag)) {
        TentacleBody_GetDirectionFrame(actor->direction, &variant, &flags);
        Select(actor, entry[0x38], variant, flags);
        TentacleBody_RefreshDirectionalAnimations(
            ((struct SceneActorRegistryResetView *)actor)->context, 15);
        TentacleActor_AssignSegmentPhases(((struct SceneActorRegistryResetView *)actor)->context);
    } else {
        TentacleBody_GetDirectionFrame(actor->direction, &variant, &flags);
        Select(actor, entry[0x38], variant, flags);
        TentacleBody_RefreshDirectionalAnimations(
            ((struct SceneActorRegistryResetView *)actor)->context, 15);
    }
}
