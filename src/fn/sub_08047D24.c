// MATCHING_CONCESSION: register-pinning
#include "temporary_animation_pool.h"
struct TemporaryAnimationPool *TemporaryAnimationPool_Create(void);
struct TemporaryAnimationSlot *TemporaryAnimationPool_FindInactive(struct TemporaryAnimationPool *);
void Widget_CopyMasked(void *, u32, u32);
void Anim_LinkToList(void *, u32);
struct TemporaryAnimationSlot *TemporaryAnimationPool_CreateEntry(u32 sequence, u32 lifetime,
                                                                  u32 flags, s32 paletteIndex,
                                                                  const void *paletteData)
{
    struct TemporaryAnimationPool *pool = gUnk_020000A8;
    struct TemporaryAnimationSlot *slot;
    struct ProjectedDisplayEntry *animation;
    if (!pool) {
        pool = TemporaryAnimationPool_Create();
        if (!pool)
            return NULL;
    }
    slot = TemporaryAnimationPool_FindInactive(pool);
    if (!slot)
        return NULL;
    animation = &slot->animation;
    Widget_CopyMasked(animation, sequence, 27);
    animation->flags |= flags;
    if (paletteIndex > 0) {
        const void *palette;
        animation->paletteIndex = paletteIndex;
        palette = paletteData;
        if (!palette)
            palette = gPaletteBankData + paletteIndex * 32;
        animation->paletteData = palette;
    }
    if (animation->flags & 128)
        Anim_LinkToList(animation, 1);
    else
        Anim_LinkToList(animation, 0);
    {
        register u32 frames asm("r1") = lifetime;
        slot->lifetime = frames;
    }
    slot->active = 1;
    return slot;
}
