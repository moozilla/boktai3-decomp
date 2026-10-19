#include "global.h"
#ifdef NONMATCHING
// fuzzy: 58.1%
// Special-animation start (categories 5/6/7 select sequences 0x63/0x65/0x67 through an inline
// copy of SceneActorAnimation_SelectDirectionalEntryFlags with an explicit selection), then the
// shared chase (include/scene_actor_chase.h).
#include "scene_actor_chase.h"
#include "scene_actor_profile.h"

void sub_0807C72C(struct SceneActorRegistryResetView *, u32, u32, u32);
void SceneActorBehavior_CallActionScript(struct SceneActorRegistryResetView *);
void SceneActorReaction_PlayAnimationBUntilComplete(void);

/* SceneActorAnimation_SelectDirectionalEntryFlags with an explicit selection: the profile
 * entry (index 0) supplies only the flags. */
static inline void SelectDirectionalSequence(struct SceneActorAnimationView *actor, u32 selection)
{
    struct SceneActorProfileEntry *entry;
    u8 directionVariant, mirror;
    u8 *mirrorPointer;
    u8 *variantPointer = &directionVariant;
    u32 flags, variant;
    u32 entryFlags;
    u8 *variantReload;
    u32 force;
    u8 *restart;
    {
        u8 *mirrorAddress = &mirror;
        s32 quadrant = (((s32)actor->direction + 0x60) >> 6) & 3;
        mirrorPointer = mirrorAddress;
        if (quadrant > 1) {
            *mirrorPointer = 1;
            *variantPointer = 3 - quadrant;
        } else {
            *mirrorPointer = 0;
            *variantPointer = quadrant;
        }
    }
    entry = (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, 0);
    variantReload = &directionVariant;
    flags = *mirrorPointer;
    entryFlags = entry->flags;
    variant = *variantReload;
    flags |= entryFlags;
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

static inline void StartSpecialAnimation(struct SceneActorRegistryResetView *actor, u32 selection)
{
    if ((actor->flags2D4 & 0x200) || actor->unknown2B7 == selection) {
        SelectDirectionalSequence((struct SceneActorAnimationView *)actor, selection);
        actor->field2B0 = 1;
    } else {
        void *target = SceneActorReaction_PlayAnimationBUntilComplete;
        Chase_SetCtl(actor, target, 0xA);
    }
}

void sub_080BF0E4(struct SceneActorRegistryResetView *actor)
{
    u8 *group = (u8 *)&actor->group;

    if (Chase_TakeArrived(actor)) {
        u8 *category;
        actor->direction = actor->fieldE9;
        category = &actor->category;
        if (*category == 5)
            StartSpecialAnimation(actor, 0x63);
        else if (*category == 6)
            StartSpecialAnimation(actor, 0x65);
        else if (*category == 7)
            StartSpecialAnimation(actor, 0x67);
        sub_0807C72C(actor, 0, 0, *category);
        SceneActorBehavior_CallActionScript(actor);
        *(u16 *)((u8 *)actor + 0x414) |= 4;
        actor->flags2D4 |= 0x80;
        *(u16 *)((u8 *)actor + 0x418) = *(u16 *)((u8 *)actor + 0x416);
    }
    if (actor->field2B0) {
        Chase_SetBehavior(actor, 12);
        Chase_SetCtl(actor, *(void **)(group + 0x48), 0x1A);
        Chase_Arrive(actor);
    } else
        actor->counter2AC++;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080BF0E4);
#endif
