#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_animation.h"
void Crow_GetDirectionalFrameAndFlip(u16, s32 *, s32 *);
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

void Crow_UpdateDirectionalPose(struct SceneActorAnimationView *actor)
{
    struct SceneActorProfileEntry *entry;
    u8 *body;
    s32 variant, flags;
    u32 selection;
    if (!SceneActorReaction_TakePrepared((void *)actor))
        return;
    body = (u8 *)actor + 0x50;
    entry = (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, 2);
    Crow_GetDirectionalFrameAndFlip(body[9], &variant, &flags);
    selection = entry->value;
    Select(actor, selection, variant, flags);
}
