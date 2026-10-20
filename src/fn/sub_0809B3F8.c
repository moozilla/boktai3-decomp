#include "global.h"

// confidence=low: follows the existing Lun movement callback interpretation.

#ifdef NONMATCHING
// fuzzy: 82.8%
// Remaining differences: instruction ordering and register allocation.
#include "scene_actor_animation.h"
#include "scene_actor_reset.h"
void Lun_GetDirectionalFrame(u8, u32 *, u32 *);
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

void Anim_SetDurationScale(void *, u16);
void Anim_SetFrameDurationScale(void *, u16);
static inline u8 TakeFlag(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}
static inline void SetControl(struct SceneActorRegistryResetView *a, void *callback, u32 kind)
{
    a->field2B2 = 1;
    a->field2B0 = 0;
    a->field2B5 = kind;
    a->target = callback;
    a->field2C8 = 0;
    {
        u32 *flags = &a->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    a->word104 = 0;
}
// confidence=low: turns Lun in 32-angle increments until its saved direction is reached.
void Lun_TurnTowardSavedDirection(struct SceneActorRegistryResetView *a)
{
    u8 *identity = (u8 *)a + 0x50;
    u8 *anim;
    u16 *scale;
    if (TakeFlag(&a->field2B2)) {
        s32 facing = ((a->direction + 0x70) >> 5) & 7;
        u8 saved = *((u8 *)a + 0x3CD);
        s32 goal = ((saved + 0x70) >> 5) & 7;
        if (facing == goal) {
            a->direction = saved;
            SetControl(a, a->savedTarget, 0);
            a->field2B0 = 1;
            return;
        }
        if (((facing - (goal - 8)) & 7) <= 4)
            a->flags2CC &= ~0x10000;
        else {
            u32 mask = 0x10000;
            a->flags2CC |= mask;
        }
    }
    anim = (u8 *)a->drawable + 0x48;
    if (a->flags2D4 & 0xA02F) {
        scale = &a->field2C2;
        *scale = 64;
    } else {
        scale = &a->field2C2;
        if (!*scale)
            *scale = 64;
    }
    if (!a->drawableKind)
        Anim_SetDurationScale((u8 *)a->drawable + 0x48, *scale);
    else
        Anim_SetFrameDurationScale((u8 *)a->drawable + 0x20, *scale);
    if (a->field2CA & 4) {
        struct SceneActorProfileEntry *entry =
            (void *)SceneActorProfiles_GetEntryAddress(a->profileId, a->profileMode, 1);
        u32 variant, flags;
        SetControl(a, a->entryTarget224, 2);
        if (a->flags2CC & 0x10000)
            identity[9] += 32;
        else
            identity[9] -= 32;
        Lun_GetDirectionalFrame(identity[9], &variant, &flags);
        anim[5] = 0;
        Select((void *)a, entry->value, variant, flags);
    }
}

#else
INCLUDE_ASM("asm/nonmatching", Lun_TurnTowardSavedDirection);
#endif
