#include "scene_actor_animation.h"
static inline u8 TakeFlag(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}
void Ghost_GetDirectionalFrame(u8, u32 *, u32 *);
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
#define Ghost_UpdateDirectionalPoseB sub_08095AC0

void Ghost_UpdateDirectionalPoseB(struct SceneActorAnimationView *actor)
{
    struct SceneActorProfileEntry *entry;
    u32 variant, flags, selection;
    if (TakeFlag(&actor->directionalEntryFlag))
        *(u16 *)((u8 *)actor + 0x2C2) = 0x40;
    SceneActorAnimation_SelectDirectionalEntry(actor, 1);
    Ghost_GetDirectionalFrame(actor->direction, &variant, &flags);
    entry = (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, 1);
    selection = entry->value;
    Select(actor, selection, variant, flags);
}
