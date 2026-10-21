#include "scene_actor_animation.h"
void Bee_GetDirectionalFrameAndFlip(u8, u32 *, u32 *);
static inline u8 Take(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}
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

void Bee_UpdateTurnAnimation(struct SceneActorAnimationView *actor)
{
    u8 *body = *(u8 **)((u8 *)actor + 0x3D0);
    u32 variant, flags;
    if (Take(&actor->directionalEntryFlag)) {
        struct SceneActorProfileEntry *entry =
            (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, 1);
        Bee_GetDirectionalFrameAndFlip(((u8 *)actor)[0x59], &variant, &flags);
        {
            u8 *sequence = (u8 *)actor->drawable + 0x48;
            sequence[5] = 0;
        }
        Select(actor, entry->value, variant, flags);
        {
            u32 direction = ((u8 *)actor)[0x59];
            body[0x3DF] = direction;
        }
    } else if (body[0x3DF] != ((u8 *)actor)[0x59]) {
        struct SceneActorProfileEntry *entry =
            (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, 1);
        u8 *sequence = (u8 *)actor->drawable + 0x48;
        u32 frame = *(u16 *)(sequence + 8), time = *(u16 *)(sequence + 14);
        Bee_GetDirectionalFrameAndFlip(((u8 *)actor)[0x59], &variant, &flags);
        Select(actor, entry->value, variant, flags);
        *(u16 *)(sequence + 8) = frame;
        *(u16 *)(sequence + 14) = time;
        {
            u32 direction = ((u8 *)actor)[0x59];
            body[0x3DF] = direction;
        }
    } else
        actor->restart = 0;
}
