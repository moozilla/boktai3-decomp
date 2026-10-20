#include "scene_actor_animation.h"
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
static inline u16 Flag(struct SceneActorAnimationView *actor, u16 mask)
{
    u16 *p = (void *)((u8 *)actor + 0x2D6);
    return *p = mask | *p;
}
// confidence=low: selects Ghost's waiting animation and marks the wait complete after 60 ticks.

void Ghost_UpdateWaitingAnimation(struct SceneActorAnimationView *actor, s32 frame)
{
    struct SceneActorProfileEntry *entry;
    u32 variant, flags, selection;
    if (actor->directionalEntryFlag)
        actor->directionalEntryFlag = 0;
    Ghost_GetDirectionalFrame(actor->direction, &variant, &flags);
    entry = (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, 11);
    selection = entry->value;
    Select(actor, selection, variant, flags);
    if (frame <= 29)
        Flag(actor, 32);
    if (frame > 59)
        actor->unknown2B0 = 1;
}
